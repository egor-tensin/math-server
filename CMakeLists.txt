cmake_minimum_required(VERSION 3.8)

project(math_server CXX)

option(ENABLE_TESTS "build the tests")

include(cmake/common.cmake)

add_subdirectory(client)
add_subdirectory(server)
if(ENABLE_TESTS)
    add_subdirectory(test)
endif()
