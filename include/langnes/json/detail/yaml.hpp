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

#include "json.hpp"

#include <stdexcept>
#include <string>

namespace langnes {
namespace json {
namespace detail {

inline void to_yaml(std::ostream& os,
                    const value& v,
                    size_t indent_level = 0,
                    value::type ancestor_type = value::type::null) {
    using t = value::type;
    auto indent = [](size_t level) {
        return std::string(level * 2, ' ');
    };
    auto type{v.get_type()};
    if (ancestor_type == t::object
        && (type != t::object || v.as_object().empty())
        && (type != t::array || v.as_array().empty())) {
        os << " ";
    }
    switch (type) {
    case t::object: {
        auto& object{v.as_object()};
        if (object.empty()) {
            os << "{}\n";
            break;
        }
        size_t i{};
        for (const auto& kv : object) {
            if (ancestor_type == t::object && i == 0) {
                os << "\n" << indent(indent_level);
            }
            os << indent(i == 0 ? 0 : indent_level) << kv.first << ":";
            to_yaml(os, kv.second, indent_level + 1, type);
            ++i;
        }
        break;
    }
    case t::array: {
        auto& array{v.as_array()};
        if (array.empty()) {
            os << "[]\n";
            break;
        }
        size_t i{};
        for (const auto& element : array) {
            if (ancestor_type == t::object && i == 0) {
                os << "\n" << indent(indent_level);
            }
            os << indent(i == 0 ? 0 : indent_level) << "- ";
            to_yaml(os, element, indent_level + 1, v.get_type());
            ++i;
        }
        break;
    }
    case t::string: {
        auto s{escape(v.as_string())};
        os.write(s.data(), s.size());
        os << "\n";
        break;
    }
    case t::boolean:
        os << (v.as_boolean() ? "true" : "false") << "\n";
        break;
    case t::null:
        os << "null\n";
        break;
    case t::number:
        os << v.as_number() << "\n";
        break;
    default:
        throw std::invalid_argument{"Invalid value type"};
    }
}

} // namespace detail
} // namespace json
} // namespace langnes
