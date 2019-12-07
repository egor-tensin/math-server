find_package(Boost REQUIRED COMPONENTS filesystem program_options)

option(DEBUG_ASIO "enable debug output for Boost.Asio" OFF)

add_executable(server main.cpp server.cpp session.cpp session_manager.cpp)
target_link_libraries(server PRIVATE common parser)

target_include_directories(server SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${Boost_LIBRARIES})

if(DEBUG_ASIO)
    target_compile_definitions(server PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif()
