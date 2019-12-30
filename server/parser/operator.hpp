// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"

#include "../lexer/token.hpp"
#include "../lexer/token_type.hpp"

#include <cmath>

namespace math::server::parser {

class BinaryOp {
public:
    using Token = lexer::Token;
    using Type = Token::Type;

    static bool is(const Token& token) {
        switch (token.get_type()) {
            case Type::PLUS:
            case Type::MINUS:
            case Type::ASTERISK:
            case Type::SLASH:
            case Type::CARET:
                return true;

            default:
                return false;
        }
    }

    static BinaryOp from_token(const Token& token) {
        if (!is(token)) {
            throw ParserError{"internal: token is not a binary operator"};
        }
        return BinaryOp{token};
    }

    static constexpr unsigned min_precedence() { return 0; }

    unsigned get_precedence() const {
        switch (m_type) {
            case Type::PLUS:
            case Type::MINUS:
                return min_precedence();

            case Type::ASTERISK:
            case Type::SLASH:
                return min_precedence() + 1;

            case Type::CARET:
                return min_precedence() + 2;

            default:
                throw ParserError{"internal: undefined operator precedence"};
        }
    }

    bool is_right_associative() const {
        switch (m_type) {
            case Type::CARET:
                return true;

            default:
                return false;
        }
    }

    bool is_left_associative() const {
        return !is_right_associative();
    }

    double exec(double lhs, double rhs) const {
        switch (m_type) {
            case Type::PLUS:
                return lhs + rhs;

            case Type::MINUS:
                return lhs - rhs;

            case Type::ASTERISK:
                return lhs * rhs;

            case Type::SLASH:
                // Trapping the CPU would be better?
                if (rhs == 0.) {
                    throw ParserError{"division by zero"};
                }
                return lhs / rhs;

            case Type::CARET:
                return std::pow(lhs, rhs);

            default:
                throw ParserError{"internal: unsupported operator"};
        }
    }

private:
    explicit BinaryOp(const Token& token)
        : m_type{token.get_type()}
    { }

    Type m_type;
};

}
