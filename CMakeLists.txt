cmake_minimum_required(VERSION 3.7)
project(gs_matterhorn)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)
find_package(Threads)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(ground_station)
include_directories(ground_station/)

add_subdirectory(gs_tests)
add_subdirectory(lib/qcustomplot)

add_executable(gs_run main.cpp)
target_link_libraries(gs_run ground_station_lib)
target_link_libraries(gs_run ${CMAKE_THREAD_LIBS_INIT})