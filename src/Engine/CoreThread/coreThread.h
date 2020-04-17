//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_CORETHREAD_H
#define VENUS_CORETHREAD_H

#include <Error/venusExceptions.h>
#include <Threading/threading.h>
#include <PlatformDefines.h>
#include <Helpers/flags.h>
#include <memory>
#include <Module.h>
#include "commandQueue.h"

namespace Venus::Core {
    /** Flags that control how is a command submitted to the command queue. */
    enum CoreThreadQueueFlag {
        /**
         * Default flag, meaning the commands will be added to the per-thread queue and only begin executing after
         * submit() has been called.
         */
        CTQF_Default = 0,
        /**
         * Specifies that the queued command should be executed on the internal queue. Internal queue doesn't require
         * a separate CoreThread::submit() call, and the queued command is instead immediately visible to the core thread.
         * @note It is slower than normal queue because it requires additional synchronization.
         */
        CTQF_InternalQueue = 1 << 0,
        /**
         * If true, the method will block until the command finishes executing on the core thread. Only relevant for the
         * internal queue commands since contents of the normal queue won't be submitted to the core thread until the
         * CoreThread::submit() call.
         */
        CTQF_BlockUntilComplete = 1 << 1
    };

    typedef Flags<CoreThreadQueueFlag> CoreThreadQueueFlags;

    FLAGS_OPERATORS(CoreThreadQueueFlag);

    /**
     * Manager for the core thread. Takes care of starting, running, queuing commands and shutting down the core thread.
     */
    class CoreThread : public std::enable_shared_from_this<CoreThread>, public Venus::Module<CoreThread> {
    public:

        /** Returns the id of the core thread */
        ThreadId getThreadId() {
            return this->_coreThreadId;
        }

        /** Shuts down the core thread */
        void shutdownCoreThread();

        /** Exception throws if the current thread is not the core thread */
        static void throwIfNotCoreThread();

        /** Exception throws if the current thread is the core thread */
        static void throwIfCoreThread();

        /** Runs the core thread loop. */
        void _go();

        /**
         * Queues a new command that will be added to the global command queue.
         *
         * @param[in]	commandCallback		Command to queue.
         * @param[in]	flags				Flags that further control command submission.
         *
         * @see		CommandQueue::queue()
         * @note	Thread safe
         */
        void queueCommand(std::function<void()> commandCallback, const CoreThreadQueueFlags &flags = CTQF_Default);

        /**
         * Submits the commands from all queues and starts executing them on the core thread.
         * @note If called with having previously called queueCommand i.e the queue is empty, it might cause a deadlock
         */
        void submitAll(bool blockUntilComplete = false);

        /**
         * Submits the commands from the current thread's queue and starts executing them on the core thread.
         * @note If called with having previously called queueCommand i.e the queue is empty, it might cause a deadlock
         */
        void submit(bool blockUntilComplete = false);

        /** Contains data about an queue for a specific thread. */
        struct ThreadQueueContainer {
            std::shared_ptr<CommandQueue<CommandQueueUnSynced>> queue;
            bool isMain;
        };


        /** Initialises the core thread*/
        void ignition() override;

    private:
        ThreadId _coreThreadId{};
        ThreadId _workerThread{};

        bool _shutdownCoreThread{false};

        std::shared_ptr<CommandQueue<CommandQueueSync>> _commandQueue;
        std::vector<std::weak_ptr<ThreadQueueContainer>> _allQueues;

        Mutex _commandQueueMutex;
        Mutex _commandReadyCondition;
        Mutex _submitMutex;
        Mutex _startUpMutex;

        Signal _commandReadySignal;

        static std::weak_ptr<Venus::Core::CoreThread> _coreThreadInstance;
        static thread_local std::shared_ptr<ThreadQueueContainer> _perThreadContainerData;

        /** Creates or retrieves a queue for the calling thread. */
        std::shared_ptr<CommandQueue<CommandQueueUnSynced>> getQueue();

        /**
         * Submits all the commands from the provided command queue to the internal command queue.
         *
         * @note Optionally blocks the calling thread until all the submitted commands have done executing.
         */
        void
        submitCommandQueue(std::weak_ptr<CommandQueue<CommandQueueUnSynced>> queue, bool blockUntilComplete = false);
    };
}

#if VENUS_DEBUG
#define THROW_IF_NOT_CORE_THREAD Venus::Core::CoreThread::throwIfNotCoreThread();
#define THROW_IF_CORE_THREAD Venus::Core::CoreThread::throwIfCoreThread();
#else
#define THROW_IF_NOT_CORE_THREAD
#define THROW_IF_CORE_THREAD
#endif

#endif //VENUS_CORETHREAD_H
