#include <server/parser.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_parser_exec) {
    using namespace math::server;

    {
        Parser parser{""};
        BOOST_CHECK_THROW(parser.exec(), parser::Error);
    }
    {
        Parser parser{"1"};
        BOOST_TEST(parser.exec() == 1);
    }
    {
        Parser parser{" 1 + "};
        BOOST_CHECK_THROW(parser.exec(), parser::Error);
    }
    {
        Parser parser{" 1 + 2 "};
        BOOST_TEST(parser.exec() == 3);
    }
    {
        Parser parser{" 2 * 1 + 3 "};
        BOOST_TEST(parser.exec() == 5);
    }
    {
        Parser parser{" 2 * (1 + 3) "};
        BOOST_TEST(parser.exec() == 8);
    }
    {
        Parser parser{" 2 * (1 + 3 "};
        BOOST_CHECK_THROW(parser.exec(), parser::Error);
    }
    {
        Parser parser{" 2 * (1 + 3) )"};
        BOOST_CHECK_THROW(parser.exec(), parser::Error);
    }
    {
        Parser parser{" 2 * (1 + 3 * (1 - -3)) "};
        BOOST_TEST(parser.exec() == 26);
    }
    {
        Parser parser{" -2 * ---- (3 + -100e-1)  "}; // Looks weird, but also works in e.g. Python
        BOOST_TEST(parser.exec() == 14);
    }
}
