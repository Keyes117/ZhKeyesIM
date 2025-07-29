#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <iostream>

#include "IMClient.h"

#ifdef _WIN32
void stopWSASocket()
{
    WSACleanup();
}

#endif // _WIN32


int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    int wsaRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaRet != 0)
    {
        std::cout << "WSAStartup failed: %d" << wsaRet << std::endl;
        return 0;
    }
#endif

    IMClient client;
    client.init("127.0.0.1", 9000);
    if (!client.connect())
    {
#ifdef _WIN32
        stopWSASocket();
#endif
        return 0;
    }

    while (1)
    {
        int i = 0;
    }
#ifdef _WIN32
    stopWSASocket();
#endif
    return 1;



}