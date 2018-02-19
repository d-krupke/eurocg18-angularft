
file(GLOB_RECURSE GTEST_SOURCES "../**/*_gtest.cpp")
MESSAGE("Including the following gtest files: ${GTEST_SOURCES}")

file(GLOB_RECURSE GTEST_ASSETS "./assets/*")
MESSAGE("Including the following gtest assets: ${GTEST_ASSETS}")
FILE(MAKE_DIRECTORY "./assets")
FILE(COPY ${GTEST_ASSETS} DESTINATION "./assets")

add_executable(unit_tester
        ${GTEST_SOURCES}
        ./gtest_main.cpp)


target_include_directories(unit_tester PRIVATE ${INCLUDE_DIRS})
target_link_libraries(unit_tester
        ${LIBRARIES}
        gtest
        instance
        optimal_solver)
