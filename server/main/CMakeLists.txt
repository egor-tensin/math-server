option(DEBUG_ASIO "enable debug output for Boost.Asio" OFF)

file(GLOB server_cpp "*.cpp")
file(GLOB server_hpp "*.hpp")
add_executable(server ${server_cpp} ${server_hpp})
set_target_properties(server PROPERTIES OUTPUT_NAME math-server)
if(DEBUG_ASIO)
    target_compile_definitions(server PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif()

target_link_libraries(server PRIVATE common parser)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)

find_package(Boost 1.66.0 REQUIRED COMPONENTS filesystem program_options)
target_link_libraries(server PRIVATE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::program_options)

install(TARGETS server RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:server>" DESTINATION bin OPTIONAL)
endif()
