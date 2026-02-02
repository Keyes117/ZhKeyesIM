// RegisterTask.cpp
#include "RegisterTask.h"
#include <QMetaObject>
#include "Logger.h"

RegisterTask::RegisterTask(
    std::shared_ptr<IMClient> client,
    uint64_t taskId,
    std::string username,
    std::string email,
    std::string password,
    std::string code)
    :  Task(taskId,Task::TaskType::TASK_TYPE_REGISTER),
    m_client(client),
    m_username(std::move(username)),
    m_email(std::move(email)),
    m_password(std::move(password)),
    m_code(std::move(code))
{
}

void RegisterTask::doTask() {
    LOG_INFO("RegisterTask: Executing register for user: %s", m_username.c_str());

    nlohmann::json requestJson;
    requestJson["username"] = m_username;
    requestJson["password"] = m_password;
    requestJson["email"] =  m_email;
    requestJson["code"] = m_code;

    auto selfTask = std::static_pointer_cast<RegisterTask>(shared_from_this());

    // 在网络线程调用IMClient
    m_client->requestRegister(
        std::bind(&RegisterTask::onSuccess, selfTask, std::placeholders::_1),
        std::bind(&RegisterTask::onError, selfTask, std::placeholders::_1),
        m_username, m_email, m_password, m_code     
    );
}

void RegisterTask::onSuccess(int uid) {
    LOG_INFO("RegisterTask: Register succeeded, uid=%d", uid);
 
}

void RegisterTask::onError(const std::string& error) {
    LOG_ERROR("RegisterTask: Register failed: %s", error.c_str());
}


