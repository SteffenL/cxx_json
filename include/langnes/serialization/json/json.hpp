#pragma once

#include "../parsing.hpp"

#include <cassert>
#include <cmath>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace langnes {
namespace serialization {
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

namespace rules {

constexpr inline bool eol(std::istream& is, char c) { return c == '\r' || c == '\n'; }
constexpr inline bool ws(std::istream& is, char c) { return c == ' ' || c == '\t' || eol(is, c); }
constexpr inline bool dquote(std::istream& is, char c) {return c == '"'; }
constexpr inline bool digit(std::istream& is, char c) { return c >= '0' && c <= '9'; }
constexpr inline bool digit_1_through_9(std::istream& is, char c) { return c >= '1' && c <= '9'; }
constexpr inline bool hex_digit(std::istream& is, char c) { return digit(is, c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
constexpr inline bool decimal_point(std::istream& is, char c) { return c == '.'; }
constexpr inline bool escape_start(std::istream& is, char c) { return c == '\\'; }
constexpr inline bool object_open(std::istream& is, char c) { return c == '{'; }
constexpr inline bool object_close(std::istream& is, char c) { return c == '}'; }
constexpr inline bool array_open(std::istream& is, char c) { return c == '['; }
constexpr inline bool array_close(std::istream& is, char c) { return c == ']'; }
constexpr inline bool value_separator(std::istream& is, char c) { return c == ','; }
constexpr inline bool member_separator(std::istream& is, char c) { return c == ':'; }

} // namespace rules

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

constexpr bool is_json_special_char(char c) {
  return c == '"' || c == '\\' || c == '\b' || c == '\f' || c == '\n' ||
         c == '\r' || c == '\t';
}

constexpr bool is_ascii_control_char(char c) { return c >= 0 && c <= 0x1f; }

inline std::string escape(const std::string &s, bool add_quotes = true) {
  // Calculate the size of the resulting string.
  // Add space for the double quotes.
  size_t required_length = add_quotes ? 2 : 0;
  for (auto c : s) {
    if (is_json_special_char(c)) {
      // '\' and a single following character
      required_length += 2;
      continue;
    }
    if (is_ascii_control_char(c)) {
      // '\', 'u', 4 digits
      required_length += 6;
      continue;
    }
    ++required_length;
  }
  // Allocate memory for resulting string only once.
  std::string result;
  result.reserve(required_length);
  if (add_quotes) {
    result += '"';
  }
  // Copy string while escaping characters.
  for (auto c : s) {
    if (is_json_special_char(c)) {
      static constexpr char special_escape_table[257] =
          "\0\0\0\0\0\0\0\0btn\0fr\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\"\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
          "\0\0\0\0\0\0\0\0\0\0\0\0\\";
      result += '\\';
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      result += special_escape_table[static_cast<unsigned char>(c)];
      continue;
    }
    if (is_ascii_control_char(c)) {
      // Escape as \u00xx
      static constexpr char hex_alphabet[]{"0123456789abcdef"};
      auto uc = static_cast<unsigned char>(c);
      auto h = (uc >> 4) & 0x0f;
      auto l = uc & 0x0f;
      result += "\\u00";
      // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
      result += hex_alphabet[h];
      result += hex_alphabet[l];
      // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
      continue;
    }
    result += c;
  }
  if (add_quotes) {
    result += '"';
  }
  // Should have calculated the exact amount of memory needed
  assert(required_length == result.size());
  return result;
}

inline std::string to_utf8_char(size_t c) {
    std::string s;
    if (c <= 0x7f) {
        s.push_back(static_cast<char>(c));
    } else if (c <= 0x7ff) {
        s.push_back(static_cast<char>(0xc0 | (c >> 6)));
        s.push_back(static_cast<char>(0x80 | (c & 0x3f)));
    } else if (c <= 0xffff) {
        s.push_back(static_cast<char>(0xe0 | (c >> 12)));
        s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3f)));
        s.push_back(static_cast<char>(0x80 | (c & 0x3f)));
    } else if (c <= 0x10ffff) {
        s.push_back(static_cast<char>(0xe0 | (c >> 18)));
        s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3f)));
        s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3f)));
        s.push_back(static_cast<char>(0x80 | (c & 0x3f)));
    } else {
        throw std::out_of_range{"Invalid code point"};
    }
    return s;
};

