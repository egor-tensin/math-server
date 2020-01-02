add_library(lexer details/parse.cpp lexer.cpp token.cpp token_type.cpp)
target_link_libraries(lexer PUBLIC common)
