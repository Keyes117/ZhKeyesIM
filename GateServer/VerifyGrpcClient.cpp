#include "VerifyGrpcClient.h"

#include "const.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyRequest;
using message::GetVerifyResponse;
using message::VerifyService;

VerifyGrpcClient& VerifyGrpcClient::getInstance()
{
    static VerifyGrpcClient verifyGrpcClient;
    return verifyGrpcClient;
}

GetVerifyResponse VerifyGrpcClient::GetVerifyCode(std::string email)
{
    ClientContext context;
    GetVerifyResponse response;
    GetVerifyRequest request;
    request.set_email(email);

    Status status = m_stub->GetVerifyCode(&context, request, &response);

    if (status.ok())
    {
        return response;
    }
    else
    {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }
}

VerifyGrpcClient::VerifyGrpcClient()
{
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    m_stub = VerifyService::NewStub(channel);
}