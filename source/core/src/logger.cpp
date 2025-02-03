#include "Logger.h"
#include <mutex>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace core
{
    std::shared_ptr<spdlog::logger> Logger::logger = nullptr;
    static std::once_flag           initFlag;

    void Logger::init(bool logToFile, const std::string& logFilePath, spdlog::level::level_enum logLevel)
    {
        std::call_once(initFlag, [&]() {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(logLevel);

            if (logToFile)
            {
                auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);
                file_sink->set_level(logLevel);

                std::vector<spdlog::sink_ptr> sinks = {console_sink, file_sink};
                logger                              = std::make_shared<spdlog::logger>("Logger", sinks.begin(), sinks.end());
            }
            else
            {
                logger = std::make_shared<spdlog::logger>("Logger", console_sink);
            }

            logger->set_level(logLevel);
        });
    }

    std::shared_ptr<spdlog::logger> Logger::get()
    {
        if (!logger)
        {
            init();
        }
        return logger;
    }

    void Logger::setLogLevel(spdlog::level::level_enum level)
    {
        if (logger)
        {
            logger->set_level(level);
        }
    }

} // namespace core
