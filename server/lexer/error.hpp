// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "../common/error.hpp"

#include <string>

namespace math::server {

class LexerError : public Error {
public:
    explicit LexerError(const std::string& what) : Error{"lexer error: " + what} {}
};

} // namespace math::server
