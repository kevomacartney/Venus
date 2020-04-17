//
// Created by Kelvin Macartney on 18/03/2020.
//

#ifndef VENUS_RENDERAPIMANAGER_H
#define VENUS_RENDERAPIMANAGER_H

#include <loggingFactory.h>
#include <RenderAPIs/renderWindow.h>
#include <Error/venusExceptions.h>
#include <memory>


namespace Venus::Core::Managers {
    using namespace Venus::Core;
    class RenderAPIManager {
    public:
        RenderAPIManager() = default;

        /**
         * Does necessary initialisation
         */
        std::shared_ptr<RenderApis::RenderWindow> ignition();
    private:
        bool _isIgnited{false};
    };
}


#endif //VENUS_RENDERAPIMANAGER_H
