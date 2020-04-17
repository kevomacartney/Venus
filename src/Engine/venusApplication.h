//
// Created by Kelvin Macartney on 04/03/2020.
//

#ifndef VENUS_VENUSAPPLICATION_H
#define VENUS_VENUSAPPLICATION_H

#include <memory>
#include <spdlog/sinks/sink.h>
#include <Utility/Time/vTime.h>
#include <spdlog/spdlog.h>
#include <Factories/loggingFactory.h>
#include <Core/System/system.h>
#include <Managers/renderWindowManager.h>
#include <CoreThread/coreThread.h>
#include <RenderAPIs/renderWindow.h>
#include <RenderAPIs/renderApi.h>
#include <Threading/threadPool.h>
#include <Events/eventDispatcher.h>

namespace Venus {
    class VenusApplication {
    public:
        VenusApplication();

        /** Ignites the engine and prepares it for operation */
        void Ignition();

        /** Engine prepares engine for main loop */
        int go();

        /** Shuts down the application */
        void shutDown();

    protected:
        /** Create the applications logger */
        void createLogger();

        /** Called by worker thread to run main loop */
        void _runMainLoop();

    private:
        std::shared_ptr<spdlog::sinks::sink> _dailyFileSink{nullptr};
        std::unique_ptr<Venus::Utility::Time::VTime> _time;

        std::shared_ptr<Venus::Core::RenderApis::RenderApi> _renderApi{nullptr};
        std::shared_ptr<spdlog::logger> _logger;

        std::thread _appThread{};
        bool _iterateLoop{false};

        /** Function called on every tick */
        void applicationTick();

        /** Requests the core application to end*/
        void quitRequest();

        void beginMainLoop() {
            _iterateLoop = true;
        }

        void endMainLoop() {
            _iterateLoop = false;
        }
    };
}

#endif //VENUS_VENUSAPPLICATION_H
