cmake_minimum_required(VERSION 3.8)

project(math_server CXX)

option(ENABLE_TESTS "build the tests")

# C++17 is mandatory:
set(CC_CXX_STANDARD 17)

include(cmake/common.cmake)

add_subdirectory(client)
add_subdirectory(server)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()
