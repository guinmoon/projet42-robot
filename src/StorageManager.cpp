#include "StorageManager.h"

StorageManager::StorageManager() {
    preferences.begin(namespaceName, false);
}

StorageManager::~StorageManager() {
    preferences.end();
}

void StorageManager::saveWiFiSettings(const char* ssid, const char* password) {
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putBool("valid", true);
    Serial.println("WiFi settings saved to flash memory");
}

WiFiSettings StorageManager::loadWiFiSettings() {
    WiFiSettings settings;
    
    settings.isValid = preferences.getBool("valid", false);
    
    if (settings.isValid) {
        String storedSSID = preferences.getString("ssid", "");
        String storedPassword = preferences.getString("password", "");
        
        strncpy(settings.ssid, storedSSID.c_str(), sizeof(settings.ssid) - 1);
        settings.ssid[sizeof(settings.ssid) - 1] = '\0';
        
        strncpy(settings.password, storedPassword.c_str(), sizeof(settings.password) - 1);
        settings.password[sizeof(settings.password) - 1] = '\0';
    }
    
    return settings;
}

void StorageManager::clearWiFiSettings() {
    preferences.remove("ssid");
    preferences.remove("password");
    preferences.remove("valid");
    Serial.println("WiFi settings cleared from flash memory");
}

bool StorageManager::hasSavedSettings() {
    return preferences.getBool("valid", false);
}