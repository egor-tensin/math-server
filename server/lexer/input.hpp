// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "error.hpp"

#include <cstddef>
#include <string_view>

namespace math::server::lexer {

class Input {
public:
    explicit Input(const std::string_view& input) : m_pos{0}, m_input{input} {}

    const std::string_view& get_input() const { return m_input; }

    std::size_t get_pos() const { return m_pos; }

    std::size_t get_length() const { return m_input.length(); }

    bool empty() const { return m_input.empty(); }

    void consume(std::size_t len) {
        if (m_input.length() < len) {
            throw LexerError{"internal: not enough input to consume"};
        }
        m_pos += len;
        m_input.remove_prefix(len);
    }

    void consume(const std::string_view& sub) { consume(sub.length()); }

private:
    std::size_t m_pos;
    std::string_view m_input;
};

} // namespace math::server::lexer
