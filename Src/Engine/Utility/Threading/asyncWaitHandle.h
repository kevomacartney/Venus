//
// Created by Kelvin Macartney on 17/04/2020.
//

#ifndef VENUS_ASYNCWAITHANDLE_H
#define VENUS_ASYNCWAITHANDLE_H

#include "threading.h"
#include <atomic>

namespace Venus::Utility::Threading {
    class AsyncWaitHandle {
    public:
        AsyncWaitHandle() = default;

        ~AsyncWaitHandle() = default;

        /** Waits to be signalled to be set */
        void wait();

        /**
         * Waits to be signalled or for the timeout
         * @param msBeforeTimeout The length to wait before giving up and returning
         * @return True if successfully signalled and false if timed out
         */
        bool wait(uint64_t msBeforeTimeout);

    protected:
        Signal _handleOpenSignal;
        Mutex _handleOpenLock;

        std::atomic<bool> _handleOpen{false};
    };
}
#endif //VENUS_ASYNCWAITHANDLE_H
