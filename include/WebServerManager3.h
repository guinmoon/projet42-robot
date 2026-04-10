#ifndef WEBSERVERMANAGER3_H
#define WEBSERVERMANAGER3_H

#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include "StorageManager.h"

class Proj42;

class WebServerManager3 {
private:
    // Константы настроек
    static constexpr uint16_t DEFAULT_PORT = 80;
    static constexpr const char* DEFAULT_AP_SSID = "ESP32_Config_V3";
    static constexpr const char* DEFAULT_AP_PASS = "12345678";
    static constexpr const char* DEFAULT_NTP_SERVER = "pool.ntp.org";
    static constexpr long DEFAULT_GMT_OFFSET = 10800; // +3 часа (Москва)
    static constexpr int DEFAULT_DAYLIGHT_OFFSET = 0;
    static constexpr unsigned long WIFI_TIMEOUT_MS = 20000;
    static constexpr unsigned long NTP_UPDATE_INTERVAL_MS = 3600000; // 1 час

    WebServer server;
    bool isConnected;
    String currentSSID;
    Proj42* proj42; 
    StorageManager storageManager;
    bool autoConnectAttempted;
    
    // NTP настройки
    const char* ntpServer = DEFAULT_NTP_SERVER;
    long gmtOffset_sec = DEFAULT_GMT_OFFSET;
    int daylightOffset_sec = DEFAULT_DAYLIGHT_OFFSET;
    
    unsigned long lastRequestTime = 0;
    unsigned long lastNtpUpdateTime = 0;
    
    // Состояние подключения к WiFi (для асинхронности)
    enum class WiFiState { IDLE, CONNECTING, CONNECTED, FAILED };
    WiFiState connectionState = WiFiState::IDLE;
    unsigned long connectionStartTime = 0;

    // Обработчики запросов
    void handleRoot();
    void handleConnect();
    void handleStatus();
    void handleClearSettings();
    
    // Вспомогательные методы
    String getJsonStatus();
    String updateAndGetTime(); // Синхронизация с NTP и RTC (редко)
    void refreshDisplayTime(); // Обновление строки времени для дисплея (часто)
    void startSoftAP();
    void stopSoftAP();
    void processAsyncConnect();
    void checkWiFiTimeout(); // Полное отключение WiFi по таймауту
    
public:
    char weatherStr[15];
    
    WebServerManager3(Proj42* _proj42);
    void init();
    void handleClient();
    bool getConnectedStatus();
    void checkWiFiConnection();
    bool tryAutoConnect();    
    static void StartWebServerThread(void *_this);
    void WebServerTask();
};

#endif