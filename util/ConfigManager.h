#ifndef UTIL_CONFIGMANAGER_H_
#define UTIL_CONFIGMANAGER_H_

#include <optional>
#include <vector>
#include <string>

#include "nlohmann/json.hpp"
#include "util_export.h"

#include "JsonUtil.h"


class Util_API ConfigManager
{
public:
    ConfigManager();

    ~ConfigManager() = default;

    /**
     * @brief 从文件加载配置
     * @param filename 配置文件路径
     * @return 加载成功返回 true，失败返回 false
     */
    bool load(const std::string& filename);

    /**
     * @brief 安全获取配置值（支持嵌套路径和单层键）
     * @param keys 键路径，可以是：
     *   - 单层键：{"key"}
     *   - 嵌套路径：{"GateServer", "threadNum"}
     * @return 成功返回值，失败返回 std::nullopt
     */
    template<typename T>
    std::optional<T> getSafe(std::initializer_list<std::string> keys) const;

    /**
     * @brief 安全获取配置值（支持嵌套路径和单层键）
     * @param keys 键路径 vector
     * @return 成功返回值，失败返回 std::nullopt
     */
    template<typename T>
    std::optional<T> getSafe(const std::vector<std::string>& keys) const;

    /**
     * @brief 检查配置路径是否存在
     * @param keys 键路径
     * @return 路径存在返回 true，否则返回 false
     */
    bool hasPath(const std::vector<std::string>& keys) const;

    /**
     * @brief 检查配置键是否存在
     * @param key 键名
     * @return 键存在返回 true，否则返回 false
     */
    bool hasKey(const std::string& key) const;

    /**
     * @brief 检查配置是否已加载
     * @return 已加载返回 true，否则返回 false
     */
    bool isLoaded() const;

private:
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(const ConfigManager&&) = delete;

    nlohmann::json  m_json;
    bool m_loaded = false;
};

// 模板实现：支持初始化列表
template<typename T>
std::optional<T> ConfigManager::getSafe(std::initializer_list<std::string> keys) const
{
    return getSafe<T>(std::vector<std::string>(keys));
}

// 模板实现：支持 vector
template<typename T>
std::optional<T> ConfigManager::getSafe(const std::vector<std::string>& keys) const
{
    return ZhKeyes::Util::JsonUtil::getSafe<T>(m_json, keys);
}


#endif //!GATESERVER_CONFIGMANAGER_H_