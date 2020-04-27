//
// Created by Kelvin Macartney on 30/03/2020.
//

#ifndef VENUS_POOLEDTHREAD_H
#define VENUS_POOLEDTHREAD_H

#include <string>
#include <memory>
#include <thread>
#include <utility>
#include <iostream>
#include "TaskScheduler/task.h"
#include "threading.h"

namespace Venus::Utility::Threading {
    class ThreadPool;

    /**
     * Represents a single pooled thread
     */
    class PooledThread : public std::enable_shared_from_this<PooledThread> {
    public:
        friend ThreadPool;

        /**
         * Constructor
         * @param threadId The read identifier
         * @param threadPool The parent thread pool
         * @param tempWorker Boolean indicating if the thread has been added because another worker is sleeping
         */
        explicit PooledThread(uint32_t threadId, const std::shared_ptr<ThreadPool> &threadPool,
                              bool tempWorker = false);

        /** Ignites the thread*/
        void ignition();

        /**
         * Queues a task to be executed by the thread
         * @note Should call _sortWorkQueue() to ensure task is positioned correctly
         * @param taskItem The task to be queued
         */
        std::shared_ptr<AsyncWaitHandle> queueWork(const std::shared_ptr<Task> &taskItem);

        /** Attempts to join the currently running thread and destroys it.
         * @note
         * Graceful death, will wait till all work is complete
         * */
        void destroy();

        /** Forcefully kills the thread without ensuring all work is complete */
        void kill();

        /** Returns true if this thread is a worker thread*/
        bool isTemporaryWorker() const;

        /** Returns the thread pool instance this thread belongs to */
        std::shared_ptr<ThreadPool> getThreadPool();

        /** Returns true if the thread is idle and false otherwise */
        bool isIdle();

        /** Returns the thread identifier*/
        uint32_t getId();

        /** Returns the amount time the thread has idle for */
        time_t getIdleTime();

        /** Wait for this thread to complete */
        void waitTillComplete();

        /** Returns the number of work items in this threads queue */
        uint32_t workSize();

        /** Returns true if this thread has work queue of is executing work */
        bool hasWork();

        /** Returns the next work identifier */
        uint32_t getNextWorkId();

        /**
         * The calling thread steals all the work from the victim worker
         * @param victim The victim losing the work
         */
        void stealWork(const std::shared_ptr<PooledThread> &victim);

        /** Returns a boolean indicating if this worker has been destroyed*/
        bool isDestroyed() const;

        /** Destructor */
        virtual ~PooledThread() = default;

    private:

        /**
         * Creates and registers an AsyncWaitHandle for the task
         *
         * @return AsyncWaitHandle for the given task
         */
        void _registerTaskAsyncWaitHandle(const std::shared_ptr<Task> &taskItem);

        /**
         * Sorts the work queue to bring high priority task to the front
         */
        void _sortWorkQueue();

        /** Primary worker method that is ran when the thread is first initialized.*/
        void _run();

        /**
         * Executes the given task
         * @param task The task to be executed
         */
        static void _executeTask(std::shared_ptr<Task> &task);

        /** Signals that the given tasks has completed */
        void _signalAndRemoveWaitHandle(uint32_t taskId);

        /**
         * Updates the threads current idle state
         * @param isIdle If true thread state is updated to idle, none idle otherwise
         * @note Not thread safe, requires caller synchronisation
         */
        void _updateIdleState(bool isIdle);

        /**
         * Merges this instance's queue with the parameter queue
         * @param queue Queue to merge from
         */
        void _mergeWithQueue(DeQueue<std::shared_ptr<Task>> queue);

        /**
         * Makes the queue in the parameter empty
         * @param queue The queue to be made empty
         */
        static void _makeQueueEmpty(DeQueue<std::shared_ptr<Task>> &queue);

        /** Creates and initialises the current worker thread*/
        void _createWorkThread();

        /** Prepares the pooled for execution by updating execution states*/
        void _prepareForExecution();

        DeQueue<std::shared_ptr<Task>> _workQueue{};
        Mutex _waitHandleMapMutex;
        Map<uint32_t, std::shared_ptr<AsyncWaitHandleImpl >> _taskIdToAsyncWaitHandleMap;

        bool _idle{true};
        bool _threadStarted{false};
        bool _threadReady{false};
        bool _destroyed{false};
        std::atomic_bool _currentlyExecuting{false};
        const bool _tempWorker{false};

        const std::weak_ptr<ThreadPool> _threadPool{};
        std::thread *_thread{nullptr};
        const uint32_t _threadId{0};
        time_t _idleTime = 0;

        Signal _startedCondition;
        Signal _readyCondition;
        Signal _workerFinishedCondition;
        std::atomic_int32_t _workId{0};

        Mutex _mutex;
    };
}


#endif //VENUS_POOLEDTHREAD_H
