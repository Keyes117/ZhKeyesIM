#include "TCPConnection.h"

#include "common.h"

TCPConnection::TCPConnection(SOCKET socket, const std::shared_ptr<EventLoop>& spEventLoop)
    :m_socket(socket),
    m_spEventLoop(spEventLoop)
{

}

TCPConnection::~TCPConnection()
{
    if (m_enableRead)
        unregisterReadEvent();
    if (m_enableWrite)
        unregisterWriteEvent();

    closesocket(m_socket);
}

void TCPConnection::startRead()
{
    if (m_spEventLoop) {
        m_spEventLoop->registerCustomTask([this] {
            if (!m_enableRead) {
                enableRead(true);
                registerReadEvent(); // 此时已在所属 EventLoop 线程
            }
            });
    }
}

bool TCPConnection::send(const char* buf, size_t bufLen)
{
    return send(std::string(buf, bufLen));
}

bool TCPConnection::send(const std::string& buf)
{
    if (isCallableInOwnerThread())
    {
        return sendInterval(buf.c_str(), buf.length());
    }
    else
    {
        m_spEventLoop->registerCustomTask(std::bind(static_cast<bool(TCPConnection::*)(const std::string&)>(&TCPConnection::send), this, buf));
        return true;
    }
}

void TCPConnection::onRead()
{
    if (!m_enableRead)
        return;

    char buf[65536];
    int n = ::recv(m_socket, buf, sizeof(buf), 0);
    if (n == 0)
    {
        onClose();
    }
    else if (n < 0)
    {
        auto err = GetSocketError();
        if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
        {
            return;
        }

        //����������ǳ���
        onClose();
    }
    else
    {
        //n>0 ˵��������
        m_recvBuf.append(buf, n);
        if (m_enableRead)
        {
            m_readCallBack(m_recvBuf);
        }
    }

}

void TCPConnection::onWrite()
{
    if (!m_enableWrite)
        return;

    if (m_sendBuf.readableBytes() > 0)
    {
        int n = ::send(m_socket, m_sendBuf.peek(), static_cast<int>(m_sendBuf.readableBytes()), 0);

        if (n < 0)
        {
            auto err = GetSocketError();
            if (err == EINTR || err == EWOULDBLOCK || err == EAGAIN)
            {
                return;
            }

            onClose();
        }
        else if (n > 0)
        {
            m_sendBuf.retrieve(n);

            if (m_sendBuf.readableBytes() == 0)
            {
                // ���ͻ������ѿ�
                enableWrite(false);
                unregisterWriteEvent();

                if (m_writeCallBack)
                    m_writeCallBack();
            }
        }
    }
    else
    {
        //���ͻ�����Ϊ�գ���Ӧ�ô���д�¼�
        enableWrite(false);
        unregisterWriteEvent();
    }
}

void TCPConnection::onClose()
{
    if (m_enableRead)
    {
        enableRead(false);
        unregisterReadEvent();
    }

    if (m_enableWrite)
    {
        enableWrite(false);
        unregisterWriteEvent();
    }

    if (m_closeCallBack)
        m_closeCallBack();

}

bool TCPConnection::sendInterval(const char* buf, size_t bufLen)
{
    if (bufLen == 0)
        return true;

    if (m_socket == INVALID_SOCKET)
        return false;

    m_sendBuf.append(buf, bufLen);

    if (!m_enableRead)
    {
        int n = ::send(m_socket, m_sendBuf.peek(), static_cast<int>(m_sendBuf.readableBytes()), 0);
        if (n > 0)
        {
            //send ����ֵ >0 �ɹ�������n�ֽ�
            m_sendBuf.retrieve(n);

            if (m_sendBuf.readableBytes() > 0)
            {
                //��������û�з���
                //��Socket�ķ��ͻ���������
                enableRead(true);
                registerReadEvent();
                return true;
            }
            //���ݳɹ��������
        }
        else if (n < 0)
        {
            auto err = GetSocketError();

            if (err != EWOULDBLOCK && err != EAGAIN)
            {
                onClose();
                return false;
            }
            enableWrite(true);
            registerWriteEvent();

        }
        else if (n == 0)
        {
            //�Զ˹ر�������
            onClose();
            return false;
        }

    }
    return true;
}

bool TCPConnection::isCallableInOwnerThread()
{
    return std::this_thread::get_id() == m_spEventLoop->getThreadID();
}

void TCPConnection::registerReadEvent()
{
    if (!m_spEventLoop)
        return;

    if (m_registerReadEvent)
        return;

    m_spEventLoop->registerReadEvent(m_socket, this);
    m_registerReadEvent = true;
}

void TCPConnection::registerWriteEvent()
{
    if (!m_spEventLoop)
        return;

    if (m_registerWriteEvent)
        return;

    m_spEventLoop->registerWriteEvent(m_socket, this);
    m_registerWriteEvent = true;
}

void TCPConnection::unregisterReadEvent()
{
    if (!m_spEventLoop)
        return;

    if (!m_registerReadEvent)
        return;

    m_spEventLoop->unregisterReadEvent(m_socket, this);
    m_registerReadEvent = false;
}

void TCPConnection::unregisterWriteEvent()
{
    if (!m_spEventLoop)
        return;

    if (!m_registerWriteEvent)
        return;

    m_spEventLoop->unregisterWriteEvent(m_socket, this);
    m_registerWriteEvent = false;
}

