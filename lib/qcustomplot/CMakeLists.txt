cmake_minimum_required(VERSION 3.7)
project(qcustomplot_cmake_lib LANGUAGES CXX)

configure_file(CMakeLists.txt.ddl ${CMAKE_BINARY_DIR}/qcustomplot-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qcustomplot-download )
if(result)
    message(FATAL_ERROR "CMake download step for qcustomplot failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qcustomplot-download )
if(result)
    message(FATAL_ERROR "CMake build step for qcustomplot failed: ${result}")
endif()

configure_file(CMakeLists.txt.build ${CMAKE_BINARY_DIR}/qcustomplot-src/CMakeLists.txt)
add_subdirectory(${CMAKE_BINARY_DIR}/qcustomplot-src
        ${CMAKE_BINARY_DIR}/qcustomplot-download)
