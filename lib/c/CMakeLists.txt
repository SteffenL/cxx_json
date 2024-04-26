# Targets should either link this with PRIVATE visibility or not link it at all.
add_library(langnes_json_c_private INTERFACE)
langnes_json_target_enable_warnings(langnes_json_c_private INTERFACE)

if(MSVC)
    target_compile_options(langnes_json_c_private INTERFACE /utf-8)
endif()

#
# Header library
#

add_library(langnes_json_c_headers INTERFACE)
add_library(langnes::json::c ALIAS langnes_json_c_headers)

target_include_directories(
    langnes_json_c_headers
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/c/include>"
        "$<INSTALL_INTERFACE:include>")
set_target_properties(langnes_json_c_headers PROPERTIES
    EXPORT_NAME headers)

target_compile_features(langnes_json_c_headers INTERFACE c_std_99)

#
# Static library
#

add_library(langnes_json_c_static STATIC src/exports.cpp)
add_library(langnes::json::c::static ALIAS langnes_json_c_static)

target_link_libraries(langnes_json_c_static PUBLIC langnes::json::c PRIVATE langnes::json::cxx langnes_json_c_private)
target_compile_definitions(langnes_json_c_static PUBLIC langnes_json_c_static)

# Change .lib file name for MSVC because otherwise it would be the same for shared and static
if(MSVC)
    set(STATIC_LIBRARY_OUTPUT_NAME langnes_json_c_static)
else()
    set(STATIC_LIBRARY_OUTPUT_NAME langnes_json)
endif()

set_target_properties(langnes_json_c_static PROPERTIES
    OUTPUT_NAME "${STATIC_LIBRARY_OUTPUT_NAME}"
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME static)

#
# Shared library
#

add_library(langnes_json_c_shared SHARED src/exports.cpp)
add_library(langnes::json::c::shared ALIAS langnes_json_c_shared)

target_link_libraries(langnes_json_c_shared PUBLIC PUBLIC langnes::json::c PRIVATE langnes::json::cxx langnes_json_c_private)
target_compile_definitions(langnes_json_c_shared INTERFACE langnes_json_c_shared PRIVATE LANGNES_JSON_BUILD_SHARED)

set_target_properties(langnes_json_c_shared PROPERTIES
    OUTPUT_NAME langnes_json
    VERSION "${LANGNES_JSON_VERSION_NUMBER}"
    SOVERSION "${LANGNES_JSON_VERSION_COMPATIBILITY}"
    EXPORT_NAME shared)

#
# Installation
#

if(LANGNES_JSON_INSTALL)
    install(DIRECTORY "include/langnes_json/c"
        DESTINATION "include/langnes_json"
        COMPONENT langnes_json_c_library_headers)

    install(TARGETS langnes_json_c_headers EXPORT langnes_json_c_targets)
    install(TARGETS langnes_json_c_private EXPORT langnes_json_c_targets)

    install(EXPORT langnes_json_c_targets
        DESTINATION "lib/cmake/langnes_json_c"
        NAMESPACE langnes::json::c::
        FILE langnes_json_c-targets.cmake
        COMPONENT langnes_json_c_library_cmake)

    install(TARGETS langnes_json_c_static langnes_json_c_shared
        EXPORT langnes_json_c_targets
        RUNTIME
            DESTINATION "bin/$<CONFIG>"
            COMPONENT langnes_json_c_library_runtime
        LIBRARY
            DESTINATION "lib/$<CONFIG>"
            COMPONENT langnes_json_c_library_runtime
            NAMELINK_COMPONENT langnes_json_c_library_link
        ARCHIVE
            DESTINATION "lib/$<CONFIG>"
            COMPONENT langnes_json_c_library_link
    )

    export(EXPORT langnes_json_c_targets FILE "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_c-targets.cmake")

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_c-config-version.cmake"
        VERSION "${LANGNES_JSON_VERSION_COMPATIBILITY}"
        COMPATIBILITY SameMinorVersion)

    configure_package_config_file(
        langnes_json_c-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_c-config.cmake"
        INSTALL_DESTINATION "lib/cmake/langnes_json_c"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_c-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/langnes_json_c-config-version.cmake"
        DESTINATION "lib/cmake/langnes_json_c"
        COMPONENT langnes_json_c_library_cmake)
endif()
