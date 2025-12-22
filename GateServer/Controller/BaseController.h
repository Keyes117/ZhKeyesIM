#ifndef GATESERVER_CONTROLLER_BASECONTROLLER_H_
#define GATESERVER_CONTROLLER_BASECONTROLLER_H_

#include "Http/HttpServer.h"
#include "Http/HttpRequest.h"
#include "Http/HttpResponse.h"
#include "../const.h"
#include "nlohmann/json.hpp"
#include <vector>

namespace ZhKeyesIMHttp = ZhKeyesIM::Net::Http;
using HttpStatusCode = ZhKeyesIM::Net::Http::HttpStatusCode;
using HttpServer = ZhKeyesIM::Net::Http::HttpServer;
using HttpRequest = ZhKeyesIM::Net::Http::HttpRequest;
using HttpResponse = ZhKeyesIM::Net::Http::HttpResponse;

class BaseController {

public:
    BaseController() = default;
    virtual ~BaseController() = default;

protected:
    // 发送成功响应
    virtual  void sendSuccess(HttpServer::AsyncDone& done,
                HttpStatusCode httpCode,
                ServerStatus::ErrorCodes errorCode,
                const std::string& message = "");
    
    // 发送错误响应
    virtual void sendError(HttpServer::AsyncDone& done,
                  HttpStatusCode httpCode,
                  ServerStatus::ErrorCodes errorCode,
                  const std::string& message = "");
    
    // 验证必需字段
    virtual bool validateRequired(const nlohmann::json& json,
                         const std::vector<std::string>& fields,
                         std::string& errorMsg);
};

#endif // GATESERVER_CONTROLLER_BASECONTROLLER_H_