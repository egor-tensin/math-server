add_subdirectory(benchmarks)
add_subdirectory(unit_tests)

if(CMAKE_HOST_UNIX)
    # Not going to bother with bash on Windows.

    # stress_test.py is a Python 3 script.
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME stress_test COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/stress_test.sh" "$<TARGET_FILE:server>" "$<TARGET_FILE:client>")
endif()
