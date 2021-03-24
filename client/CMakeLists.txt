file(GLOB client_src "*.cpp" "*.hpp")
add_executable(client ${client_src})
set_target_properties(client PROPERTIES OUTPUT_NAME math-client)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)

find_package(Boost 1.66.0 REQUIRED COMPONENTS filesystem program_options)
target_link_libraries(client PRIVATE
    Boost::disable_autolinking
    Boost::filesystem
    Boost::program_options)

install(TARGETS client RUNTIME DESTINATION bin)
install_pdbs(TARGETS client DESTINATION bin)
