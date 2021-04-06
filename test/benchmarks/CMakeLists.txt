file(GLOB benchmarks_src "*.cpp")
add_executable(benchmarks ${benchmarks_src})
set_target_properties(benchmarks PROPERTIES OUTPUT_NAME math-server-benchmarks)
target_link_libraries(benchmarks PRIVATE lexer)
target_link_libraries(benchmarks PRIVATE benchmark benchmark_main)
install(TARGETS benchmarks RUNTIME DESTINATION bin)
install_pdbs(TARGETS benchmarks DESTINATION bin)

add_test(NAME benchmarks COMMAND benchmarks --benchmark_color=false)
