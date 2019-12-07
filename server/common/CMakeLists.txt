find_package(Boost REQUIRED)

add_library(common INTERFACE)

target_include_directories(common SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(common INTERFACE ${Boost_LIBRARIES})
