//
// Created by Kelvin Macartney on 13/03/2020.
//

#include "vTimer.h"

void Venus::Utility::Time::VTimer::resetTimer() {
    this->_startTime = this->_clock.now();
}

uint64_t Venus::Utility::Time::VTimer::getStartMilliseconds() {
    auto startTimeNs = this->_startTime.time_since_epoch();
    return duration_cast<std::chrono::milliseconds>(startTimeNs).count();
}

uint64_t Venus::Utility::Time::VTimer::getElapsedMilliseconds() {
    std::chrono::duration<double> duration = (this->_clock.now()) - this->_startTime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

uint64_t Venus::Utility::Time::VTimer::getElapsedMicroseconds() {
    std::chrono::duration<double> duration = (this->_clock.now()) - this->_startTime;
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}
