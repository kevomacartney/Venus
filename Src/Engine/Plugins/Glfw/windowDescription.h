//
// Created by Kelvin Macartney on 25/03/2020.
//

#ifndef VENUS_WINDOWDESCRIPTION_H
#define VENUS_WINDOWDESCRIPTION_H

#include <string>

namespace Venus::Plugins::Glfw {

    /**
     * Contains video mode information about a window
     */
    struct VideoMode {
        /** The windows width */
        int width;

        /** The windows height */
        int height;

        /** Maximum refresh rate allowed*/
        float maxRefreshRate{60.f};
    };

    /**
     * Contains a description of windows properties used to create the window
     */
    struct WindowDescription {

        /** Output monitor, frame buffer resize and refresh rate. */
        VideoMode videoMode;

        /** Boolean indicating if the window should be in fullscreen*/
        bool isFullScreen{false};

        /** Boolean indicating if the window should be created with depth buffer*/
        bool depthBuffer{true};

        /** Boolean indicating the window should be resizeable */
        bool allowResize{true};

        /** The windows title */
        std::string windowTitle;
    };
}
#endif //VENUS_WINDOWDESCRIPTION_H
