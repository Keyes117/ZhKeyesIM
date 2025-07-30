#include <iostream>

#include "TCPServer.h"

int main()
{

    TCPServer server;
    server.init(4, "127.0.0.1", 9000);
    server.start();
}