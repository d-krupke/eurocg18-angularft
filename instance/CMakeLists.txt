include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(instance
        ./point.h ./point.cpp
        ./problem_instance.h ./problem_instance.cpp angles.cpp angles.h)