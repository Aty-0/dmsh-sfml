#pragma once
#include "singleton.h"
#include <source_location>

namespace dmsh::core
{
    class Logger : public Singleton<Logger>
    {
        public:
            enum LogLevel
            {
                Debug = 0,
                Warning,
                Error, 
                Fatal,

                LOG_LEVEL_SIZE 
            };

            void print(const Logger::LogLevel& level, const std::source_location source, const char* fmt, ...);
                        
        private:
    };

    static inline auto log = dmsh::core::Logger::getInstance();
}

#define DMSH_ASSERT(cond, fmt, ...) do {                                                                                                   \
        if (!(cond))                                                                                                                    \
        {                                                                                                                             \
            DMSH_ERROR("Assertion failed: ");                                                                                              \
            dmsh::core::log->print(dmsh::core::Logger::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__); \
        } } while(false)                                                                                                                      


#define DMSH_DEBUG(fmt, ...) dmsh::core::log->print(dmsh::core::Logger::LogLevel::Debug, std::source_location::current(), fmt, ##__VA_ARGS__)
#define DMSH_WARN(fmt, ...) dmsh::core::log->print(dmsh::core::Logger::LogLevel::Warning, std::source_location::current(), fmt, ##__VA_ARGS__)
#define DMSH_ERROR(fmt, ...) dmsh::core::log->print(dmsh::core::Logger::LogLevel::Error,  std::source_location::current(), fmt, ##__VA_ARGS__)
#define DMSH_FATAL(fmt, ...) dmsh::core::log->print(dmsh::core::Logger::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__)
