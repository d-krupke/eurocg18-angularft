include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(optimal_solver
        ../utils/cplex.hpp
        ./boost_graph.h
        ./ip_graph.h ./ip_graph.cpp
        ./ip_point.h ./ip_point.cpp
        ./optimal_solver.h ./optimal_solver.cpp
        ./solution_graph.h ./solution_graph.cpp)

target_include_directories(optimal_solver PRIVATE ${INCLUDE_DIRS})
#target_include_directories(optimal_solver ${INCLUDE_DIRS})
target_link_libraries(optimal_solver ${LIBRARIES} instance)