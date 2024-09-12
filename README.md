# cxx_json

A simple library for working with JSON which offers both a header-only implementation compatible with C++11 and an accompanying C API.

## Requirements

* Compiler with support for C++11 or newer.

## CMake Options

Name                            | Description
----                            | -----------
`LANGNES_JSON_BUILD_EXAMPLES`   | Build examples.
`LANGNES_JSON_BUILD_TESTS`      | Build tests.
`LANGNES_JSON_ENABLE_PACKAGING` | Enable packaging.
`LANGNES_JSON_INSTALL_TARGETS`  | Install targets.

## CMake Targets

Name                   | Description
----                   | -----------
`langnes::json`        | Header library (C++).
`langnes::json_shared` | Shared library (C).
`langnes::json_static` | Static library (C).

## Examples

See examples for C++ and C in the `examples` directory.

Here are some minimal code snippets.

`CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.15)
project(example LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    langnes_json
    GIT_REPOSITORY https://github.com/SteffenL/cxx_json.git
    GIT_TAG master
)
FetchContent_MakeAvailable(langnes_json)

add_executable(example_cxx main.cpp)
target_link_libraries(example_cxx PRIVATE langnes::json)

add_executable(example_c main.c)
target_link_libraries(example_c PRIVATE langnes::json_static)
```

`main.cpp`:
```cpp
#include "langnes_json/json.hpp"
#include <iostream>

int main() {
    try {
        using namespace langnes::json;
        auto document{load(R"({"color":"red"})")};
        auto& object{document.as_object()};
        std::cout << object["color"].as_string() << "\n"; // red
        object["color"] = "blue";
        object["size"] = 10;
        std::cout << save(document) << "\n"; // {"size":10,"color":"blue"}
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
}
```

`main.c`:
```c
#include "langnes_json/json.h"
#include <stdio.h>

int main() {
    langnes_json_value_t* object = NULL;
    langnes_json_load_from_cstring("{\"color\":\"red\"}", &object);

    langnes_json_value_t* color =
        langnes_json_value_object_get_value_s(object, "color");
    printf("%s\n", langnes_json_value_get_cstring_s(color)); // red

    langnes_json_value_set_cstring(color, "blue");

    langnes_json_value_t* size = langnes_json_value_number_new_s(10);
    langnes_json_value_object_set_value(object, "size", size);

    langnes_json_string_t* json_str = NULL;
    langnes_json_save_to_string(object, &json_str);
    printf("%s\n", langnes_json_string_get_cstring_s(
                       json_str)); // {"size":10,"color":"blue"}
    langnes_json_string_free(json_str);

    langnes_json_value_free(object);
    return 0;
}
```

Build:
```
cmake -G Ninja -B build -S .
cmake --build build
```
