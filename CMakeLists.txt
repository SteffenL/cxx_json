cmake_minimum_required(VERSION 3.15)
project(langnes_json)

if(NOT MSVC)
    add_compile_options(-Werror -Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(langnes_json INTERFACE)
add_library(langnes::json ALIAS langnes_json)

target_include_directories(langnes_json INTERFACE include)
target_compile_features(langnes_json INTERFACE cxx_std_11)

add_subdirectory(example)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(CTest)
    add_test(NAME example COMMAND example)
endif()
