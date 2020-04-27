//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_QUEUEDCOMMAND_H
#define VENUS_QUEUEDCOMMAND_H

#include <functional>
#include <Threading/asyncResultImpl.h>
#include <memory>
#include <utility>

namespace Venus::Core {
    using namespace Utility::Threading;

    /**
     * Represents a single queued command in the command queue
     *
     * @note This command contains all the data it needs to be executed
     */
    struct QueuedCommand {
    public:
        /** Constructor*/
        QueuedCommand(std::function<void()> callback, std::shared_ptr<AsyncResultObject> asyncResult)

                : _callback{std::move(callback)},
                  _asyncResult{std::move(asyncResult)} {}

        /** Constructor */
        QueuedCommand(std::function<GenericObject()> callback, std::shared_ptr<AsyncResultObject> asyncResult)

                : _callbackWithReturnValue(std::move(callback)),
                  _returnsValue(true),
                  _asyncResult{std::move(asyncResult)} {}

        /**
         * Executes the given this command
         */
        void execute() {

            if (this->_returnsValue) {
                GenericObject returnedValue = _callbackWithReturnValue();
                this->_asyncResult->_markAsCompleteWithValue(returnedValue);

                return;
            }

            _callback();
            this->_asyncResult->_markAsComplete();
        }

    private:
        std::function<void()> _callback{nullptr};
        std::function<GenericObject()> _callbackWithReturnValue{nullptr};

        bool _returnsValue{false};

        std::shared_ptr<AsyncResultObject> _asyncResult{nullptr};
    };
}


#endif //VENUS_QUEUEDCOMMAND_H
