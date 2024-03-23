#include "langnes/serialization/json/json.hpp"

#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
    std::istringstream json{R"({
    "name": "John\u2753\x24\u00a3\u0418\u0939\u20ac\ud55c",
    "age": 3.5e1,
    "alias": null,
    "escaped": "\b\t\n\f\r\\\"",
    "appearance": {
        "tall": true,
        "short": false
    },
    "city": "Tromsø",
    "mottos": [
        "don't be evil",
        "be good"
    ]
})", std::ios::binary};
    using namespace langnes::serialization;
    auto root{json::load(json)};
    std::cout << "----- JSON -----\n";
    std::cout << json::save(root) << "\n";
    std::cout << "----- YAML -----\n";
    std::cout << json::save(root, json::stored_format::yaml);
    std::cout << "--- Explicit ---\n";
    const auto& members{root.as_object()};
    std::cout << "name: " << members["name"].as_string() << "\n";
    std::cout << "alias: " << (members["alias"].is_null() ? "null" : "?") << "\n";
    std::cout << "escaped: " << members["escaped"].as_string() << "\n";
    std::cout << "age: " << members["age"].as_number() << "\n";
    std::cout << "city: " << members["city"].as_string() << "\n";
    std::cout << "mottos:\n";
    for (const auto& element : members["mottos"].as_array()) {
        std::cout << "  - " << element.as_string() << "\n";
    }
    std::cout << "appearance:\n";
    const auto& appearance{members["appearance"].as_object()};
    std::cout << "  tall: " << std::boolalpha << appearance["tall"].as_boolean() << "\n";
    std::cout << "  short: " << std::boolalpha << appearance["short"].as_boolean() << "\n";
    std::cout << std::flush;
    std::cout << "---- Modify ----\n";
    auto& mutable_members{root.as_object()};
    mutable_members.clear();
    mutable_members.emplace("name", json::value{"Jane"});
    std::cout << json::save(root, json::stored_format::yaml);
    return 0;
}
