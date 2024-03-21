#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <stack>

enum class JsonNodeType {
    indeterminate,
    null,
    array,
    object,
    string,
    number
};

enum class JsonState {
    start,
    end,
    expect_value_start,
    in_object,
    in_array,
    in_string,
    in_number,
    in_boolean,
    in_null,
    read_value,
    expect_object_key_start
};

enum struct JsonLocation {
    size_t line{};
    size_t col{};
    size_t length{};
};

struct JsonNode {
    JsonNodeType type{JsonNodeType::indeterminate};
    JsonLocation start;
    JsonLocation end;
    std::optional<std::string> value;
};

void parse_json(const std::string& input, std::vector<JsonNode>& nodes) {
    std::stack<JsonState> stack;
    stack.push(JsonState::start);
    JsonNode node;

    std::string value;

    for (size_t i{}; i < input.size(); ++i) {
        const auto c{input[i]};
        const auto state{stack.top()};
        switch(state) {
            case JsonState::start:
                stack.push(JsonState::expect_value_start);
                break;
            case JsonState::end:
                stack.pop();
                break;
            case JsonState::expect_value_start:
                if (c == '{') {
                    std::cout << "start of object" << std::endl;
                    stack.push(JsonState::in_object);
                } else if (c == '[') {
                    std::cout << "start of array" << std::endl;
                    stack.push(JsonState::in_array);
                } else if (c == '"') {
                    std::cout << "start of string" << std::endl;
                    value.clear();
                    stack.push(JsonState::in_string);
                } else if (c >= '0' && c <= '9') {
                    std::cout << "start of number" << std::endl;
                    value = c;
                    stack.push(JsonState::in_number);
                } else if (c == 't' || c == 'f') {
                    std::cout << "start of boolean" << std::endl;
                    value = c;
                    stack.push(JsonState::in_boolean);
                } else if (c == 'n') {
                    std::cout << "start of null" << std::endl;
                    value = c;
                    stack.push(JsonState::in_null);
                } else {
                    throw std::runtime_error{"Unexpected token at " + std::to_string(i)};
                }
                break;
            case JsonState::in_object:
                if (c == '}') {
                    std::cout << "end of object" << std::endl;
                    stack.pop();
                } else if (c == '"') {
                    std::cout << "start of key" << std::endl;
                    value.clear();
                    stack.push(JsonState::in_object_key);
                break;
            case JsonState::in_object_key:
                if (c == '}') {
                    std::cout << "end of object" << std::endl;
                    stack.pop();
                } else if (c == '"') {
                    std::cout << "start of key" << std::endl;
                    value.clear();
                    stack.push(JsonState::);
                break;
        }
    }
}

int main() {
    std::string json{R"({"name": "John", "age": 30, "city": "New York"})"};

    std::vector<JsonNode> nodes;
    parse_json(json, nodes);

    for (const auto& node : nodes) {
        /*if (node.type == '"') {
            std::cout << json.substr(node.start_line, node.end_line - node.start_line);
        }*/
    }

    return 0;
}
