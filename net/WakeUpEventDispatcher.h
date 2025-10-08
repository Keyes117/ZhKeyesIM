#ifndef NET_WAKEUPEVENTDISPATCHER_H_
#define NET_WAKEUPEVENTDISPATCHER_H_

#include "common.h"
#include "EventDispatcher.h"


class WakeUpEventDispatcher : public EventDispatcher
{
public:
    WakeUpEventDispatcher(SOCKET socket);
    virtual ~WakeUpEventDispatcher();

    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose() override;

    virtual void enableRead(bool isEnabled) override;
    virtual void enableWrite(bool isEnabled) override;

    void WakeUp();
#ifdef _WIN32
    void setWakeUpSocketAddr(const sockaddr_in& addr);
#endif
private:
    int     m_wakeUpSocket;
    bool    m_enabledRead{ false };
    bool    m_enabledWrite{ false };

#ifdef _WIN32
    sockaddr_in   m_wakeUpAddr;
#endif
};

#endif