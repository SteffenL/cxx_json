#include "langnes_json/json.hpp"

#include <iomanip>
#include <iostream>

void indent(size_t level);
bool dump_value(const langnes::json::value& value_to_dump, size_t level);

// Construct an object and save it to JSON.
void example_save() {
    using namespace langnes::json;

    // Create an array and add elements into it dynamically.
    value values{make_array()};
    auto& values_{values.as_array()};
    values_.emplace_back("hello");
    values_.emplace_back(3.14159);
    values_.emplace_back(1);
    values_.emplace_back(nullptr);

    // Initialize an object with members.
    values_.push_back(make_object({
        {"x", value{1}},
        {"y", value{2}},
    }));

    // Initialize an array with elements.
    value array_elements{make_array()};
    auto& array_elements_{values.as_array()};
    array_elements_.emplace_back(1);
    array_elements_.emplace_back(2);

    // Set member value on an object by member name.
    auto object{make_object({})};
    object.as_object()["values"] = std::move(values);

    // Save to JSON.
    auto json_str{save(object)};
    std::cout << json_str << '\n';
}

// Load an object from JSON.
void example_load() {
    using namespace langnes::json;
    constexpr auto* json_str{
        R"({"values":["hello",3.14159,[true,false,null],{"colors":["red","green","blue"]},{},[]]})"};
    auto object{load(json_str)};
    dump_value(object, 0);
}

void indent(size_t level) {
    for (size_t j = 0; j < level * 2; ++j) {
        std::cout.put(' ');
    }
}

// Performs a deep dump of a JSON value with a custom output format.
bool dump_value(const langnes::json::value& value_to_dump, size_t level) {
    using t = langnes::json::value::type;

    if (level > 100) {
        return false;
    }

    auto type{value_to_dump.get_type()};

    switch (type) {
    case t::string: {
        const auto& str{value_to_dump.as_string()};
        std::cout << str;
        break;
    }
    case t::number: {
        auto value{value_to_dump.as_number()};
        std::cout << std::setprecision(6) << value;
        break;
    }
    case t::boolean: {
        auto value{value_to_dump.as_boolean()};
        std::cout << std::boolalpha << value;
        break;
    }
    case t::null:
        std::cout << "null";
        break;
    case t::object: {
        const auto& members{value_to_dump.as_object()};
        if (!members.empty()) {
            std::cout.put('{');
            std::cout.put('\n');
            for (const auto& member : members) {
                indent(level + 1);
                std::cout << member.first << ": ";
                if (!dump_value(member.second, level + 1)) {
                    return false;
                }
                std::cout.put('\n');
            }
            indent(level);
            std::cout.put('}');
        } else {
            std::cout << "{}";
        }
        break;
    }
    case t::array: {
        const auto& elements{value_to_dump.as_array()};
        if (!elements.empty()) {
            std::cout.put('[');
            std::cout.put('\n');
            for (const auto& element : elements) {
                indent(level + 1);
                if (!dump_value(element, level + 1)) {
                    return false;
                }
                std::cout.put('\n');
            }
            indent(level);
            std::cout.put(']');
        } else {
            std::cout << "[]";
        }
        break;
    }
    }
    return true;
}

int main() {
    try {
        example_save();
        example_load();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return 1;
    }
}
