cmake_minimum_required(VERSION 3.8)
project(gs_matterhorn)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(gs_run ${SOURCE_FILES})

include_directories(ground_station)

add_subdirectory(ground_station)
add_subdirectory(gs_tests)

target_link_libraries(gs_run ground_station)