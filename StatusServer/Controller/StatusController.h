#ifndef STATUSSERVER_STATUSCONTROLLER_H_
#define STATUSSERVER_STATUSCONTROLLER_H_


#include <memory>


#include "Service/StatusService.h"

#include "message.grpc.pb.h"


class StatusController : public message::StatusService::Service
{
public:
    StatusController(std::shared_ptr<StatusService> statusService);
    ~StatusController() = default;


    grpc::Status GetChatServer(grpc::ServerContext* context,
        const message::GetChatServerRequest* request,
        message::GetChatServerResponse* response) override;

private:
    std::shared_ptr<StatusService> m_spStatusService;
};

#endif