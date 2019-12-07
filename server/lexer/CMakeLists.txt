add_library(lexer lexer.cpp token.cpp token_type.cpp)
target_link_libraries(lexer PUBLIC common)
