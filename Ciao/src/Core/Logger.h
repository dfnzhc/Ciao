#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Ciao
{
	class Logger
	{
	public:

        Logger() = default;
        ~Logger() = default;

        static void Init();
        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#ifdef CIAO_DEBUG

// Core log macros
#define CIAO_CORE_TRACE(...)        ::Ciao::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define CIAO_CORE_INFO(...)         ::Ciao::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define CIAO_CORE_WARN(...)         ::Ciao::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define CIAO_CORE_ERROR(...)        ::Ciao::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define CIAO_CORE_CRITICAL(...)     ::Ciao::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define CIAO_TRACE(...)             ::Ciao::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define CIAO_INFO(...)              ::Ciao::Logger::GetClientLogger()->info(__VA_ARGS__)
#define CIAO_WARN(...)              ::Ciao::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define CIAO_ERROR(...)             ::Ciao::Logger::GetClientLogger()->error(__VA_ARGS__)
#define CIAO_CRITICAL(...)          ::Ciao::Logger::GetClientLogger()->critical(__VA_ARGS__)

#else

// Core log macros
#define CIAO_CORE_TRACE(...)   
#define CIAO_CORE_INFO(...)    
#define CIAO_CORE_WARN(...)    
#define CIAO_CORE_ERROR(...)   
#define CIAO_CORE_CRITICAL(...)
// Client log macros
#define CIAO_TRACE(...)        
#define CIAO_INFO(...)         
#define CIAO_WARN(...)         
#define CIAO_ERROR(...)        
#define CIAO_CRITICAL(...)

#endif

#ifdef CIAO_ENABLE_ASSERT
#define CIAO_ASSERT(x, msg) if ((x)){} else {CIAO_CORE_CRITICAL("ASSERT - {}\n\t{}\n\t{}\tline: {}", \
                #x, msg, __FILE__, __LINE__); CIAO_BREAK(); }
#else
#define CIAO_ASSERT(x, msg)
#endif