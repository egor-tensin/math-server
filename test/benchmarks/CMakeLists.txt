file(GLOB benchmarks_cpp "*.cpp")
add_executable(benchmarks ${benchmarks_cpp})
set_target_properties(benchmarks PROPERTIES OUTPUT_NAME math-server-benchmarks)

target_link_libraries(benchmarks PRIVATE lexer)

target_link_libraries(benchmarks PRIVATE benchmark benchmark_main)

install(TARGETS benchmarks RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:benchmarks>" DESTINATION bin OPTIONAL)
endif()
