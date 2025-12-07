/**
 * @file SettingsManager.cpp
 * @author Vld251
 * @brief Implementation of settings management with file persistence.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "SettingsManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "PlatformUtils.h"

SettingsManager::SettingsManager() : settingsFile("tank_game_settings.cfg") {
    loadSettings();
}

SettingsManager::~SettingsManager() {
    saveSettings();
}

void SettingsManager::loadSettings() {
    std::ifstream file(settingsFile);
    if (!file.is_open()) {
        setDefaultSettings();
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            settings[key] = value;
        }
    }
    file.close();
}

void SettingsManager::saveSettings() {
    std::ofstream file(settingsFile);
    if (!file.is_open()) {
        std::cerr << "Ошибка сохранения настроек!" << std::endl;
        return;
    }
    
    for (const auto& setting : settings) {
        file << setting.first << "=" << setting.second << std::endl;
    }
    file.close();
}

void SettingsManager::setDefaultSettings() {
    // Проверяем поддержку Unicode по умолчанию
    bool defaultUnicode = PlatformUtils::checkUnicodeSupport();
    settings["advanced_graphics"] = defaultUnicode ? "1" : "0";
    settings["sound_enabled"] = "1";
    settings["difficulty"] = "1";
}

void SettingsManager::setSetting(const std::string& key, const std::string& value) {
    settings[key] = value;
}

std::string SettingsManager::getSetting(const std::string& key, const std::string& defaultValue) {
    auto it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    return defaultValue;
}

bool SettingsManager::getBoolSetting(const std::string& key, bool defaultValue) {
    std::string value = getSetting(key, defaultValue ? "1" : "0");
    return (value == "1" || value == "true" || value == "yes");
}

void SettingsManager::setBoolSetting(const std::string& key, bool value) {
    settings[key] = value ? "1" : "0";
}

int SettingsManager::getIntSetting(const std::string& key, int defaultValue) {
    std::string value = getSetting(key, std::to_string(defaultValue));
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

void SettingsManager::setIntSetting(const std::string& key, int value) {
    settings[key] = std::to_string(value);
}
