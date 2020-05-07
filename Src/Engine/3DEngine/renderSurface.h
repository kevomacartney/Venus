//
// Created by Kelvin Macartney on 24/04/2020.
//

#ifndef VENUS_RENDERSURFACE_H
#define VENUS_RENDERSURFACE_H

#include <Error/venusExceptions.h>

namespace Venus::Core::RenderApis {
    /**
     * Represents a surface that can be rendered to by a render API
     */
    class RenderSurface {
    public:
        /** Constructor */
        RenderSurface() = default;

        /** Does the render surface initialisation */
        virtual void ignition() { throwNotImplement(); }

        /** Invoked every frame by the core thread */
        virtual void update() { throwNotImplement(); }

        /** Returns the unique rendersurface id */
        virtual uint32_t getId() { throwNotImplement(); }

        /**
         * Resize the windows dimension
         * @param width    The new width in pixels
         * @param height   The new height in pixels
         * @note pixels should be doubled for retina displays
         */
        virtual void resize(uint32_t width, uint32_t height) { throwNotImplement(); }

        /** Called by the core thread when renderApi shutdowns */
        virtual void shutdown() {}

        /**
         * Gets the render surface's extensions
         * @param extensionCount The extension Count
         * @return Pointer to extensions
         */
        virtual const char **getExtensionCount(uint32_t &extensionCount) { throwNotImplement(); }

        virtual ~RenderSurface() = default;

    private:
        /** Throws a not implemented exception */
        void throwNotImplement() {
            VENUS_EXCEPT(NotImplementedException, "Method has not been implement")
        }
    };
}

#endif //VENUS_RENDERSURFACE_H