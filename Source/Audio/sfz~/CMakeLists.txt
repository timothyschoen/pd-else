cmake_minimum_required(VERSION 3.10)

project(sfizz
    DESCRIPTION  "SFZ sampler, providing AU, LV2, PureData and VST plugins."
    HOMEPAGE_URL "http://sfztools.github.io/sfizz"
    LANGUAGES    CXX C
)

set(BASEFLAGS
    "-DENABLE_LTO=OFF"
    "-DSFIZZ_JACK=OFF"
    "-DSFIZZ_RENDER=OFF"
    "-DSFIZZ_SHARED=OFF"
    "-DPLUGIN_AU=OFF"
    "-DPLUGIN_LV2=OFF"
    "-DPLUGIN_LV2_UI=OFF"
    "-DPLUGIN_PUREDATA=OFF"
    "-DPLUGIN_VST2=OFF"
    "-DPLUGIN_VST3=OFF"
    "-DSFIZZ_BENCHMARKS=OFF"
    "-DSFIZZ_DEMOS=OFF"
    "-DSFIZZ_DEVTOOLS=OFF"
    "-DSFIZZ_TESTS=OFF"
    "-DSFIZZ_USE_SNDFILE=OFF"
    "-DSFIZZ_SNDFILE_STATIC=OFF"
    "-DSFIZZ_USE_SYSTEM_ABSEIL=OFF"
    "-DSFIZZ_USE_SYSTEM_CATCH=OFF"
    "-DSFIZZ_USE_SYSTEM_CXXOPTS=OFF"
    "-DSFIZZ_USE_SYSTEM_GHC_FS=OFF"
    "-DSFIZZ_USE_SYSTEM_LV2=OFF"
    "-DSFIZZ_USE_SYSTEM_SIMDE=OFF"
    "-DSFIZZ_USE_SYSTEM_KISS_FFT=OFF"
    "-DSFIZZ_USE_SYSTEM_PUGIXML=OFF"
    "-DSFIZZ_USE_SYSTEM_VST3SDK=OFF"
    "-DSFIZZ_PROFILE_BUILD=OFF"
    "-DSFIZZ_RELEASE_ASSERTS=OFF"
    "-DCMAKE_BUILD_TYPE=Release"
    "-DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}"
    "-DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}"
    "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"
)

if(MSVC)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MSVC_TARGET_ARCH "-A x64")
else()
    set(MSVC_TARGET_ARCH "-A Win32")
endif()
elseif(APPLE)
list(APPEND BASEFLAGS "-GUnix\ Makefiles")
endif()

message(STATUS "Configuring sfizz")
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/build")
execute_process(COMMAND ${CMAKE_COMMAND} .. ${BASEFLAGS} ${MSVC_TARGET_ARCH} ERROR_QUIET OUTPUT_QUIET
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/build
)

if(MSVC)
# For some reason, it appears in the Debug folder, even when building in release mode...
set(SFIZZ_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/build/library/lib/$<CONFIG>)
else()
set(SFIZZ_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/build/library/lib)
endif()

