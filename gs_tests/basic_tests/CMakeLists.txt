cmake_minimum_required(VERSION 3.7)
project(basic_tests)

file(GLOB SOURCES "*.cpp")
add_executable(run_basic_tests ${SOURCES})

target_link_libraries(run_basic_tests gtest gtest_main)
target_link_libraries(run_basic_tests ground_station_lib)
