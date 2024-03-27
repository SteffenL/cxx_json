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

#include <istream>
#include <streambuf>
#include <string>

namespace langnes {
namespace json {
namespace detail {

class string_streambuf : public std::streambuf {
public:
    string_streambuf(const char* data, size_t length) {
        auto* p{const_cast<char*>(data)};
        setg(p, p, p + length);
    }

    string_streambuf(string_streambuf&& other) {
        setg(other.eback(), other.gptr(), other.egptr());
    }
};

class string_istream_data {
protected:
    string_istream_data(const char* data, size_t length)
        : m_buf{data, length} {}

    string_istream_data(const std::string& s)
        : string_istream_data{s.data(), s.size()} {}

    string_istream_data(std::string&& s)
        : m_owned_data{std::move(s)},
          m_buf{m_owned_data.data(), m_owned_data.size()} {}

    string_istream_data(string_istream_data&& other)
        : m_owned_data{std::move(other.m_owned_data)},
          m_buf{std::move(other.m_buf)} {}

    string_streambuf* get_buf() { return &m_buf; }

private:
    std::string m_owned_data;
    string_streambuf m_buf;
};

class string_istream : private string_istream_data, public std::istream {
public:
    string_istream(const char* data, size_t length)
        : string_istream_data{data, length}, std::istream{get_buf()} {}

    string_istream(const std::string& s) : string_istream{s.data(), s.size()} {}

    string_istream(std::string&& s)
        : string_istream_data{std::move(s)}, std::istream{get_buf()} {}

    string_istream(string_istream&& other)
        : string_istream_data{std::move(other)}, std::istream{get_buf()} {}
};

inline string_istream make_istream(const char* data, size_t length) {
    return {data, length};
}

template<typename Container>
inline string_istream make_istream(Container&& data) {
    return {std::forward<Container>(data)};
}

} // namespace detail
} // namespace json
} // namespace langnes
