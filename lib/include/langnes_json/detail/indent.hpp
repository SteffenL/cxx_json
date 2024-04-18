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

#include <cstddef>
#include <ostream>

namespace langnes {
namespace json {
namespace detail {

struct indent_info {
    size_t level;
    size_t indent_width;
    char indent_char;
};

inline indent_info indent(size_t level, size_t indent_width = 2,
                          char indent_char = ' ') {
    return {level, indent_width, indent_char};
}

template<typename T, typename Traits>
std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& os,
                                          indent_info indent) {
    for (size_t i{}; i < indent.level; ++i) {
        for (size_t j{}; j < indent.indent_width; ++j) {
            os.put(indent.indent_char);
        }
    }
    return os;
}

} // namespace detail
} // namespace json
} // namespace langnes
