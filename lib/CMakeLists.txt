# Targets should either link this with PRIVATE visibility or not link it at all.
add_library(langnes_json_private INTERFACE)
langnes_json_target_enable_warnings(langnes_json_private INTERFACE)

if(MSVC)
    target_compile_options(langnes_json_private INTERFACE /utf-8)
endif()

#
# Header library
#

add_library(langnes_json_headers INTERFACE)
add_library(langnes::json ALIAS langnes_json_headers)
add_library(langnes::json::headers ALIAS langnes_json_headers)

target_include_directories(
    langnes_json_headers
    INTERFACE
        "$<BUILD_INTERFACE:${LANGNES_JSON_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
set_target_properties(langnes_json_headers PROPERTIES
    EXPORT_NAME library_headers)

target_compile_features(langnes_json_headers INTERFACE cxx_std_11)

#
# Static library
#

add_library(langnes_json_static STATIC src/exports.cpp)
add_library(langnes::json::static ALIAS langnes_json_static)

target_link_libraries(langnes_json_static PUBLIC langnes::json PRIVATE langnes_json_private)
target_compile_definitions(langnes_json_static PUBLIC LANGNES_JSON_STATIC)

# Change .lib file name for MSVC because otherwise it would be the same for shared and static
if(MSVC)
    set(STATIC_LIBRARY_OUTPUT_NAME langnes_json_static)
else()
    set(STATIC_LIBRARY_OUTPUT_NAME langnes_json)
endif()

set_target_properties(langnes_json_static PROPERTIES
    OUTPUT_NAME "${STATIC_LIBRARY_OUTPUT_NAME}"
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME library_static)

#
# Shared library
#

add_library(langnes_json_shared SHARED src/exports.cpp)
add_library(langnes::json::shared ALIAS langnes_json_shared)

target_link_libraries(langnes_json_shared PUBLIC langnes::json PRIVATE langnes_json_private)
target_compile_definitions(langnes_json_shared INTERFACE LANGNES_JSON_SHARED PRIVATE LANGNES_JSON_BUILD_SHARED)

set_target_properties(langnes_json_shared PROPERTIES
    OUTPUT_NAME langnes_json
    VERSION "${LANGNES_JSON_VERSION_NUMBER}"
    SOVERSION "${LANGNES_JSON_VERSION_COMPATIBILITY}"
    EXPORT_NAME library_shared)
