find_package(Boost REQUIRED COMPONENTS regex)

add_library(lexer details/parse.cpp lexer.cpp token.cpp token_type.cpp)
target_link_libraries(lexer PUBLIC common)
target_link_libraries(lexer PRIVATE Boost::disable_autolinking Boost::regex)
