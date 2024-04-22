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

#include "dict.hpp"
#include "type_traits.hpp"
#include "value_fwd.hpp"

#include <deque>
#include <memory>
#include <string>
#include <type_traits>

struct langnes_json_value_t {
    virtual ~langnes_json_value_t() = default;
};

namespace langnes {
namespace json {
namespace detail {

struct value_impl_base;

class value : public langnes_json_value_t {
public:
    enum class type { object, array, string, number, boolean, null };

    value() noexcept;
    value(const value& rhs) noexcept;
    value(value&& rhs) noexcept;
    explicit value(std::unique_ptr<value_impl_base>&& impl) noexcept;
    explicit value(const char* data) noexcept;
    explicit value(const std::string& data) noexcept;
    explicit value(std::string&& data) noexcept;
    explicit value(std::nullptr_t) noexcept;
    ~value() override = default;

    template<typename T, typename std::enable_if<
                             (std::is_integral<T>::value &&
                              !std::is_same<remove_cvref_t<T>, bool>::value) ||
                             std::is_floating_point<T>::value>::type* = nullptr>
    explicit value(T from) noexcept;

    template<typename T, typename std::enable_if<std::is_same<
                             remove_cvref_t<T>, bool>::value>::type* = nullptr>
    explicit value(T from) noexcept;

    const std::string& as_string() const;
    double as_number() const;
    bool as_boolean() const;
    const dict<std::string, value>& as_object() const;
    const std::deque<value>& as_array() const;

    std::string& as_string();
    double& as_number();
    bool& as_boolean();
    dict<std::string, value>& as_object();
    std::deque<value>& as_array();

    bool is_type(value::type type) const noexcept;
    bool is_string() const noexcept;
    bool is_number() const noexcept;
    bool is_boolean() const noexcept;
    bool is_object() const noexcept;
    bool is_array() const noexcept;
    bool is_null() const noexcept;

    template<typename T, typename std::enable_if<
                             (std::is_integral<T>::value &&
                              !std::is_same<remove_cvref_t<T>, bool>::value) ||
                             std::is_floating_point<T>::value>::type* = nullptr>
    value& operator=(T from) noexcept;

    template<typename T, typename std::enable_if<std::is_same<
                             remove_cvref_t<T>, bool>::value>::type* = nullptr>
    value& operator=(T from) noexcept;

    value& operator=(const value& rhs) noexcept;
    value& operator=(value&& rhs) noexcept;
    value& operator=(const char* rhs) noexcept;
    value& operator=(const std::string& rhs) noexcept;
    value& operator=(std::string&& rhs) noexcept;
    value& operator=(std::nullptr_t) noexcept;

    type get_type() const noexcept;

private:
    std::unique_ptr<value_impl_base> m_impl;
};

} // namespace detail
} // namespace json
} // namespace langnes
