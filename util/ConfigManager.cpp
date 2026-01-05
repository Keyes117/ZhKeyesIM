#include "ConfigManager.h"

#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include "Logger.h"

namespace ZhKeyes{
    namespace Util{


ConfigManager::ConfigManager()
{
}

bool ConfigManager::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        LOG_ERROR("Failed to open config file: %s", filename.c_str());
        m_loaded = false;
        return false;
    }

    // ʹ�� JsonUtil �����ļ�
    auto jsonOpt = ZhKeyes::Util::JsonUtil::parseSafe(file);
    if (jsonOpt)
    {
        m_json = std::move(*jsonOpt);
        m_loaded = true;
        LOG_INFO("Successfully loaded config from: %s", filename.c_str());
        return true;
    }
    else
    {
        m_loaded = false;
        return false;
    }
}


bool ConfigManager::hasPath(const std::vector<std::string>& keys) const
{
    return ZhKeyes::Util::JsonUtil::hasPath(m_json, keys);
}

bool ConfigManager::hasKey(const std::string& key) const
{
    return ZhKeyes::Util::JsonUtil::hasKey(m_json, key);
}

bool ConfigManager::isLoaded() const
{
    return m_loaded;
}

}
}
