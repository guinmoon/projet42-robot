#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <Preferences.h>
#include <Arduino.h>

struct WiFiSettings {
    char ssid[64];
    char password[64];
    bool isValid;
};

class StorageManager {
private:
    Preferences preferences;
    const char* namespaceName = "wifi_config";

public:
    StorageManager();
    ~StorageManager();
    
    void saveWiFiSettings(const char* ssid, const char* password);
    WiFiSettings loadWiFiSettings();
    void clearWiFiSettings();
    bool hasSavedSettings();
};

#endif