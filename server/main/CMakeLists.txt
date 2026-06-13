find_package(Boost 1.70.0 CONFIG REQUIRED COMPONENTS program_options)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(DEBUG_ASIO "enable debug output for Boost.Asio" OFF)

file(GLOB server_src CONFIGURE_DEPENDS "*.cpp" "*.hpp")
add_executable(server ${server_src})
set_target_properties(server PROPERTIES OUTPUT_NAME math-server)
if(DEBUG_ASIO)
    target_compile_definitions(server PRIVATE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif()
target_link_libraries(server PRIVATE common parser)
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE
    Boost::disable_autolinking
    Boost::program_options)
install(TARGETS server RUNTIME DESTINATION bin)
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:server> DESTINATION bin OPTIONAL)
endif()
