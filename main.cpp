#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
    //std::ifstream is{argv[1], std::ios::in | std::ios::binary};
    std::stringstream json{R"({
    "name": "John",
    "age": 3e1,
    "alias": null,
    "appearance": {
        "tall": true,
        "short": false
    },
    "city": "Isehara",
    "policies": [
        "do no harm",
        "do good"
    ]
})"};
    auto root{json::parse(json)};
    const auto& members{root->as_object()};
    std::cout << "name: " << members.at("name")->as_string() << "\n";
    std::cout << "alias: " << (members.at("alias")->is_null() ? "(null)" : "?") << "\n";
    std::cout << "age: " << std::to_string(members.at("age")->as_number()) << "\n";
    std::cout << "city: " << members.at("city")->as_string() << "\n";
    std::cout << "policies:\n";
    for (const auto& element : members.at("policies")->as_array()) {
        std::cout << "  - " << element->as_string() << "\n";
    }
    std::cout << "appearance:\n";
    const auto& appearance{members.at("appearance")->as_object()};
    std::cout << "  tall: " << (appearance.at("tall")->as_boolean() ? "yes" : "no") << "\n";
    std::cout << "  short: " << (appearance.at("short")->as_boolean() ? "yes" : "no") << "\n";
    std::cout << std::flush;
    return 0;
}
