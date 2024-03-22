#include "json.hpp"

#include <stdexcept>
#include <iostream>
#include <sstream>

void dump_as_yaml(std::ostream& os, const json::value& v, size_t indent_level) {
    using t = json::value::type;
    auto indent = [] (size_t level) { return std::string(level * 2, ' '); };
    switch (v.get_type()) {
        case t::object:
            for (const auto& kv : v.as_object()) {
                os << indent(indent_level) << kv.first <<  ": ";
                auto type{kv.second.get_type()};
                if (type == t::object || type == t::array) {
                    os << "\n";
                    dump_as_yaml(os, kv.second, indent_level + 1);
                } else {
                    dump_as_yaml(os, kv.second, 0);
                }
            }
            break;
        case t::array:
            for (const auto& element : v.as_array()) {
                os << indent(indent_level) << "- ";
                auto type{element.get_type()};
                if (type == t::object || type == t::array) {
                    dump_as_yaml(os, element, indent_level + 1);
                } else {
                    dump_as_yaml(os, element, 0);
                }
            }
            break;
        case t::string:
            os << indent(indent_level) << v.as_string() << "\n";
            break;
        case t::boolean:
            os << indent(indent_level) << (v.as_boolean() ? "true" : "false") << "\n";
            break;
        case t::null:
            os << indent(indent_level) << "null\n";
            break;
        case t::number:
            os << indent(indent_level) << v.as_number() << "\n";
            break;
        default:
            throw std::invalid_argument{"Invalid value type"};
    }
}

void dump_as_yaml(std::ostream& os, const json::value& v) {
    dump_as_yaml(os, v, 0);
}

std::string dump_as_yaml(const json::value& v) {
    std::ostringstream os;
    dump_as_yaml(os, v);
    return os.str();
}

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
    std::cout << dump_as_yaml(root) << "\n";
    std::cout << "----------------\n";
    const auto& members{root.as_object()};
    std::cout << "name: " << members["name"].as_string() << "\n";
    std::cout << "alias: " << (members["alias"].is_null() ? "null" : "?") << "\n";
    std::cout << "age: " << members["age"].as_number() << "\n";
    std::cout << "city: " << members["city"].as_string() << "\n";
    std::cout << "policies:\n";
    for (const auto& element : members["policies"].as_array()) {
        std::cout << "  - " << element.as_string() << "\n";
    }
    std::cout << "appearance:\n";
    const auto& appearance{members["appearance"].as_object()};
    std::cout << "  tall: " << std::boolalpha << appearance["tall"].as_boolean() << "\n";
    std::cout << "  short: " << std::boolalpha << appearance["short"].as_boolean() << "\n";
    std::cout << std::flush;
    return 0;
}
