#
# Headers
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

target_link_libraries(langnes_json_c_static PUBLIC langnes::json::c PRIVATE langnes_json_cxx_headers langnes_json_private)
target_compile_definitions(langnes_json_c_static PUBLIC LANGNES_JSON_STATIC)

# Change .lib file name for MSVC because otherwise it would be the same for shared and static
if(MSVC)
    set(STATIC_LIBRARY_OUTPUT_NAME langnes_json_static)
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

target_link_libraries(langnes_json_c_shared PUBLIC PUBLIC langnes::json::c PRIVATE langnes_json_cxx_headers langnes_json_private)
target_compile_definitions(langnes_json_c_shared INTERFACE LANGNES_JSON_SHARED PRIVATE LANGNES_JSON_BUILD_SHARED)

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
        COMPONENT langnes_json_c_headers)

    install(TARGETS langnes_json_c_headers EXPORT langnes_json_c_targets)

    install(EXPORT langnes_json_c_targets
        DESTINATION "lib/cmake/langnes_json/c"
        NAMESPACE langnes::json::c::
        FILE targets.cmake
        COMPONENT langnes_json_c_cmake)

    install(TARGETS langnes_json_c_shared
        EXPORT langnes_json_c_targets
        RUNTIME
            DESTINATION "bin/$<CONFIG>"
            COMPONENT langnes_json_c_shared_lib
        LIBRARY
            DESTINATION "lib/$<CONFIG>"
            COMPONENT langnes_json_c_shared_lib
            NAMELINK_COMPONENT langnes_json_c_shared_lib_dev
        ARCHIVE
            DESTINATION "lib/$<CONFIG>"
            COMPONENT langnes_json_c_shared_lib_dev
    )

    install(TARGETS langnes_json_c_static
        EXPORT langnes_json_c_targets
        LIBRARY
            DESTINATION "lib/$<CONFIG>"
            COMPONENT langnes_json_c_static_lib_dev
        ARCHIVE
            DESTINATION "lib/$<CONFIG>"
            COMPONENT langnes_json_c_static_lib_dev
    )

    export(EXPORT langnes_json_c_targets FILE "${CMAKE_CURRENT_BINARY_DIR}/targets.cmake")
endif()
