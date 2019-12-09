find_package(Boost REQUIRED COMPONENTS filesystem program_options)

option(DEBUG_ASIO "enable debug output for Boost.Asio" OFF)

add_executable(server main.cpp server.cpp session.cpp session_manager.cpp)
target_link_libraries(server PRIVATE common parser)

target_include_directories(server SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${Boost_LIBRARIES})
target_compile_definitions(server PRIVATE BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

if(DEBUG_ASIO)
    target_compile_definitions(server PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif()

install(TARGETS server RUNTIME DESTINATION bin)
