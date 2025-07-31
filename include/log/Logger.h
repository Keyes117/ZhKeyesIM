#ifndef LOG_LOGGER_H_
#define LOG_LOGGER_H_


#include <atomic>
#include <chrono>
#include <cinttypes>
#include <condition_variable>
#include <cstdarg>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

#include "log_export.h"

#define LOG_DEBUG(fmt, ...) Logger::instance().log(LogLevel::DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Logger::instance().log(LogLevel::INFO, fmt , ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Logger::instance().log(LogLevel::WARN, fmt , ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::instance().log(LogLevel::ERR, fmt, ##__VA_ARGS__)

enum class LogLevel
{
    DEBUG = 0,
    INFO,
    WARN,
    ERR
};

struct LogMessage
{
    LogLevel level;
    std::string msg;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
};

class LOG_API Logger
{
public:
    static Logger& instance();

    void log(LogLevel level, const char* fmt, ...);

    void setLogLevel(LogLevel level);
    void setLogFile(const std::string filename);
    void stop();

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void logThreadFunc();
    std::string formatMessage(const LogMessage& logMsg);
    std::string levelToString(LogLevel level);

    inline std::tm getLocalTime(std::time_t t);

    std::queue<LogMessage>      m_logQueue;
    std::mutex                  m_queueMutex;
    std::condition_variable     m_queueCondition;
    std::thread                 m_logThread;
    std::atomic<bool>           m_running;
    LogLevel                    m_currentLevel;
    std::ofstream               m_logFile;
};

#endif // !LOG_LOGGER_H_