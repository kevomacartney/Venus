//
// Created by Kelvin Macartney on 26/03/2020.
//

#ifndef VENUS_WINDOWPROPERTIES_H
#define VENUS_WINDOWPROPERTIES_H
#include <string>

namespace Venus::Plugins::Glfw {

    /** Contains various properties for a window */
    struct WindowProperties {
    public:
        /** Window height in pixels*/
        uint32_t height;

        /** Window width in pixel*/
        uint32_t width;

        /** Window title text */
        std::string windowTitle;

        /** Boolean indicating if the window is to be shown */
        bool isShown;

        /** Boolean indicating if the window is is to in full screen*/
        bool isFullScreen;
    };
}
#endif //VENUS_WINDOWPROPERTIES_H
