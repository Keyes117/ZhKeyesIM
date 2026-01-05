#include "StatusServer.h"

#include "common.h"

int main()
{
    Logger::instance().setLogLevel(LogLevel::INFO);

    //Logger::instance().setLogFile("server.log");

#ifdef _WIN32
    if (!net::SocketUtil::InitNetwork())
    {
        LOG_ERROR("初始化网络库失败");
        return 1;
    }
#endif // _WIN32

    ZhKeyes::Util::ConfigManager config;
    if (!config.load("config.json"))
    {
        LOG_ERROR("配置文件读取失败...");
        return 0;
    }

    StatusServer server;
    if (!server.init(config))
    {
        LOG_ERROR("failed to initialize GateSerevr ");
        return 1;
    }
    server.start();

#ifdef _WIN32
    net::SocketUtil::CleanupNetwork();
#endif // _WIN32

    return 0;
}