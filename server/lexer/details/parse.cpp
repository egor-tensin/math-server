// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include "../error.hpp"
#include "../token_type.hpp"

#include <boost/regex.hpp>

#include <cstddef>

#include <exception>
#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <string_view>

namespace math::server::lexer::details {
namespace {

std::cmatch std_regex_search(const std::string_view& input, const std::regex& regex) {
    std::cmatch match;
    {
        const auto begin = input.data();
        const auto end = begin + input.length();
        std::regex_search(begin, end, match, regex);
    }
    return match;
}

boost::cmatch boost_regex_search(const std::string_view& input, const boost::regex& regex) {
    boost::cmatch match;
    {
        const auto begin = input.data();
        const auto end = begin + input.length();
        boost::regex_search(begin, end, match, regex);
    }
    return match;
}

// CMatch is either std::cmatch or boost::cmatch.
template <typename CMatch>
void check_exponent(const CMatch& match) {
    // If we have the numeric part of a number followed by 'e' and no digits,
    // 1) that 'e' definitely belongs to this number token,
    // 2) the user forgot to type in the required digits.
    const auto& exponent = match[1];
    const auto& abs_power = match[2];
    if (exponent.matched && abs_power.matched && abs_power.length() == 0) {
        throw LexerError{"exponent has no digits: " + match[0].str()};
    }
}

// This is a hacky attempt to describe a C-like grammar for floating-point
// numbers using a regex (the tests seem to pass though).
// A proper NFA would be better, I guess.
const std::string_view NUMBER_REGEX = R"REGEX(^(?:\d+(?:\.\d*)?|\.\d+)(e[+-]?(\d*))?)REGEX";

std::string_view std_match_number(const std::string_view& input) {
    static constexpr auto flags =
        std::regex_constants::ECMAScript |
        std::regex_constants::icase;
    static const std::regex number_regex{NUMBER_REGEX.data(), NUMBER_REGEX.length(), flags};

    const auto match = std_regex_search(input, number_regex);
    if (match.empty()) {
        return {};
    }
    check_exponent(match);
    return {match[0].first, static_cast<std::size_t>(match[0].length())};
}

std::string_view boost_match_number(const std::string_view& input) {
    static const boost::regex number_regex{NUMBER_REGEX.data(), NUMBER_REGEX.length(), boost::regex::icase};

    const auto match = boost_regex_search(input, number_regex);
    if (match.empty()) {
        return {};
    }
    check_exponent(match);
    return {match[0].first, static_cast<std::size_t>(match[0].length())};
}

std::string_view match_number(const std::string_view& input) {
    return std_match_number(input);
}

using NumberMatcher = std::function<std::string_view (const std::string_view&)>;

std::optional<double> parse_number(const std::string_view& input, const NumberMatcher& match, std::string_view& token) {
    const auto view = match(input);
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

bool starts_with(const std::string_view& a, const std::string_view& b) noexcept {
    return a.length() >= b.length()
        && a.compare(0, b.length(), b) == 0;
}

}

namespace impl {

std::optional<double> std_parse_number(const std::string_view& input, std::string_view& token) {
    return parse_number(input, &std_match_number, token);
}

std::optional<double> std_parse_number(const std::string_view& input) {
    std::string_view token;
    return std_parse_number(input, token);
}

std::optional<double> boost_parse_number(const std::string_view& input, std::string_view& token) {
    return parse_number(input, &boost_match_number, token);
}

std::optional<double> boost_parse_number(const std::string_view& input) {
    std::string_view token;
    return boost_parse_number(input, token);
}

}

std::optional<double> parse_number(const std::string_view& input, std::string_view& token) {
    return impl::std_parse_number(input, token);
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
