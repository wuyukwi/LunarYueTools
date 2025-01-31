#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class Logger
{
public:
    static void init(bool logToFile = false, const std::string& logFilePath = "logs.txt", spdlog::level::level_enum logLevel = spdlog::level::info)
    {
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
        spdlog::set_default_logger(logger);
    }

    static std::shared_ptr<spdlog::logger> get() { return logger; }

    static void setLogLevel(spdlog::level::level_enum level) { logger->set_level(level); }

private:
    static std::shared_ptr<spdlog::logger> logger;
};

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

#define APPLOG_INFO(...) Logger::get()->info(__VA_ARGS__)
#define APPLOG_TRACE(...) Logger::get()->trace(__VA_ARGS__)
#define APPLOG_ERROR(...) Logger::get()->error(__VA_ARGS__)
#define APPLOG_WARNING(...) Logger::get()->warn(__VA_ARGS__)
#define APPLOG_DEBUG(...) Logger::get()->debug(__VA_ARGS__)
#define APPLOG_SEPARATOR() APPLOG_INFO("-----------------------------")
