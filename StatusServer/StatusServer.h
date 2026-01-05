#ifndef STATUSSERVER_STATUSSERVER_H_
#define STATUSSERVER_STATUSSERVER_H_
#include <future>

#include <cstdint>
#include <memory>

#include <grpcpp/grpcpp.h>

#include "ConfigManager.h"
#include "WorkThreadPool.h"

#include "Service/StatusService.h"
#include "Controller/StatusController.h"
#include "Infrastructure/RedisManager.h"
#include "JsonUtil.h"



class StatusServer
{
public:
    StatusServer();
    ~StatusServer();

    bool init(ZhKeyes::Util::ConfigManager& config);
    void start();
    void shutdown();

private:
    bool initChatServers(ZhKeyes::Util::ConfigManager& config);

protected:

    std::unique_ptr<grpc::Server> m_spGrpcServer;

    std::shared_ptr<RedisManager>   m_spRedisManager;

    std::shared_ptr<StatusService> m_spStatusService;

    std::shared_ptr<StatusController> m_spStatusController;

    std::shared_ptr<TokenRepository> m_spTokenRepo;

    std::shared_ptr<ChatServerRepository> m_spChatServerRepo;

private:
    StatusServer(const StatusServer&) = delete;
    StatusServer(StatusServer&&) noexcept = delete;

    StatusServer& operator=(const StatusServer&) = delete;
    StatusServer& operator=(StatusServer&&) = delete;
};

#endif
