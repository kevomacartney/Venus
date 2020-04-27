cmake_minimum_required(VERSION 3.15)

set(MODULE_NAME "Venus-Engine-CoreThread")
project(${MODULE_NAME} VERSION 1.0.1 DESCRIPTION "Venus Engine Core Thread")

include(GNUInstallDirs)
include(CMakeSources.cmake)

add_library(${MODULE_NAME} STATIC
        ${ENGINE_CORETHREAD_CORE_SRC}
        )

target_link_libraries(${MODULE_NAME}
        ${ENGINE_UTILITY_LIB_NAME}
        )


target_include_directories(${MODULE_NAME} PUBLIC .)

install(TARGETS ${MODULE_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION .)
