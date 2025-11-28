#ifndef UTIL_CONFIGMANAGER_H_
#define UTIL_CONFIGMANAGER_H_

#include "nlohmann/json.hpp"
#include "util_export.h"
class Util_API ConfigManager
{
public:
    ConfigManager();

    ~ConfigManager() = default;

    bool load(const std::string& filename);

    nlohmann::json& operator[](const std::string& key);

    const nlohmann::json& operator[](const std::string& key) const;

    nlohmann::json& at(const std::string& key);

    bool contains(const std::string& key) const;

    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const;

    template<typename T>
    T value(const std::string& key, const T& defaultValude) const;

    bool isLoaded() const;

private:
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;

    nlohmann::json  m_json;
    bool m_loaded = false;
};

template<typename T>
T ConfigManager::get(const std::string& key, const T& defaultValue) const
{
    if (m_json.contains(key))
    {
        try
        {
            return m_json[key].get<T>();
        }
        catch (const nlohmann::json::type_error&)
        {
            return defaultValue;
        }

        return defaultValue;
    }
}

template<typename T>
T ConfigManager::value(const std::string& key, const T& defaultValude) const
{
    return m_json.value(key, defaultValude);
}

#endif //!GATESERVER_CONFIGMANAGER_H_