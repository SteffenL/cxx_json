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

#include <stdexcept>
#include <string>

namespace langnes {
namespace json {
namespace detail {

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
}

} // namespace detail
} // namespace json
} // namespace langnes
