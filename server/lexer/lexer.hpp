#pragma once

#include "input.hpp"
#include "token.hpp"
#include "token_type.hpp"

#include <functional>
#include <optional>
#include <string_view>
#include <vector>

namespace math::server {
namespace lexer::details {

// Exposed for testing:
std::string_view parse_whitespace(const std::string_view&);
std::optional<double> parse_number(const std::string_view&);
std::optional<token::Type> parse_const_token(const std::string_view&);

}

class Lexer {
public:
    explicit Lexer(const std::string_view& input);
    explicit Lexer(const lexer::Input& input);

    using Token = lexer::Token;
    using ParsedToken = lexer::ParsedToken;
    using Type = Token::Type;
    using TokenProcessor = std::function<bool (const ParsedToken&)>;

    bool for_each_token(const TokenProcessor& process);

    std::vector<ParsedToken> get_tokens();

    bool has_token() const {
        return peek_token().has_value();
    }

    std::optional<ParsedToken> peek_token() const {
        return m_token_buffer;
    }

    void drop_token();
    std::optional<ParsedToken> drop_token_of_type(Type type);

private:
    std::optional<ParsedToken> parse_whitespace() const;
    std::optional<ParsedToken> parse_const_token() const;
    std::optional<ParsedToken> parse_number() const;

    ParsedToken parse_token() const;

    void consume_whitespace();
    void consume_token();

    lexer::Input m_input;
    std::optional<ParsedToken> m_token_buffer;
};

}
