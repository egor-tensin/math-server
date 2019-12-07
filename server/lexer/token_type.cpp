#include "error.hpp"
#include "token_type.hpp"

#include <functional>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace math::server::lexer::token {
namespace {

using ToStringMap = std::unordered_map<Type, std::string>;
using FromStringMap = std::map<std::string, Type, std::greater<std::string>>;

class ToStringConverter {
public:
    ToStringConverter() : m_map{to_string_map()} {
        validate();
    }

    const ToStringMap& map() const { return m_map; }

private:
    static const ToStringMap& to_string_map() {
        static const ToStringMap map{
            {Type::WHITESPACE, "whitespace"},
            {Type::PLUS, "+"},
            {Type::MINUS, "-"},
            {Type::ASTERISK, "*"},
            {Type::SLASH, "/"},
            {Type::LEFT_PAREN, "("},
            {Type::RIGHT_PAREN, ")"},
            {Type::NUMBER, "number"},
        };
        return map;
    }

    void validate() const {
        check_for_duplicates();
    }

    void check_for_duplicates() const {
        std::unordered_set<std::string> strings;
        for (const auto& [type, str] : m_map) {
            const auto [_, inserted] = strings.emplace(str);
            if (!inserted) {
                throw std::logic_error{"multiple tokens have the same string representation: " + str};
            }
        }
    }

    const ToStringMap& m_map;
};

const ToStringMap& to_string_map() {
    static const ToStringConverter converter;
    return converter.map();
}

class FromStringConverter {
public:
    FromStringConverter(const ToStringMap& to_string)
        : m_map{build_map(to_string)} {
    }

    const FromStringMap& map() const { return m_map; }

private:
    static FromStringMap build_map(const ToStringMap& to_string) {
        FromStringMap from_string;
        for (const auto& [type, str] : to_string) {
            const auto [_, inserted] = from_string.emplace(str, type);
            if (!inserted) {
                throw std::logic_error{"multiple tokens have the same string representation: " + str};
            }
        }
        return from_string;
    }

    FromStringMap m_map;
};

const FromStringMap& from_string_map() {
    static const FromStringConverter converter{to_string_map()};
    return converter.map();
}

class ConstTokens {
public:
    ConstTokens() {
        const auto& map = to_string_map();
        for (const auto& [type, _] : map) {
            if (is_const_token(type)) {
                m_set.emplace(type);
            }
        }
    }

    const TypeSet& set() const { return m_set; }

private:
    TypeSet m_set;
};

}

TypeInt type_to_int(Type type) {
    return static_cast<TypeInt>(type);
}

std::string type_to_int_string(Type type) {
    return std::to_string(type_to_int(type));
}

bool is_const_token(Type type) {
    switch (type) {
        case Type::WHITESPACE:
        case Type::NUMBER:
            return false;
        default:
            return true;
    }
}

const TypeSet& const_tokens() {
    static const ConstTokens tokens;
    return tokens.set();
}

bool token_has_value(Type type) {
    switch (type) {
        case Type::NUMBER:
            return true;
        default:
            return false;
    }
}

std::string type_to_string(Type type) {
    const auto& map = to_string_map();
    const auto it = map.find(type);
    if (it == map.cend()) {
        throw LexerError{"type_to_string: unsupported token type: " + type_to_int_string(type)};
    }
    return it->second;
}

Type type_from_string(const std::string& src) {
    const auto& map = from_string_map();
    const auto it = map.find(src);
    if (it == map.cend()) {
        throw LexerError{"type_from_string: unsupported token: " + std::string{src}};
    }
    return it->second;
}

std::ostream& operator<<(std::ostream& os, const Type& type) {
    os << type_to_int(type);
    return os;
}

}
