#include "udp/udp_discovery_peer.hpp"
#include "udp/udp_socket.hpp"
#include "readerwriterqueue/readerwriterqueue.h"
#include "link.h"
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <sys/types.h>
#ifdef _WIN32
#include <vector>
#include <iphlpapi.h>
#else
#include <ifaddrs.h>
#endif

constexpr int kPort = 12021;
constexpr unsigned int kMulticastAddress = (224 << 24) + (0 << 16) + (0 << 8) + 123; // 224.0.0.123

// t_link manages the process of finding connectable devices, and opening a 2-way UDP connection
class t_link {
public:
    udpdiscovery::Peer peer;
    std::list<udpdiscovery::DiscoveredPeer> discovered_peers;
    int port;
    udp_server server;
    std::unordered_map<int, std::unique_ptr<udp_client>> clients;
    std::unordered_map<int, int> client_ping;
    bool socket_bound;
    std::string ip;
    moodycamel::ReaderWriterQueue<std::string, 1024> send_queue;
    moodycamel::ReaderWriterQueue<std::string, 1024> receive_queue;
    std::mutex client_lock;
    std::thread socket_thread;
    std::atomic<bool> quit;

    t_link(std::string identifier, std::string platform, bool local, uint64_t application_id) : port(get_random_port()), server(port) {
        ip = local ? std::string("127.0.0.1") : get_ip();

        // Try to bind our server UDP socket
        socket_bound = try_bind();
        if(!socket_bound) throw std::runtime_error("Socket bind failed");

        // Create identifier for UDP discovery. This should contain all the necessary info to establish a socket connection
        identifier += std::string("\x1F") + platform + std::string("\x1F") + get_hostname() + std::string("\x1F") + ip + std::string("\x1F") + std::to_string(port);

        // Set discovery parameters
        udpdiscovery::PeerParameters parameters;
        parameters.set_can_discover(true);
        parameters.set_can_be_discovered(true);
        parameters.set_can_use_broadcast(true);
        parameters.set_multicast_group_address(kMulticastAddress);
        parameters.set_can_use_multicast(true);
        parameters.set_discover_self(true);
        parameters.set_port(kPort);
        parameters.set_application_id(application_id);
        parameters.set_send_timeout_ms(1500);
        parameters.set_discovered_peer_ttl_ms(10000);

        socket_thread = std::thread(&t_link::process_sockets, this);

        if (!peer.Start(parameters, identifier)) {
            std::cerr << "Failed to start peer!" << std::endl;
            throw std::runtime_error("Peer start failed");
        }
    }

    ~t_link()
    {
        peer.Stop();
        quit = true;
        socket_thread.join();
    }

    static int get_current_time_ms() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    }

    static int get_random_port()
    {
        // Get random port number outside of system ranges
        return (rand() % 48127) + 1024;
    }

    void process_sockets()
    {
        while(!quit) {
            using namespace std::chrono;
            auto interval = microseconds(200);
            auto next_time = high_resolution_clock::now() + interval;

            std::string message;
            while(server.receive_data(message))
            {
                receive_queue.enqueue(message);
            }

            while(send_queue.try_dequeue(message)) {
                std::lock_guard lock(client_lock);
                for(auto& [port_num, client] : clients)
                {
                    client->send_data(message);
                }
            }
            std::this_thread::sleep_until(next_time);
            next_time += interval;
        }
    }

    // Try to bind the server socket to a port
    bool try_bind()
    {
        int tries = 0;
        while(server.socket_bind() != 0 && tries < 32) {
            port = get_random_port();
            server.set_port(port);
            std::this_thread::sleep_for(std::chrono::microseconds(tries * 150 + 5)); // unfortunate, but othewise random generation breaks!
            tries++;
        }

        //server.set_timeout(1);
        server.set_blocking(false);

        return tries != 32;
    }

    // Get hostname as a string. This is a readable identifier for your computers identity
    std::string get_hostname() const
    {
        char hostname[128] = {0};
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            return std::string(hostname);
        }

        return {};
    }

    // Get IP address. It picks the one from your first network adapter. If you have multiple network adapters, it could pick the wrong one!
    static std::string get_ip()
    {
#ifdef _WIN32
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_INET;  // Use AF_INET6 for IPv6 addresses
    ULONG bufferSize = 0;

    // First, call GetAdaptersAddresses to get the required buffer size.
    GetAdaptersAddresses(family, flags, nullptr, nullptr, &bufferSize);

    std::vector<BYTE> buffer(bufferSize);
    PIP_ADAPTER_ADDRESSES adapterAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

    // Now call GetAdaptersAddresses again with the allocated buffer.
    if (GetAdaptersAddresses(family, flags, nullptr, adapterAddresses, &bufferSize) == NO_ERROR)
    {
        for (PIP_ADAPTER_ADDRESSES adapter = adapterAddresses; adapter != nullptr; adapter = adapter->Next)
        {
            if (adapter->OperStatus != IfOperStatusUp) continue;

            for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast != nullptr; unicast = unicast->Next)
            {
                if (unicast->Address.lpSockaddr->sa_family == AF_INET)
                {
                    char ip[INET_ADDRSTRLEN];
                    sockaddr_in* sa_in = reinterpret_cast<sockaddr_in*>(unicast->Address.lpSockaddr);
                    InetNtop(AF_INET, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);

                    // Skip loopback addresses
                    if (strcmp(ip, "127.0.0.1") != 0)
                    {
                        return std::string(ip);
                    }
                }
            }
        }
    }
