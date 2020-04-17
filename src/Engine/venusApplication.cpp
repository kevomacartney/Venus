//
// Created by Kelvin Macartney on 04/03/2020.
//

#include "venusApplication.h"

#include <unistd.h>
#include <spdlog/async.h>
#include <Events/eventDispatcher.h>
#include <Plugins/Glfw/glfwUtility.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <Managers/renderWindowManager.h>

namespace Venus {

    VenusApplication::VenusApplication() {
        this->_time = std::make_unique<Venus::Utility::Time::VTime>();
    }

    void VenusApplication::Ignition() {
        createLogger();
        this->_logger->info("Application initialization");

        Core::CoreThread::ignite();
        Utility::Events::EventDispatcher::ignite();
        Venus::Core::System::ignite();
        Venus::Core::Managers::RenderWindowManager::ignite();

        auto desc = Utility::Threading::ThreadPoolDescription();
        desc.absoluteMaximum = BS_THREAD_HARDWARE_CONCURRENCY;
        desc.enableWorkStealing = true;
        Utility::Threading::ThreadPool::ignite(desc);

        this->_renderApi = Core::Managers::RenderWindowManager::instance()->createPrimaryWindow();

        this->_logger->info("Application initialization complete");
    }

    void VenusApplication::createLogger() {
        using namespace Venus::Factories::Logging;
        this->_logger = LoggingFactory::CreateLogger(
                []() -> std::__1::shared_ptr<spdlog::logger> {
                    std::vector<spdlog::sink_ptr> sinks;
                    sinks.push_back(
                            std::make_shared<spdlog::sinks::daily_file_sink_st>("logs/engine_logs.log", 23, 59));
                    sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
                    // Create a thread pool of 8k max items and 2 thread
                    spdlog::init_thread_pool(8192, 2);
                    // Create logger
                    auto logger = std::make_shared<spdlog::async_logger>("Venus::Application",
                                                                         begin(sinks), end(sinks),
                                                                         spdlog::thread_pool());
                    spdlog::register_logger(logger);
                    return logger;
                });
    }

    int VenusApplication::go() {
        this->_appThread = std::thread(&VenusApplication::_runMainLoop, this); // run main loop on secondary thread
        Venus::Core::getCoreThread()->_go(); // run core thread on main thread

        this->_appThread.join();

        return 0;
    }

    void VenusApplication::applicationTick() {
        this->_time->_tick();
        this->_renderApi->update();
    }

    void VenusApplication::shutDown() {
        Venus::Core::getCoreThread()->shutdownCoreThread();
        this->_appThread.join();
    }

    void VenusApplication::quitRequest() {
        this->endMainLoop();

    }

    void VenusApplication::_runMainLoop() {
        this->beginMainLoop();

        while (this->_iterateLoop) {
            this->applicationTick();
        }
    }
}