// Copyright (c) 2019 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "math-server" project.
// For details, see https://github.com/egor-tensin/math-server.
// Distributed under the MIT License.

#include <server/parser/error.hpp>
#include <server/parser/parser.hpp>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include <string>
#include <string_view>
#include <vector>

BOOST_AUTO_TEST_SUITE(parser_tests)

namespace bdata = boost::unit_test::data;
using math::server::Parser;
using math::server::ParserError;

namespace {
namespace exec::valid {

const std::vector<std::string_view> input{
    // Constants and unary operators:
    "1",
    "-1",
    "--1",
    "+--+-2",
    // Basic exponentiation:
    "0^0",
    "4^.5",
    " 4 ^ ---0.5 ",
    // Exponentiation is right-associative:
    "2 ^ 3 ^ 3",
    "(2 ^ 3) ^ 3",
    // Exponentiation has higher precedence than the unary minus:
    "(-2) ^ 2",
    "-2 ^ 2",
    "(.5 ^ -1) ^ 4",
    ".5 ^ -1 ^ 4",
    // Basic binary operators:
    " 1 + 2 ",
    " 2 * 1 + 3 ",
    " 2 * (1 + 3) ",
    " 2 * (1 + 3 * (1 - -3)) ",
    " -2 * (1 + --3 * (1 - -3)) ",
    // Looks weird, but also works in e.g. Python:
    " -2 * -+--- (3 + -100e-1)  ",
    " -2^2 --+- -3^0 / (4 + +12) ^.5^2 ",
    " -2^2 --+- -3^0 / ((4 + +12) ^.5)^2 ",
};

const std::vector<double> expected{
    // Constants and unary operators:
    1,
    -1,
    1,
    -2,
    // Basic exponentiation:
    1,
    2,
    0.5,
    // Exponentiation is right-associative:
    134217728,
    512,
    // Exponentiation has higher precedence than the unary minus:
    4,
    -4,
    16,
    2,
    // Basic binary operators:
    3,
    5,
    8,
    26,
    -26,
    // Looks weird, but also works in e.g. Python:
    14,
    -3.5,
    -3.9375,
};

} // namespace exec::valid

namespace exec::invalid {

const std::vector<std::string_view> input{
    "",
    "-",
    "-+-",
    // Missing operand:
    " 1 + ",
    "-2 + 3 ^ -",
    // Unmatched parentheses:
    " 2 * (1 + 3 ",
    " 2 * (1 + 3) )",
};

const std::vector<std::string> error_msg{
    "server error: parser error: expected '-', '+', '(' or a number",
    "server error: parser error: expected '-', '+', '(' or a number",
    "server error: parser error: expected '-', '+', '(' or a number",
    // Missing operand:
    "server error: parser error: expected '-', '+', '(' or a number",
    "server error: parser error: expected '-', '+', '(' or a number",
    // Unmatched parentheses:
    "server error: parser error: missing closing ')'",
    "server error: parser error: expected a binary operator",
};

} // namespace exec::invalid
} // namespace

BOOST_DATA_TEST_CASE(test_exec_valid,
                     bdata::make(exec::valid::input) ^ exec::valid::expected,
                     input,
                     expected) {
    Parser parser{input};
    BOOST_TEST(parser.exec() == expected);
}

BOOST_DATA_TEST_CASE(test_exec_invalid,
                     bdata::make(exec::invalid::input) ^ exec::invalid::error_msg,
                     input,
                     error_msg) {
    BOOST_REQUIRE_THROW(
        do {
            Parser parser{input};
            parser.exec();
        } while (0),
        ParserError);

    try {
        Parser parser{input};
        parser.exec();
    } catch (const ParserError& e) {
        BOOST_TEST(error_msg == e.what());
    }
}

BOOST_AUTO_TEST_SUITE_END()
