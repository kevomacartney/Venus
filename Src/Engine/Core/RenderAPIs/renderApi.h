//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_RENDERAPI_H
#define VENUS_RENDERAPI_H

#include <memory>
#include <coreThread.h>
#include "renderSurface.h"

namespace Venus::Core::Managers {
    class RenderWindowManager;
}

namespace Venus::Core::RenderApis {
    /**
     * Provides access to various render apis
     * @note Can be called from any thread
     */
    class RenderApi {
        friend Venus::Core::Managers::RenderWindowManager;

    public:
        explicit RenderApi(std::shared_ptr<RenderSurface> renderSurface);

        /** Queues initialization of the render api and the window on the core thread */
        void ignition();

        /** Retrieve the render API's window instance */
        std::shared_ptr<RenderSurface> getRenderWindow();

        /** Destroy the renderAPI and the render window associated with it */
        void destrory();

    protected:
        /** Called once per frame */
        void _update();

    private:
        std::shared_ptr<RenderSurface> _renderSurface;

        bool _isStarted{false};
    };
}


#endif //VENUS_RENDERAPI_H
