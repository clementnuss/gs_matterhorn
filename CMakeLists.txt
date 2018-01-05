cmake_minimum_required(VERSION 3.7)
project(gs_matterhorn)

set(EP_UPDATE_DISCONNECTED 1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(lib/qcustomplot)
add_subdirectory(lib/boost-cmake)
add_subdirectory(lib/serial)

add_subdirectory(ground_station)
include_directories(ground_station/)
add_subdirectory(gs_tests)
add_subdirectory(qt_tests)


