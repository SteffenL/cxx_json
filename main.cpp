#include "parser.hpp"
#include <iostream>
#include <sstream>

int main() {
    //std::stringstream json{R"({"name": "John", "age": 30, "city": "New York"})"};
    std::stringstream json{R"({"a": "1", "b": "2"})"};
    auto root{json::parse(json)};
    json::parse_at(root.location, json);
    /*
    for (const auto& member : root.members) {
        auto name{json::get_data(member.name.value, json)};
        auto value{json::get_data(member.value, json)};
        std::cout << name << " = " << value << std::endl;
    }*/

    return 0;
}
