// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include "../error.hpp"
#include "../token_type.hpp"

#include <cstddef>

#include <exception>
#include <optional>
#include <regex>
#include <string>
#include <string_view>

namespace math::server::lexer::details {
namespace {

std::string_view match_number(const std::string_view& input) {
    static constexpr std::regex::flag_type flags =
        std::regex_constants::ECMAScript |
        std::regex_constants::icase;
    // This is a hacky attempt to describe a C-like grammar for floating-point
    // numbers using a regex (the tests seem to pass though).
    // A proper NFA would be better, I guess.
    static const std::regex number_regex{R"REGEX(^(?:\d+(?:\.\d*)?|\.\d+)(e[+-]?(\d*))?)REGEX", flags};

    std::cmatch match;
    {
        const auto begin = input.data();
        const auto end = begin + input.length();
        if (!std::regex_search(begin, end, match, number_regex)) {
            return {};
        }
    }
    {
        // If we have the numeric part of a number followed by 'e' and no digits,
        // 1) that 'e' definitely belongs to this number token,
        // 2) the user forgot to type in the required digits.
        const auto& exponent = match[1];
        const auto& abs_power = match[2];
        if (exponent.matched && abs_power.matched && abs_power.length() == 0) {
            throw LexerError{"exponent has no digits: " + match[0].str()};
        }
    }
    return {match[0].first, static_cast<std::size_t>(match[0].length())};
}

bool starts_with(const std::string_view& a, const std::string_view& b) noexcept {
    return a.length() >= b.length()
        && a.compare(0, b.length(), b) == 0;
}

}

std::optional<double> parse_number(const std::string_view& input, std::string_view& token) {
    const auto view = match_number(input);
    if (!view.data()) {
        return {};
    }
    try {
        const auto result = std::stod(std::string{view});
        token = view;
        return result;
    } catch (const std::exception&) {
        throw LexerError{"internal: couldn't parse number from: " + std::string{view}};
    }
    return {};
}

std::optional<double> parse_number(const std::string_view& input) {
    std::string_view token;
    return parse_number(input, token);
}

std::optional<token::Type> parse_const_token(const std::string_view& input, std::string_view& token) {
    for (const auto type : token::const_tokens()) {
        const auto str = token::type_to_string(type);
        if (starts_with(input, str)) {
            token = std::string_view(input.data(), str.length());
            return {type};
        }
    }
    return {};
}

std::optional<token::Type> parse_const_token(const std::string_view& input) {
    std::string_view token;
    return parse_const_token(input, token);
}

std::string_view parse_whitespace(const std::string_view& input) {
    static const std::regex ws_regex{R"(^\s+)"};

    std::cmatch match;
    {
        const auto begin = input.data();
        const auto end = begin + input.length();
        if (std::regex_search(begin, end, match, ws_regex)) {
            return std::string_view(match[0].first, match[0].length());
        }
    }
    return {};
}

}
