cmake_minimum_required(VERSION 3.9)
project(Code)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(CPLEX_ROOT "/opt/ibm/ILOG/CPLEX_Studio1271")
find_package(CPLEX REQUIRED)
find_package(Threads REQUIRED)
# Boost and its components
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options graph REQUIRED)

add_executable(OptimalSolver main.cpp)

set(LIBRARIES ${CPLEX_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)
set(INCLUDE_DIRS ${CPLEX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_subdirectory(./instance)
add_subdirectory(./ip)
add_subdirectory(./gtest)

target_include_directories(OptimalSolver PRIVATE ${INCLUDE_DIRS})
target_link_libraries(OptimalSolver ${LIBRARIES} instance optimal_solver)