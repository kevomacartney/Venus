cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME "Venus-Engine-Rendering")
project(${MODULE_NAME} VERSION 1.0.1 DESCRIPTION "Venus 3D Engine lib")

include(GNUInstallDirs)
include(CMakeSources.cmake)

set(ENGINE_RENDERING_SRC
        ${RENDERING_SRC})

# project source code
add_library(${MODULE_NAME} SHARED
        ${ENGINE_RENDERING_SRC}
        )

target_link_libraries(${MODULE_NAME}
        ${ENGINE_UTILITY_LIB_NAME}
        ${Vulkan_LIBRARIES}
        )

target_include_directories(${MODULE_NAME} PUBLIC .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION Venus/Engine/)