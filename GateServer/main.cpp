#include "GateServer.h"
#include "Logger.h"

int main()
{

    Logger::instance().setLogLevel(LogLevel::INFO);

    Logger::instance().setLogFile("server.log");

    TCPServer server;
    server.init(4, "127.0.0.1", 9000);
    server.start();
    return 0;
}