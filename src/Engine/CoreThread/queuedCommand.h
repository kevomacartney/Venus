//
// Created by Kelvin Macartney on 24/03/2020.
//

#ifndef VENUS_QUEUEDCOMMAND_H
#define VENUS_QUEUEDCOMMAND_H

#include <functional>

namespace Venus::Core {
    /**
     * Represents a single queued command in the command queue
     *
     * @note This command contains all the data it needs to be executed
     */
    struct QueuedCommand {
    public:
        QueuedCommand(std::function<void()> callback, uint32_t debugId)
                : _callback{std::move(callback)},
                  _debugId{debugId} {
        }

        /**
         * Executes the given this command
         */
        void execute(){
            _callback();
        }

    private:
        std::function<void()> _callback{nullptr};
        uint32_t _debugId{0};

        bool _returnsValue{false};
    };
}


#endif //VENUS_QUEUEDCOMMAND_H
