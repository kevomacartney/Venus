//
// Created by Kelvin Macartney on 30/03/2020.
//

#include "threadPool.h"


namespace Venus::Utility::Threading {

    /** The thread pool will check for unused threads every UNUSED_CHECK_PERIOD getOrCreateThread() calls*/
    static constexpr int TEMP_WORKER_CHECK_PERIOD = 32;

    ThreadPool::ThreadPool(ThreadPoolDescription description) :
            _quota(description.absoluteMaximum),
            _enableWorkStealing(description.enableWorkStealing),
            _tempPooledThreads(this->_pooledThreads) {
        this->_description = description;
    }

    std::shared_ptr<PooledWorkDescription> ThreadPool::queueWork(const std::function<void()> &workMethod) {
        auto worker = _getOrCreateLeastBusyWorker();

        auto task = ThreadPool::_createTaskFromWorkMethod(workMethod, worker->getNextWorkId());
        worker->queueWork(task);

        auto workDesc = std::make_shared<PooledWorkDescription>(task->getWaitHandle(), task->getTaskId());
        return workDesc;
    }

    std::shared_ptr<Task>
    ThreadPool::_createTaskFromWorkMethod(const std::function<void()> &workMethod, uint32_t workId) {
        auto taskDesc = TaskDescription();
        taskDesc.TaskName = fmt::format("POOLEDTASK::{}", workId);
        taskDesc.TaskId = workId;
        taskDesc.Work = workMethod;
        taskDesc.Priority = TaskPriority::DefaultPool;

        return std::make_shared<Task>(taskDesc);
    }

    std::shared_ptr<PooledThread> ThreadPool::_getOrCreateLeastBusyWorker() {
        _doTempWorkerCleanup();
        Lock lock(this->_mutex);

        auto workerSize = _getWorkerCount();
        if (workerSize < this->_quota) {

            auto worker = this->_createNewWorker();
            _insertWorkerToPool(worker);

            return worker;
        }

        return _getLeastBusyWorkerFromExistingPools();
    }

    uint32_t ThreadPool::_getWorkerCount() const {
        auto workerSize = _pooledThreads.size() + _tempPooledThreads.size();
        return workerSize;
    }

    std::shared_ptr<PooledThread> ThreadPool::_getLeastBusyWorkerFromExistingPools() {
        this->_sortWorkerPoolsByWorkload();

        if (_tempPooledThreads.empty())
            return _pooledThreads.front();

        auto permWorker = _pooledThreads.front();
        auto tempWorker = _tempPooledThreads.front();

        return permWorker->workSize() > tempWorker->workSize()
               ? tempWorker
               : permWorker;
    }

    std::shared_ptr<PooledThread> ThreadPool::_getBusiestWorkerFromExistingPools() {
        this->_sortWorkerPoolsByWorkload();

        if (_tempPooledThreads.empty())
            return _pooledThreads.back();

        auto permWorker = _pooledThreads.back();
        auto tempWorker = _tempPooledThreads.back();

        return permWorker->workSize() > tempWorker->workSize()
               ? permWorker
               : tempWorker;
    }

    void ThreadPool::_sortWorkerPoolsByWorkload() {
        auto comparator = [](const std::shared_ptr<PooledThread> &a, const std::shared_ptr<PooledThread> &b) -> bool {
            return a->workSize() < b->workSize();
        };

        std::sort(this->_pooledThreads.begin(), this->_pooledThreads.end(), comparator);
        std::sort(this->_tempPooledThreads.begin(), this->_tempPooledThreads.end(), comparator);
    }

    void ThreadPool::_insertWorkerToPool(std::shared_ptr<PooledThread> &worker) {
        if (!worker->isTemporaryWorker()) {
            _pooledThreads.push_back(worker);
        } else {
            _tempPooledThreads.push_back(worker);
        }
    }

    void ThreadPool::_doTempWorkerCleanup() {
        uint32_t age = ++_workerAge;

        if (age > TEMP_WORKER_CHECK_PERIOD) {
            _removeLeastBusyTempWorker();
            _workerAge = 0;
        }
    }

    void ThreadPool::_removeLeastBusyTempWorker() {
        Lock lock(this->_mutex);

        for (const auto &worker : this->_tempPooledThreads) {
            if (!worker->hasWork())
                worker->destroy();
        }

        this->_removeTempWorkerIfDestroyed();
    }

    void ThreadPool::_removeTempWorkerIfDestroyed() {
        auto removeIfDestroyed = [](const std::shared_ptr<PooledThread> &worker) -> bool {
            return worker->isDestroyed();
        };

        _tempPooledThreads.erase(
                std::remove_if(_tempPooledThreads.begin(), _tempPooledThreads.end(), removeIfDestroyed),
                _tempPooledThreads.end());
    }

    void ThreadPool::addWorker() {
        Lock lock(this->_mutex);
        ++this->_quota;
    }

    void ThreadPool::removeWorker() {
        {
            Lock lock(this->_mutex);
            --this->_quota;
        }

        if (this->_getWorkerCount() > this->_quota)
            this->_removeOneWorker();
    }

    void ThreadPool::_removeOneWorker() {
        Lock lock(this->_mutex);
        if (!this->_enableWorkStealing || this->_tempPooledThreads.empty())
            return;

        auto leastBusy = this->_getLeastBusyWorkerFromExistingPools();
        auto workId = leastBusy->getNextWorkId();

        auto task = this->_createTaskFromWorkMethod([this]() {
            auto workerId = this->_leastBusyStealWork();
            this->_removeTempWorkerById(workerId);
        }, workId);
        task->_setPriority(TaskPriority::VeryHigh); // We want this to be executed immediately

        ThreadPool::_queueTaskAndSort(leastBusy, task);
    }

    uint32_t ThreadPool::_leastBusyStealWork() {
        Lock(this->_mutex);

        auto leastBusyWorker = _getLeastBusyWorkerFromExistingPools();
        auto tempWorker = _tempPooledThreads.front();

        leastBusyWorker->stealWork(tempWorker);

        return tempWorker->getId();
    }

    void ThreadPool::_removeTempWorkerById(uint32_t workerId) {
        for (const auto &worker : this->_tempPooledThreads) {
            if (worker->getId() == workerId) {
                worker->destroy();
                break;
            }
        }

        this->_removeTempWorkerIfDestroyed();
    }

    void ThreadPool::shutdown() {
        for (const auto &worker : this->_pooledThreads) {
            worker->kill();
        }

        for (const auto &tempWorker : this->_tempPooledThreads) {
            tempWorker->kill();
        }
    }

    std::shared_ptr<PooledThread> ThreadPool::_createNewWorker() {
        bool isTemp = this->_getWorkerCount() >= this->_description.absoluteMaximum;

        auto worker = std::make_shared<PooledThread>(++this->_threadIds, this->shared_from_this(), isTemp);
        worker->ignition();

        return worker;
    }

    void ThreadPool::_addScheduledWork(const std::shared_ptr<Task> &task) {
        auto worker = this->_getOrCreateLeastBusyWorker();
        task->setTaskId(worker->getNextWorkId());
        ThreadPool::_queueTaskAndSort(worker, task);
    }

    void ThreadPool::_queueTaskAndSort(const std::shared_ptr<PooledThread> &worker, const std::shared_ptr<Task> &task) {
        worker->queueWork(task);
        worker->_sortWorkQueue();
    }
}