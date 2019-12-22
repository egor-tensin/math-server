// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include <stdexcept>
#include <string>

namespace math::client {

class Error : public std::runtime_error {
public:
    explicit Error(const std::string& what)
        : std::runtime_error{"client error: " + what}
    { }
};

}
