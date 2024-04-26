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

/**
 * Error code.
 */
enum class error_code {
    parse_error = -6,
    out_of_range = -5,
    bad_access = -4,
    invalid_state = -3,
    invalid_argument = -2,
    unspecified = -1,
    ok = 0,
};

/**
 * Base class for errors.
 */
class error : public std::exception {
public:
    /**
     * Construct a new error.
     *
     * @param code Error code.
     * @param message Error message.
     * @param cause Exception that caused the error.
     */
    error(error_code code, const std::string& message,
          std::exception_ptr cause) noexcept
        : m_code{code},
          m_message{message},
          // NOLINTNEXTLINE(bugprone-throw-keyword-missing)
          m_cause{std::move(cause)} {}

    /**
     * Construct a new error.
     *
     * @param code Error code.
     * @param message Error message.
     */
    error(error_code code, const std::string& message) noexcept
        : m_code{code},
          m_message{message} {}

    /// Default constructor.
    error() = default;

    /**
     * Get the error code.
     *
     * @return Error code.
     */
    error_code code() const { return m_code; }

    /**
     * Get the exception that caused the error.
     *
     * @return Pointer of the exception that caused the error.
     */
    std::exception_ptr cause() const { return m_cause; }

    /**
     * Get the error message.
     *
     * @return The error message as a C string.
     */
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    error_code m_code{error_code::unspecified};
    std::string m_message;
    std::exception_ptr m_cause;
};

/**
 * Parse error.
 */
class parse_error : public error {
public:
    /**
     * Construct a new parse error.
     *
     * @param message Error message.
     */
    explicit parse_error(const std::string& message)
        : error{error_code::parse_error, "Parse error: " + message} {}
};

/**
 * Bad access error.
 */
class bad_access : public error {
public:
    /// Default constructor.
    bad_access() : error{error_code::bad_access, "Bad access"} {}
};

/**
 * Invalid argument error.
 */
class invalid_argument : public error {
public:
    /// Default constructor.
    invalid_argument()
        : error{error_code::invalid_argument, "Invalid argument"} {}
};

/**
 * Invalid state error.
 */
class invalid_state : public error {
public:
    /// Default constructor.
    invalid_state() : error{error_code::invalid_state, "Invalid state"} {}

    /**
     * Construct a new invalid state error.
     *
     * @param message Error message.
     */
    explicit invalid_state(const std::string& message)
        : error{error_code::invalid_state, "Invalid state: " + message} {}
};

/**
 * Out of range error.
 */
class out_of_range : public error {
public:
    /**
     * Construct a new out-of-range error.
     *
     * @param message Error message.
     */
    explicit out_of_range(const std::string& message)
        : error{error_code::out_of_range, "Out of range: " + message} {}
};

} // namespace json
} // namespace langnes
