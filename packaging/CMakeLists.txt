set(CPACK_PACKAGE_NAME "liblangnes_json")
set(CPACK_PACKAGE_VENDOR "Langnes")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_CONTACT "${CPACK_PACKAGE_VENDOR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/../LICENSE")
set(CPACK_VERBATIM_VARIABLES TRUE)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_OUTPUT_FILE_PREFIX dist)

string(REGEX REPLACE "(.)" "\\\\\\1" ESCAPED_BINARY_DIR "${CMAKE_BINARY_DIR}")
set(CPACK_SOURCE_IGNORE_FILES
    "\\.git/"
    "${ESCAPED_BINARY_DIR}/"
)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(CPACK_GENERATOR TGZ ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()

set(CPACK_SOURCE_GENERATOR TGZ)

string(TOLOWER "${CMAKE_SYSTEM_NAME}" CMAKE_SYSTEM_NAME_LC)
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" CMAKE_SYSTEM_PROCESSOR_LC)
string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" CMAKE_CXX_COMPILER_ID_LC)

if(MSVC AND DEFINED MSVC_TOOLSET_VERSION)
    set(COMPILER_VERSION "${MSVC_TOOLSET_VERSION}")
else()
    string(REPLACE "." ";" COMPILER_VERSION_LIST "${CMAKE_CXX_COMPILER_VERSION}")
    list(GET COMPILER_VERSION_LIST 0 COMPILER_VERSION_MAJOR)
    list(GET COMPILER_VERSION_LIST 1 COMPILER_VERSION_MINOR)
    if(COMPILER_VERSION_MINOR GREATER 0)
        set(COMPILER_VERSION "${COMPILER_VERSION_MAJOR}.${COMPILER_VERSION_MINOR}")
    else()
        set(COMPILER_VERSION "${COMPILER_VERSION_MAJOR}")
    endif()
endif()

set(COMPILER "${CMAKE_CXX_COMPILER_ID_LC}-${COMPILER_VERSION}")

if(APPLE AND CMAKE_OSX_ARCHITECTURES MATCHES ";")
    set(PACKAGE_NAME_ARCH universal)
else()
    set(PACKAGE_NAME_ARCH ${CMAKE_SYSTEM_PROCESSOR_LC})
endif()

if(CMAKE_CXX_STANDARD)
    set(CXX_STANDARD "cxx${CMAKE_CXX_STANDARD}")
endif()

# Package file names
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")
set(CPACK_ARCHIVE_RUNTIME_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-bin-${CMAKE_SYSTEM_NAME_LC}-${PACKAGE_NAME_ARCH}-${COMPILER}-${CXX_STANDARD}")
set(CPACK_ARCHIVE_LINK_LIBRARIES_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-dev-${CMAKE_SYSTEM_NAME_LC}-${PACKAGE_NAME_ARCH}-${COMPILER}-${CXX_STANDARD}")
set(CPACK_ARCHIVE_DEVELOPMENT_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-dev")

include(CPack)

#
# Components
#

cpack_add_component(langnes_json_library_runtime
    DISPLAY_NAME "Library runtime"
    DESCRIPTION "Shared libraries"
    GROUP runtime
    INSTALL_TYPES full development minimal)

cpack_add_component(langnes_json_library_link
    DISPLAY_NAME "Link libraries"
    DESCRIPTION "Static- and import libraries"
    GROUP link_libraries
    DEPENDS langnes_json_library_headers
    INSTALL_TYPES full development)

cpack_add_component(langnes_json_library_headers
    DISPLAY_NAME "Library headers"
    DESCRIPTION "Library header files"
    GROUP development
    INSTALL_TYPES full development)

cpack_add_component(langnes_json_library_docs
    DISPLAY_NAME "Library documentation"
    DESCRIPTION "Library API documentation"
    GROUP development
    INSTALL_TYPES full development)

cpack_add_component(langnes_json_library_cmake
    DISPLAY_NAME "Library CMake files"
    DESCRIPTION "Library integration for CMake"
    GROUP development
    INSTALL_TYPES full development)

cpack_add_component_group(runtime
    DISPLAY_NAME "Runtime"
    DESCRIPTION "Runtime components")

cpack_add_component_group(development
    DISPLAY_NAME "Development"
    DESCRIPTION "Development components")

cpack_add_component_group(link_libraries
    DISPLAY_NAME "Runtime"
    DESCRIPTION "Link-time components"
    PARENT_GROUP development)

cpack_add_install_type(full DISPLAY_NAME "Everything")
cpack_add_install_type(development DISPLAY_NAME "Development")
cpack_add_install_type(minimal DISPLAY_NAME "Minimal")
