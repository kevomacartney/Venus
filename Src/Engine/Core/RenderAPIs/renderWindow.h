//
// Created by Kelvin Macartney on 15/03/2020.
//

#ifndef VENUS_RENDERWINDOW_H
#define VENUS_RENDERWINDOW_H

#include <glfwUtility.h>
#include <windowDescription.h>
#include <windowProperties.h>
#include <memory>
#include <Threading/threading.h>
#include <Events/event.h>
#include "renderSurface.h"

namespace Venus::Core::RenderApis {
    /**
     * A class that represents a render window that has size, handles, and position
     */
    class RenderWindow : public RenderSurface {
    public:
        /** Constructor*/
        explicit RenderWindow(Plugins::Glfw::WindowDescription description);

        /** Does render window initialisation */
        void ignition() override;

        /** Called once per frame */
        void update() override;

        /** @copydoc Venus::Core::RenderApis::RenderSurface */
        void resize(uint32_t width, uint32_t height) override;

        /** Returns the id of the render window */
        uint32_t getId() override;

        /** Sets the current visibility of the window as shown*/
        void show();

        /** Set the current visibility of the window as hidden */
        void hide();

        /** Called by the core thread when renderApi shutdowns  */
        void shutdown() override;

        /** Destructor*/
        ~RenderWindow() = default;

    private:
        /** Registers to the GLFW window events*/
        void registerToWindowEvents();

        /** Event invoked when GLFW window closes */
        void onWindowClosedEvent();

        std::shared_ptr<Venus::Plugins::Glfw::GlfwUtility> _glfwUtility{nullptr};
        std::shared_ptr<Plugins::Glfw::WindowProperties> _windowProperties{nullptr};
        Plugins::Glfw::WindowDescription _description;

        Mutex _windowPropsMutex;
    };
}


#endif //VENUS_RENDERWINDOW_H
