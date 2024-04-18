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

#include <exception>
#include <string>

namespace langnes {
namespace json {

enum class error_code {
    parse_error = -6,
    out_of_range = -5,
    bad_access = -4,
    invalid_state = -3,
    invalid_argument = -2,
    unspecified = -1,
    ok = 0,
};

class error : public std::exception {
public:
    error(error_code code, const std::string& message,
          std::exception_ptr cause) noexcept
        : m_code{code},
          m_message{message},
          // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
          m_cause{std::move(cause)} {}

    error(error_code code, const std::string& message) noexcept
        : m_code{code},
          m_message{message} {}

    error() = default;

    error_code code() const { return m_code; }
    std::exception_ptr cause() const { return m_cause; }

    const char* what() const noexcept override { return m_message.c_str(); }

private:
    error_code m_code{error_code::unspecified};
    std::string m_message;
    std::exception_ptr m_cause;
};

class parse_error : public error {
public:
    explicit parse_error(const std::string& message)
        : error{error_code::parse_error, "Parse error: " + message} {}
};

class bad_access : public error {
public:
    bad_access() : error{error_code::bad_access, "Bad access"} {}
};

class invalid_argument : public error {
public:
    invalid_argument()
        : error{error_code::invalid_argument, "Invalid argument"} {}
};

class invalid_state : public error {
public:
    invalid_state() : error{error_code::invalid_state, "Invalid state"} {}

    explicit invalid_state(const std::string& message)
        : error{error_code::invalid_state, "Invalid state: " + message} {}
};

class out_of_range : public error {
public:
    explicit out_of_range(const std::string& message)
        : error{error_code::out_of_range, "Out of range: " + message} {}
};

} // namespace json
} // namespace langnes