#else
        struct ifaddrs *ifaddr, *ifa;
        // Get a linked list of network interfaces
        if (getifaddrs(&ifaddr) == -1) {
            perror("getifaddrs");
            return "127.0.0.1";
        }

        // Iterate through the list of interfaces
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL)
                continue;

            // Check if the address is IPv4
            if (ifa->ifa_addr->sa_family == AF_INET) {
                char ip[INET_ADDRSTRLEN];
                // Convert the address to a human-readable string
                void *addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN);
                // Skip loopback addresses
                if (strcmp(ip, "127.0.0.1") != 0) {
                    freeifaddrs(ifaddr);
                    return std::string(ip);
                }
            }
        }

        // Free the linked list of interfaces
        freeifaddrs(ifaddr);
#endif
        return "127.0.0.1";
    }

    const char* get_own_ip()
    {
        return ip.c_str();
    }

    // Connect to a port and IP combination
    bool connect(int target_port, std::string target_ip)
    {
        std::lock_guard lock(client_lock);
        auto& client = clients[target_port];
        if(!client) { // Check if we already have a client for this port
            client = std::make_unique<udp_client>(target_port, target_ip);
            client_ping[target_port] = get_current_time_ms();
            return true;
        }
        return false;
    }

    // Receive messages sent to server into the passed in callback
    void receive(void* object, void(*callback)(void*, size_t, const char*))
    {
        std::string message;
        while(receive_queue.try_dequeue(message))
        {
            if (message.rfind("#PING#", 0) == 0) // Check if the message starts with "#PING#"
            {
                handle_ping(message);
            }
            else {
                callback(object, message.size(), message.c_str());
            }
        }
    }

    void handle_ping(std::string& ping)
    {
        std::string port_num_str = ping.substr(6); // Extract the port number part
        if (!port_num_str.empty())
        {
            try
            {
                const int port_num = std::stoi(port_num_str); // Convert the port number string to an integer
                client_ping[port_num] = get_current_time_ms();
            }
            catch (const std::invalid_argument&)
            {
            }
            catch (const std::out_of_range&)
            {
            }
        }
    }

    void ping(void* object, void(*connection_lost_callback)(void*, int))
    {
        send_queue.enqueue(std::string("#PING#") + std::to_string(port));

        auto current_time = get_current_time_ms();
        for(const auto& [port_num, ping] : client_ping)
        {
            if((current_time - ping) > 5000)
            {
                connection_lost_callback(object, port_num);
            }
        }

        // C++17 equivalent for std::erase_if
        // TODO: use std::erase_if once C++20 is ready on all platforms
        for (auto it = client_ping.begin(); it != client_ping.end();) {
            auto const& [port_num, ping] = *it;
            if ((current_time - ping) > 5000) {
                it = client_ping.erase(it);
            } else {
                ++it;
            }
        }

        std::lock_guard lock(client_lock);
        for (auto it = clients.begin(); it != clients.end();) {
            auto const& [port_num, client] = *it;
            if (!client_ping.count(port_num)) {
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Send data from client to connected server
    void send(const std::string& data)
    {
        send_queue.enqueue(data);
    }

    // Update discoverable devices
    void discover()
    {
        discovered_peers = peer.ListDiscovered();
    }

    // Get number of discoverable devices
    int get_num_peers()
    {
        return discovered_peers.size();
    }

    // Get info about a discoverable device
    t_link_discovery_data get_discovered_peer(int idx) const
    {
        auto peer_front = discovered_peers.begin();
        std::advance(peer_front, idx);

        std::string user_data = peer_front->user_data();
        t_link_discovery_data data;
        memset(&data, 0, sizeof(t_link_discovery_data));

        // Use a stringstream to parse the user_data
        std::istringstream iss(user_data);
        std::string token;

        // Extract name
        if (std::getline(iss, token, '\x1F')) {
            data.sndrcv = (char*)malloc(token.length() + 1);
            memcpy(data.sndrcv, token.c_str(), token.length());
            data.sndrcv[token.length()] = '\0';
        }

        // Extract platform
        if (std::getline(iss, token, '\x1F')) {
            data.platform = (char*)malloc(token.length() + 1);
            memcpy(data.platform, token.c_str(), token.length());
            data.platform[token.length()] = '\0';
        }

        // Extract hostname
        if (std::getline(iss, token, '\x1F')) {
            data.hostname = (char*)malloc(token.length() + 1);
            memcpy(data.hostname, token.c_str(), token.length());
            data.hostname[token.length()] = '\0';
        }

        // Extract IP
        if (std::getline(iss, token, '\x1F')) {
            data.ip = (char*)malloc(token.length() + 1);
            memcpy(data.ip, token.c_str(), token.length());
            data.ip[token.length()] = '\0';
        }

        // Extract port
        if (std::getline(iss, token, '\x1F')) {
            try {
                data.port = std::stoi(token); // Convert string to integer
            } catch (const std::invalid_argument& e) {
                data.port = 0; // Or handle error as appropriate
            } catch (const std::out_of_range& e) {
                data.port = 0; // Or handle error as appropriate
            }
        }

        return data;
    }
};

// C interface
t_link_handle link_init(const char* identifier, const char* platform, int local, uint64_t application_id) {
    try {
        t_link* wrapper = new t_link(identifier, platform, local, application_id);
        return static_cast<t_link_handle>(wrapper);
    } catch (const std::exception&) {
        return nullptr;
    }
}

void link_free(t_link_handle link_handle) {
    if (link_handle) delete static_cast<t_link*>(link_handle);
}

int link_get_num_peers(t_link_handle link_handle) {
    return static_cast<t_link*>(link_handle)->get_num_peers();
}

t_link_discovery_data link_get_discovered_peer_data(t_link_handle link_handle, int idx) {
    return static_cast<t_link*>(link_handle)->get_discovered_peer(idx);
}

// C-compatible function to run the discovery loop
void link_discover(t_link_handle link_handle) {
    static_cast<t_link*>(link_handle)->discover();
}

void link_send(t_link_handle link_handle, size_t size, const char* data)
{
    static_cast<t_link*>(link_handle)->send(std::string((const char*)data, size));
}

void link_receive(t_link_handle link_handle, void* object, void(*callback)(void*, size_t, const char*))
{
    static_cast<t_link*>(link_handle)->receive(object, callback);
}

int link_connect(t_link_handle link_handle, int port, const char* ip)
{
    return static_cast<t_link*>(link_handle)->connect(port, std::string(ip));
}

int link_isconnected(t_link_handle link_handle)
{
    return !static_cast<t_link*>(link_handle)->clients.empty();
}

const char* link_get_own_ip(t_link_handle link_handle)
{
    return static_cast<t_link*>(link_handle)->get_own_ip();
}

int link_get_own_port(t_link_handle link_handle)
{
    return static_cast<t_link*>(link_handle)->port;
}

void link_ping(t_link_handle link_handle, void* object, void(*connection_lost_callback)(void*, int))
{
    return static_cast<t_link*>(link_handle)->ping(object, connection_lost_callback);
}
