/*
 * Copyright 2024 Steffen André Langnes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "errors.hpp"
#include "macros.hpp"
#include "memory.hpp"
#include "optional.hpp"
#include "parsing.hpp"
#include "token_rules.hpp"
#include "utf8.hpp"
#include "value.hpp"
#include "value_impl.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

LANGNES_JSON_CXX_NS_BEGIN
namespace detail {

inline std::string escape(const std::string& s, bool add_quotes = true) {
    using namespace token_rules;
    // Calculate the size of the resulting string.
    // Add space for the double quotes.
    size_t required_length = add_quotes ? 2 : 0;
    for (auto c : s) {
        if (json_special_char(c)) {
            // '\' and a single following character
            required_length += 2;
            continue;
        }
        if (json_control_char(c)) {
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
        if (json_special_char(c)) {
            static constexpr std::array<char, 256> special_escape_table = {
                0, 0, 0,   0, 0, 0, 0, 0, 'b', 't', 'n', 0, 'f', 'r', 0, 0,
                0, 0, 0,   0, 0, 0, 0, 0, 0,   0,   0,   0, 0,   0,   0, 0,
                0, 0, '"', 0, 0, 0, 0, 0, 0,   0,   0,   0, 0,   0,   0, 0,
                0, 0, 0,   0, 0, 0, 0, 0, 0,   0,   0,   0, 0,   0,   0, 0,
                0, 0, 0,   0, 0, 0, 0, 0, 0,   0,   0,   0, 0,   0,   0, 0,
                0, 0, 0,   0, 0, 0, 0, 0, 0,   0,   0,   0, '\\'};
            result += '\\';
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            result += special_escape_table[static_cast<unsigned char>(c)];
            continue;
        }
        if (json_control_char(c)) {
            // Escape as \u00xx
            static constexpr std::array<char, 16> hex_alphabet = {
                '0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
            auto uc = static_cast<unsigned int>(static_cast<unsigned char>(c));
            auto h = (uc >> 4U) & 0x0fU;
            auto l = uc & 0x0fU;
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

inline void unescape_one(std::istream& is, std::ostream& os) {
    using namespace parsing;
    using namespace token_rules;
    static constexpr std::array<char, 19> escape_table = {
        '\b', 0, 0, 0, '\f', 0, 0, 0, 0, 0, 0, 0, '\n', 0, 0, 0, '\r', 0, '\t'};
    char c{};
    if (!next(is, c, escape_start)) {
        os.put(c);
        return;
    }
    c = get_next(is);
    if (c >= 'b' && c <= 't') {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        auto unescaped_char{escape_table[c - 'b']};
        if (unescaped_char != '\0') {
            os.put(unescaped_char);
            return;
        }
    } else if (c == 'x' || c == 'u') {
        const auto length{c == 'x' ? 2U : 4U};
        std::string digits;
        read_while(is, digits, hex_digit);
        if (digits.size() != length) {
            throw unexpected_token{};
        }
        auto code_point{std::stoul(digits, nullptr, 16)};
        auto utf8_char{to_utf8_char(code_point)};
        os.write(utf8_char.data(),
                 static_cast<std::streamsize>(utf8_char.size()));
        return;
    }
    os.put(c);
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
            os << "\"" << kv.first << "\":";
            to_json(os, kv.second);
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
            to_json(os, element);
        }
        os.put(']');
        break;
    }
    case t::string: {
        auto s{escape(v.as_string())};
        os.write(s.data(), static_cast<std::streamsize>(s.size()));
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
        throw invalid_state{"Unexpected value type"};
    }
}

value parse_value(std::istream& is);

inline optional<std::string> try_parse_string(std::istream& is) {
    using namespace parsing;
    using namespace token_rules;
    if (!peek(is, dquote)) {
        return nullopt;
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

inline optional<bool> try_parse_boolean(std::istream& is) {
    using namespace parsing;
    if (peek_next(is) == 't') {
        expect_exact(is, "true");
        return true;
    }
    if (peek_next(is) == 'f') {
        expect_exact(is, "false");
        return false;
    }
    return nullopt;
}

inline bool parse_boolean(std::istream& is) {
    using namespace parsing;
    if (auto v{try_parse_boolean(is)}) {
        return *v;
    }
    throw unexpected_token{};
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
inline optional<double> try_parse_number(std::istream& is) {
    using namespace parsing;
    using namespace token_rules;
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
    if (!has_reached_end(is)) {
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
            char first_digit{};
            if (!next(is, first_digit, digit)) {
                throw unexpected_token{};
            }
            exponent_digits.push_back(first_digit);
            read_while(is, exponent_digits, digit);
        }
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

inline optional<object_impl> try_parse_object(std::istream& is) {
    using namespace parsing;
    using namespace token_rules;
    if (!peek(is, object_open)) {
        return nullopt;
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
        result.members().emplace(std::move(member_name),
                                 std::move(member_value));
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

inline optional<array_impl> try_parse_array(std::istream& is) {
    using namespace parsing;
    using namespace token_rules;
    if (!peek(is, array_open)) {
        return nullopt;
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
        result.elements().push_back(std::move(element_value));
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
    using namespace token_rules;
    skip_while(is, ws);
    if (auto v{try_parse_string(is)}) {
        return value{make_unique<string_impl>(std::move(*v))};
    }
    if (auto v{try_parse_object(is)}) {
        return value{make_unique<object_impl>(std::move(*v))};
    }
    if (auto v{try_parse_array(is)}) {
        return value{make_unique<array_impl>(std::move(*v))};
    }
    if (auto v{try_parse_boolean(is)}) {
        return value{make_unique<boolean_impl>(*v)};
    }
    if (try_parse_null(is)) {
        return value{make_unique<null_impl>()};
    }
    if (auto v{try_parse_number(is)}) {
        return value{make_unique<number_impl>(*v)};
    }
    throw unexpected_token{};
}

inline value fully_parse_value(std::istream& is) {
    using namespace parsing;
    auto value{parse_value(is)};
    expect_fully_consumed(is);
    return value;
}

} // namespace detail
LANGNES_JSON_CXX_NS_END
