#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace core
{
    class Logger
    {
    public:
        static void
        init(bool logToFile = false, const std::string& logFilePath = "logs.txt", spdlog::level::level_enum logLevel = spdlog::level::info);
        static std::shared_ptr<spdlog::logger> get();
        static void                            setLogLevel(spdlog::level::level_enum level);

    private:
        static std::shared_ptr<spdlog::logger> logger;
    };

#define APPLOG_INFO(...) Logger::get()->info(__VA_ARGS__)
#define APPLOG_TRACE(...) Logger::get()->trace(__VA_ARGS__)
#define APPLOG_ERROR(...) Logger::get()->error(__VA_ARGS__)
#define APPLOG_WARNING(...) Logger::get()->warn(__VA_ARGS__)
#define APPLOG_DEBUG(...) Logger::get()->debug(__VA_ARGS__)
#define APPLOG_SEPARATOR() APPLOG_INFO("-----------------------------")

} // namespace core
