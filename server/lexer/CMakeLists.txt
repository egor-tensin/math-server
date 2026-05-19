find_package(Boost REQUIRED COMPONENTS regex)

file(GLOB_RECURSE lexer_src CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_library(lexer ${lexer_src})
target_include_directories(lexer PUBLIC ..)
target_link_libraries(lexer PUBLIC common)
target_link_libraries(lexer PRIVATE Boost::disable_autolinking Boost::regex)
