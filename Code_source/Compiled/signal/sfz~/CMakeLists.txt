
project(sfizz
    DESCRIPTION  "SFZ sampler, providing AU, LV2, PureData and VST plugins."
    HOMEPAGE_URL "http://sfztools.github.io/sfizz"
    LANGUAGES    CXX C
    VERSION      1.2.2
)

# Custom command for running the sfizz build script at compilation time
add_custom_target(sfizz_build
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/sfz~make.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SFIZ_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/build/library/lib)

if(MSVC)
set(SFIZZ_LIBS
    ${SFIZ_LIB_DIR}/libabsl_bad_optional_access.lib
    ${SFIZ_LIB_DIR}/libabsl_hash.lib
    ${SFIZ_LIB_DIR}/libsfizz.lib
    ${SFIZ_LIB_DIR}/libabsl_bad_variant_access.lib
    ${SFIZ_LIB_DIR}/libabsl_hashtablez_sampler.lib
    ${SFIZ_LIB_DIR}/libsfizz_cephes.lib
    ${SFIZ_LIB_DIR}/libabsl_base.lib
    ${SFIZ_LIB_DIR}/libabsl_int128.lib
    ${SFIZ_LIB_DIR}/libsfizz_cpuid.lib
    ${SFIZ_LIB_DIR}/libabsl_city.lib
    ${SFIZ_LIB_DIR}/libabsl_log_severity.lib
    ${SFIZ_LIB_DIR}/libsfizz_filesystem_impl.lib
    ${SFIZ_LIB_DIR}/libabsl_civil_time.lib
    ${SFIZ_LIB_DIR}/libabsl_low_level_hash.lib
    ${SFIZ_LIB_DIR}/libsfizz_fmidi.lib
    ${SFIZ_LIB_DIR}/libabsl_cord.lib
    ${SFIZ_LIB_DIR}/libabsl_malloc_internal.lib
    ${SFIZ_LIB_DIR}/libsfizz_hiir_polyphase_iir2designer.lib
    ${SFIZ_LIB_DIR}/libabsl_cord_internal.lib
    ${SFIZ_LIB_DIR}/libabsl_raw_hash_set.lib
    ${SFIZ_LIB_DIR}/libsfizz_import.lib
    ${SFIZ_LIB_DIR}/libabsl_cordz_functions.lib
    ${SFIZ_LIB_DIR}/libabsl_raw_logging_internal.lib
    ${SFIZ_LIB_DIR}/libsfizz_internal.lib
    ${SFIZ_LIB_DIR}/libabsl_cordz_handle.lib
    ${SFIZ_LIB_DIR}/libabsl_spinlock_wait.lib
    ${SFIZ_LIB_DIR}/libsfizz_kissfft.lib
    ${SFIZ_LIB_DIR}/libabsl_cordz_info.lib
    ${SFIZ_LIB_DIR}/libabsl_stacktrace.lib
    ${SFIZ_LIB_DIR}/libsfizz_messaging.lib
    ${SFIZ_LIB_DIR}/libabsl_crc32c.lib
    ${SFIZ_LIB_DIR}/libabsl_strings.lib
    ${SFIZ_LIB_DIR}/libsfizz_parser.lib
    ${SFIZ_LIB_DIR}/libabsl_crc_cord_state.lib
    ${SFIZ_LIB_DIR}/libabsl_strings_internal.lib
    ${SFIZ_LIB_DIR}/libsfizz_pugixml.lib
    ${SFIZ_LIB_DIR}/libabsl_crc_cpu_detect.lib
    ${SFIZ_LIB_DIR}/libabsl_symbolize.lib
    ${SFIZ_LIB_DIR}/libsfizz_spin_mutex.lib
    ${SFIZ_LIB_DIR}/libabsl_crc_internal.lib
    ${SFIZ_LIB_DIR}/libabsl_synchronization.lib
    ${SFIZ_LIB_DIR}/libsfizz_spline.lib
    ${SFIZ_LIB_DIR}/libabsl_debugging_internal.lib
    ${SFIZ_LIB_DIR}/libabsl_throw_delegate.lib
    ${SFIZ_LIB_DIR}/libsfizz_tunings.lib
    ${SFIZ_LIB_DIR}/libabsl_demangle_internal.lib
    ${SFIZ_LIB_DIR}/libabsl_time.lib
    ${SFIZ_LIB_DIR}/libst_audiofile.lib
    ${SFIZ_LIB_DIR}/libabsl_exponential_biased.lib
    ${SFIZ_LIB_DIR}/libabsl_time_zone.lib
    ${SFIZ_LIB_DIR}/libst_audiofile_formats.lib
    ${SFIZ_LIB_DIR}/libabsl_graphcycles_internal.lib
    ${SFIZ_LIB_DIR}/libaiff.lib
)
else()
set(SFIZZ_LIBS
    ${SFIZ_LIB_DIR}/libabsl_bad_optional_access.a
    ${SFIZ_LIB_DIR}/libabsl_hash.a
    ${SFIZ_LIB_DIR}/libsfizz.a
    ${SFIZ_LIB_DIR}/libabsl_bad_variant_access.a
    ${SFIZ_LIB_DIR}/libabsl_hashtablez_sampler.a
    ${SFIZ_LIB_DIR}/libsfizz_cephes.a
    ${SFIZ_LIB_DIR}/libabsl_base.a
    ${SFIZ_LIB_DIR}/libabsl_int128.a
    ${SFIZ_LIB_DIR}/libsfizz_cpuid.a
    ${SFIZ_LIB_DIR}/libabsl_city.a
    ${SFIZ_LIB_DIR}/libabsl_log_severity.a
    ${SFIZ_LIB_DIR}/libsfizz_filesystem_impl.a
    ${SFIZ_LIB_DIR}/libabsl_civil_time.a
    ${SFIZ_LIB_DIR}/libabsl_low_level_hash.a
    ${SFIZ_LIB_DIR}/libsfizz_fmidi.a
    ${SFIZ_LIB_DIR}/libabsl_cord.a
    ${SFIZ_LIB_DIR}/libabsl_malloc_internal.a
    ${SFIZ_LIB_DIR}/libsfizz_hiir_polyphase_iir2designer.a
    ${SFIZ_LIB_DIR}/libabsl_cord_internal.a
    ${SFIZ_LIB_DIR}/libabsl_raw_hash_set.a
    ${SFIZ_LIB_DIR}/libsfizz_import.a
    ${SFIZ_LIB_DIR}/libabsl_cordz_functions.a
    ${SFIZ_LIB_DIR}/libabsl_raw_logging_internal.a
    ${SFIZ_LIB_DIR}/libsfizz_internal.a
    ${SFIZ_LIB_DIR}/libabsl_cordz_handle.a
    ${SFIZ_LIB_DIR}/libabsl_spinlock_wait.a
    ${SFIZ_LIB_DIR}/libsfizz_kissfft.a
    ${SFIZ_LIB_DIR}/libabsl_cordz_info.a
    ${SFIZ_LIB_DIR}/libabsl_stacktrace.a
    ${SFIZ_LIB_DIR}/libsfizz_messaging.a
    ${SFIZ_LIB_DIR}/libabsl_crc32c.a
    ${SFIZ_LIB_DIR}/libabsl_strings.a
    ${SFIZ_LIB_DIR}/libsfizz_parser.a
    ${SFIZ_LIB_DIR}/libabsl_crc_cord_state.a
    ${SFIZ_LIB_DIR}/libabsl_strings_internal.a
    ${SFIZ_LIB_DIR}/libsfizz_pugixml.a
    ${SFIZ_LIB_DIR}/libabsl_crc_cpu_detect.a
    ${SFIZ_LIB_DIR}/libabsl_symbolize.a
    ${SFIZ_LIB_DIR}/libsfizz_spin_mutex.a
    ${SFIZ_LIB_DIR}/libabsl_crc_internal.a
    ${SFIZ_LIB_DIR}/libabsl_synchronization.a
    ${SFIZ_LIB_DIR}/libsfizz_spline.a
    ${SFIZ_LIB_DIR}/libabsl_debugging_internal.a
    ${SFIZ_LIB_DIR}/libabsl_throw_delegate.a
    ${SFIZ_LIB_DIR}/libsfizz_tunings.a
    ${SFIZ_LIB_DIR}/libabsl_demangle_internal.a
    ${SFIZ_LIB_DIR}/libabsl_time.a
    ${SFIZ_LIB_DIR}/libst_audiofile.a
    ${SFIZ_LIB_DIR}/libabsl_exponential_biased.a
    ${SFIZ_LIB_DIR}/libabsl_time_zone.a
    ${SFIZ_LIB_DIR}/libst_audiofile_formats.a
    ${SFIZ_LIB_DIR}/libabsl_graphcycles_internal.a
    ${SFIZ_LIB_DIR}/libaiff.a
)
endif()

# Create an interface target for the external libraries
add_library(sfizz INTERFACE)

# Link the .a files to the interface target
target_link_libraries(sfizz INTERFACE ${SFIZZ_LIBS})

add_dependencies(sfizz sfizz_build)

target_include_directories(sfizz INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/sfizz/library/src)