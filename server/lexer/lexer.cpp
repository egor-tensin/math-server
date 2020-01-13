// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include "lexer.hpp"

#include "details/parse.hpp"
#include "error.hpp"
#include "token.hpp"
#include "token_type.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace math::server {

Lexer::Lexer(const std::string_view& input) : Lexer{lexer::Input{input}} {}

Lexer::Lexer(const lexer::Input& input) : m_input{input} {
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
    for_each_token([&tokens](const ParsedToken& token) {
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
    const auto type = lexer::details::parse_const_token(m_input.get_input(), token_view);
    if (!type.has_value()) {
        return {};
    }
    return ParsedToken{Token{*type}, m_input.get_pos(), token_view};
}

std::optional<Lexer::ParsedToken> Lexer::parse_number() const {
    std::string_view token_view;
    const auto number = lexer::details::parse_number(m_input.get_input(), token_view);
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

} // namespace math::server
