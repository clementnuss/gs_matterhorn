cmake_minimum_required(VERSION 3.7)
project(decoder_tests)

file(GLOB SOURCES "*.cpp")
add_executable(run_decoder_tests ${SOURCES})

target_link_libraries(run_decoder_tests gtest gtest_main)
target_link_libraries(run_decoder_tests ground_station_lib)
