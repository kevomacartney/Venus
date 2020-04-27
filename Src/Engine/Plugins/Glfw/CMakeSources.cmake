set(GLFW_PLUGIN_INC # include directories
        "glfwUtility.h"
        "windowDescription.h"
        "windowProperties.h"
        )

set(GLFW_PLUGIN_SRC # src directories
        "glfwUtility.cpp"
        )

set(ENGINE_GLFW_PLUGIN_SRC
        ${GLFW_PLUGIN_INC}
        ${GLFW_PLUGIN_SRC}
        )