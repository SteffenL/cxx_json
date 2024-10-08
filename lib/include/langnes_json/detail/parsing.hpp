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
#include "macros.hpp"

#include <istream>
#include <string>

LANGNES_JSON_CXX_NS_BEGIN
namespace detail {
namespace parsing {

class reached_end : public parse_error {
public:
    reached_end() : parse_error{"Reached end of input"} {}
};

class unexpected_token : public parse_error {
public:
    unexpected_token() : parse_error{"Found unexpected token"} {}
};

inline bool is_eof(int c) {
    using std_traits = std::istream::traits_type;
    return std_traits::eq_int_type(c, std_traits::eof());
}

inline bool has_reached_end(std::istream& is) { return is_eof(is.peek()); }

inline char peek_next(std::istream& is) {
    using std_traits = std::istream::traits_type;
    auto i{is.peek()};
    if (is_eof(i)) {
        throw reached_end{};
    }
    return std_traits::to_char_type(i);
}

inline char get_next(std::istream& is) {
    using std_traits = std::istream::traits_type;
    auto i{is.get()};
    if (std_traits::eq_int_type(i, std_traits::eof())) {
        throw reached_end{};
    }
    return std_traits::to_char_type(i);
}

template<typename Predicate>
bool peek(std::istream& is, Predicate predicate) {
    auto c{peek_next(is)};
    return predicate(is, c);
}

inline void skip(std::istream& is) { get_next(is); }

template<typename Predicate>
bool next(std::istream& is, char& c, Predicate predicate) {
    return predicate(is, c = get_next(is));
}

template<typename Predicate>
void expect(std::istream& is, Predicate predicate) {
    if (!predicate(is, get_next(is))) {
        throw unexpected_token{};
    }
}

inline void expect_fully_consumed(std::istream& is) {
    try {
        peek_next(is);
        throw unexpected_token{};
    } catch (const reached_end&) { // NOLINT(bugprone-empty-catch)
        // We want to be at the end of the stream
    }
}

template<typename Predicate>
void skip_while(std::istream& is, Predicate predicate) {
    is.peek();
    while (is.good() && predicate(is, peek_next(is))) {
        skip(is);
        is.peek();
    }
}

template<typename Predicate>
void read_while(std::istream& is, std::string& s, Predicate predicate) {
    is.peek();
    char c{};
    while (is.good() && predicate(is, c = peek_next(is))) {
        skip(is);
        s += c;
        is.peek();
    }
}

inline void expect_exact(std::istream& is, const std::string& expected) {
    for (size_t i{}; i < expected.size(); ++i) {
        expect(is, [&](std::istream& /*unused*/, char c) {
            return c == expected[i];
        });
    }
}

} // namespace parsing
} // namespace detail
LANGNES_JSON_CXX_NS_END
