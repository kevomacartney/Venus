//
// Created by Kelvin Macartney on 17/04/2020.
//

#ifndef VENUS_TASK_H
#define VENUS_TASK_H

#include <string>
#include <Threading/asyncResult.h>
#include <memory>
#include <atomic>
#include <utility>

namespace Venus::Utility::Threading {
    /**
     * Task priority. Tasks with higher priority will get executed sooner.
     */
    enum class TaskPriority {
        VeryLow = 97,
        Low = 98,
        /** Default priority for tasks added to threadpool directly */
        DefaultPool = 99,
        Normal = 100,
        High = 101,
        VeryHigh = 102
    };

    /**
     * The current state the task is in
     */
    enum class TaskStatus {
        /**
         * Task is still in the task scheduler
         * @note Task id and waitHandle are not set at this stage
         */
        Waiting,
        /**
         * Task has been queued on a worker and will be executed
         * @note Task Id and wait handle are set from this stage on
         */
        Scheduled,
        /**
         * Task is current being executed
         * @note use wait() to wait for execution to complete
         */
        InProgress,
        /**
         * Task has completed executing
         */
        Completed,
        /**
         * Task been cancelled and will not be Scheduled
         * @note If task has been Scheduled it cannot be cancelled
         */
        Cancelled
    };

    class Task;

    class ThreadPool;

    class PooledThread;

    class TaskGroup;

    struct TaskDescription {
    public:
        /** *Task the Name */
        std::string TaskName{"UnnamedTask"};

        /** Task priority */
        TaskPriority Priority{TaskPriority::Normal};

        /** The task the worker method */
        std::function<void()> Work;

        /** The task's dependency */
        std::shared_ptr<Task> Dependency{nullptr};

        /** Task identifier */
        std::uint32_t TaskId;
    };

    /**
     * Represents a single executable work item in the task scheduler
     */
    class Task {
    public:
        /** Constructor */
        explicit Task(const TaskDescription &model)
                : _work(model.Work),
                  _taskId(model.TaskId),
                  _taskName(model.TaskName),
                  _priority(model.Priority),
                  _taskDependency(model.Dependency) {
            this->_taskStatus = TaskStatus::Waiting;
        }

        /** Returns true if the task has completed */
        [[nodiscard("Unnecessary call")]]
        bool isComplete() {
            Lock lock(_taskMutex);
            return this->_taskStatus == TaskStatus::Completed;
        }

        /** Returns true if the task has been cancelled */
        [[nodiscard("Unnecessary call")]]
        bool isCancelled() {
            Lock lock(_taskMutex);
            return this->_taskStatus == TaskStatus::Cancelled;
        }

        /** Returns true if the task has started */
        [[nodiscard("Unnecessary call")]]
        bool hasStarted() {
            Lock lock(_taskMutex);
            return this->_taskStatus == TaskStatus::InProgress;
        }

        /** Returns the tasks wait handle */
        std::shared_ptr<AsyncWaitHandle> getWaitHandle() {
            Lock lock(this->_taskMutex);
            return this->_asyncWaitHandle;
        }

        /** Returns the task's id */
        [[nodiscard("Unnecessary call")]]
        uint32_t getTaskId() {
            Lock lock(this->_taskMutex);
            return this->_taskId;
        }

        /**
         * Blocks the calling thread until the task has been marked as complete
         */
        void wait() {
            {
                Lock lock(_taskMutex);

                if (this->_asyncWaitHandle == nullptr) {
                    spdlog::warn("Task {} waiting for start, task not scheduled",
                                 this->_taskId);

                    this->_startedSignal.wait();
                }
            }

            this->_asyncWaitHandle->wait();
        }

        /**
         * Marks the task as cancelled, will not stop task if it is already running
         */
        void cancel() {
            Lock lock(this->_taskMutex);
            this->_taskStatus = TaskStatus::Cancelled;
        }

    private:
        friend class TaskScheduler;

        friend class ThreadPool;

        friend class PooledThread;

        /** Sets the task id  */
        void setTaskId(uint32_t taskId) {
            Lock lock(this->_taskMutex);
            this->_taskId = taskId;
        }

        /** Sets the async wait handle */
        void _setAsyncWaitHandle(std::shared_ptr<AsyncWaitHandle> waitHandle) {
            Lock lock(_taskMutex);
            this->_asyncWaitHandle = std::move(waitHandle);
        }

        /** Sets the task's status */
        void _setTaskStatus(TaskStatus taskStatus) {
            Lock lock(this->_taskMutex);
            this->_taskStatus = taskStatus;
        }

        /** Sets the task's priority*/
        void _setPriority(TaskPriority priority) {
            Lock lock(this->_taskMutex);
            this->_priority = priority;
        }

        Mutex _taskMutex;
        AsyncWaitHandleImpl _startedSignal;

        std::shared_ptr<AsyncWaitHandle> _asyncWaitHandle;

        TaskStatus _taskStatus{};
        TaskPriority _priority{TaskPriority::Normal};

        std::string _taskName{};

        uint32_t _taskId{0};

        bool _isInGroup{false};
        uint32_t _taskGroupId{0};

        const std::function<void()> _work;
        const std::shared_ptr<Task> _taskDependency{nullptr};
    };

    /** Description model used to create task groups */
    struct TaskGroupDescription {
    public:
        /** The name of the group */
        std::string Name;

        /** The Task description models */
        std::vector<TaskDescription> Tasks;

        /** The priority of the tasks in the group s*/
        TaskPriority GroupPriority;

        /** The groups dependency */
        std::shared_ptr<Task> Dependency;
    };

    /**
     * Represents a group of tasks that may be queued in the TaskScheduler to be processed in parallel.
     */
    class TaskGroup {
    public:

        TaskGroup() = default;

        explicit TaskGroup(const TaskGroupDescription &description)
                : _groupPriority(description.GroupPriority),
                  _groupName(description.Name),
                  _dependency(description.Dependency) {
            this->createNewTaskFromGroupDescription(description);
        }

        /** Returns true if all the tasks in the group have completed. */
        [[nodiscard("Unnecessary call")]]
        bool isComplete() {
            for (const auto &task : this->_tasks) {
                if (!task->isComplete())
                    return false;
            }

            return true;
        }

        /**
         * Blocks the calling thread until all tasks have been completed
         */
        void waitForAll() {
            for (const auto &task : this->_tasks) {
                task->wait();
            }
        }

        /**
         * Adds a new task to the group
         * @param taskDescription Task description instance
         */
        void addNewTask(TaskDescription taskDescription) {
            overWriteTaskPriority(taskDescription);
            overWriteTaskDependency(taskDescription);

            auto task = std::make_shared<Task>(taskDescription);
            this->_tasks.push_back(task);
        }

    private:
        friend class TaskScheduler;

        std::vector<std::shared_ptr<Task>> _tasks{nullptr};
        TaskPriority _groupPriority{TaskPriority::Normal};

        uint32_t _id{0};
        std::string _groupName;

        std::shared_ptr<Task> _dependency{nullptr};

        /** Create a new task the existing group description */
        void createNewTaskFromGroupDescription(const TaskGroupDescription &description) {
            for (const auto &taskDescription : description.Tasks) {
                this->addNewTask(taskDescription);
            }
        }

        /** Overwrites the task description's priority to that of the group */
        void overWriteTaskPriority(TaskDescription &taskDescription) {
            taskDescription.Priority = this->_groupPriority;
        }

        void overWriteTaskDependency(TaskDescription &taskDescription) {
            taskDescription.Dependency = this->_dependency;
        }
    };
}

#endif //VENUS_TASK_H
