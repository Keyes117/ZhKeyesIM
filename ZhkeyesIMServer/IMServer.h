#ifndef ZHKEYESIMSERVER_IMSERVER_H_
#define ZHKEYESIMSERVER_IMSERVER_H_

#include "TCPServer.h"

#include "ConfigManager.h"

class IMServer
{
public:
    IMServer();
    ~IMServer();

    bool init(const std::string& ip, uint16_t port);

private:
    std::unique_ptr<TCPServer> m_spTcpServer;
};


#endif