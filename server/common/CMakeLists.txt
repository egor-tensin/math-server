find_package(Boost REQUIRED)

add_library(common INTERFACE)
target_include_directories(common INTERFACE ..)
target_link_libraries(common INTERFACE Boost::disable_autolinking Boost::boost)
