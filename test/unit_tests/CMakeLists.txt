find_package(Boost 1.67.0 REQUIRED COMPONENTS unit_test_framework)
# It would be Boost 1.66 in a perfect world, but the tests fail to compile with
# that: https://lists.boost.org/boost-bugs/2018/01/49711.php.

file(GLOB unit_tests_src "*.cpp")
add_executable(unit_tests ${unit_tests_src})
set_target_properties(unit_tests PROPERTIES OUTPUT_NAME math-server-unit-tests)
target_link_libraries(unit_tests PRIVATE lexer parser)
target_link_libraries(unit_tests PRIVATE
    Boost::disable_autolinking
    Boost::unit_test_framework)
install(TARGETS unit_tests RUNTIME DESTINATION bin)
install_pdbs(TARGETS unit_tests DESTINATION bin)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(NAME unit_tests COMMAND Python3::Interpreter
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/tools/ctest-driver.py"
    run
    --pass-regex [=[^\*\*\* No errors detected$]=]
    --
    "$<TARGET_FILE:unit_tests>"
    --no_color_output)
