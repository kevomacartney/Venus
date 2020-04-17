cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME "Venus-Engine-Core")
project(${MODULE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine core")

include(GNUInstallDirs)
include(Renderer/CMakeSources.cmake)
include(Vulkan/CMakeSources.cmake)
include(Threading/CMakeSources.cmake)
include(RenderAPIs/CMakeSources.cmake)
include(Managers/CMakeSources.cmake)
include(System/CMakeSources.cmake)
include(CoreThread/CMakeSources.cmake)

set(VENUS_ENGINE_SRC
        ${ENGINE_THREADING_CORE_SRC}
        ${ENGINE_RENDER_CORE_SRC}
        ${ENGINE_VULKAN_CORE_SRC}
        ${ENGINE_RENDERAPI_CORE_SRC}
        ${ENGINE_MANAGERS_CORE_SRC}
        ${ENGINE_SYSTEM_CORE_SRC}
        ${ENGINE_CORETHREAD_CORE_SRC}
        )

add_library(${MODULE_NAME} SHARED
        ${VENUS_ENGINE_SRC}
        )

target_link_libraries(${MODULE_NAME}
        ${ENGINE_FACTORY_LIB_NAME}
        ${ENGINE_PLUGINS_LIB_NAME}
        ${ENGINE_UTILITY_LIB_NAME}
        ${ENGINE_GLFW_PLUGIN_LIB_NAME}
        ${GLFW_LIB_NAME}
        ${IMGUI_LIB_NAME}
        ${GLM_LIB_NAME}
        ${SPDLOG_LIB_NAME}
        )


target_include_directories(${MODULE_NAME} PUBLIC .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION Venus/Engine/Core)
