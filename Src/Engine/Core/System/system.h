//
// Created by Kelvin Macartney on 18/03/2020.
//

#ifndef VENUS_SYSTEM_H
#define VENUS_SYSTEM_H

#include <Module.h>
#include <spdlog/spdlog.h>
#include <loggingFactory.h>
#include <glm/glm.hpp>

namespace Venus::Core {
    /**
     * Provides to system different functionality
     * @note, all methods are thread safe unless stated otherwise
     */
    class System : public Module<System> {
    public:
        System();

        /**
         * Does the necessary initialization of the module
         */
        void ignition() override;

        /**
         * Terminates the program and causes it to create crash report
         */
        static void terminate();

        /**
         * Shuts down the module
         */
        void shutdown() override;

        /**
         * Retrieves the cursor position in screen space
         * @return
         */
        static glm::vec2 getCursorPosition();

        ~System() = default;

    private:
        std::shared_ptr<spdlog::logger> _logger;
    };
}

#endif //VENUS_SYSTEM_H
