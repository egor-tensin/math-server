file(GLOB_RECURSE lexer_cpp "*.cpp")
file(GLOB_RECURSE lexer_hpp "*.hpp")
add_library(lexer ${lexer_cpp} ${lexer_hpp})
target_include_directories(lexer PUBLIC ..)

target_link_libraries(lexer PUBLIC common)

find_package(Boost REQUIRED COMPONENTS regex)
target_link_libraries(lexer PRIVATE Boost::disable_autolinking Boost::regex)
