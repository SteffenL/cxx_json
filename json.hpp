#pragma once

#include "parsing.hpp"

#include <cmath>
#include <map>
#include <memory>
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

constexpr inline bool eol(char c) { return c == '\r' || c == '\n'; }
constexpr inline bool ws(char c) { return c == ' ' || c == '\t' || eol(c); }
constexpr inline bool dquote(char c) {return c == '"'; }
struct digit { constexpr bool operator()(char c) const { return c >= '0' && c <= '9'; } };
struct digit_1_through_9 { constexpr bool operator()(char c) const { return c >= '1' && c <= '9'; } };
struct decimal_point { constexpr bool operator()(char c) const { return c == '.'; } };
inline bool escape(char c) { return c == '\\'; }
struct object_open { constexpr bool operator()(char c) const { return c == '{'; } };
struct object_close { constexpr bool operator()(char c) const { return c == '}'; } };
struct array_open { constexpr bool operator()(char c) const { return c == '['; } };
struct array_close { constexpr bool operator()(char c) const { return c == ']'; } };
struct value_separator { constexpr bool operator()(char c) const { return c == ','; } };
struct member_separator { constexpr bool operator()(char c) const { return c == ':'; } };

value_owned_ptr parse_value(std::istream& is);

inline std::string parse_string(std::istream& is) {
    using namespace parsing;
    std::string data;
    expect(is, dquote);
    for (char c{get_next(is)};; c = get_next(is)) {
        if (escape(c)) {
            get_next(is);
            continue;
        }
        if (dquote(c)) {
            is.unget();
            break;
        }
        data.push_back(c);
    }
    expect(is, dquote);
    return data;
}

inline bool parse_boolean(std::istream& is) {
    using namespace parsing;
    if (peek_next(is) == 't') {
        expect_exact(is, "true");
        return true;
    }
    expect_exact(is, "false");
    return false;
}

inline double parse_number(std::istream& is) {
    using namespace parsing;
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
        if (peek_next(is) == '.') {
            skip(is);
            read_while<digit>(is, fraction_digits);
        }
    } else {
        char first_digit{};
        if (!next<digit_1_through_9>(is, first_digit)) {
            throw unexpected_token{};
        }
        number_digits.push_back(first_digit);
        read_while<digit>(is, number_digits);
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
        read_while<digit>(is, exponent_digits);
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

inline value_owned_ptr parse_string_value(std::istream& is) {
    return std::make_unique<string>(parse_string(is));
}

inline value_owned_ptr parse_boolean_value(std::istream& is) {
    return std::make_unique<boolean>(parse_boolean(is));
}

inline value_owned_ptr parse_number_value(std::istream& is) {
    return std::make_unique<number>(parse_number(is));
}

inline value_owned_ptr parse_null_value(std::istream& is) {
    using namespace parsing;
    expect_exact(is, "null");
    return std::make_unique<null>();
}

inline value_owned_ptr parse_object_value(std::istream& is) {
    using namespace parsing;
    expect<object_open>(is);
    skip_while(is, ws);
    auto result{std::make_unique<object>()};
    if (peek<object_close>(is)) {
        expect<object_close>(is);
        return result;
    }
    while (true) {
        if (!peek(is, dquote)) {
            throw unexpected_token{};
        }
        auto member_name{parse_string(is)};
        skip_while(is, ws);
        expect<member_separator>(is);
        auto member_value{parse_value(is)};
        result->members.emplace(std::move(member_name), std::move(member_value));
        if (peek<value_separator>(is)) {
            skip(is);
            skip_while(is, ws);
            continue;
        }
        break;
    }
    skip_while(is, ws);
    expect<object_close>(is);
    return result;
}

inline value_owned_ptr parse_array_value(std::istream& is) {
    using namespace parsing;
    expect<array_open>(is);
    skip_while(is, ws);
    auto result{std::make_unique<array>()};
    if (peek<array_close>(is)) {
        expect<array_close>(is);
        return result;
    }
    while (true) {
        auto element_value{parse_value(is)};
        result->elements.push_back(std::move(element_value));
        if (peek<value_separator>(is)) {
            skip(is);
            skip_while(is, ws);
            continue;
        }
        break;
    }
    skip_while(is, ws);
    expect<array_close>(is);
    return result;
}

inline value_owned_ptr parse_value(std::istream& is) {
    using namespace parsing;
    skip_while(is, ws);
    auto c{peek_next(is)};
    if (dquote(c)) {
        return parse_string_value(is);
    } else if (peek<object_open>(is)) {
        return parse_object_value(is);
    } else if (peek<array_open>(is)) {
        return parse_array_value(is);
    }
    if (c == 't' || c == 'f') {
        return parse_boolean_value(is);
    }
    if (c == 'n') {
        return parse_null_value(is);
    }
    return parse_number_value(is);
}

}

inline value_owned_ptr parse(std::istream& is) {
    using namespace detail;
    return parse_value(is);
}

}
