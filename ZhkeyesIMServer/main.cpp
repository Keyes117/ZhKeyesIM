

#include <atomic>
#include <iostream>
#include <csignal>    
#include <thread>     
#include <chrono>      


#include "util/ConfigManager.h"
#include "IMServer.h"
#include "Logger.h"

#ifndef _WIN32
std::atomic<bool> g_running = true;

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        g_running = false;
    }
}
#endif

int main()
{

    Logger::instance().setLogLevel(LogLevel::INFO);

    //Logger::instance().setLogFile("server.log");

#ifdef _WIN32
    if (!net::SocketUtil::InitNetwork())
    {
        LOG_ERROR("Failed to init Windows Network lib");
        return 1;
    }
#endif // _WIN32

#ifndef _WIN32
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
#endif

    ZhKeyes::Util::ConfigManager config;
    if (!config.load("config.json"))
    {
        LOG_ERROR("failed to read config...");
        return 0;
    }

    IMServer server;
    if (!server.init(config))
    {
        LOG_ERROR("failed to initialize IMServer ");
        return 1;
    }

    server.start();
#ifndef _WIN32
    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#endif
    LOG_INFO("Server shutting down...");

#ifdef _WIN32
    net::SocketUtil::CleanupNetwork();
#endif // _WIN32

    return 0;
}