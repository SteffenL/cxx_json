#include "json.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
    std::stringstream json{R"({
    "name": "John",
    "age": 3.5e1,
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
    const auto& members{root.as_object()};
    std::cout << "name: " << members["name"].as_string() << "\n";
    std::cout << "alias: " << (members["alias"].is_null() ? "(null)" : "?") << "\n";
    std::cout << "age: " << std::to_string(members["age"].as_number()) << "\n";
    std::cout << "city: " << members["city"].as_string() << "\n";
    std::cout << "policies:\n";
    for (const auto& element : members["policies"].as_array()) {
        std::cout << "  - " << element.as_string() << "\n";
    }
    std::cout << "appearance:\n";
    const auto& appearance{members["appearance"].as_object()};
    std::cout << "  tall: " << (appearance["tall"].as_boolean() ? "yes" : "no") << "\n";
    std::cout << "  short: " << (appearance["short"].as_boolean() ? "yes" : "no") << "\n";
    std::cout << std::flush;
    return 0;
}
