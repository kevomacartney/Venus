//
// Created by Kelvin Macartney on 24/03/2020.
//

#include "coreThread.h"

#include <utility>
#include "queuedCommand.h"

std::weak_ptr<Venus::Core::CoreThread>  Venus::Core::CoreThread::_coreThreadInstance;

thread_local std::shared_ptr<Venus::Core::CoreThread::ThreadQueueContainer>
        Venus::Core::CoreThread::_perThreadContainerData;

void Venus::Core::CoreThread::ignition() {
    Venus::Core::CoreThread::_coreThreadInstance = shared_from_this();

    this->_workerThread = THREAD_CURRENT_ID;
    this->_coreThreadId = this->_workerThread; // for now

    this->_commandQueue = std::make_shared<CommandQueue<CommandQueueSync>>(this->_coreThreadId);
}

void Venus::Core::CoreThread::throwIfCoreThread() {
    auto instance = CoreThread::instance();
    if (THREAD_CURRENT_ID == instance->getThreadId()) VENUS_EXCEPT(InternalErrorException,
                                                                   "This method cannot be accessed from core thread.");
}

void Venus::Core::CoreThread::throwIfNotCoreThread() {
    auto instance = CoreThread::instance();
    if (THREAD_CURRENT_ID != instance->getThreadId()) VENUS_EXCEPT(InternalErrorException,
                                                                   "This method can only be accessed from core thread.");
}

void Venus::Core::CoreThread::_go() {

    {
        Lock lock(this->_startUpMutex);
        this->_coreThreadId = THREAD_CURRENT_ID;
    }
    while (true) {

        std::shared_ptr<Queue<QueuedCommand>> commands = nullptr;
        {
            Lock lock(this->_commandQueueMutex);

            while (this->_commandQueue->isEmpty()) {

                // todo give core thread to task scheduler

                if (this->_shutdownCoreThread)
                    return;

                // Go to sleep till i'm notified
                this->_commandReadySignal.wait(lock);
            }

            commands = this->_commandQueue->flushQueue();
        }

        this->_commandQueue->playBack(commands);
    }
}

void Venus::Core::CoreThread::shutdownCoreThread() {
    {
        Lock lock(this->_commandQueueMutex);
        this->_shutdownCoreThread = true;
    }

    this->_commandReadySignal.notify_all();
}

std::shared_ptr<Venus::Core::CommandQueue<Venus::Core::CommandQueueUnSynced>> Venus::Core::CoreThread::getQueue() {

    if (_perThreadContainerData == nullptr) {
        _perThreadContainerData = std::make_shared<ThreadQueueContainer>();
        _perThreadContainerData->queue = std::make_shared<CommandQueue<CommandQueueUnSynced>>(THREAD_CURRENT_ID);
        _perThreadContainerData->isMain = THREAD_CURRENT_ID == this->_coreThreadId;

        Lock(this->_submitMutex);
        this->_allQueues.push_back(_perThreadContainerData);
    }

    return _perThreadContainerData->queue;
}

void
Venus::Core::CoreThread::queueCommand(std::function<void()> commandCallback,
                                      const Venus::Core::CoreThreadQueueFlags &flags) {
    if (flags.isSet(CTQF_InternalQueue)) {
        bool blockUntilComplete = flags.isSet(CTQF_BlockUntilComplete);

        {
            Lock lock(this->_commandQueueMutex);

            uint32_t callbackId;
            this->_commandQueue->genCallbackId(callbackId);
            this->_commandQueue->queue(std::move(commandCallback), callbackId);
        }

        this->_commandReadySignal.notify_all();
        return;
    }

    auto queue = this->getQueue();
    queue->queue(commandCallback, 0);
}

void Venus::Core::CoreThread::submit(bool blockUntilComplete) {
    Lock submitLock(this->_submitMutex);

    auto queue = this->getQueue();

    uint32_t callbackId;
    queue->genCallbackId(callbackId);

    {
        Lock cmdLock(this->_commandQueueMutex);
        auto commands = queue->flushQueue();
        this->_commandQueue->queue([queue, commands]() {
            queue->playBack(commands);
        }, callbackId);
    }

    this->_commandReadySignal.notify_all();
}

void Venus::Core::CoreThread::submitAll(bool blockUntilComplete) {
    Lock lock(this->_submitMutex);

    std::weak_ptr<ThreadQueueContainer> mainQueue;

    // Do workers first
    for (const auto &q : this->_allQueues) {
        auto queue = q.lock();

        if (queue->isMain)
            mainQueue = q;
        else
            this->submitCommandQueue(queue->queue);
    }

    // Do main threads work last
    if (!mainQueue.expired())
        this->submitCommandQueue(mainQueue.lock()->queue);

    this->_commandReadySignal.notify_all();
}

void Venus::Core::CoreThread::submitCommandQueue(std::weak_ptr<CommandQueue<CommandQueueUnSynced>> queue,
                                                 bool blockUntilComplete) {

}

