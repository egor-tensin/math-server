#pragma once

#include "error.hpp"

#include <cmath>

#include <exception>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace math::server {
namespace lexer {

class Error : public server::Error {
public:
    explicit Error(const std::string &what)
        : server::Error{"lexer error: " + what}
    { }
};

class Token {
public:
    enum class Type {
        LEFT_PAREN,
        RIGHT_PAREN,
        PLUS,
        MINUS,
        ASTERISK,
        SLASH,
        NUMBER,
    };

    explicit Token(Type type)
        : m_type{type}, m_number_value{nan()}
    { }

    explicit Token(double number_value)
        : m_type{Type::NUMBER}, m_number_value{number_value}
    { }

    bool operator==(const Token& other) const {
        return m_type == other.m_type
            && ((is_nan(m_number_value) && is_nan(other.m_number_value))
                || m_number_value == other.m_number_value);
    }

    bool operator!=(const Token& other) const { return !(*this == other); }

    Type get_type() const { return m_type; }

    double get_number_value() const {
        if (get_type() != Type::NUMBER) {
            throw Error{"token must be a number to query its value"};
        }
        return m_number_value;
    }

private:
    static constexpr double nan() { return std::numeric_limits<double>::quiet_NaN(); }

    static bool is_nan(double x) { return std::isnan(x); }

    Type m_type;
    double m_number_value;
};

namespace details {

inline std::string_view match_number(const std::string_view& input) {
    static constexpr std::regex::flag_type flags =
        std::regex_constants::ECMAScript |
        std::regex_constants::icase;
    // This is a hacky attempt to describe a C-like grammar for floating-point
    // numbers using a regex (the tests seem to pass though).
    // A proper NFA would be better, I guess.
    static const std::regex number_regex{R"REGEX(^(?:\d+(?:\.\d*)?|\.\d+)(e[+-]?(\d*))?)REGEX", flags};

    std::cmatch match;
    if (!std::regex_search(input.cbegin(), input.cend(), match, number_regex)) {
        return {};
    }
    // If we have the numeric part of a number followed by 'e' and no digits,
    // 1) that 'e' definitely belongs to this number token,
    // 2) the user forgot to type in the required digits.
    const auto& exponent = match[1];
    const auto& abs_power = match[2];
    if (exponent.matched && abs_power.matched && abs_power.length() == 0) {
        throw lexer::Error{"exponent has no digits: " + match[0].str()};
    }
    return {match[0].first, match[0].length()};
}

inline std::optional<double> parse_number(const std::string_view& input, std::string_view& token) {
    const auto view = match_number(input);
    if (!view.data()) {
        return {};
    }
    try {
        const auto result = std::stod(std::string{view});
        token = view;
        return result;
    } catch (const std::exception& e) {
        throw lexer::Error{"couldn't parse number from: " + std::string{view}};
    }
    return {};
}

inline std::optional<double> parse_number(const std::string_view& input) {
    std::string_view token;
    return parse_number(input, token);
}

inline bool starts_with(const std::string_view& a, const std::string_view& b) noexcept {
    return a.length() >= b.length()
        && a.compare(0, b.length(), b) == 0;
}

inline std::optional<Token::Type> parse_const_token(const std::string_view& input, std::string_view& token) {
    // FIXME: Potentially error-prone if there's const token A which is a
    // prefix of token B (if the map is not sorted, we'd parse token A, when it
    // could've been token B).
    // Can be solved by sorting the keys accordingly.

    static const std::unordered_map<std::string_view, Token::Type> const_tokens{
        {"(", Token::Type::LEFT_PAREN},
        {")", Token::Type::RIGHT_PAREN},
        {"+", Token::Type::PLUS},
        {"-", Token::Type::MINUS},
        {"*", Token::Type::ASTERISK},
        {"/", Token::Type::SLASH},
    };

    for (const auto& it : const_tokens) {
        const auto& str = it.first;
        const auto& type = it.second;

        if (starts_with(input, str)) {
            token = input.substr(0, str.length());
            return type;
        }
    }

    return {};
}

inline std::optional<Token::Type> parse_const_token(const std::string_view& input) {
    std::string_view token;
    return parse_const_token(input, token);
}

inline std::string_view parse_whitespace(const std::string_view& input) {
    static const std::regex ws_regex{R"(\s*)"};

    std::cmatch match;
    if (std::regex_search(input.cbegin(), input.cend(), match, ws_regex)) {
        return {match[0].first, match[0].length()};
    }
    return {};
}

}

}

class Lexer {
public:
    explicit Lexer(const std::string_view& input)
        : m_input{input} {
    }

    using TokenProcessor = std::function<bool (const lexer::Token&)>;

    bool for_each_token(const TokenProcessor& process) {
        parse_token();
        for (auto token = peek_token(); token.has_value(); drop_token(), token = peek_token()) {
            if (!process(*token)) {
                return false;
            }
        }
        return true;
    }

    std::vector<lexer::Token> get_tokens() {
        std::vector<lexer::Token> tokens;
        for_each_token([&tokens] (const lexer::Token& token) {
            tokens.emplace_back(token);
            return true;
        });
        return tokens;
    }

    void parse_token() {
        if (m_input.length() == 0) {
            return;
        }
        std::string_view token_view;
        m_token_buffer = parse_token(token_view);
        if (m_token_buffer.has_value()) {
            m_input.remove_prefix(token_view.length());
        }
    }

    bool has_token() const {
        return peek_token().has_value();
    }

    std::optional<lexer::Token> peek_token() const {
        return m_token_buffer;
    }

    void drop_token() {
        if (!has_token()) {
            throw lexer::Error{"internal: no tokens to drop"};
        }
        m_token_buffer = {};
        parse_token();
    }

    std::optional<lexer::Token> drop_token_if(lexer::Token::Type type) {
        if (!has_token()) {
            throw lexer::Error{"internal: no tokens to drop"};
        }
        if (m_token_buffer.value().get_type() != type) {
            return {};
        }
        const auto result = m_token_buffer;
        drop_token();
        return result;
    }

private:
    void consume_whitespace() {
        const auto ws = lexer::details::parse_whitespace(m_input);
        m_input.remove_prefix(ws.length());
    }

    std::optional<lexer::Token> parse_token(std::string_view& token_view) {
        consume_whitespace();
        if (m_input.length() == 0) {
            return {};
        }
        if (const auto const_token = lexer::details::parse_const_token(m_input, token_view); const_token.has_value()) {
            return lexer::Token{*const_token};
        }
        if (const auto number = lexer::details::parse_number(m_input, token_view); number.has_value()) {
            return lexer::Token{*number};
        }
        throw lexer::Error{"invalid input at: " + std::string{m_input}};
    }

    std::string_view m_input;
    std::optional<lexer::Token> m_token_buffer;
};

}
