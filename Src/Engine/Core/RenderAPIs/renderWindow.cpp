//
// Created by Kelvin Macartney on 15/03/2020.
//

#include "renderWindow.h"
#include <Managers/renderWindowManager.h>
#include <iostream>
#include <utility>
#include <coreThread.h>

using namespace Venus::Plugins::Glfw;
namespace Venus::Core::RenderApis {
    RenderWindow::RenderWindow(Plugins::Glfw::WindowDescription description) :
            _description(std::move(description)) {
        this->_glfwUtility = std::make_shared<Venus::Plugins::Glfw::GlfwUtility>();
    }

    void RenderWindow::ignition() {
        THROW_IF_NOT_CORE_THREAD

        this->_glfwUtility->initialize(this->_description);
        this->registerToWindowEvents();

        this->show();
    }

    void RenderWindow::update() {
        getCoreThread()->queueCommand([this] {
                                          this->_glfwUtility->update();
                                      },
                                      CTQF_InternalQueue | CTQF_BlockUntilComplete);
    }

    void RenderWindow::resize(uint32_t width, uint32_t height) {
        auto coreThread = CoreThread::instance();

        coreThread->queueCommand([this, width, height]() {
            {
                Lock lock(this->_windowPropsMutex);
                this->_windowProperties->width = width;
                this->_windowProperties->width = height;
            }

            this->_glfwUtility->setDimensions(width, height);
        });
        coreThread->submit();
    }

    void RenderWindow::show() {
        auto coreThread = CoreThread::instance();

        coreThread->queueCommand([this]() {
            this->_glfwUtility->showWindow(true);
        });
        coreThread->submit();
    }

    void RenderWindow::hide() {
        auto coreThread = CoreThread::instance();

        coreThread->queueCommand([this]() {
            this->_glfwUtility->showWindow(false);
        });
        coreThread->submit();
    }

    void RenderWindow::registerToWindowEvents() {
        this->_glfwUtility->WindowClosedEvent.subscribe([this] { this->onWindowClosedEvent(); });
    }

    void RenderWindow::onWindowClosedEvent() {
        Core::Managers::RenderWindowManager::instance()->notifyWindowClosed(this->getId());
    }

    uint32_t RenderWindow::getId() {
        return this->_glfwUtility->getWindowId();
    }

    void RenderWindow::shutdown() {
        RenderSurface::shutdown();
    }
}




