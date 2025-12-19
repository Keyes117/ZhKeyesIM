#ifndef GATESERVER_CONTROLLER_VERIFYCONTROLLER_H_
#define GATESERVER_CONTROLLER_VERIFYCONTROLLER_H_

#include "BaseController.h"
#include "../service/VerifyService.h"
#include <map>

class VerifyController : public BaseController {
public:
    explicit VerifyController(VerifyService* verifyService);
    ~VerifyController() = default;

    void getVerifyCode(const HttpRequest& request,
                      HttpServer::AsyncDone done,
                      const std::map<std::string, std::string>& params);

private:
    VerifyService* m_verifyService;
};

#endif // GATESERVER_CONTROLLER_VERIFYCONTROLLER_H_