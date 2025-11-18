#include "GateServer.h"

int main()
{
    
    Logger::instance().setLogLevel(LogLevel::INFO);

    //Logger::instance().setLogFile("server.log");

    GateServer server;
    if (!server.init(0, "127.0.0.1", 8080))
    {
        LOG_ERROR("failed to initialize GateSerevr ");
        return 1;
    }
    server.start();
    return 0;
}