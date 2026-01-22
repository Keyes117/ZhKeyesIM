// UserSession.cpp
#include "UserSession.h"
#include "Logger.h"

UserSession& UserSession::getInstance()
{
    static UserSession instance;
    return instance;
}

void UserSession::setUser(const User& user)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentUser = user;
    LOG_INFO("UserSession: 用户登录成功, uid=%lld, username=%s",
        user.uid, user.username.c_str());
}

User UserSession::getUser() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentUser;
}

std::string UserSession::getToken() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentUser.token;
}

int64_t UserSession::getUid() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentUser.uid;
}

std::string UserSession::getUsername() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentUser.username;
}

std::string UserSession::getEmail() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentUser.email;
}

bool UserSession::isLoggedIn() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentUser.uid != 0 && !m_currentUser.token.empty();
}

void UserSession::logout()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG_INFO("UserSession: 用户登出, uid=%lld", m_currentUser.uid);
    m_currentUser = User();  // 重置为空用户
}

void UserSession::updateToken(const std::string& newToken)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentUser.token = newToken;
    LOG_INFO("UserSession: Token已更新");
}

std::vector<std::shared_ptr<FriendInfo>> UserSession::GetContactUserListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    int begin = m_contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= m_friend_list.size()) {
        return friend_list;
    }

    if (end > m_friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(m_friend_list.begin() + begin, m_friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<FriendInfo>>(m_friend_list.begin() + begin, m_friend_list.begin() + end);
    return friend_list;
}

void UserSession::UpdateContactLoadedCount()
{
    int begin = m_contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= m_friend_list.size()) {
        return;
    }

    if (end > m_friend_list.size()) {
        m_contact_loaded = m_friend_list.size();
        return;
    }

    m_contact_loaded = end;
}