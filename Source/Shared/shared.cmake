function(message)
    if (NOT MESSAGE_QUIET)
        _message(${ARGN})
    endif()
endfunction()

set(LINK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/link/udp/udp_discovery_peer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/link/udp/udp_discovery_protocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/link/link.cpp
)

add_library(link STATIC ${LINK_SOURCES})


file(GLOB ELSE_SHARED_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/libsamplerate/*.c
)

add_library(else_shared SHARED ${ELSE_SHARED_SOURCES})

set_target_properties(else_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PD_OUTPUT_PATH})
set_target_properties(else_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PD_OUTPUT_PATH})
set_target_properties(else_shared PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PD_OUTPUT_PATH})
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
		set_target_properties(else_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PD_OUTPUT_PATH})
		set_target_properties(else_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PD_OUTPUT_PATH})
		set_target_properties(else_shared PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PD_OUTPUT_PATH})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

if(APPLE)
    target_link_options(else_shared PRIVATE -undefined dynamic_lookup)
elseif(WIN32)
    find_library(PD_LIBRARY NAMES pd HINTS ${PD_LIB_PATH})
	target_link_libraries(else_shared PRIVATE ${PD_LIBRARY})
endif()

if(PD_FLOATSIZE64)
    target_compile_definitions(else_shared PRIVATE PD_FLOATSIZE=64)
endif()

# External directories
set(FFMPEG_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Source/Shared/ffmpeg-7.0.1")
set(LIBSAMPLERATE_INCLUDE "${CMAKE_SOURCE_DIR}/Source/Shared/libsamplerate")

set(FFMPEG_LIBS
    "${FFMPEG_OUT_DIR}/libavformat/libavformat.a"
    "${FFMPEG_OUT_DIR}/libavcodec/libavcodec.a"
    "${FFMPEG_OUT_DIR}/libavutil/libavutil.a"
    "${FFMPEG_OUT_DIR}/libswresample/libswresample.a"
)

message(STATUS "Configuring ffmpeg")
file(MAKE_DIRECTORY ${FFMPEG_OUT_DIR})
string(REGEX REPLACE "^([A-Za-z]):" "/\\1" FFMPEG_TAR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/ffmpeg/ffmpeg-7.0.1.tar.bz2")
execute_process(
    COMMAND tar xjf ${FFMPEG_TAR_PATH} -C ${CMAKE_CURRENT_BINARY_DIR}/Source/Shared
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
file(REMOVE ${FFMPEG_OUT_DIR}/VERSION) # Causes problems, because there is some header somewhere named just <version>

add_custom_command(
    OUTPUT ${FFMPEG_LIBS}
    COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/ffmpeg/build_ffmpeg.sh ${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/ffmpeg ${CMAKE_C_COMPILER_LAUNCHER}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Source/Shared
    COMMENT "Building FFmpeg libraries..."
    VERBATIM
)

message(STATUS "Configuring Opus")
set(MESSAGE_QUIET ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Source/Shared/opus)
set(MESSAGE_QUIET OFF)

# Define the custom target that depends on the FFmpeg build
add_custom_target(
    ffmpeg_deps
    ALL
    DEPENDS ${FFMPEG_LIBS}
)

add_library(ffmpeg INTERFACE)
target_link_libraries(ffmpeg INTERFACE ${FFMPEG_LIBS})
target_include_directories(ffmpeg INTERFACE ${FFMPEG_OUT_DIR})
add_dependencies(ffmpeg ffmpeg_deps)

target_include_directories(else_shared PUBLIC ${LIBSAMPLERATE_INCLUDE})

if(WIN32)
    target_link_options(else_shared PRIVATE -static-libgcc -static-libstdc++ -static)
    target_link_libraries(else_shared PRIVATE "ws2_32;iphlpapi;stdc++;bcrypt")
    target_compile_options(else_shared PRIVATE -msse2)
    target_compile_definitions(opus PUBLIC FLOAT_APPROX=1 _POSIX_SEM_VALUE_MAX=32767)
    target_compile_definitions(link PUBLIC _POSIX_SEM_VALUE_MAX=32767)
endif()
