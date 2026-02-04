#include "ChatGrpcClient.h"

ChatGrpcClient::ChatGrpcClient()
{
}

ChatGrpcClient::~ChatGrpcClient()
{

    m_running = false;
    if (m_cq)
    {
        m_cq->Shutdown();
    }

    if (m_cqThread.joinable())
    {
        m_cqThread.join();
    }

}

bool ChatGrpcClient::init(const ZhKeyes::Util::ConfigManager& config)
{
    return false;
}

void ChatGrpcClient::processCQ()
{
}
