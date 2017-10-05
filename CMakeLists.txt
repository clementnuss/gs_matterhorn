cmake_minimum_required(VERSION 3.8)
project(gs_matterhorn)
set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 COMPONENTS Core Gui REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_subdirectory(ground_station)
include_directories(ground_station/)

add_subdirectory(gs_tests)

add_executable(gs_run main.cpp)
target_link_libraries(gs_run ground_station_lib)
