//
// Created by Kelvin Macartney on 24/03/2020.
//

#include <Error/venusExceptions.h>
#include "commandQueueBase.h"
#include "coreThread.h"

namespace Venus::Core {
    CommandQueueBase::CommandQueueBase(ThreadId id)
            : _threadId(id) {
        this->_queue = std::make_shared<Queue<QueuedCommand>>();
    }

    std::shared_ptr<AsyncResult> CommandQueueBase::queueVoidCommand(std::function<void()> commandCallback) {
        auto asyncResult = std::make_shared<AsyncResultObject>();
        QueuedCommand newCommand(std::move(commandCallback), asyncResult);

        this->_queue->push(newCommand);
        return asyncResult;
    }

    std::shared_ptr<AsyncResult>
    CommandQueueBase::queueReturningCommand(std::function<GenericObject()> commandCallback) {
        auto asyncResult = std::make_shared<AsyncResultObject>();
        QueuedCommand newCommand(std::move(commandCallback), asyncResult);

        this->_queue->push(newCommand);
        return asyncResult;
    }

    void CommandQueueBase::playBack(const std::weak_ptr<Queue<QueuedCommand>> &queue) {
        THROW_IF_NOT_CORE_THREAD

        auto q = queue.lock();

        if (q->empty())
            return;

        while (!q->empty()) {

            auto command = q->front();
            command.execute();

            q->pop();
        }
    }

    void CommandQueueBase::cancelAll() {

        while (!this->_queue->empty())
            this->_queue->pop();
    }

    bool CommandQueueBase::isEmpty() {
        return this->_queue->empty();
    }

    void CommandQueueBase::throwInvalidThreadException(const std::string &message) {
        VENUS_EXCEPT(InternalErrorException, message);
    }

    void CommandQueueBase::genCallbackId(uint32_t &callbackId) {
        callbackId = this->_lastCallbackId++;
    }

    std::shared_ptr<Queue<QueuedCommand>> CommandQueueBase::flushQueue() {
        auto flushed = this->_queue;
        this->_queue = std::make_shared<Queue<QueuedCommand>>();

        return flushed;
    }
}

