#include "StatusGrpcClient.h"

#include "const.h"
#include "fmt/format.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerRequest;
using message::GetChatServerResponse;
using message::StatusService;
using namespace ServerStatus;

StatusGrpcClient::StatusGrpcClient()
{
}



StatusGrpcClient::~StatusGrpcClient()
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

bool StatusGrpcClient::init(const ConfigManager& config)
{

    auto hostOpt = config.getSafe<std::string>({ "rpcServer","host" });
    auto portOpt = config.getSafe<std::string>({ "rpcServer","port" });

    if (!hostOpt || !portOpt)
    {
        return false;
    }

    std::string grpcHost = *hostOpt;
    std::string grpcPort = *portOpt;

    std::string rpcParam = fmt::format("{}:{}", grpcHost, grpcPort);

    std::shared_ptr<Channel> channel = grpc::CreateChannel(rpcParam, grpc::InsecureChannelCredentials());
    m_stub = StatusService::NewStub(channel);

    m_cq = std::make_unique<grpc::CompletionQueue>();
    m_cqThread = std::thread(&StatusGrpcClient::processCQ, this);
    m_running = true;
    LOG_INFO("VerifyGrpcClient Initialized with CompletionQueue");
    return true;
}

message::GetVerifyResponse StatusGrpcClient::GetChatStatus(int uid)
{
    return message::GetVerifyResponse();
}


void StatusGrpcClient::GetStatusAsync(const std::string& email, GetStatusCallback&& callback)
{
}

void StatusGrpcClient::processCQ() {

}
