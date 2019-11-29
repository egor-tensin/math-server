#include <server/lexer.hpp>

#include <boost/test/unit_test.hpp>

#include <vector>

BOOST_AUTO_TEST_CASE(test_lexer_parse_number) {
    using namespace math::server::lexer;

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
    BOOST_CHECK_THROW(details::parse_number("12e"), Error);
}

BOOST_AUTO_TEST_CASE(test_lexer_parse_const_token) {
    using namespace math::server::lexer;

    // TODO: No time to implement the required string conversions, hence the
    // extra parentheses.
    BOOST_TEST((details::parse_const_token("+").value() == Token::Type::PLUS));
    // parse_* functions only consume a single token:
    BOOST_TEST((details::parse_const_token("+++").value() == Token::Type::PLUS));
    BOOST_TEST((details::parse_const_token("-").value() == Token::Type::MINUS));
    BOOST_TEST(!details::parse_const_token("&+").has_value());
}

BOOST_AUTO_TEST_CASE(test_lexer_get_tokens) {
    using namespace math::server;
    using namespace math::server::lexer;

    // TODO: No time to implement the required string conversions, hence the
    // extra parentheses.
    {
        Lexer lexer{""};
        BOOST_TEST((lexer.get_tokens() == std::vector<Token>{}));
    }
    {
        Lexer lexer{" + - "};
        BOOST_TEST((lexer.get_tokens() == std::vector<Token>{
            Token{Token::Type::PLUS},
            Token{Token::Type::MINUS},
        }));
    }
    {
        Lexer lexer{"&"};
        BOOST_CHECK_THROW((lexer.get_tokens()), lexer::Error);
    }
    {
        Lexer lexer{" 1 + 123 & 456"};
        BOOST_CHECK_THROW((lexer.get_tokens()), lexer::Error);
    }
    {
        Lexer lexer{"1+2"};
        BOOST_TEST((lexer.get_tokens() == std::vector<Token>{
            Token{1},
            Token{Token::Type::PLUS},
            Token{2},
        }));
    }
    {
        Lexer lexer{"1+2 *  (3- 4e-2)"};
        BOOST_TEST((lexer.get_tokens() == std::vector<Token>{
            Token{1},
            Token{Token::Type::PLUS},
            Token{2},
            Token{Token::Type::ASTERISK},
            Token{Token::Type::LEFT_PAREN},
            Token{3},
            Token{Token::Type::MINUS},
            Token{4e-2},
            Token{Token::Type::RIGHT_PAREN},
        }));
    }
    {
        Lexer lexer{" 2 * (1 + 3 * (1 - -3)) "};
        BOOST_TEST((lexer.get_tokens() == std::vector<Token>{
            Token{2},
            Token{Token::Type::ASTERISK},
            Token{Token::Type::LEFT_PAREN},
            Token{1},
            Token{Token::Type::PLUS},
            Token{3},
            Token{Token::Type::ASTERISK},
            Token{Token::Type::LEFT_PAREN},
            Token{1},
            Token{Token::Type::MINUS},
            Token{Token::Type::MINUS},
            Token{3},
            Token{Token::Type::RIGHT_PAREN},
            Token{Token::Type::RIGHT_PAREN},
        }));
    }
}
