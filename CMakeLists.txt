# oldstable as of May '26.
# Plus, I'm pretty sure it's required for things like CONFIGURE_DEPENDS and
# CMAKE_MSVC_RUNTIME_LIBRARY and stuff.
cmake_minimum_required(VERSION 3.25)

project(math_server CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/MP /W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)

# Fix this warning:
#
#     Please define _WIN32_WINNT or _WIN32_WINDOWS appropriately.
#
# https://docs.microsoft.com/en-gb/windows/win32/winprog/using-the-windows-headers
if(WIN32)
    add_definitions(/DNTDDI_VERSION=NTDDI_WIN7 /D_WIN32_WINNT=_WIN32_WINNT_WIN7)
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
