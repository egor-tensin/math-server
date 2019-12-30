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
        return exec_expr(exec_primary(), parser::BinaryOp::min_precedence());
    }

    double exec_expr(double lhs, unsigned min_prec) {
        for (auto op = peek_operator(); op.has_value() && op->get_precedence() >= min_prec;) {
            const auto lhs_op = *op;
            m_lexer.drop_token();
            auto rhs = exec_primary();

            for (op = peek_operator(); op.has_value(); op = peek_operator()) {
                const auto op_prec = op->get_precedence();
                const auto lhs_prec = lhs_op.get_precedence();
                const auto ok_left_assoc = op->is_left_associative() && op_prec > lhs_prec;
                const auto ok_right_assoc = op->is_right_associative() && op_prec == lhs_prec;
                const auto ok = ok_left_assoc || ok_right_assoc;

                if (!ok) {
                    break;
                }

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
            throw ParserError{"expected '-', '(' or a number"};
        }

        using Type = lexer::Token::Type;

        if (m_lexer.drop_token_of_type(Type::MINUS).has_value()) {
            return -exec_primary();
        }

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

        throw ParserError{"expected '-', '(' or a number"};
    }

    Lexer m_lexer;
};

}
