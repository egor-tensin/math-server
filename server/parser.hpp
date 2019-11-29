#pragma once

#include "error.hpp"
#include "lexer.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace math::server {
namespace parser {

class Error : public server::Error {
public:
    explicit Error(const std::string& what)
        : server::Error{"parser error: " + what}
    { }
};

class BinaryOp {
public:
    static bool is(const lexer::Token& token) {
        using Type = lexer::Token::Type;
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

    static BinaryOp from_token(const lexer::Token& token) {
        if (!is(token)) {
            throw Error{"internal: token is not a binary operator"};
        }
        return BinaryOp{token};
    }

    static constexpr unsigned min_precedence() { return 0; }

    unsigned get_precedence() const {
        using Type = lexer::Token::Type;
        switch (m_type) {
            case Type::PLUS:
            case Type::MINUS:
                return min_precedence();

            case Type::ASTERISK:
            case Type::SLASH:
                return min_precedence() + 1;

            default:
                throw Error{"internal: undefined operator precedence"};
        }
    }

    double exec(double lhs, double rhs) const {
        using Type = lexer::Token::Type;
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
                    throw Error{"division by zero"};
                }
                return lhs / rhs;
            default:
                throw Error{"internal: unsupported operator"};
        }
    }

private:
    explicit BinaryOp(const lexer::Token& token)
        : m_type{token.get_type()}
    { }

    lexer::Token::Type m_type;
};

}

class Parser {
public:
    // I did simple recursive descent parsing a long time ago (see
    // https://github.com/egor-tensin/simple-interpreter), this appears to be
    // a finer algorithm for parsing arithmetic expressions.
    // Reference: https://en.wikipedia.org/wiki/Operator-precedence_parser

    explicit Parser(const std::string_view& input)
        : m_lexer{input}
    { }

    double exec() {
        m_lexer.parse_token();
        const auto result = exec_expr();
        if (m_lexer.has_token()) {
            throw parser::Error{"expected a binary operator"};
        }
        return result;
    }

private:
    double exec_expr() {
        return exec_expr(exec_primary(), parser::BinaryOp::min_precedence());
    }

    double exec_expr(double lhs, unsigned min_prec) {
        for (auto op = peek_operator(); op.has_value() && op->get_precedence() >= min_prec;) {
            const auto lhs_op = *op;
            m_lexer.drop_token();
            auto rhs = exec_primary();

            for (op = peek_operator(); op.has_value() && op->get_precedence() > lhs_op.get_precedence(); op = peek_operator()) {
                rhs = exec_expr(rhs, op->get_precedence());
            }

            lhs = lhs_op.exec(lhs, rhs);
        }
        return lhs;
    }

    std::optional<parser::BinaryOp> peek_operator() {
        const auto token = m_lexer.peek_token();
        if (!token.has_value() || !parser::BinaryOp::is(*token)) {
            return {};
        }
        return parser::BinaryOp::from_token(*token);
    }

    double exec_primary() {
        if (!m_lexer.has_token()) {
            throw parser::Error{"expected '-', '(' or a number"};
        }

        using Type = lexer::Token::Type;

        if (m_lexer.drop_token_if(Type::MINUS).has_value()) {
            return -exec_primary();
        }

        if (m_lexer.drop_token_if(Type::LEFT_PAREN).has_value()) {
            const auto inner = exec_expr();
            if (!m_lexer.has_token() || !m_lexer.drop_token_if(Type::RIGHT_PAREN).has_value()) {
                throw parser::Error{"missing closing ')'"};
            }
            return inner;
        }

        if (const auto token = m_lexer.drop_token_if(Type::NUMBER); token.has_value()) {
            return token.value().get_number_value();
        }

        throw parser::Error{"expected '-', '(' or a number"};
    }

    Lexer m_lexer;
};

}
