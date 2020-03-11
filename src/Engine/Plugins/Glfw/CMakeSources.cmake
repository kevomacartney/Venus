set(GLFW_DIR "Plugins/Glfw")

set(GLFW_PLUGIN_INC # include directories
        "${GLFW_DIR}/glfwUtility.h"
        )

set(GLFW_PLUGIN_SRC # source directories
        "${GLFW_DIR}/glfwUtility.cpp"
        )

set(GLFW_API_PLUGIN_SRC
        ${GLFW_PLUGIN_INC}
        ${GLFW_PLUGIN_SRC}
        )