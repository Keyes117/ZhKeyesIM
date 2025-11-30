#include "GateServer.h"

int main()
{
    
    Logger::instance().setLogLevel(LogLevel::INFO);

    //Logger::instance().setLogFile("server.log");

    ConfigManager config;
    if (!config.load("config.json"))
    {
        LOG_ERROR("≈‰÷√Œƒº˛∂¡»° ß∞‹...");
        return 0;
    }

    GateServer server;
    if (!server.init(config))
    {
        LOG_ERROR("failed to initialize GateSerevr ");
        return 1;
    }
    server.start();
    return 0;
}