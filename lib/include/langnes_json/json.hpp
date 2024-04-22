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

#include <memory>
#include <sstream>
#include <string>

namespace langnes {
namespace json {
namespace detail {

template<typename Container>
inline void put_array(Container& /*unused*/) noexcept {}

template<typename Container, typename First, typename... Rest>
inline void put_array(Container& container, First&& first,
                      Rest&&... rest) noexcept {
    container.emplace_back(std::forward<First>(first));
    put_array(container, std::forward<Rest>(rest)...);
}

} // namespace detail

namespace library_version {
constexpr unsigned int major{0};
constexpr unsigned int minor{1};
constexpr unsigned int patch{0};
constexpr const char* pre_release{"-dev"};
constexpr const char* build_metadata{""};
}; // namespace library_version

using value = detail::value;

/**
 * Loads JSON from a stream.
 *
 * @param is The input stream.
 * @return The JSON value.
 */
template<typename Stream,
         detail::enable_if_t<std::is_base_of<std::istream, Stream>::value>* =
             nullptr>
inline value load(Stream&& is) {
    // Satisfy clang-tidy rule cppcoreguidelines-missing-std-forward
    auto&& is_{std::forward<Stream>(is)};
    return detail::fully_parse_value(is_);
}

/**
 * Loads JSON from a character array with a fixed length.
 *
 * @param data The JSON document data.
 * @param length The length of the JSON document in bytes.
 * @return The JSON value.
 */
inline value load(const char* data, size_t length) {
    return load(detail::make_istream(data, length));
}

/**
 * Loads JSON from a container such as std::string.
 *
 * @param input The input container.
 * @return The JSON value.
 */
template<typename Container, detail::enable_if_t<!std::is_base_of<
                                 std::istream, Container>::value>* = nullptr>
inline value load(Container&& input) {
    return load(detail::make_istream(std::forward<Container>(input)));
}

/**
 * Saves a JSON value to a stream.
 *
 * @param os The output stream.
 * @param v The JSON value.
 */
inline void save(std::ostream& os, const value& v) { detail::to_json(os, v); }

/**
 * Saves a JSON value to a new string.
 *
 * @param v The JSON value.
 * @return The saved JSON document.
 */
inline std::string save(const value& v) {
    std::ostringstream os{std::ios::binary};
    save(os, v);
    return os.str();
}

/**
 * Creates a JSON object value.
 *
 * @param members An initializer list of pairs of object member names and
 * values.
 * @return The JSON value.
 */
inline value make_object(
    std::initializer_list<std::pair<std::string, value>> members) noexcept {
    // FIXME: Avoid copies.
    auto impl{detail::make_unique<detail::object_impl>()};
    for (const auto& member : members) {
        impl->members().emplace(member.first, member.second);
    }
    return value{std::move(impl)};
}

/**
 * Creates a JSON array value.
 *
 * @param elements Zero or more elements to add into the array.
 * @return The JSON value.
 */
template<typename... Args>
inline value make_array(Args&&... elements) noexcept {
    auto impl{detail::make_unique<detail::array_impl>()};
    detail::put_array(impl->elements(), std::forward<Args>(elements)...);
    return value{std::move(impl)};
}

} // namespace json
} // namespace langnes
