#pragma once

#include "parsing.hpp"

#include <cmath>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace json {

class type_mismatch : public std::runtime_error {
public:
    explicit type_mismatch() : runtime_error{"Type mismatch"} {}
};

enum class value_type {
    object,
    array,
    string,
    number,
    boolean,
    null
};

struct value;
struct string;
struct object;
struct array;
struct number;
struct boolean;

using value_owned_ptr = std::unique_ptr<value>;

struct value {
    value(value_type type) : type{type} {}
    virtual ~value() = default;
    value_type type{};

    const std::string& as_string() const;
    const std::map<std::string, value_owned_ptr>& as_object() const;
    const std::vector<value_owned_ptr>& as_array() const;
    double as_number() const;
    bool as_boolean() const;
    bool is_null() const;
};

struct string : public value {
    string() : value{value_type::string} {}
    string(std::string &&data) : value{value_type::string}, data{std::move(data)} {}
    std::string data;
};

struct object : public value {
    object() : value{value_type::object} {}
    std::map<std::string, value_owned_ptr> members;
};

struct array : public value {
    array() : value{value_type::array} {}
    std::vector<value_owned_ptr> elements;
};

struct number : public value {
    number() : value{value_type::number} {}
    number(double data) : value{value_type::number}, data{data} {}
    double data{};
};

struct boolean : public value {
    boolean() : value{value_type::boolean} {}
    boolean(bool data) : value{value_type::boolean}, data{data} {}
    bool data{};
};

struct null : public value {
    null() : value{value_type::null} {}
};

inline const std::string& value::as_string() const {
    if (type != value_type::string) {
        throw type_mismatch{};
    }
    return dynamic_cast<const string *>(this)->data;
}

inline const std::map<std::string, value_owned_ptr>& value::as_object() const {
    if (type != value_type::object) {
        throw type_mismatch{};
    }
    return dynamic_cast<const object *>(this)->members;
}

inline const std::vector<value_owned_ptr>& value::as_array() const {
    if (type != value_type::array) {
        throw type_mismatch{};
    }
    return dynamic_cast<const array*>(this)->elements;
}

inline double value::as_number() const {
    if (type != value_type::number) {
        throw type_mismatch{};
    }
    return dynamic_cast<const number *>(this)->data;
}

inline bool value::as_boolean() const {
    if (type != value_type::boolean) {
        throw type_mismatch{};
    }
    return dynamic_cast<const boolean *>(this)->data;
}

inline bool value::is_null() const {
    return type == value_type::null;
}

