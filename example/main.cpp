#include "langnes/json/json.hpp"

#include <iostream>

int main() {
    using namespace langnes;
    std::cout << std::boolalpha;

    constexpr auto json{R"({
    "strings": [
        "hello world",
        "\"\x24\u00a3\u0418\n\t\u0939\u20ac\ud55c\""
    ],
    "numbers": [3.14, 0.314e1],
    "booleans": [true, false],
    "null": null
})"};

    auto root{json::load(json)};

    std::cout << "----- JSON\n";
    std::cout << json::save(root) << "\n";

    std::cout << "----- YAML\n";
    std::cout << json::save(root, json::stored_format::yaml);

    const auto& members{root.as_object()};
    std::cout << "----- Strings\n";
    for (const auto& element : members["strings"].as_array()) {
        std::cout << element.as_string() << "\n";
    }
    std::cout << "----- Numbers\n";
    for (const auto& element : members["numbers"].as_array()) {
        std::cout << element.as_number() << "\n";
    }
    std::cout << "----- Booleans\n";
    for (const auto& element : members["booleans"].as_array()) {
        std::cout << element.as_boolean() << "\n";
    }
    std::cout << "----- Null\n";
    std::cout << "Null: " << members["null"].is_null() << "\n";

    auto& mutable_members{root.as_object()};
    mutable_members["strings"].as_array().clear();
    mutable_members["numbers"].as_array().at(0) = 42;
    mutable_members["booleans"] = json::make_array("yes", "no");
    mutable_members["null"] = nullptr;
    mutable_members["more"] = json::make_object({
        {"colors", json::make_array("red", "blue")},
        {"sizes", json::make_array("small", "medium", "large")}
    });

    std::cout << "----- Modified JSON\n";
    std::cout << json::save(root) << "\n";

    std::cout << "----- Modified YAML\n";
    std::cout << json::save(root, json::stored_format::yaml);

    std::cout << "----- New JSON\n";
    auto new_object{json::make_object({
        {"simple", json::make_array(1, true, nullptr)}
    })};
    std::cout << json::save(new_object) << "\n";

    std::cout << "----- New YAML\n";
    std::cout << json::save(new_object, json::stored_format::yaml);

    return 0;
}
