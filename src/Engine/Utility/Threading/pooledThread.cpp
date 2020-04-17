//
// Created by Kelvin Macartney on 30/03/2020.
//

#include "threadPool.h"

#include <utility>

Venus::Utility::Threading::PooledThread::PooledThread(std::string name, uint32_t threadId)
        : _name(std::move(name)),
          _thread(_threadId) {}

void Venus::Utility::Threading::PooledThread::ignition() {
    this->_thread = std::thread(std::bind(&PooledThread::_run, this));

    Lock lock(this->_mutex);

    while (!this->_threadStarted)
        this->_startedCondition.wait(lock);
}

void Venus::Utility::Threading::PooledThread::execute(std::function<void()> workerMethod, uint32_t id) {
    {
        Lock lock(this->_mutex);

        this->_workerThread = std::move(workerMethod);
        this->_idle = false;
        this->_idleTime = std::time(nullptr);
        this->_threadReady = true;
        this->_threadId = id;
    }

    this->_readyCondition.notify_one();
}

void Venus::Utility::Threading::PooledThread::_run() {
    {
        Lock lock(this->_mutex);
        this->_threadStarted = true;
    }

    this->_startedCondition.notify_one();

    while (true) {
        std::function<void()> worker = nullptr;

        {
            {
                Lock lock(this->_mutex);

                while (!this->_threadStarted)
                    this->_readyCondition.wait(lock);

                worker = this->_workerThread;
            }

            if (worker == nullptr) {
                return;
            }
        }

        worker();

        {
            Lock lock(this->_mutex);

            this->_idle = true;
            this->_idleTime = std::time(nullptr);
            this->_threadReady = false;
            this->_workerThread = nullptr; // Make sure to clear as it could have bound shared pointers and similar

            this->_workerFinishedCondition.notify_one();
        }
    }
}

bool Venus::Utility::Threading::PooledThread::isIdle() {
    Lock lock(this->_mutex);

    return this->_idle;
}

uint32_t Venus::Utility::Threading::PooledThread::getId() {
    Lock lock(this->_mutex);

    return this->_threadId;
}

void Venus::Utility::Threading::PooledThread::waitTillComplete() {
    Lock lock(this->_mutex);

    while (!this->_idle)
        this->_workerFinishedCondition.wait(lock);
}

void Venus::Utility::Threading::VenusThread::sleepTillComplete() {

}
