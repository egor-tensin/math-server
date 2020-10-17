add_library(parser INTERFACE)
target_include_directories(parser INTERFACE ..)

target_link_libraries(parser INTERFACE common lexer)
