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
#include <string>

namespace langnes {
namespace json {
namespace detail {

inline std::string to_utf8_char(size_t c) {
    std::string s;
    if (c <= 0x7fU) {
        s.push_back(static_cast<char>(c));
    } else if (c <= 0x7ffU) {
        s.push_back(static_cast<char>(0xc0U | (c >> 6U)));
        s.push_back(static_cast<char>(0x80U | (c & 0x3fU)));
    } else if (c <= 0xffffU) {
        s.push_back(static_cast<char>(0xe0U | (c >> 12U)));
        s.push_back(static_cast<char>(0x80U | ((c >> 6U) & 0x3fU)));
        s.push_back(static_cast<char>(0x80U | (c & 0x3fU)));
    } else if (c <= 0x10ffffU) {
        s.push_back(static_cast<char>(0xf0U | (c >> 18U)));
        s.push_back(static_cast<char>(0x80U | ((c >> 12U) & 0x3fU)));
        s.push_back(static_cast<char>(0x80U | ((c >> 6U) & 0x3fU)));
        s.push_back(static_cast<char>(0x80U | (c & 0x3fU)));
    } else {
        throw out_of_range{"Invalid code point"};
    }
    return s;
}

} // namespace detail
} // namespace json
} // namespace langnes
