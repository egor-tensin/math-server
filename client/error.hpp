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
