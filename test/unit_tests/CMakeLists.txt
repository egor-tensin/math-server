find_package(Boost REQUIRED)

add_executable(unit_tests main.cpp lexer.cpp parser.cpp)
target_include_directories(unit_tests SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})

target_include_directories(unit_tests PRIVATE ../..)
