#include "ConfigManager.h"

#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include "Logger.h"

ConfigManager::ConfigManager()
{
}

bool ConfigManager::load(const std::string& filename)
{
    try
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open config file: %s", filename.c_str());
            m_loaded = false;
            return false;
        }

        m_json = nlohmann::json::parse(file);
        m_loaded = true;
        LOG_INFO("Succecssfully loaded config from: %s", filename.c_str());
        return true;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        LOG_ERROR("JSON parse error in file %s: %s (byte position: %zu)",
            filename.c_str(), e.what(), e.byte);
        m_loaded = false;
        return false;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Error reading config file %s: %s", 
            filename.c_str(), e.what());
        m_loaded = false;
        return false;
    }
}

nlohmann::json& ConfigManager::operator[](const std::string& key)
{
    return m_json[key];
}

const nlohmann::json& ConfigManager::operator[](const std::string& key) const
{
    return m_json[key];
}

nlohmann::json& ConfigManager::at(const std::string& key)
{
    return m_json.at(key);
}

bool ConfigManager::contains(const std::string& key) const
{
    return m_json.contains(key);
}

bool ConfigManager::isLoaded() const
{
    return m_loaded;
}
