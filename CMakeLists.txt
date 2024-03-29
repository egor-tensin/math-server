cmake_minimum_required(VERSION 3.8)

project(math_server CXX)
enable_testing()

include(cmake/common.cmake)

# AppVeyor builds w/ Visual Studio 2017 bombard me with stupid warnings otherwise:
if(MSVC_VERSION GREATER_EQUAL "1910" AND MSVC_VERSION LESS "1920")
    add_definitions(/D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
endif()

# AppVeyor builds complain about _WIN32_WINNT not being defined.
# Not sure what's the right thing to do about it, arbitrarily setting it to
# target Windows 7 as described here:
# https://docs.microsoft.com/en-gb/windows/win32/winprog/using-the-windows-headers
if(WIN32)
    add_definitions(/DNTDDI_VERSION=NTDDI_WIN7 /D_WIN32_WINNT=_WIN32_WINNT_WIN7)
endif()

# Silence deprecation warnings in Boost.Asio:
if(WIN32)
    add_definitions(/D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

add_subdirectory(client)
add_subdirectory(server)

if(MATH_SERVER_TESTS)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Don't install benchmark the library" FORCE)
    add_subdirectory(3rdparty/google/benchmark)

    add_subdirectory(test)
endif()

install(FILES README.md LICENSE.txt DESTINATION share)
