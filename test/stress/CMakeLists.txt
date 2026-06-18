if(CMAKE_HOST_UNIX)
    # Not going to bother with bash on Windows.

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(
        NAME stress_test
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/test.sh"
            "$<TARGET_FILE:server>"
            "$<TARGET_FILE:client>"
    )
endif()
