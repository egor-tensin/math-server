// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include <ostream>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace math::server::lexer::token {

enum class Type {
    WHITESPACE,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    LEFT_PAREN,
    RIGHT_PAREN,
    NUMBER,
};

using TypeInt = std::underlying_type<Type>::type;
using TypeSet = std::unordered_set<Type>;

TypeInt type_to_int(Type);
std::string type_to_int_string(Type);

bool is_const_token(Type);
const TypeSet& const_tokens();

bool token_has_value(Type);

std::string type_to_string(Type);
Type type_from_string(const std::string&);

std::ostream& operator<<(std::ostream&, const Type&);

}
