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

#include "detail/json.hpp"
#include "detail/memory.hpp"
#include "detail/stream.hpp"
#include "detail/type_traits.hpp"
#include "detail/yaml.hpp"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace langnes {
namespace json {
namespace detail {

template<typename Container>
inline void put_array(Container& /*unused*/) {}

template<typename Container, typename First, typename... Rest>
inline void put_array(Container& container, First&& first, Rest&&... rest) {
    container.emplace_back(std::forward<First>(first));
    put_array(container, std::forward<Rest>(rest)...);
}

} // namespace detail

using value = detail::value;

enum class stored_format { json, yaml };

template<typename Stream,
         detail::enable_if_t<std::is_base_of<std::istream, Stream>::value>* =
             nullptr>
inline value load(Stream&& is) {
    // Satisfy clang-tidy rule cppcoreguidelines-missing-std-forward
    auto&& is_{std::forward<Stream>(is)};
    return detail::parse_value(is_);
}

inline value load(const char* data, size_t length) {
    return load(detail::make_istream(data, length));
}

template<typename Container, detail::enable_if_t<!std::is_base_of<
                                 std::istream, Container>::value>* = nullptr>
inline value load(Container&& input) {
    return load(detail::make_istream(std::forward<Container>(input)));
}

inline void save(std::ostream& os, const value& v,
                 stored_format format = stored_format::json) {
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

inline std::string save(const value& v,
                        stored_format format = stored_format::json) {
    std::ostringstream os{std::ios::binary};
    save(os, v, format);
    return os.str();
}

inline value
make_object(std::initializer_list<std::pair<std::string, value>> members) {
    // FIXME: Avoid copies.
    auto impl{detail::make_unique<detail::object_impl>()};
    for (const auto& member : members) {
        impl->members().emplace(member.first, member.second);
    }
    return value{std::move(impl)};
}

template<typename... Args>
inline value make_array(Args&&... elements) {
    auto impl{detail::make_unique<detail::array_impl>()};
    detail::put_array(impl->elements(), std::forward<Args>(elements)...);
    return value{std::move(impl)};
}

} // namespace json
} // namespace langnes
