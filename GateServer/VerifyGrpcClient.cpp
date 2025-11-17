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

VerifyGrpcClient::~VerifyGrpcClient()
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

GetVerifyResponse VerifyGrpcClient::GetVerifyCode(std::string email)
{
    ClientContext context;
    GetVerifyResponse response;
    GetVerifyRequest request;
    request.set_email(email);

    Status status = m_stub->GetVerifyCode(&context, request, &response);

    if(status.ok())
    {
        return response;
    }
    else
    {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }
}

void VerifyGrpcClient::GetVerifyCodeAsync(const std::string& email, GetVerifyCodeCallback&& callback)
{
    //这里在 processCQ 中delete;
    auto call = new AsyncClientCall;

    call->request.set_email(email);

    call->callback = callback;

    //发起异步grpc 调用
    call->rpc = m_stub->AsyncGetVerifyCode(
        &call->context,  //客户端上下文
        call->request,  //请求消息
        m_cq.get()      //完成队列
    );

    //注册完成通知
    call->rpc->Finish(
        &call->response,
        &call->status,
        (void*)call
    );
}

VerifyGrpcClient::VerifyGrpcClient():
    m_running(true)
{
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    m_stub = VerifyService::NewStub(channel);

    
    m_cq = std::make_unique<grpc::CompletionQueue>();
    m_cqThread = std::thread(&VerifyGrpcClient::processCQ, this);
    LOG_INFO("VerifyGrpcClient Initialized with CompletionQueue");
    
}

void VerifyGrpcClient::processCQ()
{
    void* tag;      // 用于接收 tag（AsyncClientCall 指针）
    bool ok;        // 操作是否成功

    // 循环等待完成队列中的事件
    // Next() 会阻塞，直到有事件到达或者 CQ 被 Shutdown
    while (m_cq->Next(&tag, &ok)) {
        // 1. 通过 tag 取回异步调用上下文
        auto* call = static_cast<AsyncClientCall*>(tag);

        // 2. 检查 RPC 状态
        if (!call->status.ok()) {
            // RPC 调用失败
            LOG_ERROR("Async GRPC call failed: code=%d, message=%s",
                call->status.error_code(),
                call->status.error_message().c_str());

            // 设置错误码
            call->response.set_error(ErrorCodes::RPCFailed);
        }
        else {
            // RPC 调用成功
            LOG_DEBUG("Async GRPC call succeeded, error code in response: %d",
                call->response.error());
        }

        // 3. 调用用户提供的回调函数
        //    这个回调在 CompletionQueue 处理线程中执行
        if (call->callback) {
            try {
                call->callback(call->response);
            }
            catch (const std::exception& e) {
                LOG_ERROR("Exception in user callback: %s", e.what());
            }
            catch (...) {
                LOG_ERROR("Unknown exception in user callback");
            }
        }

        // 4. 释放资源（ GetVerifyCodeAsync 中的 new）
        delete call;
    }
}
