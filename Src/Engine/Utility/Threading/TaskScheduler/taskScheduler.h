//
// Created by Kelvin Macartney on 27/03/2020.
//

#ifndef VENUS_TASKSCHEDULER_H
#define VENUS_TASKSCHEDULER_H

#include "task.h"
#include <Threading/venusThread.h>
#include <Threading/threading.h>
#include <Threading/threadPool.h>
#include <Module.h>

namespace Venus::Utility::Threading {
    /**
     * Represents a task scheduler running on multiple threads.
     *
     * @note You may queue tasks from any thread and they will be executed in user specified order
     * on any available thread.
     *
     * @note Thread safe
     *
     * @note The task scheduler uses the thread pool instance to queue new tasks, new threads can be added to threadpool
     * using addWorker and removeWorker
     */
    class TaskScheduler : public Module<TaskScheduler> {
    public:
        TaskScheduler() = default;

        /** Does the modules initial initialisation */
        void ignition() override;

        /**
         * Adds a new task to be scheduled by this scheduler and executed on the thread pool
         * @param task The task to be added
         * @note Task status must be Inactive
         * @return The task's unique id
         */
        uint32_t addTask(const std::shared_ptr<Task> &task);

        /**
         * Adds a new group of tasks to be scheduled by this scheduler and executed on the thread pool
         * @param taskGroup The task group
         * @return The group's unique id
         */
        uint32_t addTaskGroup(const std::shared_ptr<TaskGroup> &taskGroup);

        /**
         * Waits till all the tasks in the group have completed
         * @param taskGroup The tasks group
         */
        void waitTillGroupComplete(const std::shared_ptr<TaskGroup> &taskGroup);

        /** Adds an extra worker to the thread pool */
        void addWorker();

        /** Removes a worker from the thread pool */
        static void removeWorker();

        /** shutdown the task scheduler */
        void shutdown() override;

    private:

        /** Updates the given task's properties for group operation*/
        static void initTaskForGroup(const std::shared_ptr<Task> &task, uint32_t groupId);

        /** Main task scheduler method that dispatches tasks to threadpool. */
        void _runTaskSchedulerLoop();

        std::vector<std::shared_ptr<Task>> _taskQueue;

        std::unique_ptr<VenusThread> _taskSchedulerThread{nullptr};

        bool _checkTasks{true};
        bool _shutdown{false};

        Mutex _readyMutex;
        Mutex _completeMutex;
        Signal _taskReadyCond;
        Signal _taskCompleteCond;
    };
}

#endif //VENUS_TASKSCHEDULER_H
