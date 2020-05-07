//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_CORETHREAD_H
#define VENUS_CORETHREAD_H

#include <Error/venusExceptions.h>
#include <Threading/threading.h>
#include <Threading/asyncResultImpl.h>
#include <PlatformDefines.h>
#include <Helpers/flags.h>
#include <memory>
#include <Module.h>
#include "commandQueue.h"
#include "coreThreadQueueFlag.h"

namespace Venus::Core {

    using namespace Utility::Threading;
    typedef Flags<CoreThreadQueueFlag> CoreThreadQueueFlags;

    FLAGS_OPERATORS(CoreThreadQueueFlag);

    /**
     * Manager for the core thread. Takes care of starting, running, queuing commands and shutting down the core thread.
     */
    class CoreThread : public std::enable_shared_from_this<CoreThread>, public Venus::Module<CoreThread> {
    public:
        /** Contains data about an queue for a specific thread. */
        struct ThreadQueueContainer {
            std::shared_ptr<CommandQueue<CommandQueueUnSynced>> queue;
            bool isMain;
        };

        /** Returns the id of the core thread */
        ThreadId getThreadId() {
            return this->_coreThreadId;
        }

        /** Shuts down the core thread */
        void shutdown() override;

        /** Exception throws if the current thread is not the core thread */
        static void throwIfNotCoreThread();

        /** Exception throws if the current thread is the core thread */
        static void throwIfCoreThread();

        /**
         *  Runs the core thread loop.
         *  @note Blocks until corethread has been shutdown
         */
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
        std::shared_ptr<AsyncResult>
        queueCommand(std::function<void()> commandCallback, const CoreThreadQueueFlags &flags = CTQF_Default);

        /**
         * Queues a new command that will be added to the global command queue.
         *
         * @param[in]	commandCallback		Command to queue.
         * @param[in]	flags				Flags that further control command submission.
         *
         * @see		CommandQueue::queueReturning()
         * @note	Thread safe
         */
        std::shared_ptr<AsyncResult> queueReturningCommand(std::function<GenericObject()> commandCallback,
                                                           const CoreThreadQueueFlags &flags = CTQF_Default);

        /**
         * Submits the commands from all queues and starts executing them on the core thread.
         * @note If called with having previously called queueCommand i.e the queue is empty, it might cause a deadlock
         */
        void submitAll();

        /**
         * Submits the commands from the current thread's queue and starts executing them on the core thread.
         * @note If called with having previously called queueCommand i.e the queue is empty, it might cause a deadlock
         */
        void submit();

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
         */
        void _submitCommandQueue(const std::shared_ptr<CommandQueue<CommandQueueUnSynced>>& queue);

        /**
         * Adds the callback to the internal core queue
         * @param commandCallback The the command
         * @param flags Flags used to specify how the command should be handled
         * @return Async Result
         */
        std::shared_ptr<AsyncResult>
        addVoidToInternalQueue(std::function<void()> &commandCallback, const Venus::Core::CoreThreadQueueFlags &flags);

        /** Sets the core thread priority for PThread implementation*/
        void configurePThread();

        std::shared_ptr<AsyncResult>
        addReturningToInternalQueue(std::function<GenericObject()> &commandCallback,
                                    const CoreThreadQueueFlags &flags);
    };

    /** Returns global CoreThread instance*/
    std::shared_ptr<CoreThread> getCoreThread();
}

#if VENUS_DEBUG
#define THROW_IF_NOT_CORE_THREAD Venus::Core::CoreThread::throwIfNotCoreThread();
#define THROW_IF_CORE_THREAD Venus::Core::CoreThread::throwIfCoreThread();
#else
#define THROW_IF_NOT_CORE_THREAD
#define THROW_IF_CORE_THREAD
#endif

#endif //VENUS_CORETHREAD_H
