// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"
#include "operator.hpp"

#include "../lexer/lexer.hpp"

#include <optional>
#include <string_view>

namespace math::server {

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
        const auto result = exec_expr();
        if (m_lexer.has_token()) {
            throw ParserError{"expected a binary operator"};
        }
        return result;
    }

private:
    double exec_expr() {
        return exec_expr(exec_factor(), parser::BinaryOp::min_precedence());
    }

    double exec_expr(double lhs, unsigned min_prec) {
        for (auto op = peek_operator(); op.has_value() && op->get_precedence() >= min_prec;) {
            const auto prev = *op;
            const auto prev_prec = prev.get_precedence();

            m_lexer.drop_token();
            auto rhs = exec_factor();

            for (op = peek_operator(); op.has_value(); op = peek_operator()) {
                const auto next = *op;
                const auto next_prec = next.get_precedence();

                {
                    const auto acc_left_assoc = next.is_left_associative() && next_prec > prev_prec;
                    const auto acc_right_assoc = next.is_right_associative() && next_prec == prev_prec;
                    const auto acc = acc_left_assoc || acc_right_assoc;

                    if (!acc) {
                        break;
                    }
                }

                rhs = exec_expr(rhs, next_prec);
            }

            lhs = prev.exec(lhs, rhs);
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

    double exec_factor() {
        if (!m_lexer.has_token()) {
            throw ParserError{"expected '-', '+', '(' or a number"};
        }

        using Type = lexer::Token::Type;

        if (m_lexer.drop_token_of_type(Type::MINUS).has_value()) {
            return -exec_factor();
        }
        if (m_lexer.drop_token_of_type(Type::PLUS).has_value()) {
            return exec_factor();
        }
        return exec_atom();
    }

    double exec_atom() {
        if (!m_lexer.has_token()) {
            throw ParserError{"expected '-', '+', '(' or a number"};
        }

        using Type = lexer::Token::Type;

        if (m_lexer.drop_token_of_type(Type::LEFT_PAREN).has_value()) {
            const auto inner = exec_expr();
            if (!m_lexer.has_token() || !m_lexer.drop_token_of_type(Type::RIGHT_PAREN).has_value()) {
                throw ParserError{"missing closing ')'"};
            }
            return inner;
        }

        if (const auto token = m_lexer.drop_token_of_type(Type::NUMBER); token.has_value()) {
            return token.value().as_number();
        }

        throw ParserError{"expected '-', '+', '(' or a number"};
    }

    Lexer m_lexer;
};

}
