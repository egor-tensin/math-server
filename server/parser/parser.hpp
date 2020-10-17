// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"
#include "operator.hpp"

#include <lexer/lexer.hpp>

#include <optional>
#include <string_view>

namespace math::server {

class Parser {
public:
    using Type = lexer::Token::Type;

    // I did simple recursive descent parsing a long time ago (see
    // https://github.com/egor-tensin/simple-interpreter), this appears to be
    // a finer algorithm for parsing arithmetic expressions.
    // Reference: https://en.wikipedia.org/wiki/Operator-precedence_parser

    explicit Parser(const std::string_view& input) : m_lexer{input} {}

    double exec() {
        const auto result = exec_dmas();
        if (m_lexer.has_token()) {
            throw ParserError{"expected a binary operator"};
        }
        return result;
    }

private:
    // DMAS as in Division, Multiplication, Addition and Subtraction
    double exec_dmas() { return exec_binary_op(exec_factor(), parser::BinaryOp::min_precedence()); }

    // Exponentiation operator
    double exec_exp() {
        return exec_binary_op(exec_atom(), parser::BinaryOp::get_precedence(Type::CARET));
    }

    double exec_binary_op(double lhs, unsigned min_prec) {
        for (auto op = peek_operator(min_prec); op.has_value();) {
            const auto prev = *op;
            const auto prev_prec = prev.get_precedence();

            m_lexer.drop_token();
            auto rhs = exec_factor();

            for (op = peek_operator(min_prec); op.has_value(); op = peek_operator(min_prec)) {
                const auto next = *op;
                const auto next_prec = next.get_precedence();

                {
                    const auto acc_left_assoc = next.is_left_associative() && next_prec > prev_prec;
                    const auto acc_right_assoc =
                        next.is_right_associative() && next_prec == prev_prec;
                    const auto acc = acc_left_assoc || acc_right_assoc;

                    if (!acc) {
                        break;
                    }
                }

                rhs = exec_binary_op(rhs, next_prec);
            }

            lhs = prev.exec(lhs, rhs);
        }
        return lhs;
    }

    std::optional<parser::BinaryOp> peek_operator(unsigned min_prec) {
        const auto token = m_lexer.peek_token();
        if (!token.has_value() || !parser::BinaryOp::is(*token)) {
            return {};
        }
        const auto op = parser::BinaryOp::from_token(*token);
        if (op.get_precedence() < min_prec) {
            return {};
        }
        return op;
    }

    double exec_factor() {
        if (!m_lexer.has_token()) {
            throw ParserError{"expected '-', '+', '(' or a number"};
        }
        if (m_lexer.drop_token_of_type(Type::MINUS).has_value()) {
            return -exec_factor();
        }
        if (m_lexer.drop_token_of_type(Type::PLUS).has_value()) {
            return exec_factor();
        }
        return exec_exp();
    }

    double exec_atom() {
        if (!m_lexer.has_token()) {
            throw ParserError{"expected '-', '+', '(' or a number"};
        }

        if (m_lexer.drop_token_of_type(Type::LEFT_PAREN).has_value()) {
            const auto inner = exec_dmas();
            if (!m_lexer.has_token() ||
                !m_lexer.drop_token_of_type(Type::RIGHT_PAREN).has_value()) {
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

} // namespace math::server
