//
// Created by Kelvin Macartney on 24/03/2020.
//

#include <Error/venusExceptions.h>
#include "commandQueueBase.h"
#include "coreThread.h"

Venus::Core::CommandQueueBase::CommandQueueBase(ThreadId id)
        : _threadId(id) {
    this->_queue = std::make_shared<Queue<QueuedCommand>>();
}

void Venus::Core::CommandQueueBase::queue(std::function<void()> commandCallback, uint32_t _callbackId) {

    QueuedCommand newCommand(std::move(commandCallback), _callbackId);
    this->_queue->push(newCommand);
}

void Venus::Core::CommandQueueBase::playBack(std::weak_ptr<Queue<QueuedCommand>> queue) {
    THROW_IF_NOT_CORE_THREAD
    auto q = queue.lock();
    if (q->empty())
        return;

    while (!q->empty()) {

        auto command = q->front();
        command.execute();

        // TODO, notify caller
        q->pop();
    }
}

void Venus::Core::CommandQueueBase::cancelAll() {

    while (!this->_queue->empty())
        this->_queue->pop();
}

bool Venus::Core::CommandQueueBase::isEmpty() {
    return this->_queue->empty();
}

void Venus::Core::CommandQueueBase::throwInvalidThreadException(const std::string &message) {
    VENUS_EXCEPT(InternalErrorException, message);
}

void Venus::Core::CommandQueueBase::genCallbackId(uint32_t &callbackId) {
    callbackId = this->_lastCallbackId++;
}

std::shared_ptr<Queue<Venus::Core::QueuedCommand>> Venus::Core::CommandQueueBase::flushQueue() {
    auto flushed = this->_queue;
    this->_queue = std::make_shared<Queue<QueuedCommand>>();

    return flushed;
}
