//
// Created by Kelvin Macartney on 18/03/2020.
//

#include "renderWindowManager.h"

using namespace Venus::Core;

namespace Venus::Core::Managers {
    RenderWindowManager::RenderWindowManager() {
        this->createLogger();
    }

    void RenderWindowManager::createLogger() {
        auto loggerType = Factories::LoggerType::ColouredStdOut;
        this->_logger = Factories::LoggingFactory::CreateLogger_Safe(loggerType, "Venus::Managers::RenderWindowManager");
    }

    void RenderWindowManager::ignition() {
        getCoreThread()->queueCommand([]() {
            if (!glfwInit()) VENUS_EXCEPT(InternalErrorException, "There was an error while initialising GLFW")
        }, CTQF_InternalQueue | CTQF_BlockUntilComplete);
    }

    std::shared_ptr<Rendering3D::RenderApi>
    RenderWindowManager::createNewWindow(Plugins::Glfw::WindowDescription description) {
        auto renderWindow  = std::make_shared<Venus::Core::Rendering3D::RenderWindow>(description);
        auto renderApi = std::make_shared<Venus::Core::Rendering3D::RenderApi>(renderWindow);

        renderApi->ignition();

        {
            Lock lock(this->_windowsMutex);
            this->_renderSurfaces.insert({renderApi->getRenderWindow()->getId(), renderApi});
        }

        return renderApi;
    }

    void RenderWindowManager::update() {
        updateWindows();
    }

    void RenderWindowManager::updateWindows() {
        Lock lock(this->_windowsMutex);

        if (_renderSurfaces.empty())
            return;

        for (auto&[renderSurfaceId, renderSurface]: _renderSurfaces) {
            renderSurface->_update();
        }
    }

    Plugins::Glfw::WindowDescription RenderWindowManager::getDefaultWindowDescription() {

        auto dimensions = RenderWindowManager::getPrimaryDisplaySize();

        Plugins::Glfw::WindowDescription windowDesc;
        windowDesc.windowTitle = "Venus";
        windowDesc.videoMode.width = dimensions.x;
        windowDesc.videoMode.height = dimensions.y;

        return windowDesc;
    }

    glm::vec2 RenderWindowManager::getPrimaryDisplaySize() {
        auto monitor = glfwGetPrimaryMonitor();
        auto videoMode = glfwGetVideoMode(monitor);

        return glm::vec2(videoMode->width, videoMode->height);
    }

    void RenderWindowManager::registerRenderApi(std::shared_ptr<Rendering3D::RenderApi> renderApi) {
        Lock lock(this->_windowsMutex);

        this->_renderSurfaces.insert({renderApi->_renderSurface->getId(), renderApi});
    }

    bool RenderWindowManager::shouldQuit() {
        return this->_renderSurfaces.empty();
    }

    void RenderWindowManager::notifyWindowClosed(uint32_t windowId) {
        Lock lock(this->_windowsMutex);

        this->_renderSurfaces.erase(windowId);
    }

    void RenderWindowManager::shutdown() {
        this->_logger->info("Shutting down render window manager");

        for(auto [id, renderWindow] : this->_renderSurfaces){
            renderWindow->destrory();
        }

        this->_logger->info("Successfully destroyed all renderApis");
    }
}
