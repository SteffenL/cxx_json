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

enum class node_type {
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

struct node {
    node_type type{};
    location location;
};

namespace detail {

using traits = std::istream::traits_type;

struct eol { constexpr bool operator()(char c) const { return c == '\r' || c == '\n'; } };
struct ws { constexpr bool operator()(char c) const { return c == ' ' || c == '\t' || eol{}(c); }; };
struct dquote { constexpr bool operator()(char c) const { return c == '"'; } };
struct digit { constexpr bool operator()(char c) const { return c >= '0' && c <= '9'; } };
struct escape { constexpr bool operator()(char c) const { return c == '\\'; } };
struct object_open { constexpr bool operator()(char c) const { return c == '{'; } };
struct object_close { constexpr bool operator()(char c) const { return c == '}'; } };
struct array_open { constexpr bool operator()(char c) const { return c == '['; } };
struct array_close { constexpr bool operator()(char c) const { return c == ']'; } };
struct value_separator { constexpr bool operator()(char c) const { return c == ','; } };
struct member_separator { constexpr bool operator()(char c) const { return c == ':'; } };

//bool is(bool(*pred)(char c))

template<typename What>
struct negate {
    constexpr bool operator()(char c) const {
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

node parse_value(std::istream& is);

inline node parse_string(std::istream& is) {
    node v;
    v.type = node_type::string;
    v.location.index = get_offset(is);
    expect<dquote>(is);
    for (char c{get_next(is)};; c = get_next(is)) {
        if (escape{}(c)) {
            get_next(is);
            continue;
        }
        if (dquote{}(c)) {
            is.unget();
            break;
        }
    }
    expect<dquote>(is);
    v.location.length = get_offset(is) - v.location.index;
    return v;
}

inline node parse_object(std::istream& is) {
    node v;
    v.type = node_type::object;
    v.location.index = get_offset(is);
    expect<object_open>(is);
    skip_while<ws>(is);
    if (peek<object_close>(is)) {
        expect<object_close>(is);
        v.location.length = get_offset(is) - v.location.index;
        return v;
    }
    while (true) {
        if (!peek<dquote>(is)) {
            throw ParserUnexpectedToken{};
        }
        parse_string(is);
        skip_while<ws>(is);
        expect<member_separator>(is);
        parse_value(is);
        if (peek<value_separator>(is)) {
            skip(is);
            skip_while<ws>(is);
            continue;
        }
        break;
    }
    skip_while<ws>(is);
    expect<object_close>(is);
    v.location.length = get_offset(is) - v.location.index;
    return v;
}

/*inline array read_array(std::istream& is) {
    array v;
    v.index = get_offset(is);
    expect<array_open>(is);
    expect<array_close>(is);
    v.length = get_offset(is) - v.index;
    return {};
}*/

inline node parse_value(std::istream& is) {
    skip_while<ws>(is);
    if (peek<dquote>(is)) {
        return parse_string(is);
    } else if (peek<object_open>(is)) {
        return parse_object(is);
    } else if (peek<array_open>(is)) {
        //return read_array(is);
    }
    throw ParserUnexpectedToken{};
}

}

inline std::string get_data(const location& loc, std::istream& is) {
    std::string data;
    data.resize(loc.length);
    is.seekg(loc.index, std::ios::beg);
    is.read(&data[0], data.size());
    return data;
}

inline node parse(std::istream& is) {
    using namespace detail;
    return parse_value(is);
}

inline node parse_at(location loc, std::istream& is) {
    using namespace detail;
    is.seekg(static_cast<std::streamoff>(loc.index), std::ios::beg);
    return parse(is);
}

}
