#pragma once

#include "../common/error.hpp"

#include <string>

namespace math::server {

class ParserError : public Error {
public:
    explicit ParserError(const std::string& what)
        : Error{"parser error: " + what}
    { }
};

}
