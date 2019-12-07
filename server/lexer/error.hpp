#pragma once

#include "../common/error.hpp"

#include <string>

namespace math::server {

class LexerError : public Error {
public:
    explicit LexerError(const std::string &what)
        : Error{"lexer error: " + what}
    { }
};

}
