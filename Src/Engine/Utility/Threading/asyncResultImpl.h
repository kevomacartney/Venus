//
// Created by Kelvin Macartney on 14/04/2020.
//

#ifndef VENUS_ASYNCRESULTIMPL_H
#define VENUS_ASYNCRESULTIMPL_H

#include <Threading/asyncResult.h>
#include <genericObject.h>

namespace Venus::Core {
    /**
     * Manages a list of commands that can be queued for execution on the core thread
     */
    class QueuedCommand;

}
namespace Venus::Utility::Threading {
    /**
     * Implementation of AsyncResult class. Object you may use to check on the results of an asynchronous operation.
     */
    template<class ReturnType>
    class AsyncResultImpl : public AsyncResult {
        friend Venus::Core::QueuedCommand;
    public:
        AsyncResultImpl() = default;

        explicit AsyncResultImpl(const std::shared_ptr<AsyncResultSyncData> &syncData)
                : AsyncResult(syncData) {}

        /** Retrieves the value returned by the async operation as a generic type */
        ReturnType getTaskResultAsType() {
            auto result = this->getTaskResultObject();

            return GenericObject::any_cast<ReturnType>(result);
        }

    protected:
        /** Marks the task as completed */
        void _markAsComplete() {
            this->_syncData->markAsComplete();
        }

        /**
         * Sets the return value and marks the task as completed
         * @param returnValue The return value
         */
        void _markAsCompleteWithValue(const ReturnType &returnValue) {
            this->_syncData->setReturnValue(returnValue);
            this->_syncData->markAsComplete();
        }

    protected:
        template<class ReturnType2>
        friend bool operator==(const AsyncResultImpl<ReturnType2> &, std::nullptr_t);

        template<class ReturnType2>
        friend bool operator!=(const AsyncResultImpl<ReturnType2> &, std::nullptr_t);
    };

    template<class ReturnType2>
    bool operator==(const AsyncResultImpl<ReturnType2> &leftSide, std::nullptr_t) {
        return leftSide._syncData == nullptr;
    }

    template<class ReturnType2>
    bool operator!=(const AsyncResultImpl<ReturnType2> &leftSide, std::nullptr_t) {
        return leftSide._syncData == nullptr;
    }

    /** @copydoc TAsyncOp */
    using AsyncResultObject = AsyncResultImpl<GenericObject>;
}

#endif //VENUS_ASYNCRESULTIMPL_H
