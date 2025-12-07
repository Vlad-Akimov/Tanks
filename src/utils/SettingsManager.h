/**
 * @file SettingsManager.h
 * @author Vld251
 * @brief Manages game configuration settings and user preferences.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>
#include <map>

/**
 * @brief Class for managing game settings and configuration.
 * 
 * Handles loading, saving, and accessing game settings from a configuration file.
 * Supports different data types including strings, booleans, and integers.
 */
class SettingsManager {
private:
    std::map<std::string, std::string> settings;  ///< Map of setting key-value pairs
    std::string settingsFile;                     ///< Path to settings configuration file
    
    /**
     * @brief Loads settings from configuration file.
     * @returns None
     */
    void loadSettings();
    
    /**
     * @brief Saves current settings to configuration file.
     * @returns None
     */
    void saveSettings();
    
    /**
     * @brief Sets default settings if configuration file doesn't exist.
     * @returns None
     */
    void setDefaultSettings();

public:
    /**
     * @brief Constructs a SettingsManager object.
     * @returns None
     */
    SettingsManager();
    
    /**
     * @brief Destructor that saves settings on destruction.
     */
    ~SettingsManager();
    
    /**
     * @brief Sets a string setting value.
     * @param key Setting key/name.
     * @param value Setting value as string.
     * @returns None
     */
    void setSetting(const std::string& key, const std::string& value);
    
    /**
     * @brief Gets a string setting value.
     * @param key Setting key/name.
     * @param defaultValue Default value if setting doesn't exist.
     * @return Setting value as string.
     */
    std::string getSetting(const std::string& key, const std::string& defaultValue = "");
    
    /**
     * @brief Gets a boolean setting value.
     * @param key Setting key/name.
     * @param defaultValue Default value if setting doesn't exist.
     * @return Setting value as boolean.
     */
    bool getBoolSetting(const std::string& key, bool defaultValue = false);
    
    /**
     * @brief Sets a boolean setting value.
     * @param key Setting key/name.
     * @param value Setting value as boolean.
     * @returns None
     */
    void setBoolSetting(const std::string& key, bool value);
    
    /**
     * @brief Gets an integer setting value.
     * @param key Setting key/name.
     * @param defaultValue Default value if setting doesn't exist.
     * @return Setting value as integer.
     */
    int getIntSetting(const std::string& key, int defaultValue = 0);
    
    /**
     * @brief Sets an integer setting value.
     * @param key Setting key/name.
     * @param value Setting value as integer.
     * @returns None
     */
    void setIntSetting(const std::string& key, int value);
};

#endif // SETTINGSMANAGER_H
