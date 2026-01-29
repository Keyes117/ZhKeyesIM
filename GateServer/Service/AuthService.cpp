#include "AuthService.h"


#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "Logger.h"
#include "openssl/sha.h"
#include "openssl/evp.h"
#include "openssl/buffer.h"


bool AuthService::init(const ZhKeyes::Util::ConfigManager& config)
{
    return false;
}

std::string AuthService::hashPassword(const std::string& password)
{
    // 格式: $salt$hash
      // 生成随机 salt
    std::string salt = generateSalt();

    // 使用 salt + password 计算哈希
    std::string salted = salt + password;
    std::string hash = sha256(salted);

    // 返回格式: $salt$hash
    return "$" + salt + "$" + hash;
}

bool AuthService::verifyPassword(const std::string& password, const std::string& storedHash)
{
    try {
        // 解析存储的哈希: $salt$hash
        if (storedHash.empty() || storedHash[0] != '$') {
            LOG_WARN("AuthService: Invalid hash format");
            return false;
        }

        size_t pos = storedHash.find('$', 1);
        if (pos == std::string::npos) {
            LOG_WARN("AuthService: Invalid hash format - no second $");
            return false;
        }

        // 提取 salt
        std::string salt = storedHash.substr(1, pos - 1);
        std::string expectedHash = storedHash.substr(pos + 1);

        // 使用相同的 salt 计算输入密码的哈希
        std::string salted = salt + password;
        std::string actualHash = sha256(salted);

        // 比较哈希值
        return actualHash == expectedHash;

    }
    catch (const std::exception& e) {
        LOG_ERROR("AuthService: verifyPassword exception: %s", e.what());
        return false;
    }
}

std::string AuthService::generateToken(int uid)
{
    try {
        // 生成过期时间（24小时后）
        time_t now = time(nullptr);
        time_t expireTime = now + (24 * 3600);

        // Token 格式: uid.expireTime.signature
        std::stringstream ss;
        ss << uid << "." << expireTime;
        std::string payload = ss.str();

        // 生成签名: SHA256(payload + secret)
        std::string signature = sha256(payload + m_secretKey);

        // 组合 token
        std::string token = payload + "." + signature;

        // Base64 编码
        return base64Encode(token);

    }
    catch (const std::exception& e) {
        LOG_ERROR("AuthService: generateToken exception: %s", e.what());
        return "";
    }
}

bool AuthService::validateToken(const std::string& token, int& outUid)
{
    try {
        if (token.empty()) {
            return false;
        }

        // Base64 解码
        std::string decoded = base64Decode(token);

        // 解析 token: uid.expireTime.signature
        std::istringstream ss(decoded);
        std::string uidStr, expireStr, signature;

        if (!std::getline(ss, uidStr, '.') ||
            !std::getline(ss, expireStr, '.') ||
            !std::getline(ss, signature, '.')) {
            LOG_WARN("AuthService: Invalid token format");
            return false;
        }

        // 验证过期时间
        time_t expireTime = std::stoll(expireStr);
        time_t now = time(nullptr);

        if (now > expireTime) {
            LOG_WARN("AuthService: Token expired");
            return false;
        }

        // 验证签名
        std::string payload = uidStr + "." + expireStr;
        std::string expectedSignature = sha256(payload + m_secretKey);

        if (signature != expectedSignature) {
            LOG_WARN("AuthService: Invalid token signature");
            return false;
        }

        // 提取 uid
        outUid = std::stoi(uidStr);
        return true;

    }
    catch (const std::exception& e) {
        LOG_ERROR("AuthService: validateToken exception: %s", e.what());
        return false;
    }
}

std::string AuthService::generateSalt()
{
    static const char* charset =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    std::string salt;
    for (int i = 0; i < 16; i++)
    {
        salt += charset[dis(gen)];
    }

    return salt;
}

std::string AuthService::sha256(const std::string& data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(hash[i]);
    }

    return ss.str();
}

std::string AuthService::base64Encode(const std::string& data)
{
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, data.c_str(), static_cast<int>(data.length()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return result;
}

std::string AuthService::base64Decode(const std::string& data)
{
    BIO* bio, * b64;

    int decodeLen = static_cast<int>(data.length());
    char* buffer = new char[decodeLen];

    bio = BIO_new_mem_buf(data.c_str(), -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int length = BIO_read(bio, buffer, decodeLen);
    BIO_free_all(bio);

    std::string result(buffer, length);
    delete[] buffer;

    return result;
}
