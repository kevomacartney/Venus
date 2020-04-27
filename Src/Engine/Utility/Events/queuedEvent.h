//
// Created by Kelvin Macartney on 30/03/2020.
//

#ifndef VENUS_QUEUEDEVENT_H
#define VENUS_QUEUEDEVENT_H

#include <cstdint>
#include "executionFlags.h"

namespace Venus::Utility::Events {

    template<typename eventCallback>
    struct QueuedEvent {
    public:
        /**
         * Constructor
         * @param flags EventExecutionFlag describing how the event will be scheduled
         * @param func The callback function
         * @param subId The subscription id
         */
        QueuedEvent(EventExecutionFlag flags, eventCallback func, uint32_t subId)
                : executionFlag(flags),
                  callback(func),
                  subscriberId(subId) {}

        /** Specified the thread execution flag */
        EventExecutionFlag executionFlag;

        /** The function for notifying the subscriber */
        eventCallback callback;

        /** The subscriber id */
        uint32_t subscriberId;
    };
}
#endif //VENUS_QUEUEDEVENT_H
