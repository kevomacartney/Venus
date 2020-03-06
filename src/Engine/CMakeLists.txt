cmake_minimum_required(VERSION 3.15)

set(ENGINE_NAME "Venus-Engine")
project(${ENGINE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine")
set(CORE_LIB_NAME "Venus-Engine-Core")
set(ENGINE_PLUGINS_LIB_NAME "Venus-Engine-Plugins")
set(VENDORS_DIR ${PROJECT_SOURCE_DIR}/vendors)

#glfw
set(GLFW_LIB_NAME glfw)
set(GLFW_DIR ${VENDORS_DIR}/glfw)
#ImGui
set(IMGUI_LIB_NAME Venus-ImGui)
set(IMGUI_DIR ${VENDORS_DIR}/imgui)
#Glm
set(GLM_LIB_NAME glm)
set(GLM_DIR ${VENDORS_DIR}/glm)

include(GNUInstallDirs)
include(Utility/CMakeSources.cmake)

# Add core
add_subdirectory(Core)
# Add plugins
add_subdirectory(Plugins/VulkanApi)

# Sourcecode for startup class
set(STARTUP_CLASS_SRC
        startUp.h
        startUp.cpp
        )
# project source code
set(VENUS_ENGINE_SRC
        ${STARTUP_CLASS_SRC}
        )

add_library(${ENGINE_NAME} SHARED
        ${VENUS_ENGINE_SRC}
        )

target_include_directories(${ENGINE_NAME} PUBLIC .)

target_link_libraries(${ENGINE_NAME}
        ${CORE_LIB_NAME}
        ${ENGINE_PLUGINS_LIB_NAME}
        ${GLFW_LIB_NAME}
        ${IMGUI_LIB_NAME}
        ${GLM_LIB_NAME}
        )

install(TARGETS ${ENGINE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
