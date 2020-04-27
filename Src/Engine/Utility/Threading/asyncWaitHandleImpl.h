//
// Created by Kelvin Macartney on 17/04/2020.
//

#ifndef VENUS_ASYNCWAITHANDLEIMPL_H
#define VENUS_ASYNCWAITHANDLEIMPL_H

#include "asyncWaitHandle.h"

namespace Venus::Utility::Threading {
    class AsyncWaitHandleImpl : public AsyncWaitHandle {
    public:
        AsyncWaitHandleImpl() = default;
        ~AsyncWaitHandleImpl() = default;
        /**
         * Signals all waiting threads to continue
         */
        void set() {
            {
                Lock handleOpenLock(this->_handleOpenLock);
                _handleOpen = true;
            }

            this->_handleOpenSignal.notify_all();
        }

        /**
         * Resets the wait handle, any threads that call waitone will be blocked
         */
        void reset() {
            {
                Lock handleOpenLock(this->_handleOpenLock);
                _handleOpen = false;
            }

            this->_handleOpenSignal.notify_all();
        }
    };
}


#endif //VENUS_ASYNCWAITHANDLEIMPL_H
