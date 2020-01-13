// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "token_type.hpp"

#include <cstddef>
#include <string_view>
#include <utility>
#include <variant>

namespace math::server::lexer {

class Token {
public:
    using Type = token::Type;

    explicit Token(token::Type type);
    explicit Token(double value);

    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const { return !(*this == other); }

    Type get_type() const { return m_type; }

    double as_number() const;

private:
    token::Type m_type;
    std::variant<double> m_value;

    friend std::ostream& operator<<(std::ostream&, const Token&);
};

class ParsedToken : public Token {
public:
    ParsedToken(Token&& token, std::size_t pos, const std::string_view& view)
        : Token{std::move(token)}, m_pos{pos}, m_view{view} {}

    std::size_t get_pos() const { return m_pos; }

    std::size_t get_length() const { return m_view.length(); }

private:
    std::size_t m_pos;
    std::string_view m_view;
};

} // namespace math::server::lexer
