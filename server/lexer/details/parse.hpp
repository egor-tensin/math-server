// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#pragma once

#include "../token_type.hpp"

#include <optional>
#include <string_view>

namespace math::server::lexer::details {
namespace impl {

// Exposed for benchmarking:

std::optional<double> std_parse_number(const std::string_view&, std::string_view&);
std::optional<double> std_parse_number(const std::string_view&);
std::optional<double> boost_parse_number(const std::string_view&, std::string_view&);
std::optional<double> boost_parse_number(const std::string_view&);

std::string_view std_parse_whitespace(const std::string_view&);
std::string_view boost_parse_whitespace(const std::string_view&);

} // namespace impl

// Exposed for testing:
std::string_view parse_whitespace(const std::string_view&);
std::optional<double> parse_number(const std::string_view&, std::string_view&);
std::optional<double> parse_number(const std::string_view&);
std::optional<token::Type> parse_const_token(const std::string_view&, std::string_view&);
std::optional<token::Type> parse_const_token(const std::string_view&);

} // namespace math::server::lexer::details
