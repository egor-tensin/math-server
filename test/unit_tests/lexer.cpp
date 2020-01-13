// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include <server/lexer/details/parse.hpp>
#include <server/lexer/error.hpp>
#include <server/lexer/lexer.hpp>
#include <server/lexer/token.hpp>
#include <server/lexer/token_type.hpp>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <ostream>
#include <string>
#include <string_view>
#include <vector>

BOOST_AUTO_TEST_SUITE(lexer_tests)

namespace bdata = boost::unit_test::data;
using math::server::Lexer;
using math::server::LexerError;
using math::server::lexer::Token;
using math::server::lexer::token::Type;
namespace details = math::server::lexer::details;

BOOST_AUTO_TEST_CASE(test_parse_number) {
    // These are valid numbers:
    BOOST_TEST(details::parse_number("0").value() == 0);
    BOOST_TEST(details::parse_number("1.").value() == 1.);
    BOOST_TEST(details::parse_number(".1").value() == .1);
    // parse_* functions only consume a single token:
    BOOST_TEST(details::parse_number(".1+1").value() == .1);
    BOOST_TEST(details::parse_number("1e3").value() == 1e3);
    BOOST_TEST(details::parse_number(".123e1").value() == .123e1);
    BOOST_TEST(details::parse_number("123e-1").value() == 123e-1);
    BOOST_TEST(details::parse_number("123e+1").value() == 123e+1);
    BOOST_TEST(details::parse_number("1.e6").value() == 1.e6);
    BOOST_TEST(details::parse_number("2.3e-4").value() == 2.3e-4);
    // These are not numbers, but perhaps something else?
    BOOST_TEST(!details::parse_number(".").has_value());
    BOOST_TEST(!details::parse_number(".e3").has_value());
    BOOST_TEST(!details::parse_number("e12").has_value());
    // This is definitely a number, but a malformed one (an exponent must be
    // followed by some digits).
    BOOST_CHECK_THROW(details::parse_number("12e"), LexerError);
}

BOOST_AUTO_TEST_CASE(test_parse_const_token) {
    BOOST_TEST(details::parse_const_token("+").value() == Type::PLUS);
    // parse_* functions only consume a single token:
    BOOST_TEST(details::parse_const_token("+/*").value() == Type::PLUS);
    BOOST_TEST(details::parse_const_token("-").value() == Type::MINUS);
    BOOST_TEST(details::parse_const_token("^^").value() == Type::CARET);
    BOOST_TEST(!details::parse_const_token("&+").has_value());
}

namespace {
namespace get_tokens::valid {

// clang-format off
// wtf, don't binpack this
const std::vector<std::string_view> input{
    "",
    " + - ",
    "1+2",
    ".5^-1 ^ 4",
    "1+2 *  (3- 4e-2)",
    " 2 * (1 + 3 * (1 - -3)) ",
};
// clang-format on

// Some black magic-fuckery to resolve operator<< for std::vector<Token>.
// See https://stackoverflow.com/a/18817428/514684.

struct Expected {
    std::vector<Token> m_tokens;
};

std::ostream& operator<<(std::ostream& os, const Expected& expected) {
    for (const auto& token : expected.m_tokens) {
        os << token;
    }
    return os;
}

const std::vector<Expected> expected{
    {{}},
    {{
        Token{Type::PLUS},
        Token{Type::MINUS},
    }},
    {{
        Token{1},
        Token{Type::PLUS},
        Token{2},
    }},
    {{
        Token{.5},
        Token{Type::CARET},
        Token{Type::MINUS},
        Token{1},
        Token{Type::CARET},
        Token{4},
    }},
    {{
        Token{1},
        Token{Type::PLUS},
        Token{2},
        Token{Type::ASTERISK},
        Token{Type::LEFT_PAREN},
        Token{3},
        Token{Type::MINUS},
        Token{4e-2},
        Token{Type::RIGHT_PAREN},
    }},
    {{
        Token{2},
        Token{Type::ASTERISK},
        Token{Type::LEFT_PAREN},
        Token{1},
        Token{Type::PLUS},
        Token{3},
        Token{Type::ASTERISK},
        Token{Type::LEFT_PAREN},
        Token{1},
        Token{Type::MINUS},
        Token{Type::MINUS},
        Token{3},
        Token{Type::RIGHT_PAREN},
        Token{Type::RIGHT_PAREN},
    }},
};

} // namespace get_tokens::valid

namespace get_tokens::invalid {

const std::vector<std::string_view> input{
    "&",
    " 1 + 123 & 456",
};

const std::vector<std::string> error_msg{
    "server error: lexer error: invalid input at: &",
    "server error: lexer error: invalid input at: & 456",
};

} // namespace get_tokens::invalid
} // namespace

BOOST_DATA_TEST_CASE(test_get_tokens_valid,
                     bdata::make(get_tokens::valid::input) ^ get_tokens::valid::expected,
                     input,
                     expected) {
    Lexer lexer{input};
    const auto actual = lexer.get_tokens();
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.cbegin(), actual.cend(), expected.m_tokens.cbegin(),
                                  expected.m_tokens.cend());
}

BOOST_DATA_TEST_CASE(test_get_tokens_invalid,
                     bdata::make(get_tokens::invalid::input) ^ get_tokens::invalid::error_msg,
                     input,
                     error_msg) {
    BOOST_REQUIRE_THROW(
        do {
            Lexer lexer{input};
            lexer.get_tokens();
        } while (0),
        LexerError);

    try {
        Lexer lexer{input};
        lexer.get_tokens();
    } catch (const LexerError& e) {
        BOOST_TEST(error_msg == e.what());
    }
}

BOOST_AUTO_TEST_SUITE_END()
