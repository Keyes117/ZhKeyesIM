#include <iostream>


#include "util/ConfigManager.h"
#include "IMServer.h"
#include "Logger.h"

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

#ifdef _WIN32
    net::SocketUtil::CleanupNetwork();
#endif // _WIN32

    return 0;
}