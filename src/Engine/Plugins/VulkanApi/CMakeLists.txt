cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME "Venus-Engine-Plugins")
project(${MODULE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine plugins")

include(GNUInstallDirs)
include(CMakeSources.cmake)

set(ENGINE_PLUGINS_SRC
        ${VULKAN_API_PLUGIN_SRC})

# project source code
add_library(${MODULE_NAME} SHARED
        ${ENGINE_PLUGINS_SRC}
        )

target_include_directories(${MODULE_NAME} PRIVATE .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})