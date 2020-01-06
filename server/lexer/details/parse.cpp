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

// This approach gives GCC on Travis an "internal compiler error":
//template <template <typename> class MatchResultsT>

template <typename MatchResultsT>
class RegexMatcher {
public:
    virtual ~RegexMatcher() = default;

    virtual bool match_regex(const std::string_view& input) = 0;

    std::string_view to_view() const {
        return {&*m_match[0].first, static_cast<std::size_t>(m_match[0].length())};
        //      ^ I fucking hate C++.
    }

    std::string to_str() const { return m_match[0].str(); }

protected:
    MatchResultsT m_match;
};

template <typename MatchResultsT>
class RegexNumberMatcher : public RegexMatcher<MatchResultsT> {
public:
    bool match(const std::string_view& input) {
        if (!this->match_regex(input)) {
            return false;
        }
        // If we have the numeric part of a number followed by 'e' and no
        // digits,
        // 1) that 'e' definitely belongs to this number token,
        // 2) the user forgot to type in the required digits.
        if (matched_e() && !matched_e_power()) {
            throw LexerError{"exponent has no digits: " + this->to_str()};
        }
        return true;
    }

protected:
    // This is a hacky attempt to describe a C-like grammar for floating-point
    // numbers using a regex (the tests seem to pass though).
    // A proper NFA would be better, I guess.
    static constexpr std::string_view NUMBER_REGEX{R"REGEX(^(?:\d+(?:\.\d*)?|\.\d+)(e[+-]?(\d*))?)REGEX"};

private:
    bool matched_e() const { return this->m_match[1].matched; }

    bool matched_e_power() const {
        return this->m_match[2].matched && this->m_match[2].length() != 0;
    }
};

class StdNumberMatcher
    : public RegexNumberMatcher<std::match_results<std::string_view::const_iterator>> {
public:
    bool match_regex(const std::string_view& input) override {
        return std::regex_search(input.cbegin(), input.cend(), m_match, get_regex());
    }

private:
    static const std::regex& get_regex() {
        static constexpr auto flags =
            std::regex_constants::ECMAScript |
            std::regex_constants::icase;
        static const std::regex regex{NUMBER_REGEX.data(), NUMBER_REGEX.length(), flags};
        return regex;
    }
};

class BoostNumberMatcher
    : public RegexNumberMatcher<boost::match_results<std::string_view::const_iterator>> {
public:
    bool match_regex(const std::string_view& input) override {
        return boost::regex_search(input.cbegin(), input.cend(), m_match, get_regex());
    }

private:
    static const boost::regex& get_regex() {
        static constexpr boost::regex::flag_type flags =
            boost::regex::ECMAScript |
            boost::regex::icase;
        static const boost::regex regex{NUMBER_REGEX.data(), NUMBER_REGEX.length(), flags};
        return regex;
    }
};

template <typename MatchResultsT>
std::optional<double> parse_number(
    const std::string_view& input,
    RegexNumberMatcher<MatchResultsT>&& matcher,
    std::string_view& token) {

    if (!matcher.match(input)) {
        return {};
    }
    const auto view = matcher.to_view();
    try {
        const auto result = std::stod(std::string{view});
        token = view;
        return result;
    } catch (const std::exception&) {
        throw LexerError{"internal: couldn't parse number from: " + std::string{view}};
    }
    return {};
}

template <typename MatchResultsT>
class RegexWhitespaceMatcher : public RegexMatcher<MatchResultsT> {
protected:
    static constexpr std::string_view WS_REGEX{R"(^\s+)"};
};

class StdWhitespaceMatcher
    : public RegexWhitespaceMatcher<std::match_results<std::string_view::const_iterator>> {
public:
    bool match_regex(const std::string_view& input) override {
        return std::regex_search(input.cbegin(), input.cend(), m_match, get_regex());
    }

private:
    static const std::regex& get_regex() {
        static constexpr auto flags = std::regex_constants::ECMAScript;
        static const std::regex regex{WS_REGEX.data(), WS_REGEX.length(), flags};
        return regex;
    }
};

class BoostWhitespaceMatcher
    : public RegexWhitespaceMatcher<boost::match_results<std::string_view::const_iterator>> {
public:
    bool match_regex(const std::string_view& input) override {
        return boost::regex_search(input.cbegin(), input.cend(), m_match, get_regex());
    }

private:
    static const boost::regex& get_regex() {
        static constexpr boost::regex::flag_type flags = boost::regex::ECMAScript;
        static const boost::regex regex{WS_REGEX.data(), WS_REGEX.length(), flags};
        return regex;
    }
};

template <typename MatchResultsT>
std::string_view parse_whitespace(
    const std::string_view& input,
    RegexWhitespaceMatcher<MatchResultsT>&& matcher) {

    if (matcher.match_regex(input)) {
        return matcher.to_view();
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

std::string_view std_parse_whitespace(const std::string_view& input) {
    return parse_whitespace(input, StdWhitespaceMatcher{});
}

std::string_view boost_parse_whitespace(const std::string_view& input) {
    return parse_whitespace(input, BoostWhitespaceMatcher{});
}

}

std::optional<double> parse_number(const std::string_view& input, std::string_view& token) {
    return impl::boost_parse_number(input, token);
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
    return impl::boost_parse_whitespace(input);
}

}
