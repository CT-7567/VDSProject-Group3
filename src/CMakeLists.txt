add_subdirectory(test)

add_library(Manager Manager.cpp)

add_subdirectory(verify)
add_subdirectory(bench)
add_subdirectory(reachability)
