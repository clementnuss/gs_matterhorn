cmake_minimum_required(VERSION 3.8)
project(gs_matterhorn)

find_package(Qt5 COMPONENTS Quick Core REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


add_subdirectory(ground_station)
include_directories(ground_station/)

add_subdirectory(gs_tests)

add_executable(gs_run main.cpp qml.qrc)
target_link_libraries(gs_run ground_station_lib)
