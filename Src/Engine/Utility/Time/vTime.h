//
// Created by Kelvin Macartney on 13/03/2020.
//

#ifndef VENUS_VTIME_H
#define VENUS_VTIME_H

#include <memory>
#include "vTimer.h"

namespace Venus::Utility::Time {
    class VTime {
    public:
        VTime();

        /**
         * Returns the precise time since start, will return the exact time at moment of invocation
         * @return Time in milliseconds
         */
        uint64_t getPreciseMicrosecondTime();

        /**
         * Invoked by Venus Application on every frame
         */
        void _tick();

        /**
         * Returns the time in seconds since start
         * @note
         * Only updated on every tick
         * @return Time in seconds
         */
        float secondsSinceStart();

        /**
         * Returns the time in milliseconds since start
         * @note
         * Only updated on every tick
         * @return time in milliseconds
         */
        uint64_t msSinceStart();

        /**
         * Returns the time is microseconds since start
         * @note
         * Only updated on every tick
         * @return time in microseconds
         */
        uint64_t microsSinceStart();

    private:
        // The applications start time in microseconds
        uint64_t _appStartTime{0u};
        // The last tick time in microseconds
        uint64_t _lastTickTime{0u};
        // Boolean indicating if the next frame is the first frame
        bool _isFirstTick{true};

        // The tick delta
        float tickDelta{0u};

        // Current tick
        std::atomic<uint64_t> currentTick{0u};
        // Gets the timer instance
        std::unique_ptr<VTimer> _timer;
    };
}


#endif //VENUS_VTIME_H
