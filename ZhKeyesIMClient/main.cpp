

#include <iostream>

#include "IMClient.h"
#include "Logger.h"

int main()
{

    Logger::instance().setLogLevel(LogLevel::INFO);

    Logger::instance().setLogFile("client.log");

    IMClient client;
    client.init("127.0.0.1", 9000);
    if (!client.connect())
    {
        return 0;
    }

    while (1)
    {
        int i = 0;
    }

    return 1;



}