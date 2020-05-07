//
// Created by Kelvin Macartney on 14/03/2020.
//

#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <filesystem>
#include "loggingFactory.h"

namespace Venus::Factories {
    std::shared_ptr<spdlog::logger> LoggingFactory::CreateLogger(std::shared_ptr<spdlog::logger> (*f)()) {
        std::shared_ptr<spdlog::logger> logger = f();
        return logger;
    }

    std::shared_ptr<spdlog::logger> LoggingFactory::CreateLogger_Safe(Venus::Factories::LoggerType type,
                                                                      const std::string &name,
                                                                      const std::string &fileName) {
        switch (type) {
            case DailyFile: {
                auto path = LoggingFactory::createLogFilePath(fileName);
                auto dailySink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(path, 23, 59);
                auto stdOut = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

                // Collect the sinks
                std::vector<spdlog::sink_ptr> sinks;
                sinks.push_back(dailySink);
                sinks.push_back(stdOut);

                // Create the rotating logger
                return std::make_shared<spdlog::async_logger>(name,
                                                              begin(sinks), end(sinks),
                                                              spdlog::thread_pool());
            }
            case ColouredStdOut:
                return spdlog::stdout_color_mt<spdlog::async_factory>(name); // Create coloured output
            default:
                return nullptr;
        }
    }

    std::string LoggingFactory::createLogFilePath(const std::string &loggerName) {
        std::filesystem::path logPath = "logs";
        logPath /= loggerName;

        return logPath.generic_string();
    }
}