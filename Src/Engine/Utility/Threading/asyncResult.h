//
// Created by Kelvin Macartney on 14/04/2020.
//

#ifndef VENUS_ASYNCRESULT_H
#define VENUS_ASYNCRESULT_H

#include <genericObject.h>
#include <memory>
#include "threading.h"
#include <spdlog/spdlog.h>
#include "asyncWaitHandleImpl.h"

namespace Venus::Utility::Threading {

    /** Contains thread synchronization primitives used by AsyncOps and their callers.  */
    class AsyncResultSyncData {
    public:
        Mutex _returnValueMutex;

        /** Returns the sync data's wait handle */
        AsyncWaitHandle &getWaitHandle() {
            return this->_asyncWaitHandle;
        }

        /** Returns true if the task has completed and false otherwise*/
        bool isCompleted() {
            Lock lock(this->_completedMutex);

            return this->_isCompleted;
        }

        /** Returns the value returned by the task */
        GenericObject &getReturnedValue() {
            Lock lock(this->_returnValueMutex);

            return this->_returnedValue;
        }

        void setReturnValue(const GenericObject &returnValue) {
            Lock lock(this->_returnValueMutex);

            this->_returnedValue = returnValue;
        }

        /** Marks the current operation as complete*/
        void markAsComplete() {
            Lock lock(this->_completedMutex);

            this->_isCompleted = true;
            this->_asyncWaitHandle.set();
        }

    private:
        Mutex _completedMutex;
        AsyncWaitHandleImpl _asyncWaitHandle;

        GenericObject _returnedValue;
        volatile bool _isCompleted{false};
    };

    /**  Common base for all AsyncResult specializations. */
    class AsyncResult {
    public:
        explicit AsyncResult(std::shared_ptr<AsyncResultSyncData> syncData)
                : _syncData(std::move(syncData)) {}

        AsyncResult()
                : _syncData(std::make_shared<AsyncResultSyncData>()) {}

        /** Returns true if the task is marked as completed*/
        bool hasCompleted();

        /** Will block the calling thread until the task is marked as complete*/
        void blockUntilComplete();

        /**
         * Retrieves the value returned by the async operation as a generic type.
         * @note This is only valid if task has been marked complete
         */
        GenericObject getTaskResultObject();

    protected:
        std::shared_ptr<AsyncResultSyncData> _syncData{nullptr};
    };
}

#endif //VENUS_ASYNCRESULT_H
