file(GLOB unit_tests_src "*.cpp")
add_executable(unit_tests ${unit_tests_src})
set_target_properties(unit_tests PROPERTIES OUTPUT_NAME math-server-unit-tests)

target_link_libraries(unit_tests PRIVATE lexer parser)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
target_link_libraries(unit_tests PRIVATE Boost::disable_autolinking Boost::unit_test_framework)

install(TARGETS unit_tests RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:unit_tests>" DESTINATION bin OPTIONAL)
endif()
