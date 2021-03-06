// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include <lexer/error.hpp>
#include <lexer/token.hpp>
#include <lexer/token_type.hpp>

#include <cmath>
#include <limits>
#include <variant>

namespace math::server::lexer {
namespace {

constexpr double nan() {
    return std::numeric_limits<double>::quiet_NaN();
}

bool is_nan(double x) {
    return std::isnan(x);
}

bool numbers_equal(double x, double y) {
    if (is_nan(x) && is_nan(y)) {
        return true;
    }
    return x == y;
}

} // namespace

Token::Token(Type type) : m_type{type} {
    if (token::token_has_value(type)) {
        throw LexerError{"internal: must have a value: " + token::type_to_int_string(type)};
    }
}

Token::Token(double value) : m_type{Type::NUMBER}, m_value{value} {}

bool Token::operator==(const Token& other) const {
    if (m_type != other.m_type) {
        return false;
    }
    if (token::is_const_token(m_type)) {
        return true;
    }
    if (m_type == Type::NUMBER) {
        return numbers_equal(as_number(), other.as_number());
    }
    throw LexerError{"internal: can't compare tokens of type: " +
                     token::type_to_int_string(m_type)};
}

double Token::as_number() const {
    const auto type = get_type();
    if (type != Type::NUMBER) {
        throw LexerError{"internal: not a number: " + token::type_to_int_string(type)};
    }
    return std::get<double>(m_value);
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    switch (token.m_type) {
        case token::Type::NUMBER:
            os << token.as_number();
            break;
        default:
            os << token::type_to_string(token.m_type);
            break;
    }
    return os;
}

} // namespace math::server::lexer
