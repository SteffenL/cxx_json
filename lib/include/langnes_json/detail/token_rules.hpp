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

#include "macros.hpp"

#include <iosfwd>

LANGNES_JSON_CXX_NS_BEGIN
namespace detail {
namespace token_rules {

constexpr bool eol(std::istream& /*unused*/, char c) {
    return c == '\r' || c == '\n';
}

constexpr bool ws(std::istream& is, char c) {
    return c == ' ' || c == '\t' || eol(is, c);
}

constexpr bool dquote(std::istream& /*unused*/, char c) { return c == '"'; }

constexpr bool digit(std::istream& /*unused*/, char c) {
    return c >= '0' && c <= '9';
}

constexpr bool digit_1_through_9(std::istream& /*unused*/, char c) {
    return c >= '1' && c <= '9';
}

constexpr bool hex_digit(std::istream& is, char c) {
    return digit(is, c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

constexpr bool decimal_point(std::istream& /*unused*/, char c) {
    return c == '.';
}

constexpr bool escape_start(std::istream& /*unused*/, char c) {
    return c == '\\';
}

constexpr bool object_open(std::istream& /*unused*/, char c) {
    return c == '{';
}

constexpr bool object_close(std::istream& /*unused*/, char c) {
    return c == '}';
}

constexpr bool array_open(std::istream& /*unused*/, char c) { return c == '['; }

constexpr bool array_close(std::istream& /*unused*/, char c) {
    return c == ']';
}

constexpr bool value_separator(std::istream& /*unused*/, char c) {
    return c == ',';
}

constexpr bool member_separator(std::istream& /*unused*/, char c) {
    return c == ':';
}

constexpr bool json_special_char(char c) {
    return c == '"' || c == '\\' || c == '\b' || c == '\f' || c == '\n' ||
           c == '\r' || c == '\t';
}

constexpr bool json_control_char(char c) { return c >= 0 && c <= 0x1f; }

} // namespace token_rules
} // namespace detail
LANGNES_JSON_CXX_NS_END
