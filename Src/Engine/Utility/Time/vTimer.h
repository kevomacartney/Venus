//
// Created by Kelvin Macartney on 13/03/2020.
//

#ifndef VENUS_VTIMER_H
#define VENUS_VTIMER_H

#include <ctime>
#include <chrono>

namespace Venus::Utility::Time {
    class VTimer {
    public:
        VTimer(){
            this->resetTimer();
        }

        // Resets the timer
        void resetTimer();

        // Gets the elapsed milliseconds
        uint64_t getElapsedMilliseconds();

        // Gets the elapsed milliseconds
        uint64_t getElapsedMicroseconds();

        // Gets UNIX epoch start up time
        uint64_t getStartMilliseconds();
    private:
        std::chrono::high_resolution_clock _clock;
        std::chrono::time_point<std::chrono::high_resolution_clock> _startTime;
    };
}


#endif //VENUS_VTIMER_H
