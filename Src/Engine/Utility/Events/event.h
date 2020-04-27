//
// Created by Kelvin Macartney on 30/03/2020.
//

#ifndef VENUS_VENUSEVENT_H
#define VENUS_VENUSEVENT_H

#include "queuedEvent.h"
#include "executionFlags.h"
#include "eventDispatcher.h"
#include <Threading/threading.h>
#include <Threading/threadPool.h>
#include <map>

namespace Venus::Utility::Events {

    /**
     * Represents a subscribe-able event where subscribers will be notified
     * @tparam eventCallback The function type for this event
     * @note Class is Thread-safe
     */
    template<typename eventCallback, typename ...eventArgs>
    class Event {
    public:
        Event() : _notifyExecutionFlag(EventExecutionFlag::EEF_Default),
                  _eventDispatcher(Utility::Events::EventDispatcher::instance()) {}

        /** Constructor with default queue behaviour */
        explicit Event(EventExecutionFlag executionFlag) : _notifyExecutionFlag(executionFlag),
                                                           _eventDispatcher(EventDispatcher::instance()) {}

        /**
         *
         * @tparam eventCallback The callback function
         * @param callback
         * @return subscription id
         */
        uint32_t subscribe(eventCallback callback) {
            Lock lock(this->_subscriptionMutex);

            uint32_t id = ++this->_subscriberIds;
            QueuedEvent<eventCallback> subscriber(this->_notifyExecutionFlag, callback, id);
            this->_callbackMap.insert({id, subscriber});

            return id;
        }

        /**
         * Stops notifying the subscription with the given id
         * @param subscriptionId The subscription id returned when event was created
         */
        void unsubscribe(uint32_t subscriptionId) {
            Lock lock(this->_subscriptionMutex);

            this->_callbackMap.erase(subscriptionId);
        }

        /**
         * Notifies all subscribers by scheduling events to distributed
         * @tparam Args Argument types for the callback
         * @param args callback arguments
         */
        void notifyAll(eventArgs... args) {
            auto subscribers = this->retrieveSubscribers();

            for (auto subscriber: subscribers) {
                this->_eventDispatcher->scheduleInvocation(subscriber, args...);
            }
        }

        /**
         * Notifies a single subscriber with the given id
         * @tparam ArgsArgument types for the callback
         * @param id Subscriber id
         * @param args callback arguments
         */
        void notifyOne(uint32_t id, eventArgs... args) {
            Lock lock(this->_subscriptionMutex);
            auto it = this->_callbackMap.find(id);

            if (it == this->_callbackMap.end())
                return;

            QueuedEvent<eventCallback> subscriber = it->second;
            this->scheduleInvocation(subscriber, args...);
        }

        ~Event() = default;

    private:
        Mutex _subscriptionMutex;
        uint32_t _subscriberIds{0};

        EventExecutionFlag _notifyExecutionFlag{};

        std::shared_ptr<Utility::Events::EventDispatcher> _eventDispatcher{nullptr};
        std::map<uint32_t, QueuedEvent<eventCallback>> _callbackMap;

        /** Retrieves all the event subscribers in thread safe manner */
        std::vector<QueuedEvent<eventCallback>> retrieveSubscribers() {
            std::vector<QueuedEvent<eventCallback>> subscribers;

            Lock lock(this->_subscriptionMutex);
            for (auto const&[key, callback] : this->_callbackMap) {
                subscribers.push_back(callback);
            }

            return subscribers;
        }
    };
}
#endif //VENUS_VENUSEVENT_H
