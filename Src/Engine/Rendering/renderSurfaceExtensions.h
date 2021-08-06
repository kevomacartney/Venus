//
// Created by Kelvin Macartney on 04/08/2021.
//

#ifndef VENUS_RENDERSURFACEEXTENSIONS_H
#define VENUS_RENDERSURFACEEXTENSIONS_H

#include <cstdint>

namespace Venus::Core::Rendering {

    struct RenderSurfaceExtensions {
        uint32_t extensionCount{0};
        const char **extensions{nullptr};
    };

}

#endif //VENUS_RENDERSURFACEEXTENSIONS_H
