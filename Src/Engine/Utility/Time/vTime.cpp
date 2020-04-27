//
// Created by Kelvin Macartney on 13/03/2020.
//

#include "vTime.h"

Venus::Utility::Time::VTime::VTime() {
    this->_timer = std::make_unique<VTimer>();

    this->_appStartTime = this->_timer->getElapsedMicroseconds();
    this->_lastTickTime = this->_timer->getElapsedMicroseconds();
}

uint64_t Venus::Utility::Time::VTime::getPreciseMicrosecondTime() {
    return this->_timer->getElapsedMicroseconds();
}

void Venus::Utility::Time::VTime::_tick() {
    this->_lastTickTime = this->_timer->getElapsedMicroseconds();
    this->currentTick.fetch_add(1, std::memory_order_relaxed);

    if (this->_isFirstTick) {
        auto dif = (this->currentTick - this->_lastTickTime);
        std::chrono::microseconds micros(dif);

        this->tickDelta = std::chrono::duration_cast<std::chrono::seconds>(micros).count();
    }else{
        this->tickDelta = 0.0f;
        this->_isFirstTick = false;
    }


}

float Venus::Utility::Time::VTime::secondsSinceStart() {
    std::chrono::microseconds micros(this->_lastTickTime - this->_appStartTime);
    return std::chrono::duration_cast<std::chrono::seconds>(micros).count();
}

uint64_t Venus::Utility::Time::VTime::msSinceStart() {
    std::chrono::microseconds micros(this->_lastTickTime - this->_appStartTime);
    return std::chrono::duration_cast<std::chrono::milliseconds>(micros).count();
}

uint64_t Venus::Utility::Time::VTime::microsSinceStart() {
    return this->_lastTickTime - this->_appStartTime;
}
