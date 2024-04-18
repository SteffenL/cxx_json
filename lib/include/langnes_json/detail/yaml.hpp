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

#include "../errors.hpp"
#include "indent.hpp"
#include "json.hpp"

#include <string>

namespace langnes {
namespace json {
namespace detail {

inline bool is_populated_object(const value& v) noexcept {
    return v.is_object() && !v.as_object().empty();
}

inline bool is_populated_array(const value& v) noexcept {
    return v.is_array() && !v.as_array().empty();
}

void to_yaml(std::ostream& os, const value& v, size_t indent_level = 0,
             value::type ancestor_type = value::type::null);

// NOLINTNEXTLINE(misc-no-recursion)
inline void object_to_yaml(std::ostream& os, const value& v,
                           size_t indent_level, value::type ancestor_type) {
    const auto& object{v.as_object()};
    if (object.empty()) {
        os << "{}\n";
        return;
    }
    const auto ancestor_is_object{ancestor_type == value::type::object};
    size_t i{};
    for (const auto& kv : object) {
        if (ancestor_is_object && i == 0) {
            os << "\n" << indent(indent_level);
        }
        os << indent(i == 0 ? 0 : indent_level) << kv.first << ":";
        to_yaml(os, kv.second, indent_level + 1, v.get_type());
        ++i;
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
inline void array_to_yaml(std::ostream& os, const value& v, size_t indent_level,
                          value::type ancestor_type) {
    const auto& array{v.as_array()};
    if (array.empty()) {
        os << "[]\n";
        return;
    }
    const auto ancestor_is_object{ancestor_type == value::type::object};
    size_t i{};
    for (const auto& element : array) {
        if (ancestor_is_object && i == 0) {
            os << "\n" << indent(indent_level);
        }
        os << indent(i == 0 ? 0 : indent_level) << "- ";
        to_yaml(os, element, indent_level + 1, v.get_type());
        ++i;
    }
}

inline void string_to_yaml(std::ostream& os, const value& v) {
    const auto s{escape(v.as_string())};
    os.write(s.data(), static_cast<std::streamsize>(s.size()));
    os << "\n";
}

inline void bool_to_yaml(std::ostream& os, const value& v) {
    os << (v.as_boolean() ? "true" : "false") << "\n";
}

inline void null_to_yaml(std::ostream& os) { os << "null\n"; }

inline void number_to_yaml(std::ostream& os, const value& v) {
    os << v.as_number() << "\n";
}

// NOLINTNEXTLINE(misc-no-recursion)
inline void to_yaml(std::ostream& os, const value& v, size_t indent_level,
                    value::type ancestor_type) {
    using t = value::type;
    const auto ancestor_is_object{ancestor_type == t::object};
    if (ancestor_is_object && !is_populated_object(v) &&
        !is_populated_array(v)) {
        os << " ";
    }
    switch (v.get_type()) {
    case t::object:
        object_to_yaml(os, v, indent_level, ancestor_type);
        break;
    case t::array:
        array_to_yaml(os, v, indent_level, ancestor_type);
        break;
    case t::string:
        string_to_yaml(os, v);
        break;
    case t::boolean:
        bool_to_yaml(os, v);
        break;
    case t::null:
        null_to_yaml(os);
        break;
    case t::number:
        number_to_yaml(os, v);
        break;
    default:
        throw invalid_state{"Unexpected value type"};
    }
}

} // namespace detail
} // namespace json
} // namespace langnes
