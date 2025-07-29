#pragma once
#ifndef NET_IOMULTIPLEX_H_
#define NET_IOMULTIPLEX_H_

#include <vector>

#include "common.h"
#include "EventDispatcher.h"

class IOMultiplex
{
public:
    IOMultiplex() = default;
    virtual ~IOMultiplex() = default;

    virtual void poll(uint32_t timeOutUs, std::vector<EventDispatcher*>& triggerEventDispatchers) = 0;

    virtual void registerReadEvent(SOCKET fd, EventDispatcher* dispatcher) = 0;
    virtual void registerWriteEvent(SOCKET fd, EventDispatcher* dispatcher) = 0;

    virtual void unRegisterReadEvent(SOCKET fd, EventDispatcher* dispatcher) = 0;
    virtual void unRegisterWriteEvent(SOCKET fd, EventDispatcher* dispatcher) = 0;

    virtual void unRegisterAllEvent(SOCKET fd, EventDispatcher* dispatcher) = 0;

};

#endif