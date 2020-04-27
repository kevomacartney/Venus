//
// Created by Kelvin Macartney on 08/04/2020.
//

#ifndef VENUS_MODULE_H
#define VENUS_MODULE_H

#include <Error/venusExceptions.h>

namespace Venus {
    /**
     * Represents one engine module. Essentially it is a specialized type of singleton.
     */
    template<class T>
    class Module {
    public:
        /**
         * Returns a shared pointer to the module instance.
         * @note Module has to be started up first otherwise an exception will be thrown.
         */
        static std::shared_ptr<T> instance() {
            if (!isIgnited()) {
                VENUS_EXCEPT(InternalErrorException,
                             "Trying to access a module but it hasn't been started up yet.");
            }

            if (isShutdown()) {
                VENUS_EXCEPT(InternalErrorException,
                             "Trying to access a destroyed module.");
            }

            return _instance();
        }

        /** Constructs and initialises the module */
        template<class ...Args>
        static std::shared_ptr<T> ignite(Args &&...args) {
            if (isIgnited()) VENUS_EXCEPT(InternalErrorException, "Trying to start an already started module.");

            isIgnited() = true;

            _instance() = std::make_shared<T>(std::forward<Args>(args)...);
            _instance()->ignition();

            return _instance();
        }

        /** Shuts down this module and frees any resources it is using. */
        static void shutDown() {
            if (isShutdown()) {
                VENUS_EXCEPT(InternalErrorException,
                             "Trying to shut down an already shut down module.");
            }

            if (!isIgnited()) {
                VENUS_EXCEPT(InternalErrorException,
                             "Trying to shut down a module which was never started.");
            }

            _instance()->shutdown();

            isShutdown() = true;
        }

        /** These notions are redundant */
        Module(Module &&) = delete;

        Module(const Module &) = delete;

        Module &operator=(Module &&) = delete;

        Module &operator=(const Module &) = delete;

    protected:

        Module() = default;

        virtual ~Module() = default;

        /** Returns a boolean indicating if the instance has been started up */
        static bool &isIgnited() {
            static bool ignited = false;
            return ignited;
        }

        /** Returns a boolean indicating if the instance has been shut down */
        static bool &isShutdown() {
            static bool shutDown = false;
            return shutDown;
        }

        virtual void ignition() {}

        virtual void shutdown() {}

        static std::shared_ptr<T> &_instance() {
            static std::shared_ptr<T> inst = nullptr;
            return inst;
        };
    };
}

#endif //VENUS_MODULE_H
