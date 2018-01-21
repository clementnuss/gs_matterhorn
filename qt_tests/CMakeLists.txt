cmake_minimum_required(VERSION 3.7)
project(ground_station_qttests LANGUAGES CXX)

find_package(Qt5 COMPONENTS Test Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_executable(qt_tests ${SOURCES} ${HEADERS})
add_test(qt_tests DummyTests)
add_test(qt_tests IntervalTests)

# This will link_libraries for the specified Qt modules
target_link_libraries(qt_tests Qt5::Test
        Qt5::Core Qt5::Gui Qt5::Widgets
        Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)