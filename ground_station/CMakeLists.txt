cmake_minimum_required(VERSION 3.7)
project(ground_station LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
add_library(ground_station_lib ${SOURCES} ${HEADERS})

# Include QCustomPlot libtrary and include file
include_directories(${CMAKE_BINARY_DIR}/qcustomplot-src/)
target_link_libraries(ground_station_lib qcustomplot_lib ${CMAKE_THREAD_LIBS_INIT})

#Include Boost
target_link_libraries(ground_station_lib Boost::boost Boost::system Boost::thread Boost::program_options)

# Include serial library
include_directories(${CMAKE_BINARY_DIR}/serial-src/include/)
target_link_libraries(ground_station_lib serial)

IF (WIN32)
    target_link_libraries(ground_station_lib ws2_32)
ENDIF ()

# This will link_libraries for the specified Qt modules
qt5_use_modules(ground_station_lib Core Gui Widgets)