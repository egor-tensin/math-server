cmake_minimum_required(VERSION 3.8)

project(math_server CXX)

option(ENABLE_TESTS "build the tests")

# C++17 is mandatory:
set(CC_CXX_STANDARD 17)

include(cmake/common.cmake)

# AppVeyor builds w/ Visual Studio 2017 bombard me with stupid warnings otherwise:
if(MSVC_VERSION GREATER_EQUAL "1910" AND MSVC_VERSION LESS "1920")
    add_definitions(/D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
endif()

if(WIN32)
    add_definitions(/DNTDDI_VERSION=NTDDI_WIN7 /D_WIN32_WINNT=_WIN32_WINNT_WIN7)
endif()

# Silence deprecation warnings in Boost.Asio:
if(WIN32)
    add_definitions(/D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

add_subdirectory(client)
add_subdirectory(server)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()
