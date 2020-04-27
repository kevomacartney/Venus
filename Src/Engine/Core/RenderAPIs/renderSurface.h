//
// Created by Kelvin Macartney on 24/04/2020.
//

#ifndef VENUS_RENDERSURFACE_H
#define VENUS_RENDERSURFACE_H

namespace Venus::Core::RenderApis {
    class RenderSurface {
    public:
        /** Constructor */
        RenderSurface() = default;

        /** Does the render surface initialisation */
        virtual void ignition() {}

        /** Invoked every frame by the core thread */
        virtual void update() {}

        /** Returns the unique rendersurface id */
        virtual uint32_t getId() {
            VENUS_EXCEPT(NotImplementedException, "Method has not been implement")
        }

        /**
         * Resize the windows dimension
         * @param width    The new width in pixels
         * @param height   The new height in pixels
         * @note pixels should be doubled for retina displays
         */
        virtual void resize(uint32_t width, uint32_t height) {}

        /** Called by the core thread when renderApi shutdowns */
        virtual void shutdown() {}

        virtual ~RenderSurface() = default;
    };
}


#endif //VENUS_RENDERSURFACE_H
