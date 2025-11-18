#ifndef GATESERVRE_VERIFYGRPCCLIENT_H_
#define GATESERVRE_VERIFYGRPCCLIENT_H_

#include <memory>
#include <functional>
#include <thread>
#include <atomic>


#include "Logger.h"

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"



enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
};

using GetVerifyCodeCallback = std::function<void(const message::GetVerifyResponse&)>;

class VerifyGrpcClient
{
public:
    static VerifyGrpcClient& getInstance();

	~VerifyGrpcClient();

    message::GetVerifyResponse GetVerifyCode(std::string email);

	void GetVerifyCodeAsync(const std::string& email, GetVerifyCodeCallback&& callback);

private:
    VerifyGrpcClient();

	void processCQ();

	struct AsyncClientCall
	{
		grpc::ClientContext context;
		message::GetVerifyRequest request;
		message::GetVerifyResponse response;
		grpc::Status status;
		GetVerifyCodeCallback callback;
		std::unique_ptr<grpc::ClientAsyncResponseReader<message::GetVerifyResponse>> rpc;
	};

    std::unique_ptr<message::VerifyService::Stub> m_stub;
	std::unique_ptr<grpc::CompletionQueue> m_cq;
	std::thread m_cqThread;
	std::atomic<bool>	m_running;


private:
	VerifyGrpcClient(const VerifyGrpcClient&) = delete;
	VerifyGrpcClient& operator=(const VerifyGrpcClient&) = delete;
	VerifyGrpcClient(VerifyGrpcClient&&) = delete;
	VerifyGrpcClient& operator=(VerifyGrpcClient&&) = delete;
};


#endif