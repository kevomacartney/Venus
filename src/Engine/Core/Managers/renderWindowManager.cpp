//
// Created by Kelvin Macartney on 18/03/2020.
//

#include "renderApiManager.h"

using namespace Venus::Core;

std::shared_ptr<RenderApis::RenderWindow> Venus::Core::Managers::RenderAPIManager::ignition() {
    if (this->_isIgnited)
        VENUS_EXCEPT(InternalErrorException, "Trying to start a module that's already started.");

    this->_isIgnited = true;

    return nullptr;
}
