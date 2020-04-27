//
// Created by Kelvin Macartney on 24/03/2020.
//

#include "coreThread.h"
#include <thread>
#include <utility>
#include "queuedCommand.h"
#include <Threading/TaskScheduler/taskScheduler.h>

namespace Venus::Core {
    std::weak_ptr<CoreThread>  CoreThread::_coreThreadInstance;

    thread_local std::shared_ptr<Venus::Core::CoreThread::ThreadQueueContainer>
            Venus::Core::CoreThread::_perThreadContainerData;

    void CoreThread::ignition() {
        Venus::Core::CoreThread::_coreThreadInstance = shared_from_this();

        this->_workerThread = THREAD_CURRENT_ID;
        this->_coreThreadId = this->_workerThread; // for now

        this->_commandQueue = std::make_shared<CommandQueue<CommandQueueSync>>(this->_coreThreadId);
#ifdef SUPPORTS_PTHREAD
        configurePThread();
#endif
    }

    void CoreThread::configurePThread() {
#ifdef SUPPORTS_PTHREAD
        pthread_setname_np(CORE_THREAD_NAME);

        auto schedule = VenusSchedulingPolicy();
        schedule.Policy = SCHED_RR;
        schedule.Priority = ThreadPriority::Highest;

        auto thisThread = pthread_self();

        VenusThread::setScheduling(thisThread, schedule);
#endif
    }

    void CoreThread::throwIfCoreThread() {
        auto instance = CoreThread::instance();
        if (THREAD_CURRENT_ID == instance->getThreadId()) VENUS_EXCEPT(InternalErrorException,
                                                                       "This method cannot be accessed from core thread.");
    }

    void CoreThread::throwIfNotCoreThread() {
        auto instance = CoreThread::instance();
        if (THREAD_CURRENT_ID != instance->getThreadId()) VENUS_EXCEPT(InternalErrorException,
                                                                       "This method can only be accessed from core thread.");
    }

    void CoreThread::_go() {
        if (THREAD_CURRENT_ID != this->_coreThreadId) {
            VENUS_EXCEPT(InternalErrorException, "The CORETHREAD _go method must be called by the original creator")
        }

        {
            Lock lock(this->_startUpMutex);
            this->_coreThreadId = THREAD_CURRENT_ID;
        }

        while (true) {

            std::shared_ptr<Queue<QueuedCommand>> commands = nullptr;
            {
                Lock lock(this->_commandQueueMutex);

                while (this->_commandQueue->isEmpty()) {

                    if (this->_shutdownCoreThread)
                        return;

                    Venus::Utility::Threading::TaskScheduler::instance()->addWorker();
                    this->_commandReadySignal.wait(lock);
                    Venus::Utility::Threading::TaskScheduler::instance()->removeWorker();
                }

                commands = this->_commandQueue->flushQueue();
            }

            this->_commandQueue->playBack(commands);
        }

        spdlog::info("CoreThread loop Terminated");
    }

    void CoreThread::shutdown() {
        Module::shutdown();
        {
            Lock lock(this->_commandQueueMutex);
            this->_shutdownCoreThread = true;
        }

        this->_commandReadySignal.notify_all();
    }

    std::shared_ptr<AsyncResult> CoreThread::queueCommand(std::function<void()> commandCallback,
                                                          const Venus::Core::CoreThreadQueueFlags &flags) {
        if (flags.isSet(CTQF_InternalQueue))
            return addVoidToInternalQueue(commandCallback, flags);
        else
            return this->getQueue()->queueVoidCommand(commandCallback);
    }

    std::shared_ptr<AsyncResult> CoreThread::addVoidToInternalQueue(std::function<void()> &commandCallback,
                                                                    const Venus::Core::CoreThreadQueueFlags &flags) {
        std::shared_ptr<AsyncResult> result;

        {
            Lock lock(this->_commandQueueMutex);
            result = this->_commandQueue->queueVoidCommand(std::move(commandCallback));
        }

        _commandReadySignal.notify_all();

        if (flags.isSet(CTQF_BlockUntilComplete)) {
            result->blockUntilComplete();
        }

        return result;
    }

    std::shared_ptr<AsyncResult> CoreThread::queueReturningCommand(std::function<GenericObject()> commandCallback,
                                                                   const CoreThreadQueueFlags &flags) {
        if (flags.isSet(CTQF_InternalQueue))
            return addReturningToInternalQueue(commandCallback, flags);
        else
            return this->getQueue()->queueReturningCommand(commandCallback);
    }

    std::shared_ptr<AsyncResult>
    CoreThread::addReturningToInternalQueue(std::function<GenericObject()> &commandCallback,
                                            const CoreThreadQueueFlags &flags) {
        std::shared_ptr<AsyncResult> result;
        {
            Lock lock(_commandQueueMutex);
            result = _commandQueue->queueReturningCommand(std::move(commandCallback));
        }
        _commandReadySignal.notify_all();

        if (flags.isSet(CTQF_BlockUntilComplete)) {
            result->blockUntilComplete();
        }

        return result;
    }

    std::shared_ptr<CommandQueue<CommandQueueUnSynced>> CoreThread::getQueue() {

        if (_perThreadContainerData == nullptr) {
            _perThreadContainerData = std::make_shared<ThreadQueueContainer>();
            _perThreadContainerData->queue = std::make_shared<CommandQueue<CommandQueueUnSynced>>(THREAD_CURRENT_ID);
            _perThreadContainerData->isMain = THREAD_CURRENT_ID == this->_coreThreadId;

            Lock lock(this->_submitMutex);
            this->_allQueues.push_back(_perThreadContainerData);
        }

        return _perThreadContainerData->queue;
    }

    void CoreThread::submit() {
        Lock submitLock(this->_submitMutex);

        auto queue = this->getQueue();

        uint32_t callbackId;
        queue->genCallbackId(callbackId);

        this->_submitCommandQueue(queue);

        this->_commandReadySignal.notify_all();
    }

    void CoreThread::submitAll() {
        Lock lock(this->_submitMutex);

        std::weak_ptr<ThreadQueueContainer> mainQueue;

        // Do workers first
        for (const auto &q : this->_allQueues) {
            auto queue = q.lock();

            if (queue->isMain)
                mainQueue = q;
            else
                this->_submitCommandQueue(queue->queue);
        }

        // Do main threads work last
        if (!mainQueue.expired())
            this->_submitCommandQueue(mainQueue.lock()->queue);

        this->_commandReadySignal.notify_all();
    }

    void CoreThread::_submitCommandQueue(const std::shared_ptr<CommandQueue<CommandQueueUnSynced>> &queue) {
        this->queueCommand([queue]() {
            auto commands = queue->flushQueue();
            queue->playBack(commands);
        }, CTQF_InternalQueue);
    }

    std::shared_ptr<CoreThread> getCoreThread() {
        return CoreThread::instance();
    }
}