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
#include <Threading/asyncWaitHandleImpl.h>

namespace Venus {
    enum RenderSurfaceType {
        /** The engine will render to viewport specified */
        ViewportMode,

        /** The engine will create it's own main window */
        StandaloneModel
    };

    class VenusApplication {
    public:
        VenusApplication();

        /** Start running the engine */
        int Ignition(RenderSurfaceType applicationType = RenderSurfaceType::StandaloneModel);

        /** Sets the engines render surface, could be a viewport in an editor*/
        void setRenderSurface(std::shared_ptr<Core::RenderApis::RenderSurface> renderSurface);

        /** Shuts down the application */
        void shutDown();

        virtual ~VenusApplication() = default;

    protected:
        /** Starts the application's main loop*/
        virtual void beginMainLoop();

        /** Ends the applications main loop  */
        virtual void endMainLoop();

        /** Requests the core application to end*/
        virtual void quitRequested();

        /** Called on every tick, Called before engine components are updated */
        virtual void preUpdate();

        /** Called on every tick, Called after engine components are updated */
        virtual void postUpdate();

    private:
        std::shared_ptr<Venus::Core::RenderApis::RenderApi> _renderApi{nullptr};
        std::unique_ptr<Venus::Utility::Time::VTime> _time;
        std::shared_ptr<spdlog::logger> _logger;

        std::thread _appThread{};
        bool _iterateLoop{false};

        /** Function called on every tick by the main loop */
        void _applicationTick();

        /** Creates a main window for the application */
        void _createMainWindow();

        /** Create the applications logger */
        void _createLogger();

        /** Called by worker thread to run main loop */
        void _runMainLoop();

        /** Completes the applications initialisation by creating a new thread*/
        void _completeInitialisation();

        static void multiThreadingInitialisation();

        /** Initialises the thread pool*/
        static void _initialiseThreadPool();
    };
}

#endif //VENUS_VENUSAPPLICATION_H
