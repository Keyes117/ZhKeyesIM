#include "IMMessageDispatcher.h"

bool ZhKeyesIM::Protocol::IMMessageDispatcher::registerHandler(MessageType type, MessageHandler handler)
{

    std::lock_guard<std::mutex> lock(m_handlerMutex);
    if (m_messageHandlers.find(type) != m_messageHandlers.end())
    {
        return false;
    }

    m_messageHandlers[type] = std::move(handler);

}

bool ZhKeyesIM::Protocol::IMMessageDispatcher::hasRegistered(MessageType type)
{
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    return m_messageHandlers.find(type) != m_messageHandlers.end();
}

bool ZhKeyesIM::Protocol::IMMessageDispatcher::updateHandler(MessageType type, MessageHandler handler)
{
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    m_messageHandlers[type] = std::move(handler);
    return true;
}

bool ZhKeyesIM::Protocol::IMMessageDispatcher::unregisterHandler(MessageType type)
{
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    auto iter = m_messageHandlers.find(type);
    if (iter == m_messageHandlers.end())
    {
        return false;
    }
    
    m_messageHandlers.erase(iter);
    return true;
}

bool ZhKeyesIM::Protocol::IMMessageDispatcher::dispatch(std::shared_ptr<IMMessage> msg, std::shared_ptr<IMMessageSender> sender)
{
    if (!msg || !sender)
    {
        return false;
    }

    MessageType type = msg->getType();

    MessageHandler handler;
    {
        std::lock_guard<std::mutex> lock(m_handlerMutex);
        auto it = m_messageHandlers.find(type);

        if (it == m_messageHandlers.end())
        {
            return false;
        }
        handler = it->second;
    } // ËøÊÍ·Å

    try
    {
        handler(msg, sender);
        return true;
    }
    catch (const std::exception& e)
    {
       
        return false;
    }
    catch (...)
    {
        return false;
    }
}
