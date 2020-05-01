//
// Created by Kelvin Macartney on 06/03/2020.
//

#include "glfwUtility.h"
#include <iostream>
#include <Error/venusExceptions.h>

#define LOGGER_NAME "Venus::Plugin::Glfw::"
namespace Venus::Plugins::Glfw {
    /** Window ids*/
    std::atomic_int32_t GlfwUtility::_globalWindowIds{0};
    /** Boolean indicating if this utility has registered to glfw callback utility*/
    std::atomic_bool GlfwUtility::_glfwErrorCallbackRegistered{false};

    GlfwUtility::GlfwUtility() : WindowClosedEvent(EEF_Default) {}

    void Venus::Plugins::Glfw::GlfwUtility::initialize(WindowDescription windowDesc,
                                                       GLFWwindow *parent) {
        this->_windowId = _globalWindowIds++;
        this->createInstanceLogger();

        GlfwUtility::registerToGlfwErrorCallback();
        this->initialiseWindowInstance(windowDesc, parent);

        _logger->info("Glfw window created, ID: {}", this->_windowId);
    }

    void
    GlfwUtility::initialiseWindowInstance(const WindowDescription &windowDescription,
                                          GLFWwindow *parent) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        this->_windowInstance = glfwCreateWindow(windowDescription.videoMode.width,
                                                 windowDescription.videoMode.height,
                                                 windowDescription.windowTitle.c_str(),
                                                 nullptr,
                                                 parent);
    }

    void GlfwUtility::update() {
        if (this->_isTerminated) {
            _logger->warn("Window {} NOT updated because window has been terminated", this->_windowId);
            return;
        }

        if (glfwWindowShouldClose(this->_windowInstance)) {
            this->prepareTermination();
            return;
        }

        glfwPollEvents();
    }

    void GlfwUtility::setDimensions(uint32_t width, uint32_t height) {
        glfwHideWindow(this->_windowInstance);
    }

    void GlfwUtility::showWindow(bool isVisible) {
        glfwShowWindow(this->_windowInstance);
    }

    void GlfwUtility::createInstanceLogger() {
        this->_logger = Factories::LoggingFactory::CreateLogger(Factories::LoggerType::ColouredStdOut,
                                                                LOGGER_NAME + std::to_string(this->_windowId));
    }

    void GlfwUtility::prepareTermination() {
        WindowClosedEvent.notifyAll();

        glfwDestroyWindow(this->_windowInstance);
        glfwTerminate();
        this->_isTerminated = true;

        this->_windowInstance = nullptr;
        this->_logger->info("Glfw window with ID {} destroyed on request", this->_windowId);

        spdlog::drop(this->_logger->name());
    }

    void GlfwUtility::registerToGlfwErrorCallback() {
        if (GlfwUtility::_glfwErrorCallbackRegistered)
            return;

        glfwSetErrorCallback([](int error, const char *description) {
            spdlog::error("A GLFW window had an error #{} with message {}", error, description);

            VENUS_EXCEPT(InternalErrorException, "There was an internal Window error");
        });

        GlfwUtility::_glfwErrorCallbackRegistered = true;
    }

    uint32_t GlfwUtility::getWindowId() {
        return this->_windowId;
    }

    GlfwUtility::~GlfwUtility() {
        if (!this->_isTerminated)
            this->prepareTermination();
    }
}
