#pragma once

#include "error.hpp"

#include "../lexer/token.hpp"
#include "../lexer/token_type.hpp"

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

            default:
                throw ParserError{"internal: undefined operator precedence"};
        }
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
