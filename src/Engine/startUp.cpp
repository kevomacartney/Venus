//
// Created by Kelvin Macartney on 04/03/2020.
//

#include "startUp.h"
#include "iostream"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>
void Engine::StartUp::Ignition() {
    std::cout << "Engine ignition\n";

    glfwInit();
    ImGui::Text("Hello, world %d", 123);
    glm::mat4();
}

int Engine::StartUp::go() {
    return 1;
}
