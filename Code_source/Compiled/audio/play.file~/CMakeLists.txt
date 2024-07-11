cmake_minimum_required(VERSION 3.14)
project(FFmpegStaticBuild)

include(ExternalProject)

cmake_policy(SET CMP0114 NEW)
cmake_policy(SET CMP0135 NEW)

# Define FFmpeg version and URL
set(FFMPEG_VERSION 7.0.1)
set(FFMPEG_TAR ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${FFMPEG_VERSION}.tar.bz2)
set(FFMPEG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg-${FFMPEG_VERSION})
set(FFMPEG_BUILD ${FFMPEG_SRC}/build)

# Define FFmpeg libraries
set(FFMPEG_LIBS libavcodec libavformat libavutil libswscale libswresample)

if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WINDOWS_ARGS --target-os=win64 --toolchain=msvc --arch=x86_64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(WINDOWS_ARGS --target-os=win64 --toolchain=msvc --arch=x86)
    endif()
endif()

# Extract and build FFmpeg
ExternalProject_Add(
    ffmpeg
    SOURCE_DIR ${FFMPEG_SRC}
    URL ${FFMPEG_TAR}
    CONFIGURE_COMMAND ${FFMPEG_SRC}/configure --prefix=${FFMPEG_BUILD} ${WINDOWS_ARGS} --enable-static --enable-optimizations --disable-shared --disable-debug --disable-doc --disable-programs --disable-avdevice --disable-postproc --disable-everything --enable-avcodec --enable-avformat --enable-avutil --enable-swscale --enable-swresample --enable-decoder=mp3*,pcm*,flac,aac,vorbis,opus --enable-parser=mpegaudio,aac --enable-demuxer=mp3,wav,aiff,flac,aac,ogg --enable-filter=aresample --enable-protocol=file
    BUILD_COMMAND make -C <SOURCE_DIR>
    INSTALL_COMMAND make -C <SOURCE_DIR> install && ${CMAKE_COMMAND} -E rm -f <SOURCE_DIR>/VERSION
    BUILD_IN_SOURCE 1
)

# Specify the output libraries
foreach(lib ${FFMPEG_LIBS})
    add_library(${lib} STATIC IMPORTED)
    set_target_properties(${lib} PROPERTIES IMPORTED_LOCATION ${FFMPEG_BUILD}/lib/${lib}.a)
endforeach()

# Optionally, create an interface library for easier linking
add_library(ffmpeg_static STATIC IMPORTED GLOBAL)
set_target_properties(ffmpeg_static PROPERTIES IMPORTED_LOCATION ${FFMPEG_BUILD}/lib/libavcodec.a)
target_link_libraries(ffmpeg_static INTERFACE ${FFMPEG_LIBS} z iconv)
add_dependencies(ffmpeg_static ffmpeg)

target_include_directories(ffmpeg_static INTERFACE ${FFMPEG_SRC})
