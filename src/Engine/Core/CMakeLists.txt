cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME "Venus-Engine-Core")
project(${MODULE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine core")

include(GNUInstallDirs)
include(Renderer/CMakeSources.cmake)
include(Vulkan/CMakeSources.cmake)
include(Threading/CMakeSources.cmake)

set(VENUS_ENGINE_SRC
        ${ENGINE_THREADING_CORE_SRC}
        ${ENGINE_RENDER_CORE_SRC}
        ${ENGINE_VULKAN_CORE_SRC}
        )

add_library(${MODULE_NAME} SHARED
        ${VENUS_ENGINE_SRC}
        )

target_include_directories(${MODULE_NAME} PRIVATE .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
