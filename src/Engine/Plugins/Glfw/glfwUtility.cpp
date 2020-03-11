//
// Created by Kelvin Macartney on 06/03/2020.
//

#include "glfwUtility.h"
#include <GLFW/glfw3.h>

void Engine::Plugins::Glfw::GlfwUtility::initalizeGlfw() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);


    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
