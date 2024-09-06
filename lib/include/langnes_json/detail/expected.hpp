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
#include "optional.hpp"

LANGNES_JSON_CXX_NS_BEGIN
namespace detail {

template<typename Value, typename Error, typename Exception>
class expected {
public:
    using value_type = Value;
    using error_type = Error;
    using exception_type = Exception;

    expected() : expected(value_type{}) {}

    expected(const value_type& value) : m_value{value} {}
    expected(value_type&& value) noexcept
        : m_value{std::forward<value_type>(value)} {}

    expected(const error_type& error) : m_error{error} {}
    expected(error_type&& error) noexcept
        : m_error{std::forward<error_type>(error)} {}

    bool ok() const { return has_value() && !has_error(); }
    bool has_value() const { return m_value.has_value(); }
    bool has_error() const { return m_error.has_value(); }

    void ensure_ok() {
        if (!ok()) {
            throw exception_type{error()};
        }
    }

    const value_type& value() const {
        if (!has_value()) {
            throw bad_access{};
        }
        return m_value.get();
    }

    const error_type& error() const {
        if (!has_error()) {
            throw bad_access{};
        }
        return m_error.get();
    }

private:
    optional<value_type> m_value;
    optional<error_type> m_error;
};

template<typename Error, typename Exception>
class expected<void, Error, Exception> {
public:
    using value_type = void;
    using error_type = Error;
    using exception_type = Exception;

    expected() = default;

    expected(error_type&& error) noexcept
        : m_error{std::forward<error_type>(error)} {}

    bool ok() const { return !has_error(); }

    bool has_error() const { return m_error.has_value(); }

    void ensure_ok() {
        if (!ok()) {
            throw exception_type{error()};
        }
    }

    const error_type& error() const {
        if (!has_error()) {
            throw bad_access{};
        }
        return m_error.get();
    }

private:
    optional<error_type> m_error;
};

} // namespace detail
LANGNES_JSON_CXX_NS_END
