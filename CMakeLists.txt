cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME Venus)
set(VENUS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(VENUS_ROOT_DIR ${PROJECT_SOURCE_DIR})
set(VENUS_RESOURCE_DIR ${PROJECT_SOURCE_DIR}/resources)

project(${PROJECT_NAME})

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(Vendors)
add_subdirectory(Src)

