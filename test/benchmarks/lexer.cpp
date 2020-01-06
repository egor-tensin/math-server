#include <server/lexer/details/parse.hpp>

#include <benchmark/benchmark.h>

#include <string_view>
#include <vector>

// I noticed std::regex_search cropping up in profiling results.
// Switching to boost::regex_search yielded a huge benefit: sometimes a 15x
// increase in regex matching (on VS builds in particular).
// Should be easily reproducible using these micro-benchmarks.

namespace {

class NumberExamples : public benchmark::Fixture {
protected:
    std::vector<std::string_view> m_numbers{
        "0",
        "123",
        "0.123",
        ".123",
        "1e9",
        "1.87E-18",
        "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789.012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789",
    };
};

class WhitespaceExamples : public benchmark::Fixture {
protected:
    std::vector<std::string_view> m_whitespace{
        "",
        "  1",
        "                                                                                                                                123",
    };
};

}

BENCHMARK_F(NumberExamples, StdParseNumber)(benchmark::State& state) {
    using namespace math::server::lexer::details;
    for (auto _ : state) {
        for (const auto& src : m_numbers) {
            impl::std_parse_number(src);
        }
    }
}

BENCHMARK_F(NumberExamples, BoostParseNumber)(benchmark::State& state) {
    using namespace math::server::lexer::details;
    for (auto _ : state) {
        for (const auto& src : m_numbers) {
            impl::boost_parse_number(src);
        }
    }
}

BENCHMARK_F(WhitespaceExamples, StdParseWhitespace)(benchmark::State& state) {
    using namespace math::server::lexer::details;
    for (auto _ : state) {
        for (const auto& src : m_whitespace) {
            impl::std_parse_whitespace(src);
        }
    }
}

BENCHMARK_F(WhitespaceExamples, BoostParseWhitespace)(benchmark::State& state) {
    using namespace math::server::lexer::details;
    for (auto _ : state) {
        for (const auto& src : m_whitespace) {
            impl::boost_parse_whitespace(src);
        }
    }
}
