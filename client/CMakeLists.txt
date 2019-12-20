find_package(Boost REQUIRED COMPONENTS filesystem program_options)

add_executable(client main.cpp)

target_include_directories(client SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(client PRIVATE ${Boost_LIBRARIES})
target_compile_definitions(client PRIVATE BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(client PRIVATE Threads::Threads)

set_target_properties(client PROPERTIES OUTPUT_NAME math-client)

install(TARGETS client RUNTIME DESTINATION bin)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES "$<TARGET_PDB_FILE:client>" DESTINATION bin OPTIONAL)
endif()
