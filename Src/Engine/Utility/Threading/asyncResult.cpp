//
// Created by Kelvin Macartney on 14/04/2020.
//

#include <Threading/threading.h>
#include <spdlog/spdlog.h>
#include <Error/venusExceptions.h>
#include "asyncResult.h"
#include "TaskScheduler/taskScheduler.h"

namespace Venus::Utility::Threading {

    bool Threading::AsyncResult::hasCompleted() {
        return this->_syncData->isCompleted();
    }

    void AsyncResult::blockUntilComplete() {
        if (!this->_syncData) {
            spdlog::error("Could not block until complete, sync data has not been initialised.");
            return;
        }

        _syncData->getWaitHandle().wait();
    }

    GenericObject AsyncResult::getTaskResultObject() {
        if (!this->_syncData->isCompleted()) VENUS_EXCEPT(InvalidOperationException,
                                                          "Task must be completed before getting result");

        return this->_syncData->getReturnedValue();
    }
}