find_package(Boost REQUIRED COMPONENTS filesystem program_options)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(DEBUG_ASIO "enable debug output for Boost.Asio" OFF)

add_executable(server main.cpp server.cpp session.cpp session_manager.cpp)
target_link_libraries(server PRIVATE common parser)
target_link_libraries(server PRIVATE Boost::disable_autolinking Boost::filesystem Boost::program_options Threads::Threads)
set_target_properties(server PROPERTIES OUTPUT_NAME math-server)

if(DEBUG_ASIO)
    target_compile_definitions(server PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif()

install(TARGETS server RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:server>" DESTINATION bin OPTIONAL)
endif()
