//
// Created by Kelvin Macartney on 18/03/2020.
//

#ifndef VENUS_RENDERWINDOWMANAGER_H
#define VENUS_RENDERWINDOWMANAGER_H

#include <windowDescription.h>
#include <glfwUtility.h>
#include <Error/venusExceptions.h>
#include <RenderAPIs/renderApi.h>
#include <RenderAPIs/renderWindow.h>
#include <loggingFactory.h>
#include <glm/glm.hpp>
#include <Module.h>
#include <memory>
#include <map>


namespace Venus::Core::Managers {
    using namespace Venus::Core;

    class RenderWindowManager : public Module<RenderWindowManager> {
    public:
        RenderWindowManager();

        void ignition() override;

        /** Called once per frame */
        void update();

        /** Creates a new render window and returns it's render API */
        std::shared_ptr<RenderApis::RenderApi> createNewWindow(Plugins::Glfw::WindowDescription description);

        /**
         * Registers a new render API to the Manager
         * @note from this point the manager owns the api and will update it every frame
         * @param renderApi The render Api instance
         */
        void registerRenderApi(std::shared_ptr<RenderApis::RenderApi> renderApi);

        /**
         * Returns boolean indicating if the app should quit
         * @note This is true if either all windows or primary window are / is closed
         */
        bool shouldQuit();

        /**
         * Returns the default window description
         * @note Window coordinates are the same size as the primary monitor
         */
        static Plugins::Glfw::WindowDescription getDefaultWindowDescription();

        /**
         * Notifies the manager that a window has closed
         * @param windowId The window identifier
         */
        void notifyWindowClosed(uint32_t windowId);

        /** Returns the primary display's resolution*/
        static glm::vec2 getPrimaryDisplaySize();

        /** Shuts down the module by destroying all renderApi registered */
        void shutdown() override;

    private:
        /** Updates all the renderable windows*/
        void updateWindows();

        /** Creates logger for the module */
        void createLogger();

        bool _isIgnited{false};

        std::shared_ptr<spdlog::logger> _logger;

        Mutex _windowsMutex{};
        std::map<uint32_t, std::shared_ptr<RenderApis::RenderApi>> _renderSurfaces{};

    };
}


#endif //VENUS_RENDERWINDOWMANAGER_H
