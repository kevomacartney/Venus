cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME "Venus-Engine-Managed")
project(${MODULE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine Mono plugin")

include(GNUInstallDirs)
include(CMakeSources.cmake)

set(ENGINE_GLFW_SRC
        ${ENGINE_GLFW_PLUGIN_SRC})

# project source code
add_library(${MODULE_NAME} SHARED
        ${ENGINE_GLFW_SRC}
        )

target_link_libraries(${MODULE_NAME}
        ${GLFW_LIB_NAME}
        ${ENGINE_UTILITY_LIB_NAME}
        ${ENGINE_FACTORY_LIB_NAME}
        )
target_include_directories(${MODULE_NAME} PUBLIC .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION Venus/Engine/Core/Vulkan)