cmake_minimum_required(VERSION 3.15)

include(cmake/macros.cmake)
langnes_json_init_pre_project()

project(
    langnes_json
    VERSION ${LANGNES_JSON_VERSION_NUMBER}
    DESCRIPTION "A simple library for working with JSON which offers both a header-only implementation compatible with C++11 and an accompanying C API"
    HOMEPAGE_URL https://github.com/SteffenL/cxx_json
    LANGUAGES C CXX)

langnes_json_init_post_project()

add_subdirectory(private)
add_subdirectory(lib)

if(LANGNES_JSON_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if(LANGNES_JSON_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(LANGNES_JSON_BUILD_TESTS)
    add_subdirectory(test_driver)
    add_subdirectory(tests)
endif()

if(LANGNES_JSON_PACKAGE)
    add_subdirectory(packaging)
endif()
