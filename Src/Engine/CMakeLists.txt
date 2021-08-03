cmake_minimum_required(VERSION 3.15)

set(ENGINE_NAME "Venus-Engine" CACHE INTERNAL "")
set(VENUS_ENGINE_VERSION 1.0.1)
project(${ENGINE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine")

# Set our library names
set(CORE_LIB_NAME "Venus-Engine-Core" CACHE INTERNAL "")
set(ENGINE_VULKAN_LIB_NAME "Venus-Engine-Vulkan" CACHE INTERNAL "")
set(ENGINE_FACTORY_LIB_NAME "Venus-Engine-Factories" CACHE INTERNAL "")
set(ENGINE_UTILITY_LIB_NAME "Venus-Engine-Utility" CACHE INTERNAL "")
set(ENGINE_GLFW_PLUGIN_LIB_NAME "Venus-Engine-Glfw" CACHE INTERNAL "")
set(ENGINE_CORETHREAD_LIB_NAME "Venus-Engine-CoreThread" CACHE INTERNAL "")
set(ENGINE_3DENGINE_LIB_NAME "Venus-Engine-3DEngine" CACHE INTERNAL "")


# Set vendor library names
set(GLFW_LIB_NAME glfw CACHE INTERNAL "GLFW")
set(IMGUI_LIB_NAME imgui CACHE INTERNAL "IMGUI")
set(GLM_LIB_NAME glm CACHE INTERNAL "GLM")
set(BACKWARD_NAME backward CACHE INTERNAL "Backward")
set(SPDLOG_LIB_NAME spdlog CACHE INTERNAL "Spd Log")
set(FMT_LIB_NAME fmt CACHE INTERNAL "Fmt Lib")
set(MAGIC_ENUM_LIBNAME magic_enum CACHE INTERNAL "Magic Enum library")

include(GNUInstallDirs)

# Add core
add_subdirectory(Core)
#Add factories
add_subdirectory(Factories)
# Add vulkan plugins
add_subdirectory(Plugins/VulkanApi)
# Add Glfw Plugin
add_subdirectory(Plugins/Glfw)
# Add IOS
add_subdirectory(Mobile/IOS)
# Utility
add_subdirectory(Utility)
# Corethread
add_subdirectory(CoreThread)
# 3d Engine
add_subdirectory(3DEngine)

# Sourcecode for startup class
set(VENUS_APP_SRC
        venusApplication.h
        venusApplication.cpp
        )

# project source code
set(VENUS_ENGINE_SRC
        ${VENUS_APP_SRC}
        )

add_library(${ENGINE_NAME} SHARED
        ${VENUS_ENGINE_SRC}
        )

target_link_libraries(${ENGINE_NAME}
        ${CORE_LIB_NAME}
        ${ENGINE_FACTORY_LIB_NAME}
        ${ENGINE_UTILITY_LIB_NAME}
        ${ENGINE_VULKAN_LIB_NAME}
        ${ENGINE_GLFW_PLUGIN_LIB_NAME}
        ${ENGINE_3DENGINE_LIB_NAME}
        ${GLFW_LIB_NAME}
        ${IMGUI_LIB_NAME}
        ${GLM_LIB_NAME}
        ${SPDLOG_LIB_NAME}
        ${FMT_LIB_NAME}
        )

target_include_directories(${ENGINE_NAME} PUBLIC .)

install(TARGETS ${ENGINE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION Venus/Engine)
