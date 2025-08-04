#include "Logger.h"

#include <functional>



Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
    :m_running(true),
    m_currentLevel(LogLevel::DEBUG)
{
    m_logThread = std::thread(std::bind(&Logger::logThreadFunc, this));

}

Logger::~Logger()
{
    stop();
}

void Logger::logThreadFunc()
{
    while (m_running || !m_logQueue.empty())
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_queueCondition.wait(lock, [this] {return !m_logQueue.empty() || !m_running; });

        while (!m_logQueue.empty())
        {
            LogMessage logMsg = std::move(m_logQueue.front());
            m_logQueue.pop();
            lock.unlock();

            std::string formatted = formatMessage(logMsg);
            if (m_logFile.is_open())
            {
                m_logFile << formatted << std::endl;
            }
            else
            {
                std::cout << formatted << std::endl;
            }

            lock.lock();
        }
    }
}

std::string Logger::formatMessage(const LogMessage& logMsg)
{
    auto t = std::chrono::system_clock::to_time_t(logMsg.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
        logMsg.timestamp.time_since_epoch()
    ) % 1000;

    std::tm tm_time = getLocalTime(t);

    std::ostringstream oss;
    oss << "[" << std::put_time(&tm_time, "%F %T")
        << "." << std::setfill('0') << std::setw(3) << ms.count()
        << "][" << levelToString(logMsg.level)
        << "][TID:" << logMsg.threadId << "] "
        << logMsg.msg;
    return oss.str();
}

std::string Logger::levelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
    return "DEBUG";
    case LogLevel::INFO:
    return "INFO";
    case LogLevel::WARN:
    return "WARN";
    case LogLevel::ERR:
    return "ERROR";
    default:
    return "UNKNOWN";

    }
}

inline std::tm Logger::getLocalTime(std::time_t t)
{
    std::tm tm_time;

#ifdef _WIN32
    localtime_s(&tm_time, &t);
#else
    localtime_r(&t, &tm_time);
#endif

    return tm_time;
}

void Logger::log(LogLevel level, const char* fmt, ...)
{
    if (level < m_currentLevel)
        return;

    char buf[65535];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    LogMessage logMsg;
    logMsg.level = level;
    logMsg.msg = buf;
    logMsg.timestamp = std::chrono::system_clock::now();
    logMsg.threadId = std::this_thread::get_id();

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_logQueue.push(std::move(logMsg));
    }

    m_queueCondition.notify_one();

}

void Logger::setLogLevel(LogLevel level)
{

    m_currentLevel = level;
}

void Logger::setLogFile(const std::string filename)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);

    if (m_logFile.is_open())
        m_logFile.close();

    m_logFile.open(filename, std::ios::app);
}

void Logger::stop()
{
    m_running = false;
    m_queueCondition.notify_all();

    if (m_logThread.joinable())
        m_logThread.join();

    if (m_logFile.is_open())
        m_logFile.close();
}