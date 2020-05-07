//
// Created by Kelvin Macartney on 06/03/2020.
//

#ifndef VENUS_GLFWUTILITY_H
#define VENUS_GLFWUTILITY_H

#include "windowDescription.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <loggingFactory.h>
#include <Events/event.h>

namespace Venus::Plugins::Glfw {
    /**
     * Utility class that handles low level GLFW communication
     * @note Each instance creates a new glfw window
     * @note Only one instance per window, EXTERNAL SYNC REQUIRED
     */
    class GlfwUtility {
    public:
        /** Constructor */
        GlfwUtility();

        /** Initialises a window instance to be used with the current instance*/
        void initialize(WindowDescription windowDesc, GLFWwindow *parent = nullptr);

        /**
         * Updates the windows dimensions
         * @param width  The new width
         * @param height The new height
         */
        void setDimensions(uint32_t width, uint32_t height);

        /**
         * Sets the windows visibility
         * @param isVisible If true will set window as visible and hidden otherwise.
         */
        void showWindow(bool isVisible);

        /** Returns the unique window Id */
        uint32_t getWindowId();

        /**
         * @brief Called once every frame
         * @note Should be called from core thread
         */
        void update();

        /**
          * Gets the render surface's extensions
          * @param extensionCount The extension Count
          * @return Pointer to extensions
          */
        const char **getGlfwExtensions(uint32_t &extensionCount);

        Utility::Events::Event<std::function<void()>> WindowClosedEvent;

        ~GlfwUtility();

    private:
        /** Initialises a new glfw window instance from the description*/
        void initialiseWindowInstance(const WindowDescription &windowDescription, GLFWwindow *parent);

        /** Registers a glfw error callback to a local function */
        static void registerToGlfwErrorCallback();

        /** Initialises a logger */
        void createInstanceLogger();

        /** Throws an internal exception if the utility has been terminated */
        void throwIfTerminated();

        /** Prepares utility to terminate and kill the window*/
        void prepareTermination();

        static std::atomic_bool _glfwErrorCallbackRegistered;

        static std::atomic_int32_t _globalWindowIds;

        bool _isTerminated{false};
        int32_t _windowId{0};

        GLFWwindow *_windowInstance{nullptr};
        std::shared_ptr<spdlog::logger> _logger;
    };
}


#endif //VENUS_GLFWUTILITY_H
