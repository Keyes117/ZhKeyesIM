#include "JsonUtil.h"
#include "Logger.h"
#include <fstream>

namespace ZhKeyes {
namespace Util {

std::optional<nlohmann::json> JsonUtil::parseSafe(const std::string& jsonStr)
{
    try
    {
        nlohmann::json json = nlohmann::json::parse(jsonStr);
        return json;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        LOG_ERROR("JSON parse error: %s (byte position: %zu)", e.what(), e.byte);
        return std::nullopt;
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_ERROR("JSON exception: %s", e.what());
        return std::nullopt;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in JSON parse: %s", e.what());
        return std::nullopt;
    }
}

std::optional<nlohmann::json> JsonUtil::parseSafe(std::istream& is)
{
    try
    {
        nlohmann::json json = nlohmann::json::parse(is);
        return json;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        LOG_ERROR("JSON parse error from stream: %s (byte position: %zu)", e.what(), e.byte);
        return std::nullopt;
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_ERROR("JSON exception from stream: %s", e.what());
        return std::nullopt;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in JSON parse from stream: %s", e.what());
        return std::nullopt;
    }
}

std::optional<std::string> JsonUtil::dumpSafe(const nlohmann::json& json, int indent)
{
    try
    {
        std::string result;
        if (indent >= 0)
        {
            result = json.dump(indent);
        }
        else
        {
            result = json.dump();
        }
        return result;
    }
    catch (const nlohmann::json::exception& e)
    {
        LOG_ERROR("JSON dump error: %s", e.what());
        return std::nullopt;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in JSON dump: %s", e.what());
        return std::nullopt;
    }
}

bool JsonUtil::hasPath(const nlohmann::json& json, const std::vector<std::string>& keys)
{
    const nlohmann::json* current = &json;
    for (const auto& key : keys)
    {
        if (!current->contains(key))
        {
            return false;
        }
        current = &(*current)[key];
    }
    return true;
}

bool JsonUtil::hasKey(const nlohmann::json& json, const std::string& key)
{
    return json.contains(key);
}

std::optional<nlohmann::json> JsonUtil::accessPath(const nlohmann::json& json,
    const std::vector<std::string>& keys)
{
    try
    {
        const nlohmann::json* current = &json;
        for (const auto& key : keys)
        {
            if (!current->contains(key))
            {
                return std::nullopt;
            }
            current = &(*current)[key];
        }
        return std::make_optional(*current);
    }
    catch (const nlohmann::json::exception&)
    {
        return std::nullopt;
    }
}

    } // namespace Util
} // namespace ZhKeyes