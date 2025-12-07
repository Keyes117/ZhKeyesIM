#ifndef NET_EVENTDISPATCHER_H_
#define NET_EVENTDISPATCHER_H_

#include "net_export.h"

class NET_API EventDispatcher
{
public:
    EventDispatcher() = default;
    virtual ~EventDispatcher() = default;

    virtual void onRead() = 0;
    virtual void onWrite() = 0;
    virtual void onClose() = 0;

    virtual bool isReadEnabed()
    {
        return m_enableRead;
    }
    virtual bool isWriteEnabled()
    {
        return m_enableWrite;
    }

    virtual void enableRead(bool isEnabled)
    {
        m_enableRead = isEnabled;
    }
    virtual void enableWrite(bool isEnabled)
    {
        m_enableWrite = isEnabled;
    }

protected:
    //表示Dispatcher是否能够 进行读写事件
    bool    m_enableRead{ false };
    bool    m_enableWrite{ false };
};

#endif