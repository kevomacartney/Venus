//
// Created by Kelvin Macartney on 08/04/2020.
//

#ifndef VENUS_EVENTDISPATCHER_H
#define VENUS_EVENTDISPATCHER_H

#include <Module.h>
#include <coreThread.h>
#include <Threading/threadPool.h>
#include "queuedEvent.h"

namespace Venus::Utility::Events {
    class EventDispatcher : public Module<EventDispatcher> {
    public:
        EventDispatcher() = default;

        /**
         * Schedules the event to be invoked using the request execution flag
         * @tparam eventCallback
         * @tparam eventArgs
         * @param queuedEvent Callback function
         * @param arguments Callback function parameters
         */
        template<typename eventCallback, typename ...eventArgs>
        void scheduleInvocation(QueuedEvent<eventCallback> queuedEvent, eventArgs... arguments) {
            switch (queuedEvent.executionFlag) {
                case EventExecutionFlag::EEF_CoreThreadQueue: {
                    invokeWithCoreThread(queuedEvent, arguments...);
                    break;
                }
                case EventExecutionFlag::EEF_Caller: {
                    invokeWithCallThread(queuedEvent, arguments...);
                    break;
                }
                default: {
                    invokeWithThreadPool(queuedEvent, arguments...);
                    break;
                }
            }
        }

    private:
        /**
         * Event callback invocation is invoked with the calling thread
         * @tparam eventCallback
         * @tparam eventArgs
         * @param queuedEvent Callback function
         * @param arguments Callback function parameters
         */
        template<typename eventCallback, typename ...eventArgs>
        void invokeWithCallThread(QueuedEvent<eventCallback> queuedEvent, eventArgs... arguments) {
            queuedEvent.callback(arguments...);
        }

        /**
         * Event callback invocation is queued to be invoked by the core thread
         * @tparam eventCallback
         * @tparam eventArgs
         * @param queuedEvent Callback function
         * @param arguments Callback function parameters
         */
        template<typename eventCallback, typename ...eventArgs>
        void invokeWithCoreThread(QueuedEvent<eventCallback> queuedEvent, eventArgs... arguments) {
            auto coreThread = Core::CoreThread::instance();

            coreThread->queueCommand([queuedEvent, arguments...]() {
                queuedEvent.callback(arguments...);
            }, CTQF_InternalQueue);
        }

        /**
         * Event callback invocation is queued to be invoked by a thread on the thread pool
         * @tparam eventCallback
         * @tparam eventArgs
         * @param queuedEvent Callback function
         * @param arguments Callback function parameters
         */
        template<typename eventCallback, typename ...eventArgs>
        void invokeWithThreadPool(QueuedEvent<eventCallback> queuedEvent, eventArgs... arguments) {
            auto threadPool = Utility::Threading::ThreadPool::instance();
            threadPool->queueWork(queuedEvent.callback);
        }
    };
}

#endif //VENUS_EVENTDISPATCHER_H
