#include "parser.hpp"
#include <iostream>
#include <sstream>

int main() {
    //std::stringstream json{R"({"name": "John", "age": 30, "city": "New York"})"};
    std::stringstream json{R"({"a": "1", "b": "2"})"};
    json::parse(json);

    return 0;
}
