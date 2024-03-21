#pragma once

#include <istream>
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

namespace detail {

using traits = std::istream::traits_type;

struct isEol {
    constexpr auto operator()(char c) {
        return c == '\r' || c == '\n';
    }
};

struct isWs {
    constexpr auto operator()(char c) {
        return c == ' ' || c == '\t' || isEol{}(c);
    }
};

struct isComment {
    constexpr auto operator()(char c) {
        return c == '#' || c == ';';
    }
};

struct isCommentVariable {
    constexpr auto operator()(char c) {
        return c == '!';
    }
};

struct isDoubleQuote {
    constexpr auto operator()(char c) {
        return c == '"';
    }
};

struct isDelimiter {
    constexpr auto operator()(char c) {
        return c == '=';
    }
};

struct isDigit {
    constexpr auto operator()(char c) {
        return c >= '0' && c <= '9';
    }
};

struct isEscape {
    constexpr auto operator()(char c) {
        return c == '\\';
    }
};

struct isVariableNameToken {
    constexpr auto operator()(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '.' || isDigit{}(c);
    }
};

template<typename What>
struct negate {
    constexpr auto operator()(char c) {
        return !What{}(c);
    }
};

auto peek_next(std::istream& is) {
    auto i{is.peek()};
    if (traits::eq_int_type(i, traits::eof())) {
        throw ParserReachedEnd{};
    }
    return traits::to_char_type(i);
}

auto get_next(std::istream& is) {
    auto i{is.get()};
    if (traits::eq_int_type(i, traits::eof())) {
        throw ParserReachedEnd{};
    }
    return traits::to_char_type(i);
}

template<typename Predicate>
auto peek(std::istream& is) {
    auto c{peek_next(is)};
    return Predicate{}(c);
}

void skip(std::istream& is) {
    get_next(is);
}

template<typename Predicate>
auto next(std::istream& is, char& c) {
    return Predicate{}(c = get_next(is));
}

template<typename Predicate>
auto next(std::istream& is) {
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

void readQuotedString(std::istream& is, std::string& s) {
    expect<isDoubleQuote>(is);
    for (char c{get_next(is)};; c = get_next(is)) {
        if (isEscape{}(c)) {
            c = get_next(is);
            if (c == 'r') {
                s += '\r';
                continue;
            }
            if (c == 'n') {
                s += '\n';
                continue;
            }
            s += c;
            continue;
        }
        if (isDoubleQuote{}(c)) {
            is.unget();
            break;
        }
        s += c;
    }
    expect<isDoubleQuote>(is);
}

void readUInt32(std::istream& is, uint32_t& i) {
    std::string s;
    read_while<isDigit>(is, s);
    i = static_cast<uint32_t>(std::stoul(s));
}

template<typename Predicate>
void read_until(std::istream& is, std::string& s) {
    read_while<negate<Predicate>>(is, s);
}

std::string escape(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (char c : s) {
        if (isEol{}(c)) {
            r += '\\';
            switch (c) {
                case '\r':
                    r += 'r';
                    break;
                case '\n':
                    r += 'n';
                    break;
                default:
                    throw ParserUnexpectedToken{};
            }
            continue;
        }
        if (isEscape{}(c) || isDoubleQuote{}(c)) {
            r += '\\';
        }
        r += c;
    }
    return r;
}

std::string quote(const std::string& s) {
    std::string r;
    r.reserve(1 + s.size() + 1);
    r += '"';
    r += escape(s);
    r += '"';
    return r;
}

std::string str(const auto n) {
    return std::to_string(n);
}

}

enum class value_type {
    object,
    array,
    string,
    number,
    boolean,
    null
};

struct element {
    node_type type{JsonNodeType::indeterminate};
    size_t index{};
    size_t length{};
    std::optional<std::string> value;
};

void parse(std::vector<json_node>& nodes, std::istream& is) {
    using namespace detail;
    // Used to detect when parsing ends prematurely
    std::size_t refCount{};
    try {
        while (is.good()) {
            skip_while<isWs>(is);
            if (peek<isComment>(is)) {
                ++refCount;
                skip(is);
                if (peek<isCommentVariable>(is)) {
                    skip(is);
                    std::string name;
                    read_while<isVariableNameToken>(is, name);
                    skip_while<isWs>(is);
                    expect<isDelimiter>(is);
                    skip_while<isWs>(is);
                    std::string value;
                    readQuotedString(is, value);
                    if (name == "unknown2") {
                        resource.setUnknown2(static_cast<uint64_t>(std::stoull(value)));
                    } else {
                        throw ParserUnexpectedVariable{name};
                    }
                }
                skip_until<isEol>(is);
                --refCount;
            } else if (peek<isDoubleQuote>(is)) {
                ++refCount;
                std::string key;
                readQuotedString(is, key);
                skip_while<isWs>(is);
                expect<isDelimiter>(is);
                skip_while<isWs>(is);
                std::string value;
                readQuotedString(is, value);
                resource.addEntry(key, value);
                --refCount;
            } else if (peek<isDigit>(is)) {
                ++refCount;
                uint32_t intKey;
                readUInt32(is, intKey);
                skip_while<isWs>(is);
                expect<isDelimiter>(is);
                skip_while<isWs>(is);
                std::string value;
                readQuotedString(is, value);
                resource.addEntry(intKey, value);
                --refCount;
            } else {
                throw ParserUnexpectedToken{};
            }
        }
    } catch (const ParserReachedEnd&) {
        if (refCount > 0) {
            throw ParserUnexpectedEnd{};
        }
    }
}

}
