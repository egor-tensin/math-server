#include <server/lexer/details/parse.hpp>

#include <benchmark/benchmark.h>

class SelectionOfNumbers : public benchmark::Fixture {
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

BENCHMARK_F(SelectionOfNumbers, ParseStdRegex)(benchmark::State& state) {
    using namespace math::server::lexer::details;
    for (auto _ : state) {
        for (const auto& src : m_numbers) {
            impl::std_parse_number(src);
        }
    }
}

BENCHMARK_F(SelectionOfNumbers, ParseBoostRegex)(benchmark::State& state) {
    using namespace math::server::lexer::details;
    for (auto _ : state) {
        for (const auto& src : m_numbers) {
            impl::boost_parse_number(src);
        }
    }
}
