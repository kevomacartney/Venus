//
// Created by Kelvin Macartney on 04/03/2020.
//

#include "startUp.h"
#include <Plugins/Glfw/glfwUtility.h>

void Engine::StartUp::Ignition() {
    using namespace Engine;

    Plugins::Glfw::GlfwUtility glfwUtility;

    glfwUtility.initalizeGlfw();
}

int Engine::StartUp::go() {
    return 1;
}
