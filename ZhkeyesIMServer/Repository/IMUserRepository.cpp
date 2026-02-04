#include "IMUserRepository.h"

#include "util/JsonUtil.h"

IMUserRepository::IMUserRepository(std::shared_ptr<RedisManager> spRedis
    , std::shared_ptr<MySqlManager> spMysql) :
    m_spRedis(spRedis),
    m_spMysql(spMysql)
{
}

std::optional<std::string> IMUserRepository::getToken(int32_t uid)
{
    if (!m_spRedis)
        return std::nullopt;

    std::string strUidKey = token_prefix + std::to_string(uid);
    std::string token;
    if (!m_spRedis->Get(strUidKey, token))
        return std::nullopt;

    return token;
}

std::optional<UserInfo> IMUserRepository::getUserInfo(int32_t uid)
{
    std::string infoKey = userInfo_prefix + std::to_string(uid);
    std::string infoString;
    if (m_spRedis->Get(infoKey, infoString))
    {
        auto infoJsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(infoString);

        if (!infoJsonOpt)
            return std::nullopt;

        auto infoJson = *infoJsonOpt;

        UserInfo userinfo;
        //userinfo.uid = ZhKeyes::Util::JsonUtil::getSafe<uint32_t>(infoJson, "uid");
    }
    return std::optional<UserInfo>();
}
