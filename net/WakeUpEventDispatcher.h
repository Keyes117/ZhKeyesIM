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

    void WakeUp();
#ifdef _WIN32
    void setWakeUpSocketAddr(const sockaddr_in& addr);

    sockaddr_in   m_wakeUpAddr;
#endif
private:
    int     m_wakeUpSocket;

};

#endif // NET_WAKEUPEVENTDISPATCHER_H_!