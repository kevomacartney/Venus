//
// Created by Kelvin Macartney on 27/04/2020.
//
#include "asyncWaitHandle.h"
#include "TaskScheduler/taskScheduler.h"

void Venus::Utility::Threading::AsyncWaitHandle::wait()  {
    Lock lock(this->_handleOpenLock);

    while (!this->_handleOpen) {
        TaskScheduler::instance()->addWorker();
        _handleOpenSignal.wait(lock);
        TaskScheduler::instance()->addWorker();
    }
}

bool Venus::Utility::Threading::AsyncWaitHandle::wait(uint64_t msBeforeTimeout) {

    Lock lock(this->_handleOpenLock);
    if (_handleOpen)
        return true;

    auto start = std::chrono::system_clock::now();

    while (!this->_handleOpen) {
        if (_handleOpenSignal.wait_until(lock, start + std::chrono::milliseconds(msBeforeTimeout)) ==
            std::cv_status::timeout)
            return false;

        auto timeToSpuriousness =
                std::chrono::milliseconds(msBeforeTimeout) - (std::chrono::system_clock::now() - start);

        int msLeftToWait = std::chrono::duration_cast<std::chrono::milliseconds>(timeToSpuriousness).count();
        return wait(msLeftToWait);
    }

    return true;
}
