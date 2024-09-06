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

#include <istream>
#include <streambuf>
#include <string>

LANGNES_JSON_CXX_NS_BEGIN
namespace detail {

class string_streambuf : public std::streambuf {
public:
    string_streambuf(const char* data, size_t length) noexcept {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        auto* p{const_cast<char*>(data)};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        setg(p, p, p + length);
    }

    string_streambuf(const string_streambuf&) = delete;

    string_streambuf(string_streambuf&& other) noexcept {
        setg(other.eback(), other.gptr(), other.egptr());
    }

    string_streambuf& operator=(const string_streambuf&) = delete;
    string_streambuf& operator=(string_streambuf&&) = delete;
    ~string_streambuf() override = default;
};

class string_istream_data {
public:
    string_istream_data(const char* data, size_t length) noexcept
        : m_buf{data, length} {}

    explicit string_istream_data(const std::string& s) noexcept
        : string_istream_data{s.data(), s.size()} {}

    explicit string_istream_data(std::string&& s) noexcept
        : m_owned_data{std::move(s)},
          m_buf{m_owned_data.data(), m_owned_data.size()} {}

    string_istream_data(const string_istream_data&) = delete;

    string_istream_data(string_istream_data&& other) noexcept
        : m_owned_data{std::move(other.m_owned_data)},
          m_buf{std::move(other.m_buf)} {}

    string_istream_data& operator=(const string_istream_data&) = delete;
    string_istream_data& operator=(string_istream_data&& other) = delete;
    ~string_istream_data() = default;

    string_streambuf* get_buf() noexcept { return &m_buf; }

private:
    std::string m_owned_data;
    string_streambuf m_buf;
};

class string_istream : private string_istream_data, public std::istream {
public:
    explicit string_istream(const char* data, size_t length) noexcept
        : string_istream_data{data, length},
          std::istream{get_buf()} {}

    explicit string_istream(const std::string& s) noexcept
        : string_istream{s.data(), s.size()} {}

    explicit string_istream(std::string&& s) noexcept
        : string_istream_data{std::move(s)},
          std::istream{get_buf()} {}

    string_istream(const string_istream&) = delete;

    string_istream(string_istream&& other) noexcept
        : string_istream_data{std::move(other)},
          std::istream{get_buf()} {}

    string_istream& operator=(const string_istream&) = delete;
    string_istream& operator=(string_istream&&) = delete;
    ~string_istream() override = default;
};

inline string_istream make_istream(const char* data, size_t length) noexcept {
    return string_istream{data, length};
}

template<typename Container>
inline string_istream make_istream(Container&& data) noexcept {
    return string_istream{std::forward<Container>(data)};
}

} // namespace detail
LANGNES_JSON_CXX_NS_END
