//
// Created by Kelvin Macartney on 30/03/2020.
//

#include "pooledThread.h"
#include "threadPool.h"
#include "venusThread.h"
#include <algorithm>

namespace Venus::Utility::Threading {
    PooledThread::PooledThread(uint32_t threadId, const std::shared_ptr<ThreadPool> &threadPool, bool tempWorker)
            : _threadId(threadId),
              _threadPool(threadPool),
              _tempWorker(tempWorker) {}

    void PooledThread::ignition() {
        _createWorkThread();

        Lock lock(this->_mutex);

        while (!this->_threadStarted)
            this->_startedCondition.wait(lock);
    }

    void PooledThread::_createWorkThread() {
        auto policy = VenusSchedulingPolicy();
        policy.Policy = SCHED_RR;
        policy.Priority = ThreadPriority::High;

        _thread = new VenusThread([this] { _run(); }, policy);
    }

    void PooledThread::_registerTaskAsyncWaitHandle(const std::shared_ptr<Task> &taskItem) {
        auto asyncWaitHandle = std::make_shared<AsyncWaitHandleImpl>();
        taskItem->_setAsyncWaitHandle(asyncWaitHandle);

        {
            Lock lock(this->_waitHandleMapMutex);
            this->_taskIdToAsyncWaitHandleMap.insert({taskItem->getTaskId(), asyncWaitHandle});
        }
    }

    void PooledThread::_sortWorkQueue() {
        std::sort(this->_workQueue.begin(), this->_workQueue.end(),
                  [](const auto &lhs, const auto &rhs) -> bool {
                      return lhs->_priority < rhs->_priority;
                  });
    }

    std::shared_ptr<AsyncWaitHandle> PooledThread::queueWork(const std::shared_ptr<Task> &taskItem) {
        {
            Lock lock(this->_mutex);

            _prepareForExecution();
            this->_workQueue.push_back(taskItem);

            this->_registerTaskAsyncWaitHandle(taskItem);
            taskItem->_setTaskStatus(TaskStatus::Scheduled);
        }

        this->_readyCondition.notify_all();
        return taskItem->getWaitHandle();
    }

    void PooledThread::_prepareForExecution() {
        _updateIdleState(false);

        _idleTime = time(nullptr);
        _threadReady = true;
    }

    void PooledThread::_run() {
#ifdef __APPLE__
        pthread_setname_np(fmt::format("{}{}", WORKER_THREAD_NAME_PREFIX, this->_threadId).c_str());
#endif

        {
            Lock lock(this->_mutex);
            this->_threadStarted = true;
        }

        this->_startedCondition.notify_all();

        {
            Lock lock(this->_mutex);

            while (!this->_threadReady)
                this->_readyCondition.wait(lock);
        }

        while (!this->_destroyed) {
            std::shared_ptr<Task> task = nullptr;

            {
                Lock lock(this->_mutex);

                if (this->_workQueue.empty()) {

                    this->_workerFinishedCondition.notify_all();

                    this->_updateIdleState(true);
                    this->_readyCondition.wait(lock);
                    this->_updateIdleState(false);

                    continue;
                }

                task = this->_workQueue.front();
                this->_workQueue.pop_front();
            }

            this->_currentlyExecuting.store(true, std::memory_order_release);

            PooledThread::_executeTask(task);
            this->_signalAndRemoveWaitHandle(task->getTaskId());

            this->_currentlyExecuting.store(false, std::memory_order_release);
        }
    }

    void PooledThread::_executeTask(std::shared_ptr<Task> &task) {
        task->_setTaskStatus(TaskStatus::InProgress);
        task->_startedSignal.set();

        task->_work();

        task->_setTaskStatus(TaskStatus::Completed);
    }

    void PooledThread::_updateIdleState(bool isIdle) {
        this->_idle = isIdle;
        this->_idleTime = time(nullptr);
    }

    void PooledThread::_signalAndRemoveWaitHandle(uint32_t taskId) {
        Lock lock(this->_waitHandleMapMutex);

        std::map<uint32_t, std::shared_ptr<AsyncWaitHandleImpl>>::iterator it;
        it = this->_taskIdToAsyncWaitHandleMap.find(taskId);

        it->second->set();

        if (it != this->_taskIdToAsyncWaitHandleMap.end())
            this->_taskIdToAsyncWaitHandleMap.erase(it);
    }

    bool PooledThread::isIdle() {
        Lock lock(this->_mutex);

        return this->_idle;
    }

    uint32_t PooledThread::getId() {
        Lock lock(this->_mutex);

        return this->_threadId;
    }

    void PooledThread::destroy() {
        this->waitTillComplete();

        this->kill();
    }

    void PooledThread::waitTillComplete() {
        Lock lock(this->_mutex);

        while (!this->_idle)
            this->_workerFinishedCondition.wait(lock);
    }

    void PooledThread::kill() {
        {
            Lock lock(this->_mutex);
            this->_destroyed = true;
            this->_threadReady = true;
        }

        this->_readyCondition.notify_all();
        this->_thread->join();

        delete this->_thread;
    }

    time_t PooledThread::getIdleTime() {
        Lock lock(this->_mutex);

        return (time(nullptr) - this->_idleTime);
    }

    uint32_t PooledThread::workSize() {
        Lock lock(this->_mutex);

        return this->_workQueue.size();
    }

    bool PooledThread::hasWork() {
        return _currentlyExecuting.load(std::memory_order_acquire) || this->workSize();
    }

    uint32_t PooledThread::getNextWorkId() {
        return ++this->_workId;
    }

    std::shared_ptr<ThreadPool> PooledThread::getThreadPool() {
        return this->_threadPool.lock();
    }

    bool PooledThread::isTemporaryWorker() const {
        return this->_tempWorker;
    }

    bool PooledThread::isDestroyed() const {
        return this->_destroyed;
    }

    void PooledThread::stealWork(const std::shared_ptr<PooledThread> &victim) {
        Lock myLock(this->_mutex);
        Lock victimsLock(victim->_mutex);

        this->_mergeWithQueue(victim->_workQueue);
        PooledThread::_makeQueueEmpty(victim->_workQueue);
    }

    void PooledThread::_mergeWithQueue(DeQueue<std::shared_ptr<Task>> queue) {
        DeQueue<std::shared_ptr<Task>> newQueue{};

        while (!this->_workQueue.empty() || !queue.empty()) {
            if (!this->_workQueue.empty()) {
                newQueue.push_back(this->_workQueue.front());
                this->_workQueue.pop_front();
            }
            if (!queue.empty()) {
                newQueue.push_back(queue.front());
                queue.pop_front();
            }
        }

        this->_workQueue.swap(newQueue);
    }

    void PooledThread::_makeQueueEmpty(DeQueue<std::shared_ptr<Task>> &queue) {
        DeQueue<std::shared_ptr<Task>> empty;
        queue.swap(empty);
    }
}

