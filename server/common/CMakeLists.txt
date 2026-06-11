find_package(Boost CONFIG REQUIRED)

add_library(common INTERFACE)
target_include_directories(common INTERFACE ..)
target_link_libraries(common INTERFACE Boost::disable_autolinking Boost::boost)
