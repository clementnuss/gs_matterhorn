cmake_minimum_required(VERSION 3.7)
project(ground_station LANGUAGES CXX)

#find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport 3DCore 3DExtras 3DRender 3DInput REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_executable(ground_station ${SOURCES} ${HEADERS})
add_library(ground_station_lib ${SOURCES} ${HEADERS})

#Include Boost
target_link_libraries(ground_station Boost::boost Boost::system Boost::thread Boost::filesystem Boost::iostreams)
target_link_libraries(ground_station_lib Boost::boost Boost::system Boost::thread Boost::filesystem Boost::iostreams)

# Include serial library
include_directories(${CMAKE_BINARY_DIR}/serial-src/include/)
target_link_libraries(ground_station serial)
target_link_libraries(ground_station_lib serial)
