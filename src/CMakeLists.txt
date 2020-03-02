set (VENUS_FRAMEWORK_VERSION_MAJOR 1)
set (VENUS_FRAMEWORK_VERSION_MINOR 1)
set (VENUS_FRAMEWORK_VERSION_PATCH 0)
set (VENUS_FRAMEWORK_CURRENT_RELEASE_YEAR 2020)

file(GLOB VENUS_SRC
        "*.h"
        "*.cpp"
        )

add_executable(${PROJECT_NAME} ${VENUS_SRC})

# Move project Data to directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${VENUS_RESOURCE_DIR}
        ${PROJECT_BINARY_DIR}/Data
        COMMENT "Copying Data folder to build directory"
        )

add_subdirectory(Engine)
