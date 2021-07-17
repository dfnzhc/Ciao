#pragma once

#include "Common.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Ciao {

    class Log
    {
    public:
        Log() = default;
        ~Log() = default;
        
        static void Init();
        static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

}

#ifdef CIAO_DEBUG

// Core log macros
#define CIAO_CORE_TRACE(...)        ::Ciao::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CIAO_CORE_INFO(...)         ::Ciao::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CIAO_CORE_WARN(...)         ::Ciao::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CIAO_CORE_ERROR(...)        ::Ciao::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CIAO_CORE_CRITICAL(...)     ::Ciao::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define CIAO_TRACE(...)             ::Ciao::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CIAO_INFO(...)              ::Ciao::Log::GetClientLogger()->info(__VA_ARGS__)
#define CIAO_WARN(...)              ::Ciao::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CIAO_ERROR(...)             ::Ciao::Log::GetClientLogger()->error(__VA_ARGS__)
#define CIAO_CRITICAL(...)          ::Ciao::Log::GetClientLogger()->critical(__VA_ARGS__)

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