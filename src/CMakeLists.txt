add_subdirectory(test)

option(LABELS_SUPPORT "Build Manager Library with Label support" OFF)
add_library(Manager Manager.cpp)

if (LABELS_SUPPORT)
    target_compile_definitions(Manager PRIVATE ENABLE_LABELS)
endif()

add_subdirectory(verify)
add_subdirectory(bench)
add_subdirectory(reachability)
