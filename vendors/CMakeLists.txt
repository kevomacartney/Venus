cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME Venus-Vendors)

set(IOS_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ios-cmake/ios.toolchain.cmake CACHE INTERNAL "")

add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(rapidjson)
add_subdirectory(backward-cpp)

set(FMT_TEST OFF CACHE BOOL "")
set(FMT_CUDA_TEST OFF CACHE BOOL "")
add_subdirectory(fmt)

set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "Disable Magic-Enum examples")
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "Disable Magic-Enum tests")
add_subdirectory(magic_enum)

# spdlog
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "")
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "")
set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "")
add_subdirectory(spdlog)


