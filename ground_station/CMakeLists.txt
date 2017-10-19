cmake_minimum_required(VERSION 3.7)
project(ground_station LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCES "*.cpp")
add_library(ground_station_lib ${SOURCES})

# Include QCustomPlot libtrary and include file
include_directories(${CMAKE_BINARY_DIR}/qcustomplot-src/)
target_link_libraries(ground_station_lib qcustomplot_lib Boost::boost Boost::system)
IF (WIN32)
    target_link_libraries(ground_station_lib ws2_32)
ENDIF ()

# This will link_libraries for the specified Qt modules
qt5_use_modules(ground_station_lib Core Gui Widgets)