namespace detail {
namespace rules {

#define JSON_PARSING_RULE(name) \
    constexpr inline bool name(std::istream& is, char c)

JSON_PARSING_RULE(eol) { return c == '\r' || c == '\n'; }
JSON_PARSING_RULE(ws) { return c == ' ' || c == '\t' || eol(is, c); }
JSON_PARSING_RULE(dquote) {return c == '"'; }
JSON_PARSING_RULE(digit) { return c >= '0' && c <= '9'; }
JSON_PARSING_RULE(digit_1_through_9) { return c >= '1' && c <= '9'; }
JSON_PARSING_RULE(decimal_point) { return c == '.'; }
JSON_PARSING_RULE(escape) { return c == '\\'; }
JSON_PARSING_RULE(object_open) { return c == '{'; }
JSON_PARSING_RULE(object_close) { return c == '}'; }
JSON_PARSING_RULE(array_open) { return c == '['; }
JSON_PARSING_RULE(array_close) { return c == ']'; }
JSON_PARSING_RULE(value_separator) { return c == ','; }
JSON_PARSING_RULE(member_separator) { return c == ':'; }

} // namespace rules

value_owned_ptr parse_value(std::istream& is);

inline std::optional<std::string> try_parse_string(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    if (!peek(is, dquote)) {
        return std::nullopt;
    }
    skip(is);
    std::string data;
    for (char c{get_next(is)};; c = get_next(is)) {
        if (escape(is, c)) {
            get_next(is);
            continue;
        }
        if (dquote(is, c)) {
            is.unget();
            break;
        }
        data.push_back(c);
    }
    expect(is, dquote);
    return {data};
}

inline std::string parse_string(std::istream& is) {
    using namespace parsing;
    if (auto s{try_parse_string(is)}) {
        return *s;
    }
    throw unexpected_token{};
}

inline std::optional<bool> try_parse_boolean(std::istream& is) {
    using namespace parsing;
    if (peek_next(is) == 't') {
        expect_exact(is, "true");
        return {true};
    }
    if (peek_next(is) == 'f') {
        expect_exact(is, "false");
        return {false};
    }
    return std::nullopt;
}

inline bool parse_boolean(std::istream& is) {
    using namespace parsing;
    if (auto v{try_parse_boolean(is)}) {
        return *v;
    }
    throw unexpected_token{};
}

inline std::optional<double> try_parse_number(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    int number_sign{1};
    if (peek_next(is) == '-') {
        skip(is);
        number_sign = -1;
    }
    std::string number_digits;
    std::string fraction_digits;
    std::string exponent_digits;
    int exponent_sign{1};
    if (peek_next(is) == '0') {
        number_digits.push_back(get_next(is));
    } else {
        char first_digit{};
        if (!next(is, first_digit, digit_1_through_9)) {
            throw unexpected_token{};
        }
        number_digits.push_back(first_digit);
        read_while(is, number_digits, digit);
    }
    if (peek_next(is) == '.') {
        skip(is);
        char first_digit{};
        if (!next(is, first_digit, digit)) {
            throw unexpected_token{};
        }
        fraction_digits.push_back(first_digit);
        read_while(is, fraction_digits, digit);
    }
    auto c{peek_next(is)};
    if (c == 'e' || c == 'E') {
        skip(is);
        c = peek_next(is);
        if (c == '+' || c == '-') {
            skip(is);
            if (c == '-') {
                exponent_sign = -1;
            }
        }
        read_while(is, exponent_digits, digit);
    }
    double number{std::stod(number_digits)};
    if (!fraction_digits.empty()) {
        number += std::stod("0." + fraction_digits);
    }
    if (number_sign < 0) {
        number = -number;
    }
    if (!exponent_digits.empty()) {
        auto exponent{std::stod(exponent_digits)};
        if (exponent_sign < 0) {
            exponent = -exponent;
        }
        number *= std::pow(10, exponent);
    }
    return number;
}

inline double parse_number(std::istream& is) {
    using namespace parsing;
    if (auto v{try_parse_number(is)}) {
        return *v;
    }
    throw unexpected_token{};
}

inline bool try_parse_null(std::istream& is) {
    using namespace parsing;
    if (peek_next(is) == 'n') {
        expect_exact(is, "null");
        return true;
    }
    return false;
}


inline std::optional<object> try_parse_object(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    if (!peek(is, object_open)) {
        return std::nullopt;
    }
    skip(is);
    skip_while(is, ws);
    object result;
    if (peek(is, object_close)) {
        expect(is, object_close);
        return result;
    }
    while (true) {
        if (!peek(is, dquote)) {
            throw unexpected_token{};
        }
        auto member_name{parse_string(is)};
        skip_while(is, ws);
        expect(is, member_separator);
        auto member_value{parse_value(is)};
        result.members.emplace(std::move(member_name), std::move(member_value));
        if (peek(is, value_separator)) {
            skip(is);
            skip_while(is, ws);
            continue;
        }
        break;
    }
    skip_while(is, ws);
    expect(is, object_close);
    return result;
}

inline std::optional<array> try_parse_array(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    if (!peek(is, array_open)) {
        return std::nullopt;
    }
    skip(is);
    skip_while(is, ws);
    array result;
    if (peek(is, array_close)) {
        expect(is, array_close);
        return result;
    }
    while (true) {
        auto element_value{parse_value(is)};
        result.elements.push_back(std::move(element_value));
        if (peek(is, value_separator)) {
            skip(is);
            skip_while(is, ws);
            continue;
        }
        break;
    }
    skip_while(is, ws);
    expect(is, array_close);
    return result;
}

inline value_owned_ptr parse_value(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    skip_while(is, ws);
    if (auto v{try_parse_string(is)}) {
        return std::make_unique<string>(std::move(*v));
    }
    if (auto v{try_parse_object(is)}) {
        return std::make_unique<object>(std::move(*v));
    }
    if (auto v{try_parse_array(is)}) {
        return std::make_unique<array>(std::move(*v));
    }
    if (auto v{try_parse_boolean(is)}) {
        return std::make_unique<boolean>(std::move(*v));
    }
    if (try_parse_null(is)) {
        return std::make_unique<null>();
    }
    if (auto v{try_parse_number(is)}) {
        return std::make_unique<number>(std::move(*v));
    }
    throw unexpected_token{};
}

}

inline value_owned_ptr parse(std::istream& is) {
    using namespace detail;
    return parse_value(is);
}

}
