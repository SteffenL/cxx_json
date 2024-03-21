#pragma once

#include <istream>
#include <stdexcept>
#include <string>

namespace parsing {

class reached_end : public std::runtime_error {
public:
    explicit reached_end()
        : runtime_error{"Reached end of file"} {}
};

class unexpected_token : public std::runtime_error {
public:
    explicit unexpected_token()
        : runtime_error{"Found unexpected token"} {}
};

inline char peek_next(std::istream& is) {
    using std_traits = std::istream::traits_type;
    auto i{is.peek()};
    if (std_traits::eq_int_type(i, std_traits::eof())) {
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
bool peek(std::istream& is, Predicate&& predicate) {
    auto c{peek_next(is)};
    return predicate(is, c);
}

inline void skip(std::istream& is) {
    get_next(is);
}

template<typename Predicate>
bool next(std::istream& is, char& c, Predicate&& predicate) {
    return predicate(is, c = get_next(is));
}

template<typename Predicate>
void expect(std::istream& is, Predicate&& predicate) {
    if (!predicate(is, get_next(is))) {
        throw unexpected_token{};
    }
}

template<typename Predicate>
void skip_while(std::istream& is, Predicate&& predicate) {
    while (predicate(is, get_next(is))) {
        // Do nothing
    }
    is.unget();
}

template<typename Predicate>
void read_while(std::istream& is, std::string& s, Predicate predicate) {
    char c;
    while (predicate(is, c = get_next(is))) {
        s += c;
    }
    is.unget();
}

inline void expect_exact(std::istream& is, const std::string &expected) {
    for (size_t i{}; i < expected.size(); ++i) {
        expect(is, [&] (std::istream& is, char c) { return c == expected[i]; });
    }
}

}
