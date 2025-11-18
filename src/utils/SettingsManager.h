#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <string>
#include <map>

class SettingsManager {
private:
    std::map<std::string, std::string> settings;
    std::string settingsFile;
    
    void loadSettings();
    void saveSettings();
    void setDefaultSettings();

public:
    SettingsManager();
    ~SettingsManager();
    
    void setSetting(const std::string& key, const std::string& value);
    std::string getSetting(const std::string& key, const std::string& defaultValue = "");
    
    bool getBoolSetting(const std::string& key, bool defaultValue = false);
    void setBoolSetting(const std::string& key, bool value);
    
    int getIntSetting(const std::string& key, int defaultValue = 0);
    void setIntSetting(const std::string& key, int value);
};

#endif // SETTINGSMANAGER_H
