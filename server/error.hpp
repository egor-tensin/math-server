#pragma once

#include <stdexcept>
#include <string>

namespace math::server {

class Error : public std::runtime_error {
public:
    explicit Error(const std::string& what)
        : std::runtime_error{"server error: " + what}
    { }
};

}
