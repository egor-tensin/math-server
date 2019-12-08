#include "error.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include "token_type.hpp"

#include <exception>
#include <optional>
#include <regex>
#include <string_view>
#include <string>
#include <vector>

namespace math::server {
namespace lexer {
namespace {

std::string_view match_number(const std::string_view& input) {
    static constexpr std::regex::flag_type flags =
        std::regex_constants::ECMAScript |
        std::regex_constants::icase;
    // This is a hacky attempt to describe a C-like grammar for floating-point
    // numbers using a regex (the tests seem to pass though).
    // A proper NFA would be better, I guess.
    static const std::regex number_regex{R"REGEX(^(?:\d+(?:\.\d*)?|\.\d+)(e[+-]?(\d*))?)REGEX", flags};

    std::cmatch match;
    {
        const auto begin = input.data();
        const auto end = begin + input.length();
        if (!std::regex_search(begin, end, match, number_regex)) {
            return {};
        }
    }
    {
        // If we have the numeric part of a number followed by 'e' and no digits,
        // 1) that 'e' definitely belongs to this number token,
        // 2) the user forgot to type in the required digits.
        const auto& exponent = match[1];
        const auto& abs_power = match[2];
        if (exponent.matched && abs_power.matched && abs_power.length() == 0) {
            throw LexerError{"exponent has no digits: " + match[0].str()};
        }
    }
    return {match[0].first, static_cast<std::size_t>(match[0].length())};
}

std::optional<double> parse_number(const std::string_view& input, std::string_view& token) {
    const auto view = match_number(input);
    if (!view.data()) {
        return {};
    }
    try {
        const auto result = std::stod(std::string{view});
        token = view;
        return result;
    } catch (const std::exception&) {
        throw LexerError{"internal: couldn't parse number from: " + std::string{view}};
    }
    return {};
}

bool starts_with(const std::string_view& a, const std::string_view& b) noexcept {
    return a.length() >= b.length()
        && a.compare(0, b.length(), b) == 0;
}

std::optional<token::Type> parse_const_token(const std::string_view& input, std::string_view& token) {
    for (const auto type : token::const_tokens()) {
        const auto str = token::type_to_string(type);
        if (starts_with(input, str)) {
            token = std::string_view(input.data(), str.length());
            return {type};
        }
    }
    return {};
}

}

namespace details {

std::optional<double> parse_number(const std::string_view& input) {
    std::string_view token;
    return lexer::parse_number(input, token);
}

std::optional<token::Type> parse_const_token(const std::string_view& input) {
    std::string_view token;
    return lexer::parse_const_token(input, token);
}

std::string_view parse_whitespace(const std::string_view& input) {
    static const std::regex ws_regex{R"(^\s+)"};

    std::cmatch match;
    {
        const auto begin = input.data();
        const auto end = begin + input.length();
        if (std::regex_search(begin, end, match, ws_regex)) {
            return std::string_view(match[0].first, match[0].length());
        }
    }
    return {};
}

}
}

Lexer::Lexer(const std::string_view& input)
    : Lexer{lexer::Input{input}} {
}

Lexer::Lexer(const lexer::Input& input)
    : m_input{input} {

    consume_token();
}

bool Lexer::for_each_token(const TokenProcessor& process) {
    for (auto token = peek_token(); token.has_value(); drop_token(), token = peek_token()) {
        if (!process(*token)) {
            return false;
        }
    }
    return true;
}

std::vector<Lexer::ParsedToken> Lexer::get_tokens() {
    std::vector<ParsedToken> tokens;
    for_each_token([&tokens] (const ParsedToken& token) {
        tokens.emplace_back(token);
        return true;
    });
    return tokens;
}

void Lexer::drop_token() {
    if (!has_token()) {
        throw LexerError{"internal: no tokens to drop"};
    }
    m_token_buffer = {};
    consume_token();
}

std::optional<Lexer::ParsedToken> Lexer::drop_token_of_type(Type type) {
    if (!has_token()) {
        throw LexerError{"internal: no tokens to drop"};
    }
    if (m_token_buffer.value().get_type() != type) {
        return {};
    }
    const auto result = m_token_buffer;
    drop_token();
    return result;
}

void Lexer::consume_whitespace() {
    const auto ws = parse_whitespace();
    if (!ws.has_value()) {
        return;
    }
    m_input.consume(ws->get_length());
}

void Lexer::consume_token() {
    if (m_input.empty()) {
        return;
    }
    consume_whitespace();
    if (m_input.empty()) {
        return;
    }
    auto token{parse_token()};
    m_input.consume(token.get_length());
    m_token_buffer = std::move(token);
}

std::optional<Lexer::ParsedToken> Lexer::parse_whitespace() const {
    const auto token_view = lexer::details::parse_whitespace(m_input.get_input());
    if (token_view.empty()) {
        return {};
    }
    return ParsedToken{Token{Token::Type::WHITESPACE}, m_input.get_pos(), token_view};
}

std::optional<Lexer::ParsedToken> Lexer::parse_const_token() const {
    std::string_view token_view;
    const auto type = lexer::parse_const_token(m_input.get_input(), token_view);
    if (!type.has_value()) {
        return {};
    }
    return ParsedToken{Token{*type}, m_input.get_pos(), token_view};
}

std::optional<Lexer::ParsedToken> Lexer::parse_number() const {
    std::string_view token_view;
    const auto number = lexer::parse_number(m_input.get_input(), token_view);
    if (!number.has_value()) {
        return {};
    }
    return ParsedToken{Token{*number}, m_input.get_pos(), token_view};
}

Lexer::ParsedToken Lexer::parse_token() const {
    if (const auto const_token = parse_const_token(); const_token.has_value()) {
        return *const_token;
    }
    if (const auto number = parse_number(); number.has_value()) {
        return *number;
    }
    throw LexerError{"invalid input at: " + std::string{m_input.get_input()}};
}

}
