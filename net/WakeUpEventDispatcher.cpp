#include "WakeUpEventDispatcher.h"

#include <cstdint>

#include "Logger.h"

WakeUpEventDispatcher::WakeUpEventDispatcher(SOCKET socket):
    m_wakeUpSocket(socket)
{
}

WakeUpEventDispatcher::~WakeUpEventDispatcher()
{
    if (m_wakeUpSocket != INVALID_SOCKET)
    {
        closesocket(m_wakeUpSocket);
        m_wakeUpSocket = INVALID_SOCKET;
    }
  
}

void WakeUpEventDispatcher::onRead()
{
    if (!m_enabledRead || m_wakeUpSocket == INVALID_SOCKET)
        return;

    uint64_t dummyData = 0;
    sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);
#ifdef _WIN32
    while (true)
    {
  
        int n = ::recvfrom(m_wakeUpSocket, reinterpret_cast<char*>(&dummyData),
            static_cast<int>(sizeof(dummyData)), 0,(sockaddr*)&fromAddr, &fromLen);

        if (n > 0)
        {
            if (n != sizeof(dummyData))
            {
                LOG_WARN("WakeUpEventDispatcher::onRead partial read, expected %d, got %d", sizeof(dummyData), n);
            }
            //继续读，尽量清空
            continue;
        }
        else if (n == 0)
        {
            // 对端关闭
            LOG_INFO("WakeUpEventDispatcher peer closed, fd = %d", m_wakeUpSocket);
            break;
        }
        else
        {
            int err = GetSocketError();
            if (err == EWOULDBLOCK || err == EINTR)
                //无更多数据或被中断
                break;

            LOG_ERROR("WakeUpEventDispatcher::onRead failed, error = %d", err);
            break;
        }
    }
#else
    int64_t dummyData;
    int n = ::read(m_wakeUpfd, static_cast<void*>(&dummyData), sizeof(dummyData));

    if (n != sizeof(dummyData))
        LOG_INFO("WakeupEventDispatcher::onRead failed, errno = %d", errno);
    else
        LOG_INFO("WakeupEventDispatcher::onRead successfully, wakefd: %d", m_wakeUpfd);
#endif
}

void WakeUpEventDispatcher::onWrite()
{
}

void WakeUpEventDispatcher::onClose()
{
}

void WakeUpEventDispatcher::enableRead(bool isEnabled)
{
    m_enabledRead = isEnabled;
}

void WakeUpEventDispatcher::enableWrite(bool isEnabled)
{
    m_enabledWrite = isEnabled;
}

void WakeUpEventDispatcher::WakeUp()
{
    if (m_wakeUpSocket == INVALID_SOCKET)
        return;

    uint64_t one = 1;
#ifdef _WIN32
    int n = ::sendto(m_wakeUpSocket, reinterpret_cast<const char*>(&one),
                static_cast<int>(sizeof(one)), 0, (sockaddr*)&m_wakeUpAddr,sizeof(m_wakeUpAddr));

    if (n == SOCKET_ERROR)
    {
        int err = GetSocketError();
        if (err != WSAEWOULDBLOCK && err != WSAEINTR)
        {
            LOG_ERROR("WakeUpEventDispatcher::WakeUp send failed, error = %d", err);
        }
    }
#else
    int64_t dummyData = 0;
    int n = ::write(m_wakeUpfd, static_cast<const void*>(&dummyData), sizeof(dummyData));


    if (n != sizeof(dummyData))
        LOG_INFO("WakeupEventDispatcher::wakeup failed, errno = %d", errno);
    else
        LOG_INFO("WakeupEventDispatcher::wakeup successfully, wakefd: %d", m_wakeUpfd);


#endif
}

void WakeUpEventDispatcher::setWakeUpSocketAddr(const sockaddr_in& addr)
{
    m_wakeUpAddr = addr;
}
