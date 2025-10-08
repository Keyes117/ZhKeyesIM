#ifndef NET_TCPCONNECTION_H_
#define NET_TCPCONNECTION_H_

#include <functional>
#include <memory>

#include "Buffer.h"
#include "common.h"
#include "EventDispatcher.h"
#include "EventLoop.h"
#include "net_export.h"

using ReadCallBack = std::function<void(Buffer&)>;
using WriteCallBack = std::function<void()>;
using CloseCallBack = std::function<void()>;

class NET_API TCPConnection : public EventDispatcher
{
public:
    TCPConnection(SOCKET fd, const std::shared_ptr<EventLoop>& spEventLoop);
    virtual ~TCPConnection();

    void startRead();
    bool send(const char* buf, size_t bufLen);
    bool send(const std::string& buf);

public:
    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose() override;

    virtual void enableRead(bool isEnabled) override
    {
        m_enableRead = isEnabled;
    }
    virtual void enableWrite(bool isEnabled) override
    {
        m_enableWrite = isEnabled;
    }

    void setReadCallback(ReadCallBack&& readCallBack)
    {
        m_readCallBack = std::move(readCallBack);
    }
    void setWriteCallback(WriteCallBack&& writeCallBack)
    {
        m_writeCallBack = std::move(writeCallBack);
    }
    void setCloseCallback(CloseCallBack&& closeCallBack)
    {
        m_closeCallBack = std::move(closeCallBack);
    }

private:

    bool sendInterval(const char* buf, size_t bufLen);

    bool isCallableInOwnerThread();

    void registerReadEvent();
    void registerWriteEvent();
    void unregisterReadEvent();
    void unregisterWriteEvent();

private:
    SOCKET m_socket;

    bool    m_registerReadEvent;
    bool    m_registerWriteEvent;

    bool    m_enableRead{ false };
    bool    m_enableWrite{ false };

    std::shared_ptr<EventLoop>      m_spEventLoop;

    Buffer  m_recvBuf;
    Buffer  m_sendBuf;

    ReadCallBack    m_readCallBack;
    WriteCallBack   m_writeCallBack;
    CloseCallBack   m_closeCallBack;
};

#endif