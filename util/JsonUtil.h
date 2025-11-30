#ifndef UTIL_JSONUTIL_H_
#define UTIL_JSONUTIL_H_

#include <optional>
#include <string>
#include <istream>
#include <vector>

#include "nlohmann/json.hpp"

#include "util_export.h"
#include "Logger.h"


namespace ZhKeyes {
namespace Util {

class Util_API JsonUtil
{
public:
    /**
     * @brief 安全解析 Json 字符串 (不抛异常)
     * @param jsonStr（Json字符串）
     * @return  解析成功返回 json 对象， 失败返回 std::nullopt
     */
    static std::optional<nlohmann::json> parseSafe(const std::string& jsonStr);

    /**
      * @brief 安全解析 JSON 流（不抛异常）
      * @param is 输入流
      * @return 解析成功返回 json 对象，失败返回 std::nullopt
      */
    static std::optional<nlohmann::json> parseSafe(std::istream& is);

    /**
  * @brief 安全获取 JSON 值（不抛异常）
  * @param json JSON 对象
  * @param key 键名
  * @return 成功返回值，失败返回 std::nullopt
  */
    template<typename T>
    static std::optional<T> getSafe(const nlohmann::json& json, const std::string& key);

    /**
     * @brief 安全获取嵌套 JSON 值（不抛异常）
     * @param json JSON 对象
     * @param keys 键路径，如 {"user", "profile", "name"}
     * @return 成功返回值，失败返回 std::nullopt
     */
    template<typename T>
    static std::optional<T> getSafe(const nlohmann::json& json,
        const std::vector<std::string>& keys);

    /**
     * @brief 安全序列化 JSON（不抛异常）
     * @param json JSON 对象
     * @param indent 缩进空格数，-1 表示不格式化
     * @return 成功返回 JSON 字符串，失败返回 std::nullopt
     */
    static std::optional<std::string> dumpSafe(const nlohmann::json& json, int indent = -1);

    /**
     * @brief 检查 JSON 是否包含指定路径
     * @param json JSON 对象
     * @param keys 键路径
     * @return 路径存在返回 true，否则返回 false
     */
    static bool hasPath(const nlohmann::json& json, const std::vector<std::string>& keys);

    /**
     * @brief 检查 JSON 是否包含指定键
     * @param json JSON 对象
     * @param key 键名
     * @return 键存在返回 true，否则返回 false
     */
    static bool hasKey(const nlohmann::json& json, const std::string& key);

    /**
     * @brief 安全访问嵌套 JSON（链式访问的替代方案）
     * @param json JSON 对象
     * @param keys 键路径
     * @return 成功返回 JSON 对象，失败返回 std::nullopt
     */
    static std::optional<nlohmann::json> accessPath(const nlohmann::json& json,
        const std::vector<std::string>& keys);


private:
    JsonUtil() = delete;
    JsonUtil(const JsonUtil&) = delete;
    JsonUtil& operator=(const JsonUtil&) = delete;
};

// 模板实现 - 添加异常处理和日志
template<typename T>
std::optional<T> JsonUtil::getSafe(const nlohmann::json& json, const std::string& key)
{
    try
    {
        if (json.contains(key))
        {
            return json[key].get<T>();
        }
        // 键不存在时记录日志
        LOG_DEBUG("JSON key '%s' not found", key.c_str());
        return std::nullopt;
    }
    catch (const nlohmann::json::type_error& e)
    {
        // 类型转换失败，记录日志
        LOG_ERROR("JSON type error for key '%s': %s", key.c_str(), e.what());
        return std::nullopt;
    }
    catch (const nlohmann::json::exception& e)
    {
        // 其他 JSON 异常，记录日志
        LOG_ERROR("JSON exception for key '%s': %s", key.c_str(), e.what());
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> JsonUtil::getSafe(const nlohmann::json& json,
    const std::vector<std::string>& keys)
{
    try
    {
        const nlohmann::json* current = &json;
        for (size_t i = 0; i < keys.size(); ++i)
        {
            const auto& key = keys[i];
            if (!current->contains(key))
            {
                // 构建路径字符串用于日志
                std::string path;
                for (size_t j = 0; j <= i; ++j)
                {
                    if (j > 0) path += ".";
                    path += keys[j];
                }
                LOG_DEBUG("JSON path '%s' not found", path.c_str());
                return std::nullopt;
            }
            current = &(*current)[key];
        }
        return current->get<T>();
    }
    catch (const nlohmann::json::type_error& e)
    {
        // 构建路径字符串用于日志
        std::string path;
        for (size_t i = 0; i < keys.size(); ++i)
        {
            if (i > 0) path += ".";
            path += keys[i];
        }
        LOG_ERROR("JSON type error for path '%s': %s", path.c_str(), e.what());
        return std::nullopt;
    }
    catch (const nlohmann::json::exception& e)
    {
        // 构建路径字符串用于日志
        std::string path;
        for (size_t i = 0; i < keys.size(); ++i)
        {
            if (i > 0) path += ".";
            path += keys[i];
        }
        LOG_ERROR("JSON exception for path '%s': %s", path.c_str(), e.what());
        return std::nullopt;
    }
}

} //namespace ZhKeyes
} // namespace Util



#endif