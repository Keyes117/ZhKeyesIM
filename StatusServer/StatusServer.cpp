#include "StatusServer.h"

#include "fmt/format.h"

StatusServer::StatusServer():
    m_spRedisManager(std::make_shared<RedisManager>())
{
}

StatusServer::~StatusServer()
{
}

bool StatusServer::init(ZhKeyes::Util::ConfigManager& config)
{
    try
    {
        auto hostOpt = config.getSafe<std::string>({ "StatusServer", "host" });
        auto portOpt = config.getSafe<std::string>({ "StatusServer", "port" });

        if (!hostOpt || !portOpt)
        {
            LOG_ERROR("StatusServer: 获取StatusServer 相关配置失败");
            return false;
        }


        std::string host = *hostOpt;
        std::string port = *portOpt;

        
        m_spTokenRepo = std::make_shared<TokenRepository>(m_spRedisManager);
        m_spChatServerRepo = std::make_shared<ChatServerRepository>(m_spRedisManager);

        m_spStatusService = std::make_shared<StatusService>(m_spChatServerRepo,
            m_spTokenRepo);


        m_spStatusController = std::make_shared<StatusController>(m_spStatusService);

        std::string serverAddr = fmt::format("{}:{}", host, port);
        grpc::ServerBuilder builder;

        builder.AddListeningPort(
            serverAddr,
            grpc::InsecureServerCredentials()
        );

        builder.RegisterService(m_spStatusController.get());
        m_spGrpcServer = builder.BuildAndStart();

        if (!m_spGrpcServer)
        {
            LOG_ERROR("StatusServer: gRPC 服务器启动失败");
            return false;
        }

        if (!m_spRedisManager->init(config))
        {
            LOG_ERROR("StatusServer: redis 客户端 初始化失败");
            return false;
        }

        if (!initChatServers(config))
        {
            LOG_WARN("StatusServer: 聊天服务器列表初始化失败（可能已存在）");
            // 注意：这里不返回 false，因为服务器可能已经在 Redis 中
        }

    }
    catch (const std::exception& e)
    {
        return false;
    }


}

void StatusServer::start()
{
    if (m_spGrpcServer)
    {
        m_spGrpcServer->Wait();
    }

}

void StatusServer::shutdown()
{
    if (m_spGrpcServer)
    {
        LOG_INFO("StatusServer： 正在关闭服务器...");
        m_spGrpcServer->Shutdown();
        m_spGrpcServer.reset();
    }
}

bool StatusServer::initChatServers(ZhKeyes::Util::ConfigManager& config)
{
    try
    {
        // 1. 从配置文件读取聊天服务器列表
        auto chatServersOpt = config.getSafe<nlohmann::json>({ "chatServers" });

        if (!chatServersOpt || !chatServersOpt->is_array())
        {
            LOG_WARN("StatusServer: 配置文件中未找到 chatServers 配置");
            return false;
        }

        const auto& chatServers = *chatServersOpt;

        if (chatServers.empty())
        {
            LOG_WARN("StatusServer: chatServers 列表为空");
            return false;
        }

        LOG_INFO("StatusServer: 开始初始化 %zu 个聊天服务器", chatServers.size());

        // 2. 遍历并注册每个服务器
        int successCount = 0;
        for (const auto& server : chatServers)
        {
            // 获取服务器配置
            auto nameOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(server, "name");
            auto hostOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(server, "host");
            auto portOpt = ZhKeyes::Util::JsonUtil::getSafe<std::string>(server, "port");

            if (!nameOpt || !hostOpt || !portOpt)
            {
                LOG_ERROR("StatusServer: 聊天服务器配置不完整，跳过");
                continue;
            }

            std::string name = *nameOpt;
            std::string host = *hostOpt;
            std::string port = *portOpt;

            // 检查服务器是否已存在
            auto existingInfo = m_spChatServerRepo->getServerInfo(name);
            if (existingInfo.error == 0)
            {
                LOG_INFO("StatusServer: 聊天服务器 %s 已存在，跳过", name.c_str());
                successCount++;
                continue;
            }

            // 3. 添加服务器到 Redis
            // 3.1 添加到服务器列表
            if (!m_spChatServerRepo->addServer(name))
            {
                LOG_ERROR("StatusServer: 添加服务器到列表失败: %s", name.c_str());
                continue;
            }

            // 3.2 设置服务器详细信息
            std::string serverKey = "server:" + name;
            if (!m_spRedisManager->HSet(serverKey, "host", host) ||
                !m_spRedisManager->HSet(serverKey, "port", port) ||
                !m_spRedisManager->HSet(serverKey, "name", name))
            {
                LOG_ERROR("StatusServer: 设置服务器信息失败: %s", name.c_str());
                continue;
            }

            // 3.3 初始化连接数
            if (!m_spChatServerRepo->setConnectionCount(name, 0))
            {
                LOG_ERROR("StatusServer: 初始化服务器连接数失败: %s", name.c_str());
                continue;
            }

            LOG_INFO("StatusServer: 成功注册聊天服务器: %s (%s:%s)",
                name.c_str(), host.c_str(), port.c_str());
            successCount++;
        }

        LOG_INFO("StatusServer: 聊天服务器初始化完成，成功: %d/%zu",
            successCount, chatServers.size());

        return successCount > 0;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("StatusServer: initChatServers 异常: %s", e.what());
        return false;
    }
}