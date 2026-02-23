#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include "StorageManager.h"

class Proj42;

class WebServerManager {
private:
    WebServer server;
    bool isConnected;
    String currentSSID;
    static Proj42* proj42;
    StorageManager storageManager;
    bool autoConnectAttempted;
    
    // NTP настройки
    const char* ntpServer;
    const long gmtOffset_sec;
    const int daylightOffset_sec;
    
    // Обработчики запросов
    void handleRoot();
    void handleConnect();
    void handleStatus();
    void handleClearSettings();  // Новый обработчик для очистки настроек
    
    // Вспомогательные методы
    String getJsonStatus();
    String getCurrentTime();
    void startSoftAP();
    void stopSoftAP();
    bool attemptAutoConnect();   // Новый метод для автоматического подключения
    
public:
    char timeStr[15];
    char dateStr[15];
    char weatherStr[15];

    WebServerManager();
    WebServerManager(Proj42* _proj42);
    void init();
    void handleClient();
    bool getConnectedStatus();
    void checkWiFiConnection();
    bool tryAutoConnect();    
    static void StartWebServerThread(void *_this);
    void WebServerTask();   // Публичный метод для первой попытки подключения
};

#endif