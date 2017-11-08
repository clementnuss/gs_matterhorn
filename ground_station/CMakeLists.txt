cmake_minimum_required(VERSION 3.7)
project(ground_station LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport Qml Quick Quickwidgets
        3DCore 3DExtras 3DRender 3DInput REQUIRED)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_executable(ground_station ${SOURCES} ${HEADERS})
add_library(ground_station_lib ${SOURCES} ${HEADERS})

# Include QCustomPlot libtrary and include file
include_directories(${CMAKE_BINARY_DIR}/qcustomplot-src/)
target_link_libraries(ground_station qcustomplot_lib)
target_link_libraries(ground_station_lib qcustomplot_lib)

#Include Boost
target_link_libraries(ground_station Boost::boost Boost::system Boost::thread)
target_link_libraries(ground_station_lib Boost::boost Boost::system Boost::thread)

# Include serial library
include_directories(${CMAKE_BINARY_DIR}/serial-src/include/)
target_link_libraries(ground_station serial)
target_link_libraries(ground_station_lib serial)

# This will link_libraries for the specified Qt modules
target_link_libraries(ground_station
        Qt5::Core Qt5::Gui Qt5::Widgets
        Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput
        Qt5::Qml Qt5::Quick Qt5::QuickWidgets)

# This will link_libraries for the specified Qt modules
target_link_libraries(ground_station_lib
        Qt5::Core Qt5::Gui Qt5::Widgets
        Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput
        Qt5::Qml Qt5::Quick Qt5::QuickWidgets)