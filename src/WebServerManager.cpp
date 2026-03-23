#include "WebServerManager.h"
#include "wifi_config.h"
#include "time.h"
#include "proj42.hpp"

Proj42 *WebServerManager::proj42;

WebServerManager::WebServerManager(Proj42* _proj42) : 
                                    server(80),
                                    isConnected(false),
                                    ntpServer("pool.ntp.org"),
                                    gmtOffset_sec(10800),  // +3 часа (Москва)
                                    daylightOffset_sec(0),
                                    autoConnectAttempted(false){    
    proj42 = _proj42;
    // sprintf(timeStr, "00:00");
    init();       
}

void WebServerManager::StartWebServerThread(void *_this)
{
    ((WebServerManager *)_this)->WebServerTask();
    vTaskDelete(NULL);
}

void WebServerManager::WebServerTask()
{
    int i = 0;
    lastRequestTime = millis(); // Инициализация времени последнего запроса
    while (true){
        handleClient();
        // checkWiFiConnection(); 
        checkWiFiTimeout(); // Проверка таймаута WiFi
        delay(200);
        i++;
        // if (i>=5)
        // {            
        //     getCurrentTime();
        //     i=0;
        // }
    }
}


WebServerManager::WebServerManager() : 
    server(80),
    isConnected(false),
    ntpServer("pool.ntp.org"),
    gmtOffset_sec(10800),  // +3 часа (Москва)
    daylightOffset_sec(0),
    autoConnectAttempted(false) {
}

void WebServerManager::init() {
    // Установка режима AP + STA для одновременной работы
    
    WiFi.mode(WIFI_MODE_APSTA);
    
    // Запуск точки доступа
    startSoftAP();
    
    // Настройка маршрутов
    server.on("/", [this]() { this->handleRoot(); });
    server.on("/connect", [this]() { this->handleConnect(); });
    server.on("/status", [this]() { this->handleStatus(); });
    server.on("/clear", [this]() { this->handleClearSettings(); });  // Новый маршрут
    
    server.begin();
    
    // Инициализация NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    Serial.println("Web server initialized");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    xTaskCreatePinnedToCore(
            this->StartWebServerThread, /* Task function. */
            "Task15",           /* name of task. */
            4096,              /* Stack size of task */
            this,               /* parameter of the task */
            tskIDLE_PRIORITY,   /* priority of the task */
            NULL,               /* Task handle to keep track of created task */
            1);
    // Попытка автоподключения
    
    Serial.println("Попытка автоподключения...");
    if (tryAutoConnect()) {
        Serial.println("Автоподключение успешно выполнено");
    } else {
        Serial.println("Автоподключение не удалось или нет сохраненных настроек");
    }
    digitalWrite(BUILTIN_LED, HIGH);

    getCurrentTime();
    lastRequestTime = millis();
}

void WebServerManager::startSoftAP() {
    if (!WiFi.softAP("ESP32_Config", "12345678")) {
        Serial.println("Failed to start SoftAP!");
    } else {
        Serial.print("SoftAP started. IP: ");
        Serial.println(WiFi.softAPIP());
    }
}

void WebServerManager::stopSoftAP() {
    WiFi.softAPdisconnect(true);
    Serial.println("SoftAP stopped.");
}

void WebServerManager::handleRoot() {
    Serial.println("Handling root request");
    server.send(200, "text/html", loginPage);
    lastRequestTime = millis(); // Обновляем время последнего запроса
}

void WebServerManager::handleConnect() {
    if (server.method() == HTTP_POST) {
        String newSSID = server.arg("ssid");
        String newPass = server.arg("password");
        
        if (newSSID.length() > 0) {
            Serial.println("Попытка подключения к: " + newSSID);
            
            WiFi.begin(newSSID.c_str(), newPass.c_str());
            
            // Ждем подключения (максимум 10 секунд)
            unsigned long startTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
                delay(500);
                Serial.print(".");
            }
            
            if (WiFi.status() == WL_CONNECTED) {
                isConnected = true;
                currentSSID = newSSID;
                
                // Сохраняем настройки
                storageManager.saveWiFiSettings(newSSID.c_str(), newPass.c_str());
                
                Serial.println("Успешно подключено к WiFi. IP: " + WiFi.localIP().toString());
                server.send(200, "text/html", 
                    "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Успех</title></head>"
                    "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                    "<h2 style='color:green;'>Успешно подключено!</h2>"
                    "<p>Подключено к: " + newSSID + "</p>"
                    "<p>IP адрес: " + WiFi.localIP().toString() + "</p>"
                    "<p><a href='/clear'>Очистить сохраненные настройки</a></p>"
                    "<a href='/'>Назад</a>"
                    "</body></html>");
            } else {
                isConnected = false;
                WiFi.disconnect();
                Serial.println("Не удалось подключиться к WiFi");
                server.send(200, "text/html", 
                    "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Ошибка</title></head>"
                    "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                    "<h2 style='color:red;'>Ошибка подключения!</h2>"
                    "<p>Не удалось подключиться к сети: " + newSSID + "</p>"
                    "<a href='/'>Попробовать снова</a>"
                    "</body></html>");
            }
        } else {
            server.send(400, "text/plain", "SSID не может быть пустым");
        }
        
    }
    lastRequestTime = millis(); // Обновляем время последнего запроса
}

