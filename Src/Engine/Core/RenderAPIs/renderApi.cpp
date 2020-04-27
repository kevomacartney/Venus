//
// Created by Kelvin Macartney on 24/03/2020.
//

#include "renderApi.h"

#include <utility>

namespace Venus::Core::RenderApis {

    RenderApi::RenderApi(std::shared_ptr<RenderSurface> renderSurface)
            : _renderSurface(std::move(renderSurface)) {}

    void RenderApi::ignition() {
        if (this->_isStarted) VENUS_EXCEPT(InvalidOperationException, "RenderApi Instance already ignited")

        this->_isStarted = true;
        getCoreThread()->queueCommand([this]() {
            this->_renderSurface->ignition();
        }, CTQF_InternalQueue | CTQF_BlockUntilComplete);
    }

    void RenderApi::_update() {
        this->_renderSurface->update();
    }

    std::shared_ptr<RenderSurface> RenderApi::getRenderWindow() {
        return this->_renderSurface;
    }

    void RenderApi::destrory() {
        getCoreThread()->queueCommand([this]() {
            this->_renderSurface->shutdown();
        }, CTQF_InternalQueue | CTQF_BlockUntilComplete);
    }
}
