//
// Created by Kelvin Macartney on 04/03/2020.
//

#include "venusApplication.h"

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <Events/eventDispatcher.h>
#include <Threading/TaskScheduler/taskScheduler.h>
#include <Managers/renderWindowManager.h>
#include <Plugins/VulkanApi/vulkanUtility.h>
#include <CoreThread/coreThread.h>
#include <Factories/loggingFactory.h>
#include <Core/System/system.h>
#include <Threading/threadPool.h>

namespace Venus {

    VenusApplication::VenusApplication() {
        this->_time = std::make_unique<Venus::Utility::Time::VTime>();
    }


    void VenusApplication::setRenderSurface(std::shared_ptr<Core::RenderApis::RenderSurface> renderSurface) {
        if (!renderSurface) VENUS_EXCEPT(InvalidOperationException, "Render surface must be an instance")

        this->_renderApi = std::make_shared<Core::RenderApis::RenderApi>(renderSurface);
    }

    int VenusApplication::Ignition(RenderSurfaceType applicationType) {
        _createLogger();
        this->_logger->info("Application initialization started");

        Core::CoreThread::ignite();
        VenusApplication::multiThreadingInitialisation(); // We need to initialise multithreading before starting app thread
        // because modules being initialised may need to queue work

        this->_appThread = std::thread(&VenusApplication::_completeInitialisation, this);
        Venus::Core::getCoreThread()->_go(); // run core thread on main thread

        this->_appThread.join();
        return 0;
    }

    void VenusApplication::_completeInitialisation() {
#ifdef __APPLE__
        pthread_setname_np(APPLICATION_THREAD_NAME);
#endif
        Venus::Core::System::ignite(); // System
        Utility::Events::EventDispatcher::ignite(); // EventDispatcher
        Venus::Core::Managers::RenderWindowManager::ignite(); // RenderWindowManager
        this->_createMainWindow();

        this->_initialiseVulkan();

        this->_logger->info("Application initialization complete");
        this->_runMainLoop();
    }

    void VenusApplication::_initialiseVulkan() {
        Plugins::Vulkan::WSIExtensions extensions;

        extensions.Extensions = this->_renderApi
                ->getRenderWindow()
                ->getExtensionCount();

        Module<Plugins::Vulkan::VulkanUtility>::ignite(extensions);
    }

    void VenusApplication::multiThreadingInitialisation() {
        _initialiseThreadPool(); // ThreadPool
        Module<::Venus::Utility::Threading::TaskScheduler>::ignite(); // TaskScheduler
    }

    void VenusApplication::_initialiseThreadPool() {
        auto desc = Utility::Threading::ThreadPoolDescription();
        desc.absoluteMaximum = THREAD_HARDWARE_CONCURRENCY > 16 ? 16 : THREAD_HARDWARE_CONCURRENCY;
        desc.enableWorkStealing = true;

        Module<::Venus::Utility::Threading::ThreadPool>::ignite(desc);
    }

    void VenusApplication::_createLogger() {
        spdlog::init_thread_pool(8192, 2);

        this->_logger = Factories::LoggingFactory::CreateLogger(
                []() -> std::__1::shared_ptr<spdlog::logger> {
                    auto path = Factories::LoggingFactory::createLogFilePath("Venus_Engine.log");

                    std::vector<spdlog::sink_ptr> sinks;
                    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_st>(path, 23, 59));
                    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());

                    auto logger = std::make_shared<spdlog::async_logger>("Venus::Application",
                                                                         begin(sinks), end(sinks),
                                                                         spdlog::thread_pool());
                    spdlog::register_logger(logger);
                    return logger;
                });
    }

    void VenusApplication::_createMainWindow() {
        if (this->_renderApi != nullptr) {
            _logger->info("Engine rendering to custom render surface");
            Core::Managers::RenderWindowManager::instance()->registerRenderApi(this->_renderApi);
            return;
        }

        auto description = Core::Managers::RenderWindowManager::getDefaultWindowDescription();
        this->_renderApi = Core::Managers::RenderWindowManager::instance()->createNewWindow(description);
    }

    void VenusApplication::quitRequested() {
        this->endMainLoop();
    }

    void VenusApplication::shutDown() {
        this->_logger->info("Engine shutting down");

        Venus::Core::System::shutDown(); // System
        Venus::Core::Managers::RenderWindowManager::shutDown(); // RenderWindowManager
        Venus::Core::CoreThread::shutDown(); // Core thread
        Utility::Events::EventDispatcher::shutDown(); // EventDispatcher
        Venus::Utility::Threading::TaskScheduler::shutDown(); // TaskScheduler
        Venus::Core::ThreadPool::shutDown(); // Threadpool
        Venus::Plugins::Vulkan::VulkanUtility::shutDown(); // Vulkan utility
        spdlog::shutdown();

        this->_time = nullptr;
    }

    void VenusApplication::_runMainLoop() {
        this->beginMainLoop();

        while (this->_iterateLoop) {
            this->_applicationTick();
        }

        this->shutDown();
    }

    void VenusApplication::_applicationTick() {
        if (Core::Managers::RenderWindowManager::instance()->shouldQuit()) {
            this->quitRequested();
            return;
        }

        this->_time->_tick();

        this->preUpdate();
        Core::Managers::RenderWindowManager::instance()->update();

        this->postUpdate();
    }

    void VenusApplication::beginMainLoop() {
        _iterateLoop = true;
    }

    void VenusApplication::endMainLoop() {
        _iterateLoop = false;
    }

    void VenusApplication::preUpdate() {
        // Do nothing
    }

    void VenusApplication::postUpdate() {
        // Do nothing
    }
}