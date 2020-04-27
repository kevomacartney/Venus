//
// Created by Kelvin Macartney on 30/03/2020.
//

#ifndef VENUS_THREADPOOL_H
#define VENUS_THREADPOOL_H

#include <cstdint>
#include <vector>
#include <thread>
#include "Module.h"
#include <Datastructures/fibonacciHeap.h>
#include <map>
#include "pooledThread.h"

namespace Venus::Utility::Threading {

    class TaskScheduler;

    /**
     * Contains a description for a thread pool
     */
    struct ThreadPoolDescription {
    public:
        /** The maximum number of hardware threads supported by the system */
        uint32_t absoluteMaximum{0};

        /** Boolean indicating if thread are allowed to steal work */
        bool enableWorkStealing{true};
    };

    /**
     * Represents a description of a pooled work
     */
    class PooledWorkDescription {
    public:
        /**
         * Constructor
         * @param waitHandle Waithandle Ptr
         * @param workId Work Id
         */
        PooledWorkDescription(std::shared_ptr<AsyncWaitHandle> waitHandle, uint32_t workId)
                : WaitHandle(std::move(waitHandle)),
                  WorkId(workId) {}

        /**
         * Waits for the pooled work item to be completed
         * @note Waiting thread is added to thread pool
         */
        void waitTillComplete() {
            this->WaitHandle->wait();
        }

        /** Unique work id */
        const uint32_t WorkId{0};

        /** Work method wait handle */
        const std::shared_ptr<AsyncWaitHandle> WaitHandle{nullptr};
    };

    class ThreadPool : public Venus::Module<ThreadPool>, public std::enable_shared_from_this<ThreadPool> {
    public:
        friend TaskScheduler;

        /**
         * Constructor
         * @param description A ThreadPoolDescription instance
         */
        explicit ThreadPool(ThreadPoolDescription description);

        /**
         * Finds an idle thread (or creates a new one) and begins executing the specified work
         * @tparam workMethod The function type
         * @tparam Args The argument types
         * @param threadName The thread name
         * @param work The function to execute
         * @param arguments Arguments to the function
         * @return
         */
        std::shared_ptr<PooledWorkDescription> queueWork(const std::function<void()> &workMethod);

        /**
         * Increases the quota for worker threads
         * @note If threads count is above quota, then any threads created are temporary works.
         */
        void addWorker();

        /**
         * Decreases the quota for worker threads
         * @note Removes a single temporary worker. Does not remove permanent workers
         */
        void removeWorker();

        /**
         * Signals the thread pool to shut, and kill all workers
         * @note blocking call, will try too join all workers
         */
        void shutdown() override;

        /** Destructor */
        ~ThreadPool() override = default;

    private:
        /**
         * Adds a task from the scheduler to be executed by a thread in the pool
         * @param task The task
         */
        void _addScheduledWork(const std::shared_ptr<Task> &task);

        /**
         * Queues a new task to the thread and sorts the threads queue
         * @param task The task to be queued
         * @param worker The pooled thread worker
         */
        static void _queueTaskAndSort(const std::shared_ptr<PooledThread> &worker, const std::shared_ptr<Task> &task);

        /**
         * Queues a work method and adds it to the queue
         * @param workMethod Work Method
         * @param workId The unique work identifier
         */
        static std::shared_ptr<Task> _createTaskFromWorkMethod(const std::function<void()> &workMethod, uint32_t workId);

        /**
         * Retrieves the first free thread, if one is not found a new one is create if the max quota has not been
         * reached otherwise the least busy is returned
         * @return A pooled thread
         */
        std::shared_ptr<PooledThread> _getOrCreateLeastBusyWorker();

        /** Checks if there are free temporary workers who are freed */
        void _doTempWorkerCleanup();

        /** Attempts to remove all temporary workers */
        void _removeLeastBusyTempWorker();

        /**
         * Removes a single temporary worker from the pool
         * @note Least busy thread will steal the work of the thread killed
         */
        void _removeOneWorker();

        /**
         * Initiates a work stealing process.
         * @note Least busy worker steals all work from busiest,
         * busiest worker will have no work after method has executed
         * @return The id of the victim thread,
         */
        uint32_t _leastBusyStealWork();

        /**
         * Creates a new pooled worker and returns them
         * @return The pooled worker
         */
        std::shared_ptr<PooledThread> _createNewWorker();

        /**
         * Adds the given the pool
         * @note if temp worker will be added to _tempPooledThreads, otherwise to _pooledThreads
         */
        void _insertWorkerToPool(std::shared_ptr<PooledThread> &worker);

        /**
         * Gets the least busy worker from the existing pools
         * @return A pooled worker
         */
        std::shared_ptr<PooledThread> _getLeastBusyWorkerFromExistingPools();

        /**
         * Gets the busiest worker from the worker pool
         * @return A pooled worker
         */
        std::shared_ptr<PooledThread> _getBusiestWorkerFromExistingPools();

        /** Sorts all the pools in a non decreasing work count */
        void _sortWorkerPoolsByWorkload();

        /** Returns the number of workers in both pool*/
        uint32_t _getWorkerCount() const;

        /**
         * Removes the temporary worker the given Id
         * @param workerId The worker id
         */
        void _removeTempWorkerById(uint32_t workerId);

        /**
         * Removes all temporary workers marked as destroyed
         */
        void _removeTempWorkerIfDestroyed();

        std::vector<std::shared_ptr<PooledThread>> _pooledThreads;
        std::vector<std::shared_ptr<PooledThread>> _tempPooledThreads;

        Mutex _mutex{};

        uint32_t _quota{0};
        ThreadPoolDescription _description;

        bool _enableWorkStealing{false};
        std::atomic_uint32_t _threadIds{0};

        std::atomic_uint32_t _workerAge{0};
    };
}


#endif //VENUS_THREADPOOL_H
