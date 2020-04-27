cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME Venus-Engine-Factories)
set(VENUS_FACTORY_VERSION 1.0.1)
set(ENGINE_LIB_NAME Venus-Engine)

project(${MODULE_NAME})

include(CMakeSources.cmake)

add_library(${MODULE_NAME} SHARED
        ${ENGINE_CORE_FACTORY_SRC}
        )

target_link_libraries(${MODULE_NAME}
        ${SPDLOG_LIB_NAME}
        )

target_include_directories(${MODULE_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION Venus/Engine/Factories)
