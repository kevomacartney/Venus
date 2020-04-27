cmake_minimum_required(VERSION 3.15)

set(APP_NAME Venus)
set(ENGINE_LIB_NAME Venus-Engine)

project(${APP_NAME})

add_subdirectory(Engine) # Engine directory
#add_subdirectory(Editor) # Editor directory

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG"  CACHE INTERNAL "")

add_executable(${APP_NAME} entryPoint.cpp)

target_link_libraries(${APP_NAME}
        ${ENGINE_LIB_NAME}
        )

target_include_directories(${APP_NAME} PUBLIC .)