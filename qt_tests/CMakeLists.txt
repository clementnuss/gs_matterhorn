cmake_minimum_required(VERSION 3.7)
project(ground_station_qttests LANGUAGES CXX)

find_package(Qt5 COMPONENTS Test Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput REQUIRED)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

qt5_wrap_ui(UIS_HDRS ../ground_station/UI/gswidget.ui)
qt5_add_resources(RESOURCES ../ground_station/qt_resources.qrc)

add_executable(qt_tests ${SOURCES} ${HEADERS} ${UIS_HDRS} ${RESOURCES})

set_property(SOURCE ${CMAKE_BINARY_DIR}/qt_tests/ui_gswidget.h
        ${CMAKE_BINARY_DIR}/qt_tests/qrc_qt_resources.cpp PROPERTY SKIP_AUTOGEN ON)

# This will link_libraries for the specified Qt modules
target_link_libraries(qt_tests Qt5::Test
        Qt5::Core Qt5::Gui Qt5::Widgets
        Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)


# Include QCustomPlot libtrary and include file
include_directories(${CMAKE_BINARY_DIR}/qcustomplot-src/)
# Include serial library
include_directories(${CMAKE_BINARY_DIR}/serial-src/include/)

target_link_libraries(qt_tests ground_station_lib)
