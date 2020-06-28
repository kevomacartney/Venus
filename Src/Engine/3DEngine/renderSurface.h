//
// Created by Kelvin Macartney on 24/04/2020.
//

#ifndef VENUS_RENDERSURFACE_H
#define VENUS_RENDERSURFACE_H

#include <vulkan/vulkan.h>

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

        /** Returns the unique render surface id */
        virtual uint32_t getId() {
            throwNotImplement();
            return 0;
        }

        /** Returns a VK render surface for this instance*/
        VkSurfaceKHR getVkRenderSurface() {
            throwNotImplement();
            return nullptr;
        };

        /**
         * Resize the windows dimension
         * @param width    The new width in pixels
         * @param height   The new height in pixels
         * @note pixels should be doubled for retina displays
         */
        virtual void resize(uint32_t width, uint32_t height) { throwNotImplement(); }

        /**
         * Gets the render surface's extensions
         * @param extensionCount The extension Count
         * @return Pointer to extensions
         */
        virtual std::vector<const char *> getExtensionCount() {
            throwNotImplement();
            return std::vector<const char *>();
        }

        /** Called by the core thread when renderApi shutdowns */
        virtual void shutdown() {}

        virtual ~RenderSurface() = default;

    private:

        /** Throws a not implemented exception */
        static void throwNotImplement() {
            VENUS_EXCEPT(NotImplementedException, "Method has not been implement")
        }
    };
}

#endif //VENUS_RENDERSURFACE_H