cmake_minimum_required(VERSION 3.8)
project(ground_station LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB SOURCES "*.cpp")
add_library(ground_station_lib ${SOURCES})

# This will link_libraries for the specified Qt modules
qt5_use_modules(ground_station_lib Quick Core)
