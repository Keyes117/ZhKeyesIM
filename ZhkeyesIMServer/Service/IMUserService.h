#ifndef ZHKEYESIMSERVER_CONTROLLER_IMUSERSERVICE_H_
#define ZHKEYESIMSERVER_CONTROLLER_IMUSERSERVICE_H_

#include <memory>


#include "IMProtocol/IMMessage.h"
#include "IMProtocol/IMMessageSender.h"


#include "Repository/IMUserRepository.h"

class IMUserService
{
public:

    // 通知回调：targetUid + 已构造好的 IMMessage → 路由到本地或远端
    using NotifyUserCallback = std::function<bool(uint32_t targetUid,
        const ZhKeyesIM::Protocol::IMMessage& msg)>;

    IMUserService(std::shared_ptr<IMUserRepository> userRepo,
        std::string serverName,
        std::string serverIp,
        uint16_t    serverPort,
        uint16_t    grpcPort);

    ~IMUserService() = default;

    void auth(uint32_t uid, const std::string& token, uint64_t seqId,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

    void searchUser(uint32_t uid, uint64_t seqId,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

    void applyFriend(uint32_t toUid, uint64_t seqId,
        std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender);

    void setNotifyCallback(NotifyUserCallback callback)
    {
        m_notifyCallback = std::move(callback);
    }

private:
    std::shared_ptr< IMUserRepository> m_spUserRepo;
    std::string m_serverName;
    std::string m_serverIp;
    uint16_t    m_serverPort;
    uint16_t    m_grpcPort;

    NotifyUserCallback m_notifyCallback;
};


#endif

