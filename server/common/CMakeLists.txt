add_library(common INTERFACE)
target_include_directories(common INTERFACE ..)

find_package(Boost REQUIRED)
target_link_libraries(common INTERFACE Boost::boost)
