cmake_minimum_required(VERSION 3.8)
project(ground_station)

file(GLOB SOURCES "*.cpp")
add_library(ground_station ${SOURCES})