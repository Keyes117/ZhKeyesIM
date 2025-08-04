#ifndef ZHKEYESIMCLIENT_IMCLIENT_H_
#define ZHKEYESIMCLIENT_IMCLIENT_H_

#include "TCPClient.h"

class IMClient
{
public:
    IMClient();
    ~IMClient();

    void init(const std::string& serverIp, uint16_t nPort);
    bool connect();

private:
    void networkThreadFunc();

private:

    std::atomic<bool>                       m_eventLoopRunning = false;

    std::shared_ptr<EventLoop>              m_spMainEventLoop;
    std::unique_ptr<std::thread>            m_networkThread;
    std::unique_ptr<TCPClient>              m_spClient;
};


#endif
