//
// Created by Kelvin Macartney on 30/03/2020.
//

#ifndef VENUS_THREADPOOL_H
#define VENUS_THREADPOOL_H

#include <string>
#include <memory>
#include <thread>
#include "threading.h"

namespace Venus::Utility::Threading {
    class ThreadPool;

    /**
     * Represents a thread that is managed by the thread pool
     */
    class VenusThread {
    public:
        /**
         * Puts the calling thread to sleep or block till this thread signals completion
         * @note Sleeping thread is added to thread pool
         */
        void sleepTillComplete();

    private:
        uint32_t threadId{0};
        std::shared_ptr<ThreadPool> _threadPool{nullptr};
    };

    /**
     * Represents a single pooled thread
     */
    class PooledThread {
    public:
        explicit PooledThread(std::string name, uint32_t threadId);

        /** Ignites the thread*/
        void ignition();

        /**
         * Starts executing the given worker method.
         * @note
         * Caller must ensure the thread is idle
         */
        void execute(std::function<void()> workerMethod, uint32_t id);

        /** Returns true if the thread is idle and false otherwise */
        bool isIdle();

        /** Returns the thread identifier*/
        uint32_t getId();

        /** Wait for this thread to complete */
        void waitTillComplete();

        virtual ~PooledThread() = default;

    protected:
        /** Primary worker method that is ran when the thread is first initialized.*/
        void _run();

        std::string _name{""};
        std::function<void()> _workerThread;

        bool _idle{true};
        bool _threadStarted{false};
        bool _threadReady{false};

        uint32_t _threadId{0};
        std::thread _thread;
        time_t _idleTime = 0;

        Signal _startedCondition;
        Signal _readyCondition;
        Signal _workerFinishedCondition;
        Mutex _mutex;
    };

    class ThreadPool {
    public:
        /**
         * Constructor
         * @param threadCapacity    The thread capacity
         * @param maxCapacity       (optional) The maximum number of threads the pool can create
         * @param idleTimeout       (optional) How many seconds do threads need to be idle before being removed them from the pool.
         */
        explicit ThreadPool(uint32_t threadCapacity, uint32_t maxCapacity = 16, uint32_t idleTimeout = 60);
    };
}


#endif //VENUS_THREADPOOL_H
