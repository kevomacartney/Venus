//
// Created by Kelvin Macartney on 14/03/2020.
//

#ifndef VENUS_LOGGINGFACTORY_H
#define VENUS_LOGGINGFACTORY_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Venus::Factories {
    /**
     * Defines the different types of loggers the factory can create
     */
    enum {
        DailyFile,
        ColouredStdOut,
    } typedef LoggerType;

    /**
     * A class for creating new loggers for the application
     */
    class LoggingFactory {
    public:

        /**
         * Creates a new console logger
         * @param name The name of the logger
         * @return A pointer to the logger
         */
        static std::shared_ptr<spdlog::logger>
        CreateLogger(LoggerType type, const std::string &name, const std::string &fileName = "log.log");

        /**
         * Creates a new logger by calling the factory function called
         * @param f The factory function
         * @return The logger create by the factory function
         */
        static std::shared_ptr<spdlog::logger> CreateLogger(std::shared_ptr<spdlog::logger> (*f)());
    };
}


#endif //VENUS_LOGGINGFACTORY_H
