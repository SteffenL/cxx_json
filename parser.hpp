#pragma once

#include <cmath>
#include <istream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace json {

class reached_end : public std::runtime_error {
public:
    explicit reached_end()
        : runtime_error{"Reached end of file"} {}
};

class unexpected_token : public std::runtime_error {
public:
    explicit unexpected_token()
        : runtime_error{"Found unexpected token"} {}
};

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

struct eol { constexpr bool operator()(char c) const { return c == '\r' || c == '\n'; } };
struct ws { constexpr bool operator()(char c) const { return c == ' ' || c == '\t' || eol{}(c); }; };
struct dquote { constexpr bool operator()(char c) const { return c == '"'; } };
struct digit { constexpr bool operator()(char c) const { return c >= '0' && c <= '9'; } };
struct digit_1_through_9 { constexpr bool operator()(char c) const { return c >= '1' && c <= '9'; } };
struct decimal_point { constexpr bool operator()(char c) const { return c == '.'; } };
struct escape { constexpr bool operator()(char c) const { return c == '\\'; } };
struct object_open { constexpr bool operator()(char c) const { return c == '{'; } };
struct object_close { constexpr bool operator()(char c) const { return c == '}'; } };
struct array_open { constexpr bool operator()(char c) const { return c == '['; } };
struct array_close { constexpr bool operator()(char c) const { return c == ']'; } };
struct value_separator { constexpr bool operator()(char c) const { return c == ','; } };
struct member_separator { constexpr bool operator()(char c) const { return c == ':'; } };

template<typename What>
struct negate {
    constexpr bool operator()(char c) const {
        return !What{}(c);
    }
};

inline char peek_next(std::istream& is) {
    using std_traits = std::istream::traits_type;
    auto i{is.peek()};
    if (std_traits::eq_int_type(i, std_traits::eof())) {
        throw reached_end{};
    }
    return std_traits::to_char_type(i);
}

inline char get_next(std::istream& is) {
    using std_traits = std::istream::traits_type;
    auto i{is.get()};
    if (std_traits::eq_int_type(i, std_traits::eof())) {
        throw reached_end{};
    }
    return std_traits::to_char_type(i);
}

template<typename Predicate>
bool peek(std::istream& is) {
    auto c{peek_next(is)};
    return Predicate{}(c);
}

inline void skip(std::istream& is) {
    get_next(is);
}

template<typename Predicate>
bool next(std::istream& is, char& c) {
    return Predicate{}(c = get_next(is));
}

template<typename Predicate>
bool next(std::istream& is, char& c, Predicate predicate) {
    return predicate(c = get_next(is));
}

template<typename Predicate>
bool next(std::istream& is) {
    char c;
    return next<Predicate>(is, c);
}

template<typename Predicate>
bool next(std::istream& is, Predicate predicate) {
    char c;
    return next(is, c, predicate);
}

template<typename Predicate>
void expect(std::istream& is) {
    if (!next<Predicate>(is)) {
        throw unexpected_token{};
    }
}

template<typename Predicate>
void expect(std::istream& is, Predicate predicate) {
    if (!next(is, predicate)) {
        throw unexpected_token{};
    }
}

template<typename Predicate>
void skip_while(std::istream& is) {
    while (next<Predicate>(is)) {
        // Do nothing
    }
    is.unget();
}

template<typename Predicate>
void skip_until(std::istream& is) {
    skip_while<negate<Predicate>>(is);
}

template<typename Predicate>
void read_while(std::istream& is, std::string& s) {
    char c;
    while (next<Predicate>(is, c)) {
        s += c;
    }
    is.unget();
}

template<typename Predicate>
void read_until(std::istream& is, std::string& s) {
    read_while<negate<Predicate>>(is, s);
}

inline void expect_exact(std::istream& is, const std::string &expected) {
    for (size_t i{}; i < expected.size(); ++i) {
        expect(is, [&] (char c) { return c == expected[i]; });
    }
}

value_owned_ptr parse_value(std::istream& is);

inline std::string parse_string(std::istream& is) {
    std::string data;
    expect<dquote>(is);
    for (char c{get_next(is)};; c = get_next(is)) {
        if (escape{}(c)) {
            get_next(is);
            continue;
        }
        if (dquote{}(c)) {
            is.unget();
            break;
        }
        data.push_back(c);
    }
    expect<dquote>(is);
    return data;
}

inline bool parse_boolean(std::istream& is) {
    if (peek_next(is) == 't') {
        expect_exact(is, "true");
        return true;
    }
    expect_exact(is, "false");
    return false;
}

inline double parse_number(std::istream& is) {
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
    expect_exact(is, "null");
    return std::make_unique<null>();
}

inline value_owned_ptr parse_object_value(std::istream& is) {
    expect<object_open>(is);
    skip_while<ws>(is);
    auto result{std::make_unique<object>()};
    if (peek<object_close>(is)) {
        expect<object_close>(is);
        return result;
    }
    while (true) {
        if (!peek<dquote>(is)) {
            throw unexpected_token{};
        }
        auto member_name{parse_string(is)};
        skip_while<ws>(is);
        expect<member_separator>(is);
        auto member_value{parse_value(is)};
        result->members.emplace(std::move(member_name), std::move(member_value));
        if (peek<value_separator>(is)) {
            skip(is);
            skip_while<ws>(is);
            continue;
        }
        break;
    }
    skip_while<ws>(is);
    expect<object_close>(is);
    return result;
}

inline value_owned_ptr parse_array_value(std::istream& is) {
    expect<array_open>(is);
    skip_while<ws>(is);
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
            skip_while<ws>(is);
            continue;
        }
        break;
    }
    skip_while<ws>(is);
    expect<array_close>(is);
    return result;
}

inline value_owned_ptr parse_value(std::istream& is) {
    skip_while<ws>(is);
    if (peek<dquote>(is)) {
        return parse_string_value(is);
    } else if (peek<object_open>(is)) {
        return parse_object_value(is);
    } else if (peek<array_open>(is)) {
        return parse_array_value(is);
    }
    auto c{peek_next(is)};
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
