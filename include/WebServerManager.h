#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

class Proj42;

class WebServerManager {
private:
    WebServer server;
    bool isConnected;
    String currentSSID;
    static Proj42* proj42;
    // NTP настройки
    const char* ntpServer;
    const long gmtOffset_sec;
    const int daylightOffset_sec;
    
    // Обработчики запросов
    void handleRoot();
    void handleConnect();
    void handleStatus();
    
    // Вспомогательные методы
    String getJsonStatus();
    String getCurrentTime();
    
public:
    WebServerManager();
    WebServerManager(Proj42* _proj42);
    void init();
    void handleClient();
    bool getConnectedStatus();
    void checkWiFiConnection();
    static void StartWebServerThread(void *_this);
    void WebServerTask();
};

#endif