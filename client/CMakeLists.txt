find_package(Boost REQUIRED COMPONENTS filesystem program_options)

add_executable(client main.cpp)
target_include_directories(client SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(client PRIVATE ${Boost_LIBRARIES})
target_compile_definitions(client PRIVATE BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)

install(TARGETS client RUNTIME DESTINATION bin)
