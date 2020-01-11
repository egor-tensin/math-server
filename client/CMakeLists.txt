find_package(Boost 1.66.0 REQUIRED COMPONENTS filesystem program_options)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(client main.cpp)
target_link_libraries(client PRIVATE Boost::disable_autolinking Boost::filesystem Boost::program_options Threads::Threads)
set_target_properties(client PROPERTIES OUTPUT_NAME math-client)

install(TARGETS client RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:client>" DESTINATION bin OPTIONAL)
endif()
