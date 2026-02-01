#ifndef GATESERVER_CONTROLLER_VERIFYCONTROLLER_H_
#define GATESERVER_CONTROLLER_VERIFYCONTROLLER_H_

#include <map>
#include <memory>

#include "controller/BaseController.h"
#include "service/VerifyService.h"
#include "model/ServiceResult.h"

class VerifyController : public BaseController {
public:
    explicit VerifyController(std::shared_ptr<VerifyService> verifyService);
    ~VerifyController() = default;

    void handleGetVerifyCode(const HttpRequest& request,
                      HttpServer::AsyncDone done,
        const std::map<std::string, std::string>& params = {});

private:
    void onHandleGetVerifyCodeDone(HttpServer::AsyncDone done,
        const VerifyCodeResult& result);

private:
    std::shared_ptr<VerifyService> m_spVerifyService;
};

#endif // GATESERVER_CONTROLLER_VERIFYCONTROLLER_H_