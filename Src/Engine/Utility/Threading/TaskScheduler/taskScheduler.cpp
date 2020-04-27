//
// Created by Kelvin Macartney on 27/03/2020.
//

#include "taskScheduler.h"
#include <pthread.h>

namespace Venus::Utility::Threading {
    void TaskScheduler::ignition() {
        this->_taskSchedulerThread = std::make_unique<VenusThread>([this]() {
            this->_runTaskSchedulerLoop();
        });
    }

    uint32_t TaskScheduler::addTask(const std::shared_ptr<Task> &task) {
        assert(task->_taskStatus > TaskStatus::Waiting && "The task is marked as running and cannot be queued");

        Lock lock(this->_readyMutex);

        this->_taskQueue.push_back(task);
        this->_checkTasks = true;

        this->_taskReadyCond.notify_one();
        return task->getTaskId();
    }

    uint32_t TaskScheduler::addTaskGroup(const std::shared_ptr<TaskGroup> &taskGroup) {
        Lock lock(this->_readyMutex);

        for (const auto &task : taskGroup->_tasks) {
            TaskScheduler::initTaskForGroup(task, taskGroup->_id);

            this->addTask(task);
        }

        return taskGroup->_id;
    }

    void TaskScheduler::initTaskForGroup(const std::shared_ptr<Task> &task, uint32_t groupId) {
        task->_isInGroup = true;
        task->_taskGroupId = groupId;
    }

    void TaskScheduler::addWorker() {
        ThreadPool::instance()->addWorker();

        this->_checkTasks = true;
        this->_taskReadyCond.notify_one();
    }

    void TaskScheduler::removeWorker() {
        ThreadPool::instance()->removeWorker();
    }

    void TaskScheduler::_runTaskSchedulerLoop() {
        while (!this->_shutdown) {
            Lock lock(this->_readyMutex);

            while ((!this->_checkTasks) && !this->_shutdown)
                this->_taskReadyCond.wait(lock);

            this->_checkTasks = false;

            for (auto iter = this->_taskQueue.begin(); iter != this->_taskQueue.end();) {
                std::shared_ptr<Task> curTask = *iter;

                if (curTask->isCancelled()) {
                    iter = this->_taskQueue.erase(iter);
                    continue;
                }

                if (curTask->_taskDependency != nullptr && !curTask->_taskDependency->isComplete()) {
                    ++iter;
                    continue;
                }

                ThreadPool::instance()->_addScheduledWork(curTask);
                this->_taskQueue.erase(iter);
            }
        }
    }

    void TaskScheduler::shutdown() {
        {
            Lock lock(this->_readyMutex);
            this->_shutdown = true;
        }

        this->_taskReadyCond.notify_all();
        this->_taskSchedulerThread->join();
    }

    void TaskScheduler::waitTillGroupComplete(const std::shared_ptr<TaskGroup> &taskGroup) {
        for (const auto& task : taskGroup->_tasks) {
            task->wait();
        }
    }
}
