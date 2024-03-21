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

template<typename What>
struct negate {
    constexpr bool operator()(char c) const {
        return !What{}(c);
    }
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
    return predicate(c);
}

template<typename Predicate>
bool peek(std::istream& is) {
    return peek<Predicate>(is, Predicate{});
}

inline void skip(std::istream& is) {
    get_next(is);
}

template<typename Predicate>
bool next(std::istream& is, char& c, Predicate&& predicate) {
    return predicate(c = get_next(is));
}

template<typename Predicate>
bool next(std::istream& is, char& c) {
    return next<Predicate>(is, c, Predicate{});
}

template<typename Predicate>
bool next(std::istream& is, Predicate predicate) {
    char c;
    return next(is, c, predicate);
}

template<typename Predicate>
bool next(std::istream& is) {
    return next<Predicate>(is, Predicate{});
}

template<typename Predicate>
void expect(std::istream& is, Predicate predicate) {
    if (!next(is, predicate)) {
        throw unexpected_token{};
    }
}

template<typename Predicate>
void expect(std::istream& is) {
    next<Predicate>(is, Predicate{});
}

template<typename Predicate>
void skip_while(std::istream& is, Predicate predicate) {
    while (next<Predicate>(is, predicate)) {
        // Do nothing
    }
    is.unget();
}

template<typename Predicate>
void skip_while(std::istream& is) {
    skip_while<Predicate>(is, Predicate{});
}

template<typename Predicate>
void read_while(std::istream& is, std::string& s, Predicate predicate) {
    char c;
    while (next<Predicate>(is, c)) {
        s += c;
    }
    is.unget();
}

template<typename Predicate>
void read_while(std::istream& is, std::string& s) {
    read_while<Predicate>(is, s, Predicate{});
}

inline void expect_exact(std::istream& is, const std::string &expected) {
    for (size_t i{}; i < expected.size(); ++i) {
        expect(is, [&] (char c) { return c == expected[i]; });
    }
}

}