if(MSVC)
set(SFIZZ_LIBS
    ${SFIZZ_LIB_DIR}/sfizz_static.lib
    ${SFIZZ_LIB_DIR}/sfizz_internal.lib
    ${SFIZZ_LIB_DIR}/sfizz_cephes.lib
    ${SFIZZ_LIB_DIR}/sfizz_filesystem_impl.lib
    ${SFIZZ_LIB_DIR}/sfizz_fmidi.lib
    ${SFIZZ_LIB_DIR}/sfizz_hiir_polyphase_iir2designer.lib
    ${SFIZZ_LIB_DIR}/sfizz_import.lib
    ${SFIZZ_LIB_DIR}/sfizz_kissfft.lib
    ${SFIZZ_LIB_DIR}/sfizz_messaging.lib
    ${SFIZZ_LIB_DIR}/sfizz_parser.lib
    ${SFIZZ_LIB_DIR}/sfizz_pugixml.lib
    ${SFIZZ_LIB_DIR}/sfizz_spin_mutex.lib
    ${SFIZZ_LIB_DIR}/sfizz_spline.lib
    ${SFIZZ_LIB_DIR}/sfizz_tunings.lib
    ${SFIZZ_LIB_DIR}/sfizz_cpuid.lib
    ${SFIZZ_LIB_DIR}/absl_hash.lib
    ${SFIZZ_LIB_DIR}/absl_strings.lib
    ${SFIZZ_LIB_DIR}/absl_bad_optional_access.lib
    ${SFIZZ_LIB_DIR}/absl_bad_variant_access.lib
    ${SFIZZ_LIB_DIR}/absl_base.lib
    ${SFIZZ_LIB_DIR}/absl_city.lib
    ${SFIZZ_LIB_DIR}/absl_civil_time.lib
    ${SFIZZ_LIB_DIR}/absl_cord.lib
    ${SFIZZ_LIB_DIR}/absl_cord_internal.lib
    ${SFIZZ_LIB_DIR}/absl_cordz_functions.lib
    ${SFIZZ_LIB_DIR}/absl_cordz_handle.lib
    ${SFIZZ_LIB_DIR}/absl_cordz_info.lib
    ${SFIZZ_LIB_DIR}/absl_crc32c.lib
    ${SFIZZ_LIB_DIR}/absl_crc_cord_state.lib
    ${SFIZZ_LIB_DIR}/absl_crc_cpu_detect.lib
    ${SFIZZ_LIB_DIR}/absl_crc_internal.lib
    ${SFIZZ_LIB_DIR}/absl_debugging_internal.lib
    ${SFIZZ_LIB_DIR}/absl_demangle_internal.lib
    ${SFIZZ_LIB_DIR}/absl_exponential_biased.lib
    ${SFIZZ_LIB_DIR}/absl_graphcycles_internal.lib
    ${SFIZZ_LIB_DIR}/absl_hashtablez_sampler.lib
    ${SFIZZ_LIB_DIR}/absl_int128.lib
    ${SFIZZ_LIB_DIR}/absl_log_severity.lib
    ${SFIZZ_LIB_DIR}/absl_low_level_hash.lib
    ${SFIZZ_LIB_DIR}/absl_malloc_internal.lib
    ${SFIZZ_LIB_DIR}/absl_raw_hash_set.lib
    ${SFIZZ_LIB_DIR}/absl_raw_logging_internal.lib
    ${SFIZZ_LIB_DIR}/absl_spinlock_wait.lib
    ${SFIZZ_LIB_DIR}/absl_stacktrace.lib
    ${SFIZZ_LIB_DIR}/absl_strings_internal.lib
    ${SFIZZ_LIB_DIR}/absl_symbolize.lib
    ${SFIZZ_LIB_DIR}/absl_synchronization.lib
    ${SFIZZ_LIB_DIR}/absl_throw_delegate.lib
    ${SFIZZ_LIB_DIR}/absl_time.lib
    ${SFIZZ_LIB_DIR}/absl_time_zone.lib
    ${SFIZZ_LIB_DIR}/st_audiofile.lib
    ${SFIZZ_LIB_DIR}/st_audiofile_formats.lib
    ${SFIZZ_LIB_DIR}/aiff.lib
)
else()
set(SFIZZ_LIBS
    ${SFIZZ_LIB_DIR}/libsfizz.a
    ${SFIZZ_LIB_DIR}/libsfizz_internal.a
    ${SFIZZ_LIB_DIR}/libsfizz_cephes.a
    ${SFIZZ_LIB_DIR}/libsfizz_filesystem_impl.a
    ${SFIZZ_LIB_DIR}/libsfizz_fmidi.a
    ${SFIZZ_LIB_DIR}/libsfizz_hiir_polyphase_iir2designer.a
    ${SFIZZ_LIB_DIR}/libsfizz_import.a
    ${SFIZZ_LIB_DIR}/libsfizz_kissfft.a
    ${SFIZZ_LIB_DIR}/libsfizz_messaging.a
    ${SFIZZ_LIB_DIR}/libsfizz_parser.a
    ${SFIZZ_LIB_DIR}/libsfizz_pugixml.a
    ${SFIZZ_LIB_DIR}/libsfizz_spin_mutex.a
    ${SFIZZ_LIB_DIR}/libsfizz_spline.a
    ${SFIZZ_LIB_DIR}/libsfizz_tunings.a
    ${SFIZZ_LIB_DIR}/libsfizz_cpuid.a
    ${SFIZZ_LIB_DIR}/libabsl_hash.a
    ${SFIZZ_LIB_DIR}/libabsl_strings.a
    ${SFIZZ_LIB_DIR}/libabsl_bad_optional_access.a
    ${SFIZZ_LIB_DIR}/libabsl_bad_variant_access.a
    ${SFIZZ_LIB_DIR}/libabsl_base.a
    ${SFIZZ_LIB_DIR}/libabsl_city.a
    ${SFIZZ_LIB_DIR}/libabsl_civil_time.a
    ${SFIZZ_LIB_DIR}/libabsl_cord.a
    ${SFIZZ_LIB_DIR}/libabsl_cord_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_cordz_functions.a
    ${SFIZZ_LIB_DIR}/libabsl_cordz_handle.a
    ${SFIZZ_LIB_DIR}/libabsl_cordz_info.a
    ${SFIZZ_LIB_DIR}/libabsl_crc32c.a
    ${SFIZZ_LIB_DIR}/libabsl_crc_cord_state.a
    ${SFIZZ_LIB_DIR}/libabsl_crc_cpu_detect.a
    ${SFIZZ_LIB_DIR}/libabsl_crc_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_debugging_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_demangle_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_exponential_biased.a
    ${SFIZZ_LIB_DIR}/libabsl_graphcycles_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_hashtablez_sampler.a
    ${SFIZZ_LIB_DIR}/libabsl_int128.a
    ${SFIZZ_LIB_DIR}/libabsl_log_severity.a
    ${SFIZZ_LIB_DIR}/libabsl_low_level_hash.a
    ${SFIZZ_LIB_DIR}/libabsl_malloc_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_raw_hash_set.a
    ${SFIZZ_LIB_DIR}/libabsl_raw_logging_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_spinlock_wait.a
    ${SFIZZ_LIB_DIR}/libabsl_stacktrace.a
    ${SFIZZ_LIB_DIR}/libabsl_strings_internal.a
    ${SFIZZ_LIB_DIR}/libabsl_symbolize.a
    ${SFIZZ_LIB_DIR}/libabsl_synchronization.a
    ${SFIZZ_LIB_DIR}/libabsl_throw_delegate.a
    ${SFIZZ_LIB_DIR}/libabsl_time.a
    ${SFIZZ_LIB_DIR}/libabsl_time_zone.a
    ${SFIZZ_LIB_DIR}/libst_audiofile.a
    ${SFIZZ_LIB_DIR}/libst_audiofile_formats.a
    ${SFIZZ_LIB_DIR}/libaiff.a
)
endif()

add_custom_target(
    sfizz_deps
    ALL
    DEPENDS ${SFIZZ_LIBS}
)

add_custom_command(
    OUTPUT ${SFIZZ_LIBS}
    COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/build
    VERBATIM
)

add_library(sfizz STATIC ${CMAKE_CURRENT_SOURCE_DIR}/null.cpp)
target_link_libraries(sfizz PUBLIC ${SFIZZ_LIBS})
target_include_directories(sfizz INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/src)
add_dependencies(sfizz sfizz_deps)
