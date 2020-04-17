//
// Created by Kelvin Macartney on 25/03/2020.
//

#ifndef VENUS_COMMANDQUEUE_H
#define VENUS_COMMANDQUEUE_H

#include "commandQueueBase.h"

namespace Venus::Core {
    /** Provides a lock for synchronization */
    struct LockGuard {
        Lock lock;
    };

    /**
     * Command queue policy that provides synchronization
     */

    class CommandQueueSync {
    public:
        CommandQueueSync(ThreadId id)
                : _threadId(id) {}

        /** Returns true */
        bool isValidThread() {
            return true;
        }

        /**
         * Acquires a lock for the command queue mutex
         * @return A unique lock
         */
        LockGuard lock() {
            return LockGuard{Lock(_commandQueueMutex)};
        };

    private:
        Mutex _commandQueueMutex;
        ThreadId _threadId;
    };

    /**
     * Command queue policy that provides NO synchronization
     */
    class CommandQueueUnSynced {
    public:
        CommandQueueUnSynced(ThreadId id)
                : _threadId(id) {}

        /** Returns true if the current thread is the owner of the this policy object*/
        bool isValidThread() {
            return this->_threadId == THREAD_CURRENT_ID;
        }

        LockGuard lock(){
            return LockGuard();
        }

    private:
        ThreadId _threadId;
    };

    /**
     * Manages a list of commands that can be queued for later execution on the core thread.
     */
    template<class SyncPolicy>
    class CommandQueue : public CommandQueueBase, public SyncPolicy {
    public:
        explicit CommandQueue(ThreadId id)
                : CommandQueueBase(id),
                  SyncPolicy(id) {}

        /**
         * Queue up a new command to execute,
         * @note provided command is not queue to have it's return value returned
         *
         * @param commandCallback   The command to be queued for execution
         * @param _callbackId       (optional) Identifier for the callback so you can then later find
         * 									   it if needed.
         */
        void queue(std::function<void()> commandCallback, uint32_t _callbackId) override {
#if DEBUG
            if (!this->isValidThread())
                CommandQueueBase::throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
            LockGuard lockGuard = this->lock();
            return CommandQueueBase::queue(commandCallback, _callbackId);
        }

        /** Cancels all queued commands. */
        void cancelAll() override {
#if DEBUG
            if (!this->isValidThread())
                CommandQueueBase::throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
            LockGuard lockGuard = this->lock();
            CommandQueueBase::cancelAll();
        }

        /**	Returns true if no commands are queued. */
        bool isEmpty() override {
#if DEBUG
            if (!this->isValidThread())
                CommandQueueBase::throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif
            LockGuard lockGuard = this->lock();
            return CommandQueueBase::isEmpty();
        }

        /** @copydoc CommandQueueBase::flush */
        std::shared_ptr<Queue<QueuedCommand>> flushQueue() override {
#if DEBUG
            if (!this->isValidThread())
                CommandQueueBase::throwInvalidThreadException("Command queue accessed outside of its creation thread.");
#endif

            LockGuard lockGuard = this->lock();
            return CommandQueueBase::flushQueue();
        }
    };
}
#endif //VENUS_COMMANDQUEUE_H
