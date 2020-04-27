//
// Created by Kelvin Macartney on 26/04/2020.
//

#ifndef VENUS_VENUSTHREAD_H
#define VENUS_VENUSTHREAD_H

#include <thread>
#include <iostream>
#include <cstring>
#include <spdlog/spdlog.h>
#include <Error/venusExceptions.h>

#if defined(SUPPORTS_PTHREAD)

#include <pthread.h>

#elif  defined(_WIN32)

#endif

namespace Venus::Utility::Threading {

    /** Thread priorities available*/
    enum ThreadPriority {
        Lowest = 15,
        Normal = 31,
        High = 40,
        Highest = 45
    };

    /** Venus thread scheduling policy */
    struct VenusSchedulingPolicy {
#if defined(SUPPORTS_PTHREAD)
        /** The threads scheduling priority*/
        ThreadPriority Priority{ThreadPriority::Normal};
        /** The thread scheduling type, SCHED_OTHER, SCHED_RR, SCHED_FIFO */
        uint32_t Policy{SCHED_OTHER};
#endif
    };

    class VenusThread : public std::thread {
    public:
        /** Constructor  */
        template<typename Fp, typename ...Args>
        explicit VenusThread(Fp functionPointer, VenusSchedulingPolicy policy = {}, Args... args)
                :std::thread(functionPointer, args...) {
            auto thread = this->native_handle();
            VenusThread::setPThreadPriority(thread, policy.Policy, policy.Priority);
        }

#if defined(SUPPORTS_PTHREAD)

        /**
         * Sets the thread scheduling parameters
         * @param policy Scheduling policy
         * @param priority The threads priority
         */
        static void setScheduling(pthread_t &thread, VenusSchedulingPolicy policy) {
            setPThreadPriority(thread, policy.Policy, policy.Priority);
        }

#elif _WIN32

        /**
         * Sets the thread scheduling parameters
         * @param policy Scheduling policy
         * @param priority The threads priority
         */
        static void setScheduling(pthread_t &thread, VenusSchedulingPolicy policy) {
            VENUS_EXCEPT(NotImplementedException, "Not implemenented for non pthread based VenusThread")
        }
#endif

        ~VenusThread() = default;

    private:

#if defined(SUPPORTS_PTHREAD)

        /** Sets the priority for a pthreads */
        static void setPThreadPriority(pthread_t &thread, int policy, int priority) {
            if (policy == SCHED_OTHER) priority = 0;

            sched_param params{};
            int pol = 0;
            int max = 0;
            pthread_getschedparam(thread, &pol, &params);
            params.sched_priority = priority;

            if (pthread_setschedparam(thread, policy, &params)) {
                spdlog::error("Failed to set Thread scheduling : {}", strerror(errno));

                VENUS_EXCEPT(InternalErrorException, "Could not set thread scheduling parameters")
            }

            pthread_getschedparam(thread, &pol, &params);
        }

#endif
    };
}
#endif //VENUS_VENUSTHREAD_H

