//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_COMMANDQUEUEBASE_H
#define VENUS_COMMANDQUEUEBASE_H

#include <Threading/threading.h>
#include <queue>
#include "queuedCommand.h"

namespace Venus::Core {
    /**
     * Manages a list of commands that can be queued for execution on the core thread
     */
    class CommandQueueBase {
    public:
        /**
         * Constructor
         * @param id The thread identifier oof the thread which will be adding commands to the queue
         */
        explicit CommandQueueBase(ThreadId id);

        /** Returns the thread identifier for this queue */
        ThreadId getThreadId() {
            return this->_threadId;
        }

        /**
         * Executes all the current commands one by one in order.
         */
        static void playBack(std::weak_ptr<Queue<QueuedCommand>> queue);

        /**
        * Queue up a new command to execute,
        * @note provided command is not queue to have it's return value returned
        *
        * @param commandCallback   The command to be queued for execution
        * @param _callbackId       (optional) Identifier for the callback so you can then later find
        * 									   it if needed.
        */
        virtual void queue(std::function<void()> commandCallback, uint32_t _callbackId);

        /** Cancels all queued commands. */
        virtual void cancelAll();

        /**	Returns true if no commands are queued. */
        virtual bool isEmpty();

        /**
         * Generates a callback id that is unique
         * @param callbackId A reference to an integer to hold new id
         */
        virtual void genCallbackId(uint32_t &callbackId);

        /**
         * Returns a copy of all queued commands and makes room for new ones.
         *
         * @note Must be called from the thread that created the command queue.
         * @return
         */
        virtual std::shared_ptr<Queue<QueuedCommand>> flushQueue();

    protected:

        /** Throws an internal error exception*/
        static void throwInvalidThreadException(const std::string &message);

        ~CommandQueueBase() = default;

    private:
        ThreadId _threadId{};
        std::shared_ptr<Queue<QueuedCommand>> _queue{nullptr};

        std::atomic_int _lastCallbackId{0};
    };
}

#endif //VENUS_COMMANDQUEUEBASE_H