inline void unescape_one(std::istream& is, std::ostream& os) {
    using namespace parsing;
    using namespace rules;
    static constexpr char escape_table[20] =
        "\b\0\0\0\f\0\0\0\0\0\0\0\n\0\0\0\r\0\t";
    char c{};
    if (!next(is, c, escape_start)) {
        os.put(c);
        return;
    }
    c = get_next(is);
    if (c >= 'b' && c <= 't') {
        auto unescaped_char{escape_table[c - 'b']};
        if (unescaped_char != '\0') {
            os.put(unescaped_char);
            return;
        }
    } else if (c == 'x' || c == 'u') {
        const auto length{c == 'x' ? 2u : 4u};
        std::string digits;
        read_while(is, digits, hex_digit);
        if (digits.size() != length) {
            throw unexpected_token{};
        }
        auto code_point{std::stoul(digits, 0, 16)};
        auto utf8_char{to_utf8_char(code_point)};
        os.write(utf8_char.data(), utf8_char.size());
        return;
    }
    os.put(c);
}

inline void to_yaml(std::ostream& os, const value& v, size_t indent_level = 0) {
    using t = value::type;
    auto indent = [] (size_t level) { return std::string(level * 2, ' '); };
    switch (v.get_type()) {
        case t::object:
            for (const auto& kv : v.as_object()) {
                os << indent(indent_level) << kv.first <<  ": ";
                auto type{kv.second.get_type()};
                if (type == t::object || type == t::array) {
                    os << "\n";
                    to_yaml(os, kv.second, indent_level + 1);
                } else {
                    to_yaml(os, kv.second, 0);
                }
            }
            break;
        case t::array:
            for (const auto& element : v.as_array()) {
                os << indent(indent_level) << "- ";
                auto type{element.get_type()};
                if (type == t::object || type == t::array) {
                    to_yaml(os, element, indent_level + 1);
                } else {
                    to_yaml(os, element, 0);
                }
            }
            break;
        case t::string: {
            os << indent(indent_level);
            auto s{escape(v.as_string())};
            os.write(s.data(), s.size());
            os << "\n";
            break;
        }
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

inline void to_json(std::ostream& os, const value& v) {
    using t = value::type;
    switch (v.get_type()) {
        case t::object: {
            os.put('{');
            bool first{true};
            for (const auto& kv : v.as_object()) {
                if (first) {
                    first = false;
                } else {
                    os.put(',');
                }
                os << "\"" << kv.first <<  "\":";
                auto type{kv.second.get_type()};
                if (type == t::object || type == t::array) {
                    to_json(os, kv.second);
                } else {
                    to_json(os, kv.second);
                }
            }
            os.put('}');
            break;
        }
        case t::array: {
            os.put('[');
            bool first{true};
            for (const auto& element : v.as_array()) {
                if (first) {
                    first = false;
                } else {
                    os.put(',');
                }
                auto type{element.get_type()};
                if (type == t::object || type == t::array) {
                    to_json(os, element);
                } else {
                    to_json(os, element);
                }
            }
            os.put(']');
            break;
        }
        case t::string: {
            auto s{escape(v.as_string())};
            os.write(s.data(), s.size());
            break;
        }
        case t::boolean:
            os << (v.as_boolean() ? "true" : "false");
            break;
        case t::null:
            os << "null";
            break;
        case t::number:
            os << v.as_number();
            break;
        default:
            throw std::invalid_argument{"Invalid value type"};
    }
}

value parse_value(std::istream& is);

inline std::optional<std::string> try_parse_string(std::istream& is) {
    using namespace parsing;
    using namespace rules;
    if (!peek(is, dquote)) {
        return std::nullopt;
    }
    skip(is);
    std::ostringstream os{std::ios::binary};
    for (char c{get_next(is)};; c = get_next(is)) {
        if (escape_start(is, c)) {
            is.unget();
            unescape_one(is, os);
            continue;
        }
        if (dquote(is, c)) {
            is.unget();
            break;
        }
        os.put(c);
    }
    expect(is, dquote);
    return {os.str()};
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

enum class stored_format { json, yaml };

inline value load(std::istream& is) {
    return detail::parse_value(is);
}

inline value load(const std::string& s) {
    // TODO: avoid copy
    std::istringstream is{s, std::ios::binary};
    return load(is);
}

inline void save(std::ostream& os, const value& v, stored_format format = stored_format::json) {
    switch (format) {
    case stored_format::json:
        detail::to_json(os, v);
        return;
    case stored_format::yaml:
        detail::to_yaml(os, v);
        return;
    }
    throw std::invalid_argument{"Invalid format"};
}

inline std::string save(const value& v, stored_format format = stored_format::json) {
    // Avoid copy?
    std::ostringstream os{std::ios::binary};
    save(os, v, format);
    return os.str();
}


} // namespace langnes
} // namespace serialization
} // namespace json
