#ifndef GATESERVER_SERVICE_AUTHSERVICE_H_
#define GATESERVER_SERVICE_AUTHSERVICE_H_

#include <string>
#include <ConfigManager.h>
#include "repository/RedisRepository.h"

class AuthService {
public:
    AuthService() = default;
    ~AuthService() = default;

    bool init(const ConfigManager& config);

    // 密码相关
    
    /**
     * @brief 生成哈希
     * @param password 
     * @return 哈希值
     */
    std::string hashPassword(const std::string& password);

    /**
     * @brief  验证密码和哈希值
     * @param password 客户端传来密码
     * @param storedHash 数据库中存储的hash值
     * @return true:验证结果一致 false: 不一致
     */
    bool verifyPassword(const std::string& password, const std::string& storedHash);
    
    // Token相关
    std::string generateToken(int uid);
    bool validateToken(const std::string& token, int& outUid);

private:
    std::string m_secretKey;
    //std::vector<std::string> m_validKeys;

    std::string generateSalt();
    std::string sha256(const std::string& data);
    std::string base64Encode(const std::string& data);
    std::string base64Decode(const std::string& data);
};


/**
 注册时:
  password = "123456"
  salt = "randomABC123"          # 随机生成
  hash = SHA256(salt + password)  # 组合后哈希
  存储: "$randomABC123$7f83b16...48af"

登录时:
  input = "123456"
  从存储中提取 salt = "randomABC123"
  计算 hash = SHA256(salt + input)
  比较计算出的 hash 和存储的 hash

传统 Session 方式:
客户端 → 登录成功 → 服务器生成 SessionID → 存储在服务器内存/Redis
客户端每次请求带上 SessionID → 服务器查 Redis 验证

 问题:
- 需要服务器存储状态（不适合分布式）
- 需要频繁查询 Redis
- 水平扩展困难

Token (JWT) 方式:
客户端 → 登录成功 → 服务器生成 Token（包含用户信息）
客户端每次请求带上 Token → 服务器验证签名（无需查数据库）

 优势:
- 无状态 (Stateless)
- 不需要服务器存储
- 易于水平扩展

Token 格式 uid.expireTime.signature


签名原理:生成时: signature = SHA256(payload + secretKey)验证时: 重新计算签名，比较是否一致
 */


/*
*
==注册流程

1. 用户注册
   ↓
2. AuthService::hashPassword("mypassword")
   - 生成 salt: "aB3dF9xK2pQw7Rt5"
   - 计算 hash: SHA256(salt + password)
   - 返回: "$aB3dF9xK2pQw7Rt5$7f83b16..."
   ↓
3. UserRepository::create(username, email, hash)
   - 存储到数据库

==登录流程

 1. 用户登录: username="zhangsan", password="mypassword"
   ↓
2. UserRepository::findByUsername("zhangsan")
   - 查询数据库，返回 UserInfo
   - userInfo.passwordHash = "$aB3dF9xK2pQw7Rt5$7f83b16..."
   ↓
3. AuthService::verifyPassword("mypassword", "$aB3dF9xK2pQw7Rt5$7f83b16...")
   - 提取 salt: "aB3dF9xK2pQw7Rt5"
   - 计算: SHA256(salt + "mypassword")
   - 比较哈希值
   - 返回: true ✅
   ↓
4. AuthService::generateToken(12345)
   - 构建: "12345.1703388745"
   - 签名: SHA256("12345.1703388745" + secretKey)
   - 返回: "MTIzNDUuMTcwMzM4ODc0NS44YTNkZmY5..."
   ↓
5. 返回给客户端: { "token": "MTIzNDU...", "uid": 12345, ... }


==后续请求流程

1. 客户端请求: Header: "Authorization: Bearer MTIzNDU..."
   ↓
2. AuthService::validateToken("MTIzNDU...", uid)
   - Base64 解码
   - 检查是否过期
   - 验证签名
   - 提取 uid
   - 返回: true, uid=12345
   ↓
3. 通过 uid 获取用户信息，处理业务逻辑
* 
*/

#endif // GATESERVER_SERVICE_AUTHSERVICE_H_