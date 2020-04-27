cmake_minimum_required(VERSION 3.15)

set(ENGINE_NAME "Venus-Engine" CACHE INTERNAL "")
set(VENUS_ENGINE_VERSION 1.0.1)
project(${ENGINE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine")

set(CORE_LIB_NAME "Venus-Engine-Core" CACHE INTERNAL "")
set(ENGINE_PLUGINS_LIB_NAME "Venus-Engine-Plugins" CACHE INTERNAL "")
set(ENGINE_FACTORY_LIB_NAME "Venus-Engine-Factories" CACHE INTERNAL "")
set(ENGINE_UTILITY_LIB_NAME "Venus-Engine-Utility" CACHE INTERNAL "")
set(ENGINE_GLFW_PLUGIN_LIB_NAME "Venus-Engine-Glfw" CACHE INTERNAL "")
set(ENGINE_CORETHREAD_LIB_NAME "Venus-Engine-CoreThread" CACHE INTERNAL "")

set(VENDORS_DIR ${PROJECT_SOURCE_DIR}/vendors)

message(STATUS "Build Venus-Engine: ${VENUS_ENGINE_VERSION}")

#glfw
set(GLFW_LIB_NAME glfw CACHE INTERNAL "")
#ImGui
set(IMGUI_LIB_NAME Venus-ImGui CACHE INTERNAL "")
#Glm
set(GLM_LIB_NAME glm CACHE INTERNAL "")
#Backward stacktrace
set(BACKWARD_NAME backward CACHE INTERNAL "")
#spdlog
set(SPDLOG_LIB_NAME spdlog CACHE INTERNAL "")
#fmt
set(FMT_LIB_NAME fmt CACHE INTERNAL "")

include(GNUInstallDirs)
include(Plugins/Glfw/CMakeSources.cmake)

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

# Sourcecode for startup class
set(STARTUP_CLASS_SRC
        venusApplication.h
        venusApplication.cpp
        )
# project source code
set(VENUS_ENGINE_SRC
        ${STARTUP_CLASS_SRC}
        ${GLFW_API_PLUGIN_SRC}
        ${ENGINE_CORE_FACTORY_SRC})

add_library(${ENGINE_NAME} SHARED
        ${VENUS_ENGINE_SRC}
        )

target_include_directories(${ENGINE_NAME} PUBLIC .)

target_link_libraries(${ENGINE_NAME}
        ${CORE_LIB_NAME}
        ${ENGINE_FACTORY_LIB_NAME}
        ${ENGINE_PLUGINS_LIB_NAME}
        ${ENGINE_UTILITY_LIB_NAME}
        ${ENGINE_GLFW_PLUGIN_LIB_NAME}
        ${GLFW_LIB_NAME}
        ${IMGUI_LIB_NAME}
        ${GLM_LIB_NAME}
        ${SPDLOG_LIB_NAME}
        ${FMT_LIB_NAME}
        )

find_package(Vulkan REQUIRED)

if(VULKAN_FOUND)
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${ENGINE_NAME} ${Vulkan_LIBRARIES})
endif()

install(TARGETS ${ENGINE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION Venus/Engine)
