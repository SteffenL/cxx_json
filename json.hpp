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

template<typename Key, typename Value>
class dict {
    using container = std::map<Key, Value>;
    using const_iterator = typename container::const_iterator;
    using iterator = typename container::iterator;
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<const Key, Value>;

public:
    const Value& operator[](const std::string& key) const {
        return m_data.at(key);
    }

    Value& operator[](const std::string& key) { return m_data.at(key); }

    size_t size() const noexcept { return m_data.size(); }
    iterator begin() noexcept { return m_data.begin(); }
    const_iterator begin() const noexcept { return m_data.begin(); }
    const_iterator cbegin() const noexcept { return m_data.cbegin(); }
    iterator end() noexcept { return m_data.end(); }
    const_iterator end() const noexcept { return m_data.end(); }
    const_iterator cend() const noexcept { return m_data.cend(); }
    bool empty() const noexcept { return m_data.empty(); }
    size_t count(const std::string& key) const { return m_data.count(key); }

    template<typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
        return m_data.emplace(std::forward<Args>(args)...);
    }

private:
    container m_data;
};

namespace detail {

struct value_impl_base;

class value {
public:
    enum class type {
        object,
        array,
        string,
        number,
        boolean,
        null
    };

    value(std::unique_ptr<value_impl_base> &&impl) : m_impl{std::move(impl)} {}

    const std::string& as_string() const;
    const dict<std::string, value>& as_object() const;
    const std::vector<value>& as_array() const;
    double as_number() const;
    bool as_boolean() const;
    bool is_null() const;

    std::string& as_string();
    dict<std::string, value>& as_object();
    std::vector<value>& as_array();
    double as_number();
    bool as_boolean();
    bool is_null();

    type get_type() const;

private:
    std::unique_ptr<value_impl_base> m_impl;
};

struct value_impl_base {
    value_impl_base(value::type type) : type{type} {}
    virtual ~value_impl_base() = default;
    value::type type{};
};

struct string_impl : public value_impl_base {
    string_impl() : value_impl_base{value::type::string} {}
    string_impl(std::string &&data) : value_impl_base{value::type::string}, data{std::move(data)} {}
    std::string data;
};

struct object_impl : public value_impl_base {
    object_impl() : value_impl_base{value::type::object} {}
    dict<std::string, value> members;
};

struct array_impl : public value_impl_base {
    array_impl() : value_impl_base{value::type::array} {}
    std::vector<value> elements;
};

struct number_impl : public value_impl_base {
    number_impl() : value_impl_base{value::type::number} {}
    number_impl(double data) : value_impl_base{value::type::number}, data{data} {}
    double data{};
};

struct boolean_impl : public value_impl_base {
    boolean_impl() : value_impl_base{value::type::boolean} {}
    boolean_impl(bool data) : value_impl_base{value::type::boolean}, data{data} {}
    bool data{};
};

struct null : public value_impl_base {
    null() : value_impl_base{value::type::null} {}
};

inline const std::string& value::as_string() const {
    if (m_impl->type != value::type::string) {
        throw type_mismatch{};
    }
    return static_cast<const string_impl *>(m_impl.get())->data;
}

inline const dict<std::string, value>& value::as_object() const {
    if (m_impl->type != value::type::object) {
        throw type_mismatch{};
    }
    return static_cast<const object_impl *>(m_impl.get())->members;
}

inline const std::vector<value>& value::as_array() const {
    if (m_impl->type != value::type::array) {
        throw type_mismatch{};
    }
    return static_cast<const array_impl*>(m_impl.get())->elements;
}

inline double value::as_number() const {
    if (m_impl->type != value::type::number) {
        throw type_mismatch{};
    }
    return static_cast<const number_impl *>(m_impl.get())->data;
}

inline bool value::as_boolean() const {
    if (m_impl->type != value::type::boolean) {
        throw type_mismatch{};
    }
    return static_cast<const boolean_impl *>(m_impl.get())->data;
}

inline bool value::is_null() const {
    return m_impl->type == value::type::null;
}

inline std::string& value::as_string() {
    if (m_impl->type != value::type::string) {
        throw type_mismatch{};
    }
    return static_cast<string_impl *>(m_impl.get())->data;
}

inline dict<std::string, value>& value::as_object() {
    if (m_impl->type != value::type::object) {
        throw type_mismatch{};
    }
    return static_cast<object_impl *>(m_impl.get())->members;
}

inline std::vector<value>& value::as_array() {
    if (m_impl->type != value::type::array) {
        throw type_mismatch{};
    }
    return static_cast<array_impl*>(m_impl.get())->elements;
}

inline double value::as_number() {
    if (m_impl->type != value::type::number) {
        throw type_mismatch{};
    }
    return static_cast<number_impl *>(m_impl.get())->data;
}

inline bool value::as_boolean() {
    if (m_impl->type != value::type::boolean) {
        throw type_mismatch{};
    }
    return static_cast<boolean_impl *>(m_impl.get())->data;
}

inline bool value::is_null() {
    return m_impl->type == value::type::null;
}

inline value::type value::get_type() const { return m_impl->type; }

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

value parse_value(std::istream& is);

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


inline std::optional<object_impl> try_parse_object(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    if (!peek(is, object_open)) {
        return std::nullopt;
    }
    skip(is);
    skip_while(is, ws);
    object_impl result;
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

inline std::optional<array_impl> try_parse_array(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    if (!peek(is, array_open)) {
        return std::nullopt;
    }
    skip(is);
    skip_while(is, ws);
    array_impl result;
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

inline value parse_value(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    skip_while(is, ws);
    if (auto v{try_parse_string(is)}) {
        return {std::make_unique<string_impl>(std::move(*v))};
    }
    if (auto v{try_parse_object(is)}) {
        return {std::make_unique<object_impl>(std::move(*v))};
    }
    if (auto v{try_parse_array(is)}) {
        return {std::make_unique<array_impl>(std::move(*v))};
    }
    if (auto v{try_parse_boolean(is)}) {
        return {std::make_unique<boolean_impl>(std::move(*v))};
    }
    if (try_parse_null(is)) {
        return {std::make_unique<null>()};
    }
    if (auto v{try_parse_number(is)}) {
        return {std::make_unique<number_impl>(std::move(*v))};
    }
    throw unexpected_token{};
}

} // namespace detail

using value = detail::value;

inline value parse(std::istream& is) {
    return detail::parse_value(is);
}

}
