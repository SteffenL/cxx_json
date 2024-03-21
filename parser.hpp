#pragma once

#include <any>
#include <istream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace json {

class ParserReachedEnd : public std::runtime_error {
public:
    explicit ParserReachedEnd()
        : runtime_error{"Parser reached end of file"} {}
};

class ParserUnexpectedToken : public std::runtime_error {
public:
    explicit ParserUnexpectedToken()
        : runtime_error{"Parser found unexpected token"} {}
};

class ParserUnexpectedEnd : public std::runtime_error {
public:
    explicit ParserUnexpectedEnd()
        : runtime_error{"Parser reached end of file unexpectedly"} {}
};

class ParserUnexpectedVariable : public std::runtime_error {
public:
    explicit ParserUnexpectedVariable(const std::string& name)
        : runtime_error{"Parser found unexpected variable: " + name} {}
};

enum class value_type {
    object,
    array,
    string,
    number,
    boolean,
    null
};

struct location {
    size_t index{};
    size_t length{};
};

struct string {
    location outer;
    location inner;
};

struct value {
    value_type type{};
    location outer;
    location inner;
};

struct member {
    string name;
    location value;
};

struct object {
    location outer;
    std::vector<member> members;
};

struct array {
    location outer;
    std::vector<location> elements;
};

namespace detail {

using traits = std::istream::traits_type;

struct is_eol { constexpr bool operator()(char c) { return c == '\r' || c == '\n'; } };
struct is_ws { constexpr bool operator()(char c) { return c == ' ' || c == '\t' || is_eol{}(c); }; };
struct is_double_quote { constexpr bool operator()(char c) { return c == '"'; } };
struct is_digit { constexpr bool operator()(char c) { return c >= '0' && c <= '9'; } };
struct is_escape { constexpr bool operator()(char c) { return c == '\\'; } };
struct is_object_open { constexpr bool operator()(char c) { return c == '{'; } };
struct is_object_close { constexpr bool operator()(char c) { return c == '}'; } };
struct is_array_open { constexpr bool operator()(char c) { return c == '['; } };
struct is_array_close { constexpr bool operator()(char c) { return c == ']'; } };
struct is_value_separator { constexpr bool operator()(char c) { return c == ','; } };
struct is_member_separator { constexpr bool operator()(char c) { return c == ':'; } };

//bool is(bool(*pred)(char c))

template<typename What>
struct negate {
    constexpr bool operator()(char c) {
        return !What{}(c);
    }
};

inline char peek_next(std::istream& is) {
    auto i{is.peek()};
    if (traits::eq_int_type(i, traits::eof())) {
        throw ParserReachedEnd{};
    }
    return traits::to_char_type(i);
}

inline char get_next(std::istream& is) {
    auto i{is.get()};
    if (traits::eq_int_type(i, traits::eof())) {
        throw ParserReachedEnd{};
    }
    return traits::to_char_type(i);
}

template<typename Predicate>
bool peek(std::istream& is) {
    auto c{peek_next(is)};
    return Predicate{}(c);
}

inline void skip(std::istream& is) {
    get_next(is);
}

template<typename Predicate>
char next(std::istream& is, char& c) {
    return Predicate{}(c = get_next(is));
}

template<typename Predicate>
char next(std::istream& is) {
    char c;
    return next<Predicate>(is, c);
}

template<typename Predicate>
void expect(std::istream& is) {
    if (!next<Predicate>(is)) {
        throw ParserUnexpectedToken{};
    }
}

template<typename Predicate>
void skip_while(std::istream& is) {
    while (next<Predicate>(is)) {
        // Do nothing
    }
    is.unget();
}

template<typename Predicate>
void skip_until(std::istream& is) {
    skip_while<negate<Predicate>>(is);
}

template<typename Predicate>
void read_while(std::istream& is, std::string& s) {
    char c;
    while (next<Predicate>(is, c)) {
        s += c;
    }
    is.unget();
}

template<typename Predicate>
void read_until(std::istream& is, std::string& s) {
    read_while<negate<Predicate>>(is, s);
}

template<typename T>
std::string str(const T& n) {
    return std::to_string(n);
}

inline size_t get_offset(std::istream& is) {
    auto pos{is.tellg()};
    if (pos < 0) {
        throw ParserUnexpectedEnd{};
    }
    return static_cast<size_t>(pos);
}

value read_value(std::istream& is);

inline string read_string(std::istream& is) {
    string v;
    v.outer.index = get_offset(is);
    expect<is_double_quote>(is);
    v.inner.index = get_offset(is);
    for (char c{get_next(is)};; c = get_next(is)) {
        if (is_escape{}(c)) {
            get_next(is);
            v.inner.length += 2;
            continue;
        }
        if (is_double_quote{}(c)) {
            is.unget();
            v.inner.length = get_offset(is) - v.inner.index;
            break;
        }
    }
    expect<is_double_quote>(is);
    v.outer.length = get_offset(is) - v.outer.index;
    return v;
}

inline object read_object(std::istream& is) {
    object v;
    std::vector<member> members;
    expect<is_object_open>(is);
    skip_while<is_ws>(is);
    bool done{};
    while (!done) {
        if (peek<is_double_quote>(is)) {
            auto member_name{read_string(is)};
            skip_while<is_ws>(is);
            expect<is_member_separator>(is);
            auto member_value{read_value(is)};
            done = !peek<is_value_separator>(is);
            members.push_back(member{member_name, member_value});
        }
    }
    expect<is_object_close>(is);
    return {};
}

inline array read_array(std::istream& is) {
    expect<is_array_open>(is);
    expect<is_array_close>(is);
    return {};
}

inline value read_value(std::istream& is) {
    value v;
    skip_while<is_ws>(is);
    if (peek<is_double_quote>(is)) {
        v.type = value_type::string;
        //v.outer = read_string(is).outer;
    } else if (peek<is_object_open>(is)) {
        v.type = value_type::object;
        //v.outer = read_object(is).outer;
    } else if (peek<is_array_open>(is)) {
        v.type = value_type::array;
        //v.outer = read_array(is);
    }
    skip_while<is_ws>(is);
    return v;
}

}

inline value parse(std::istream& is) {
    using namespace detail;
    return read_value(is);
}

}
