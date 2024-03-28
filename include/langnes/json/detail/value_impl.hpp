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
#include "dict.hpp"
#include "memory.hpp"
#include "value.hpp"

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace langnes {
namespace json {
namespace detail {

struct value_impl_base {
public:
    virtual ~value_impl_base() = default;
    virtual std::unique_ptr<value_impl_base> clone() const noexcept = 0;
    value::type get_type() const { return m_type; }
    bool is_type(value::type type) const noexcept { return m_type == type; }

protected:
    explicit value_impl_base(value::type type) : m_type{type} {}
    value_impl_base() = default;
    value_impl_base(const value_impl_base&) = default;
    value_impl_base& operator=(const value_impl_base&) = default;
    value_impl_base(value_impl_base&&) = default;
    value_impl_base& operator=(value_impl_base&&) = default;

private:
    value::type m_type{};
};

struct string_impl : public value_impl_base {
    string_impl() : value_impl_base{value::type::string} {}

    template<typename T, typename std::enable_if<std::is_convertible<
                             T, std::string>::value>::type* = nullptr>
    explicit string_impl(T&& data)
        : value_impl_base{value::type::string}, m_data{std::forward<T>(data)} {}

    std::unique_ptr<value_impl_base> clone() const noexcept override {
        return make_unique<string_impl>(*this);
    }

    const std::string& data() const { return m_data; }
    std::string& data() { return m_data; }

private:
    std::string m_data;
};

struct number_impl : public value_impl_base {
    number_impl() : value_impl_base{value::type::number} {}

    template<typename T, typename std::enable_if<
                             std::is_integral<T>::value>::type* = nullptr>
    explicit number_impl(T data) : number_impl{static_cast<double>(data)} {}

    template<typename T, typename std::enable_if<
                             std::is_floating_point<T>::value>::type* = nullptr>
    explicit number_impl(T data)
        : value_impl_base{value::type::number}, m_data{data} {}

    std::unique_ptr<value_impl_base> clone() const noexcept override {
        return make_unique<number_impl>(*this);
    }

    double data() const { return m_data; }
    double& data() { return m_data; }

private:
    double m_data{};
};

struct boolean_impl : public value_impl_base {
    boolean_impl() : value_impl_base{value::type::boolean} {}

    explicit boolean_impl(bool data)
        : value_impl_base{value::type::boolean}, m_data{data} {}

    std::unique_ptr<value_impl_base> clone() const noexcept override {
        return make_unique<boolean_impl>(*this);
    }

    bool data() const { return m_data; }
    bool& data() { return m_data; }

private:
    bool m_data{};
};

struct null_impl : public value_impl_base {
    null_impl() : value_impl_base{value::type::null} {}

    std::unique_ptr<value_impl_base> clone() const noexcept override {
        return make_unique<null_impl>(*this);
    }
};

struct object_impl : public value_impl_base {
    object_impl() : value_impl_base{value::type::object} {}

    std::unique_ptr<value_impl_base> clone() const noexcept override {
        return make_unique<object_impl>(*this);
    }

    const dict<std::string, value>& members() const { return m_members; }
    dict<std::string, value>& members() { return m_members; }

private:
    dict<std::string, value> m_members;
};

struct array_impl : public value_impl_base {
    array_impl() : value_impl_base{value::type::array} {}

    std::unique_ptr<value_impl_base> clone() const noexcept override {
        return make_unique<array_impl>(*this);
    }

    const std::vector<value>& elements() const { return m_elements; }
    std::vector<value>& elements() { return m_elements; }

private:
    std::vector<value> m_elements;
};

inline const std::string& value::as_string() const {
    if (!m_impl->is_type(value::type::string)) {
        throw type_mismatch{};
    }
    return dynamic_cast<const string_impl*>(m_impl.get())->data();
}

inline double value::as_number() const {
    if (!m_impl->is_type(value::type::number)) {
        throw type_mismatch{};
    }
    return dynamic_cast<const number_impl*>(m_impl.get())->data();
}

inline bool value::as_boolean() const {
    if (!m_impl->is_type(value::type::boolean)) {
        throw type_mismatch{};
    }
    return dynamic_cast<const boolean_impl*>(m_impl.get())->data();
}

inline const dict<std::string, value>& value::as_object() const {
    if (!m_impl->is_type(value::type::object)) {
        throw type_mismatch{};
    }
    return dynamic_cast<const object_impl*>(m_impl.get())->members();
}

inline const std::vector<value>& value::as_array() const {
    if (!m_impl->is_type(value::type::array)) {
        throw type_mismatch{};
    }
    return dynamic_cast<const array_impl*>(m_impl.get())->elements();
}

inline std::string& value::as_string() {
    if (!m_impl->is_type(value::type::string)) {
        throw type_mismatch{};
    }
    return dynamic_cast<string_impl*>(m_impl.get())->data();
}

inline double& value::as_number() {
    if (!m_impl->is_type(value::type::number)) {
        throw type_mismatch{};
    }
    return dynamic_cast<number_impl*>(m_impl.get())->data();
}

inline bool& value::as_boolean() {
    if (!m_impl->is_type(value::type::boolean)) {
        throw type_mismatch{};
    }
    return dynamic_cast<boolean_impl*>(m_impl.get())->data();
}

inline dict<std::string, value>& value::as_object() {
    if (!m_impl->is_type(value::type::object)) {
        throw type_mismatch{};
    }
    return dynamic_cast<object_impl*>(m_impl.get())->members();
}

inline std::vector<value>& value::as_array() {
    if (!m_impl->is_type(value::type::array)) {
        throw type_mismatch{};
    }
    return dynamic_cast<array_impl*>(m_impl.get())->elements();
}

inline bool value::is_type(value::type type) const noexcept {
    return m_impl->is_type(type);
}

inline bool value::is_string() const noexcept {
    return is_type(value::type::string);
}

inline bool value::is_number() const noexcept {
    return is_type(value::type::number);
}

inline bool value::is_boolean() const noexcept {
    return is_type(value::type::boolean);
}

inline bool value::is_object() const noexcept {
    return is_type(value::type::object);
}

inline bool value::is_array() const noexcept {
    return is_type(value::type::array);
}

inline bool value::is_null() const noexcept {
    return is_type(value::type::null);
}

inline value::value() noexcept : m_impl{make_unique<null_impl>()} {}

inline value::value(const value& rhs) noexcept {
    this->m_impl = rhs.m_impl->clone();
}

inline value::value(value&& rhs) noexcept : m_impl{std::move(rhs.m_impl)} {}

inline value::value(std::unique_ptr<value_impl_base>&& impl) noexcept
    : m_impl{std::move(impl)} {}

inline value::value(const char* data) noexcept
    : m_impl{make_unique<string_impl>(data)} {}

inline value::value(const std::string& data) noexcept
    : m_impl{make_unique<string_impl>(data)} {}

inline value::value(std::string&& data) noexcept
    : m_impl{make_unique<string_impl>(std::move(data))} {}

inline value::value(std::nullptr_t) noexcept
    : m_impl{make_unique<null_impl>()} {}

template<typename T,
         typename std::enable_if<(std::is_integral<T>::value &&
                                  !std::is_same<T, bool>::value) ||
                                 std::is_floating_point<T>::value>::type*>
value::value(T from) noexcept
    : m_impl{make_unique<number_impl>(std::forward<T>(from))} {}

template<typename T,
         typename std::enable_if<std::is_same<T, bool>::value>::type*>
value::value(T from) noexcept
    : m_impl{make_unique<boolean_impl>(std::forward<T>(from))} {}

inline value::type value::get_type() const { return m_impl->get_type(); }

template<typename T,
         typename std::enable_if<(std::is_integral<T>::value &&
                                  !std::is_same<T, bool>::value) ||
                                 std::is_floating_point<T>::value>::type*>
value& value::operator=(T from) noexcept {
    m_impl = make_unique<number_impl>(std::forward<T>(from));
    return *this;
}

template<typename T,
         typename std::enable_if<std::is_same<T, bool>::value>::type*>
value& value::operator=(T from) noexcept {
    m_impl = make_unique<boolean_impl>(std::forward<T>(from));
    return *this;
}

inline value& value::operator=(const value& rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }
    m_impl = rhs.m_impl->clone();
    return *this;
}

inline value& value::operator=(value&& rhs) noexcept {
    m_impl = std::move(rhs.m_impl);
    return *this;
}

inline value& value::operator=(const char* rhs) noexcept {
    m_impl = make_unique<string_impl>(rhs);
    return *this;
}

inline value& value::operator=(const std::string& rhs) noexcept {
    m_impl = make_unique<string_impl>(rhs);
    return *this;
}

inline value& value::operator=(std::string&& rhs) noexcept {
    m_impl = make_unique<string_impl>(std::move(rhs));
    return *this;
}

inline value& value::operator=(std::nullptr_t) noexcept {
    m_impl = make_unique<null_impl>();
    return *this;
}

} // namespace detail
} // namespace json
} // namespace langnes
