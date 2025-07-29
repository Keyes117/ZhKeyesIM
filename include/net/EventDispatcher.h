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

    virtual void enableRead(bool isEnabled) = 0;
    virtual void enableWrite(bool isEnabled) = 0;
};

#endif