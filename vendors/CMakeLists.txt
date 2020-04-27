cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME Venus-Vendors)


set(IOS_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ios-cmake/ios.toolchain.cmake CACHE INTERNAL "")
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(rapidjson)
add_subdirectory(backward-cpp)
add_subdirectory(fmt)

# spdlog
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

