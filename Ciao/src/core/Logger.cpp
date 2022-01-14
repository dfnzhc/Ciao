#include "pch.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Ciao
{
    Logger::sp_log Logger::coreLogger;
    Logger::sp_log Logger::logger;

    void Logger::Init()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("%^[%T] %n: %v%$");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs.txt", true);
        file_sink->set_pattern("[%T] [%l] %n: %v");

        std::vector<spdlog::sink_ptr> logSinks{console_sink, file_sink};
        
        coreLogger = std::make_shared<spdlog::logger>("Core", logSinks.begin(), logSinks.end());
        spdlog::register_logger(coreLogger);
        coreLogger->flush_on(spdlog::level::trace);
        coreLogger->set_level(spdlog::level::trace);
        
        logger = std::make_shared<spdlog::logger>("Client", logSinks.begin(), logSinks.end());
        spdlog::register_logger(logger);
        logger->flush_on(spdlog::level::trace);
        logger->set_level(spdlog::level::trace);
        
    }

}
