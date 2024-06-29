#pragma once

/*
    In debug mode we use a LogMutex to allow logging in a multi-threaded environment.
    This of course has an inpact on the performace as we are using a mutex.
*/

#ifndef DEBUG
    #define LM_VERBOSE(fmt, ...) (void)0
    #define LM_LOG(fmt, ...) (void)0
    #define LM_ERROR(fmt, ...) (void)0
#else
    #include <cstdarg>
    #include <iostream>
    #include <mutex>

    #define LM_VERBOSE(fmt, ...) g_logMutex.verbose(fmt, ##__VA_ARGS__)
    #define LM_LOG(fmt, ...) g_logMutex.log(fmt, ##__VA_ARGS__)
    #define LM_ERROR(fmt, ...) g_logMutex.error(fmt, ##__VA_ARGS__)

/*
    Class for multi-threading logging
*/
class LogMutex
{
    public: 
        LogMutex():
            #ifdef LOG_MUTEX_LEVEL
                m_logLevel(static_cast<LogLevel>(LOG_MUTEX_LEVEL))
            #else
                m_logLevel(LOG)
            #endif
        {
        
        }

        enum LogLevel
        {
            VERBOSE,
            LOG,
            ERROR
        };

        void log(const char* format, ...) 
        {
            va_list args;
            va_start(args, format);
            log(LOG, format, args);
            va_end(args);
        }

        void verbose(const char* format, ...) 
        {
            va_list args;
            va_start(args, format);
            log(VERBOSE, format, args);
            va_end(args);
        }

        void error(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            log(ERROR, format, args);
            va_end(args);
        }

    private:
        void log(LogLevel level, const char* format, va_list args) 
        {
            std::lock_guard<std::mutex> lock(m_logMutex);

            if (static_cast<int>(level) >= static_cast<int>(m_logLevel)) 
            {
                vprintf(format, args);
                std::cout<<"\n"; // Print newline after the message
            }
        }

        // we use a mutex to protect logging when debugging
        std::mutex m_logMutex;
        const LogLevel m_logLevel; 
} g_logMutex;
#endif
