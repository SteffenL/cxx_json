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
#include <type_traits>

namespace langnes {
namespace json {
namespace detail {

struct nullopt_t {};

template<typename T>
class optional {
public:
    constexpr optional() = default;
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    constexpr optional(nullopt_t /*unused*/) noexcept {}

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    optional(const T& other) : m_has_data{true} { new (&m_data) T{other}; }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    optional(T&& other) noexcept : m_has_data{true} {
        new (&m_data) T{std::move(other)};
    }

    optional(const optional<T>& other) { *this = other; }

    optional& operator=(const optional<T>& other) {
        if (this == &other) {
            return *this;
        }
        m_has_data = other.has_value();
        if (m_has_data) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            new (&m_data) T{*reinterpret_cast<const T*>(&other.m_data)};
        }
        return *this;
    }

    optional(optional<T>&& other) noexcept { *this = std::move(other); }

    optional& operator=(optional<T>&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        m_has_data = other.has_value();
        if (m_has_data) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            new (&m_data) T{std::move(*reinterpret_cast<T*>(&other.m_data))};
        }
        return *this;
    }

    ~optional() {
        if (m_has_data) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            reinterpret_cast<T*>(&m_data)->~T();
        }
    }

    constexpr explicit operator bool() const { return has_value(); }
    constexpr const T& operator*() const { return get(); }
    T& operator*() { return get(); }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const T& get() const {
        if (!m_has_data) {
            throw bad_access{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return *reinterpret_cast<const T*>(&m_data);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    T& get() {
        if (!m_has_data) {
            throw bad_access{};
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return *reinterpret_cast<T*>(&m_data);
    }

    T steal() {
        if (!m_has_data) {
            throw bad_access{};
        }
        m_has_data = false;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return std::move(*reinterpret_cast<T*>(&m_data));
    }

    constexpr bool has_value() const { return m_has_data; }

private:
    // NOLINTNEXTLINE(bugprone-sizeof-expression): pointer to aggregate is OK
    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_data;
    bool m_has_data{};
};

template<>
class optional<void> {};

constexpr nullopt_t nullopt{};

} // namespace detail
} // namespace json
} // namespace langnes
