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

class RegexNumberMatcher {
public:
    bool match(const std::string_view& input) {
        if (!basic_match(input)) {
            return false;
        }
        // If we have the numeric part of a number followed by 'e' and no
        // digits,
        // 1) that 'e' definitely belongs to this number token,
        // 2) the user forgot to type in the required digits.
        if (matched_e() && !matched_e_power()) {
            throw LexerError{"exponent has no digits: " + to_str()};
        }
        return true;
    }

    virtual std::string_view get() const = 0;

protected:
    // This is a hacky attempt to describe a C-like grammar for floating-point
    // numbers using a regex (the tests seem to pass though).
    // A proper NFA would be better, I guess.
    static constexpr std::string_view NUMBER_REGEX{R"REGEX(^(?:\d+(?:\.\d*)?|\.\d+)(e[+-]?(\d*))?)REGEX"};

private:
    virtual bool basic_match(const std::string_view& input) = 0;

    virtual std::string to_str() const = 0;

    virtual bool matched_e() const = 0;

    virtual bool matched_e_power() const = 0;
};

class StdNumberMatcher : public RegexNumberMatcher {
public:
    std::string_view get() const override {
        return {m_match[0].first, static_cast<std::size_t>(m_match[0].length())};
    }

private:
    bool basic_match(const std::string_view& input) override {
        const auto begin = input.data();
        const auto end = begin + input.length();
        return std::regex_search(begin, end, m_match, get_regex());
    }

    static const std::regex& get_regex() {
        static constexpr auto flags =
            std::regex_constants::ECMAScript |
            std::regex_constants::icase;
        static const std::regex regex{NUMBER_REGEX.data(), NUMBER_REGEX.length(), flags};
        return regex;
    }

    std::string to_str() const override { return m_match[0].str(); }

    bool matched_e() const override { return m_match[1].matched; }

    bool matched_e_power() const override { return m_match[2].matched && m_match[2].length() != 0; }

    std::cmatch m_match;
};

class BoostNumberMatcher : public RegexNumberMatcher {
public:
    std::string_view get() const override {
        return {m_match[0].first, static_cast<std::size_t>(m_match[0].length())};
    }

private:
    bool basic_match(const std::string_view& input) override {
        const auto begin = input.data();
        const auto end = begin + input.length();
        return boost::regex_search(begin, end, m_match, get_regex());
    }

    static const boost::regex& get_regex() {
        static constexpr boost::regex::flag_type flags =
            boost::regex::ECMAScript |
            boost::regex::icase;
        static const boost::regex regex{NUMBER_REGEX.data(), NUMBER_REGEX.length(), flags};
        return regex;
    }

    std::string to_str() const override { return m_match[0].str(); }

    bool matched_e() const override { return m_match[1].matched; }

    bool matched_e_power() const override { return m_match[2].matched && m_match[2].length() != 0; }

    boost::cmatch m_match;
};

std::optional<double> parse_number(const std::string_view& input, RegexNumberMatcher&& matcher, std::string_view& token) {
    if (!matcher.match(input)) {
        return {};
    }
    const auto view = matcher.get();
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
    return parse_number(input, StdNumberMatcher{}, token);
}

std::optional<double> std_parse_number(const std::string_view& input) {
    std::string_view token;
    return std_parse_number(input, token);
}

std::optional<double> boost_parse_number(const std::string_view& input, std::string_view& token) {
    return parse_number(input, BoostNumberMatcher{}, token);
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