void WebServerManager::handleStatus() {
    Serial.println("Handling status request");
    String jsonResponse = getJsonStatus();
    server.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleClearSettings() {
    storageManager.clearWiFiSettings();
    isConnected = false;
    WiFi.disconnect();
    startSoftAP();  // Убедимся, что AP работает
    
    server.send(200, "text/html", 
        "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Настройки очищены</title></head>"
        "<body style='font-family:Arial;text-align:center;padding:50px;'>"
        "<h2 style='color:green;'>Настройки очищены!</h2>"
        "<p>Сохраненные настройки WiFi удалены.</p>"
        "<a href='/'>Назад</a>"
        "</body></html>");
}

String WebServerManager::getJsonStatus() {
    String jsonResponse;
    if (isConnected && WiFi.status() == WL_CONNECTED) {
        String currentTime = getCurrentTime();
        jsonResponse = "{\"connected\":true,\"ssid\":\"" + currentSSID + "\",\"ip\":\"" + 
                     WiFi.localIP().toString() + "\",\"time\":\"" + currentTime + "\"}";
    } else {
        jsonResponse = "{\"connected\":false,\"ssid\":\"\",\"ip\":\"\",\"time\":\"\"}";
        isConnected = false;
    }
    return jsonResponse;
}

String WebServerManager::getCurrentTime() {
    struct tm timeinfo;
    // Serial.println("get Time -> ");
    if(getLocalTime(&timeinfo)){
        char timeStringBuff[50];
        strftime(timeStringBuff, sizeof(timeStringBuff), "%d.%m.%Y %H:%M:%S", &timeinfo);
        // strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
        // strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &timeinfo);
        // Serial.print("TIME: ");
        // Serial.println(timeStr);
        
        // Устанавливаем время в RTC
        if (proj42!=nullptr && proj42->rtcManager != nullptr) {
            proj42->rtcManager->SetDateTime(timeStringBuff);
        }else{
            Serial.println("!proj42 && proj42->rtcManager");
        }
        
        return String(timeStringBuff);
    } else {
        return "Не удалось получить время";
    }
}

void WebServerManager::handleClient() {
    server.handleClient();
    
}

void WebServerManager::checkWiFiTimeout() {
    // Проверяем, прошло ли 5 секунд с последнего запроса
    if (lastRequestTime!=0 && millis() - lastRequestTime > WIFI_TIMEOUT_MS) {
        Serial.println("Нет запросов в течение 5 секунд. Полное отключение WiFi...");
        
        // Отключаем подключение к WiFi сети (STA режим)
        if (WiFi.status() == WL_CONNECTED) {
            WiFi.disconnect();
            isConnected = false;
            currentSSID = "";
        }
        
        // Останавливаем SoftAP
        stopSoftAP();
        
        // Полное отключение WiFi
        WiFi.mode(WIFI_MODE_NULL);
        
        Serial.println("WiFi полностью отключен.");
        
        // Сбрасываем таймер, чтобы не отключать постоянно
        lastRequestTime = 0;
        digitalWrite(BUILTIN_LED, LOW);
    }
}

bool WebServerManager::getConnectedStatus() {
    return isConnected && (WiFi.status() == WL_CONNECTED);
}

void WebServerManager::checkWiFiConnection() {
    if (isConnected) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Потеряно подключение WiFi. Восстановление режима AP");
            isConnected = false;
            currentSSID = "";
            startSoftAP();  // Восстанавливаем AP при потере подключения
        }
    }
}

bool WebServerManager::tryAutoConnect() {
    if (autoConnectAttempted) {
        return isConnected;
    }
    
    autoConnectAttempted = true;
    
    if (storageManager.hasSavedSettings()) {
        WiFiSettings settings = storageManager.loadWiFiSettings();
        
        if (settings.isValid) {
            Serial.println("Найдены сохраненные настройки. Попытка автоподключения к: " + String(settings.ssid));
            
            WiFi.begin(settings.ssid, settings.password);
            
            // Ждем подключения (максимум 10 секунд)
            unsigned long startTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
                delay(500);
                Serial.print(".");
            }
            
            if (WiFi.status() == WL_CONNECTED) {
                isConnected = true;
                currentSSID = String(settings.ssid);
                Serial.println("Автоподключение успешно. IP: " + WiFi.localIP().toString());
                return true;
            } else {
                isConnected = false;
                WiFi.disconnect();
                Serial.println("Автоподключение не удалось");
                return false;
            }
        }
    }
    
    return false;
}