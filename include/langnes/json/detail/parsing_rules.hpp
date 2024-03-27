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

#include <iosfwd>

namespace langnes {
namespace json {
namespace detail {
namespace parsing {
namespace rules {

constexpr inline bool eol(std::istream&, char c) {
    return c == '\r' || c == '\n';
}

constexpr inline bool ws(std::istream& is, char c) {
    return c == ' ' || c == '\t' || eol(is, c);
}

constexpr inline bool dquote(std::istream&, char c) { return c == '"'; }

constexpr inline bool digit(std::istream&, char c) {
    return c >= '0' && c <= '9';
}

constexpr inline bool digit_1_through_9(std::istream&, char c) {
    return c >= '1' && c <= '9';
}

constexpr inline bool hex_digit(std::istream& is, char c) {
    return digit(is, c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

constexpr inline bool decimal_point(std::istream&, char c) { return c == '.'; }

constexpr inline bool escape_start(std::istream&, char c) { return c == '\\'; }

constexpr inline bool object_open(std::istream&, char c) { return c == '{'; }

constexpr inline bool object_close(std::istream&, char c) { return c == '}'; }

constexpr inline bool array_open(std::istream&, char c) { return c == '['; }
constexpr inline bool array_close(std::istream&, char c) { return c == ']'; }

constexpr inline bool value_separator(std::istream&, char c) {
    return c == ',';
}

constexpr inline bool member_separator(std::istream&, char c) {
    return c == ':';
}

constexpr bool json_special_char(char c) {
    return c == '"' || c == '\\' || c == '\b' || c == '\f' || c == '\n' ||
           c == '\r' || c == '\t';
}

constexpr bool ascii_control_char(char c) { return c >= 0 && c <= 0x1f; }

} // namespace rules
} // namespace parsing
} // namespace detail
} // namespace json
} // namespace langnes
