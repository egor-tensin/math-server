find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(unit_tests main.cpp lexer.cpp parser.cpp)
target_link_libraries(unit_tests PRIVATE lexer parser)
target_include_directories(unit_tests PRIVATE ../..)

target_include_directories(unit_tests SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(unit_tests PRIVATE ${Boost_LIBRARIES})

set_target_properties(unit_tests PROPERTIES OUTPUT_NAME math-server-unit-tests)

install(TARGETS unit_tests RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:unit_tests>" DESTINATION bin OPTIONAL)
endif()
