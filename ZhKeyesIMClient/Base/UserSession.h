#ifndef ZHKEYESIMCLIENT_BASE_USERSESSION_H_
#define ZHKEYESIMCLIENT_BASE_USERSESSION_H_

#include <string>
#include <memory>
#include <mutex>
#include "Base/global.h"
#include "Base/UserData.h"

/**
 * @brief 用户会话管理类（单例模式）
 * 用于管理登录后的用户信息和Token
 */
class UserSession
{
public:
    static UserSession& getInstance();

    // 禁止拷贝和赋值
    UserSession(const UserSession&) = delete;
    UserSession& operator=(const UserSession&) = delete;

    /**
     * @brief 设置登录信息
     * @param user 用户信息（包含token）
     */
    void setUser(const User& user);

    /**
     * @brief 获取当前用户信息
     * @return 用户信息，如果未登录则返回空用户（uid=0）
     */
    User getUser() const;

    /**
     * @brief 获取Token
     * @return Token字符串，如果未登录则返回空字符串
     */
    std::string getToken() const;

    /**
     * @brief 获取用户ID
     * @return 用户ID，如果未登录则返回0
     */
    int64_t getUid() const;

    /**
     * @brief 获取用户名
     * @return 用户名，如果未登录则返回空字符串
     */
    std::string getUsername() const;

    /**
     * @brief 获取邮箱
     * @return 邮箱，如果未登录则返回空字符串
     */
    std::string getEmail() const;

    /**
     * @brief 检查是否已登录
     * @return true表示已登录，false表示未登录
     */
    bool isLoggedIn() const;

    /**
     * @brief 登出，清空用户信息
     */
    void logout();

    /**
     * @brief 更新Token（用于Token刷新场景）
     * @param newToken 新的Token
     */
    void updateToken(const std::string& newToken);

    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList()
    {
        return m_applyList;
    }

    std::vector<std::shared_ptr<FriendInfo>> GetContactUserListPerPage();

    void UpdateContactLoadedCount();

private:
    UserSession() = default;
    ~UserSession() = default;



    std::shared_ptr<UserInfo> m_user_info;
    std::vector<std::shared_ptr<ApplyInfo>> m_applyList;
    std::vector<std::shared_ptr<FriendInfo>> m_friend_list;
    QMap<int, std::shared_ptr<FriendInfo>> m_friend_map;
    QString m_token;
    int m_chat_loaded;
    int m_contact_loaded;

    mutable std::mutex m_mutex;  // 线程安全
    User m_currentUser;          // 当前用户信息
};

#endif