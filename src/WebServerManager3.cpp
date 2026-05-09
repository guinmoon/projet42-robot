#include "WebServerManager3.h"
#include "wifi_config.h"
#include "time.h"
#include "proj42.hpp"

WebServerManager3::WebServerManager3(Proj42* _proj42) : 
    server(DEFAULT_PORT),
    isConnected(false),
    proj42(_proj42),
    autoConnectAttempted(false) {
}

void WebServerManager3::StartWebServerThread(void *_this) {
    static_cast<WebServerManager3*>(_this)->WebServerTask();
    vTaskDelete(NULL);
}

void WebServerManager3::WebServerTask() {
    uint32_t loopCounter = 0;
    unsigned long lastDisplayUpdateTime = 0;
    unsigned long lastLightCheckTime = 0;
    lastRequestTime = millis();

    while (true) {
        handleClient();
        processAsyncConnect();

        unsigned long now = millis();
        loopCounter++;
        
        // 1. Обновляем строку времени для дисплея каждую секунду
        if (now - lastDisplayUpdateTime >= 1000) {
            refreshDisplayTime();
            lastDisplayUpdateTime = now;
        }

        // 2. Проверяем таймаут WiFi (отключение при бездействии)
        if (loopCounter == 20) { 
            checkWiFiTimeout();
        }

        // 3. Проверка соединения раз в 6 секунд (30 * 200мс)
        if (loopCounter >= 30) {         
            checkWiFiConnection(); 
            loopCounter = 0;
        }
        
        // 4. Обновление системного времени по NTP раз в час
        if (now - lastNtpUpdateTime > NTP_UPDATE_INTERVAL_MS) {
            updateAndGetTime();
            // lastNtpUpdateTime = now;
        }

        // 5. Проверка расписания свечения кажд 5 sec
        if (now - lastLightCheckTime >= 5000) {
            checkLightSchedule();
            lastLightCheckTime = now;
        }
        
        delay(200);
    }
}

void WebServerManager3::init() {
    WiFi.mode(WIFI_MODE_APSTA);
    startSoftAP();
    
    // Настройка маршрутов
    server.on("/", [this]() { this->handleRoot(); });
    server.on("/connect", [this]() { this->handleConnect(); });
    server.on("/status", [this]() { this->handleStatus(); });
    server.on("/clear", [this]() { this->handleClearSettings(); });
    server.on("/lightschedule", [this]() { this->handleLightSchedule(); });
    
    server.begin();
    
    // Первичная настройка времени
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    
    Serial.println("[WebMgr] Server initialized");
    Serial.print("[WebMgr] AP IP: ");
    Serial.println(WiFi.softAPIP());

    xTaskCreatePinnedToCore(
        WebServerManager3::StartWebServerThread,
        "WebSvrTask",
        4096,
        this,
        tskIDLE_PRIORITY,
        NULL,
        1);

    Serial.println("[WebMgr] Attempting auto-connect...");
    tryAutoConnect();
    
    updateAndGetTime();
    lastRequestTime = millis();    
}

void WebServerManager3::startSoftAP() {
    if (!WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASS)) {
        Serial.println("[WebMgr] Failed to start SoftAP!");
    } else {
        Serial.printf("[WebMgr] SoftAP started. IP: %s\n", WiFi.softAPIP().toString().c_str());
    }
}

void WebServerManager3::stopSoftAP() {
    WiFi.softAPdisconnect(true);
    Serial.println("[WebMgr] SoftAP stopped.");
}

void WebServerManager3::handleRoot() {
    server.send(200, "text/html", loginPage); // loginPage должен быть определен глобально
    lastRequestTime = millis();
}

void WebServerManager3::handleConnect() {
    if (server.method() == HTTP_POST) {
        String newSSID = server.arg("ssid");
        String newPass = server.arg("password");
        
        if (newSSID.length() > 0) {
            Serial.println("[WebMgr] Request to connect to: " + newSSID);
            
            // Переводим в состояние подключения (асинхронно)
            connectionState = WiFiState::CONNECTING;
            connectionStartTime = millis();
            
            WiFi.begin(newSSID.c_str(), newPass.c_str());
            unsigned long startTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
                delay(500);
                Serial.print(".");
            }
            if (WiFi.status() == WL_CONNECTED) {
                storageManager.saveWiFiSettings(newSSID.c_str(), newPass.c_str());

                server.send(200, "text/html", 
                    "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Подключение</title>"
                    "<meta http-equiv='refresh' content='3;url=/status'></head>"
                    "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                    "<h2>Пожалуйста, подождите...</h2><p>Попытка подключения к " + newSSID + "</p>"
                    "</body></html>");
            }
            else {
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
            server.send(400, "text/plain", "SSID cannot be empty");
        }
    }
    lastRequestTime = millis();
}

void WebServerManager3::handleStatus() {
    server.send(200, "application/json", getJsonStatus());
    lastRequestTime = millis();
}

void WebServerManager3::handleClearSettings() {
    storageManager.clearWiFiSettings();
    isConnected = false;
    WiFi.disconnect();
    startSoftAP();
    
    server.send(200, "text/html", 
        "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Cleared</title></head>"
        "<body style='font-family:Arial;text-align:center;padding:50px;'>"
        "<h2>Settings cleared!</h2><p>WiFi settings have been removed.</p>"
        "<a href='/'>Back</a></body></html>");
    lastRequestTime = millis();
}

void WebServerManager3::handleLightSchedule() {
    if (server.method() == HTTP_POST) {
        int onHour = server.arg("onHour").toInt();
        int onMinute = server.arg("onMinute").toInt();
        bool enabled = server.arg("enabled") == "true";

        if (onHour >= 0 && onHour <= 23 && onMinute >= 0 && onMinute <= 59) {
            storageManager.saveLightSchedule(onHour, onMinute, enabled);
            Serial.printf("[WebMgr] Light schedule saved: %02d:%02d, enabled=%d\n", onHour, onMinute, enabled);

            server.send(200, "text/html", 
                "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Расписание</title>"
                "<meta http-equiv='refresh' content='2;url=/'></head>"
                "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                "<h2>Расписание сохранено!</h2>"
                "<p>Свечение включится в: " + String(onHour) + ":" + String(onMinute < 10 ? "0" : "") + String(onMinute) + "</p>"
                "<p>Включено: " + (enabled ? "Да" : "Нет") + "</p>"
                "<a href='/'>На главную</a>"
                "</body></html>");
        } else {
            server.send(400, "text/html", 
                "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Ошибка</title></head>"
                "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                "<h2 style='color:red;'>Ошибка!</h2>"
                "<p>Некорректное время. Часы: 0-23, Минуты: 0-59</p>"
                "<a href='/'>Назад</a>"
                "</body></html>");
        }
    } else {
        LightSchedule schedule = storageManager.loadLightSchedule();
        String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Расписание свечения</title>"
            "<style>"
            "body{font-family:Arial;max-width:500px;margin:20px auto;padding:15px;background-color:#f5f5f5;}"
            ".container{background-color:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}"
            "h1{color:#333;text-align:center;font-size:24px;}"
            "label{display:block;margin:10px 0 5px 0;font-weight:bold;}"
            "input[type='number'],select{width:100%;padding:10px;margin:5px 0 15px 0;border:2px solid #ddd;border-radius:5px;box-sizing:border-box;}"
            "input[type='submit']{width:100%;background-color:#4CAF50;color:white;padding:12px;border:none;border-radius:5px;cursor:pointer;font-size:16px;}"
            "input[type='submit']:hover{background-color:#45a049;}"
            ".status{margin-top:20px;padding:10px;border-radius:5px;text-align:center;background-color:#d4edda;color:#155724;border:1px solid #c3e6cb;}"
            "</style>"
            "</head><body><div class='container'>"
            "<h1>Расписание свечения</h1>"
            "<form method='POST' action='/lightschedule'>"
            "<label for='onHour'>Время включения (часы):</label>"
            "<input type='number' id='onHour' name='onHour' min='0' max='23' value='" + String(schedule.onHour) + "'>"
            "<label for='onMinute'>Время включения (минуты):</label>"
            "<input type='number' id='onMinute' name='onMinute' min='0' max='59' value='" + String(schedule.onMinute) + "'>"
            "<label for='enabled'>Включить расписание:</label>"
            "<select id='enabled' name='enabled'>"
            "<option value='true'" + (schedule.enabled ? " selected" : "") + ">Да</option>"
            "<option value='false'" + (!schedule.enabled ? " selected" : "") + ">Нет</option>"
            "</select>"
            "<input type='submit' value='Сохранить'>"
            "</form>";
        
        if (schedule.enabled) {
            html += "<div class='status'>Текущее расписание: Свечение включится в " + 
                    String(schedule.onHour) + ":" + String(schedule.onMinute < 10 ? "0" : "") + String(schedule.onMinute) + "</div>";
        }
        
        html += "<p style='text-align:center;margin-top:20px;'><a href='/'>На главную</a></p>"
            "</div></body></html>";
        
        server.send(200, "text/html", html);
    }
    lastRequestTime = millis();
}

String WebServerManager3::getJsonStatus() {
    if (isConnected && WiFi.status() == WL_CONNECTED) {
        return "{\"connected\":true,\"ssid\":\"" + currentSSID + "\",\"ip\":\"" + 
               WiFi.localIP().toString() + "\",\"time\":\"" + updateAndGetTime() + "\"}";
    }
    return "{\"connected\":false,\"ssid\":\"\",\"ip\":\"\",\"time\":\"\"}";
}

String WebServerManager3::updateAndGetTime() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        lastNtpUpdateTime = millis();
        char timeStringBuff[50];
        strftime(timeStringBuff, sizeof(timeStringBuff), "%d.%m.%Y %H:%M:%S", &timeinfo);
        
        if (proj42 != nullptr) {
            // Обновляем только RTC, так как refreshDisplayTime делает это каждую секунду
            if (proj42->rtcManager != nullptr) {
                proj42->rtcManager->SetDateTime(timeStringBuff);
            }
        }
        return String(timeStringBuff);
    }
    return "Time unavailable";
}

void WebServerManager3::refreshDisplayTime() {
    struct tm timeinfo;
    if (proj42 != nullptr && getLocalTime(&timeinfo)) {
        strftime(proj42->timeStr, sizeof(proj42->timeStr), "%H:%M", &timeinfo);
    }
}

void WebServerManager3::checkWiFiTimeout() {
    // Условие отключения:
    // 1. Мы считаем, что подключены к WiFi
    // 2. Прошло более 20 секунд с последнего запроса
    // 3. Время хотя бы раз было синхронизировано (lastNtpUpdateTime > 0)
    // Serial.printf("[WebMgr] %i %i %i %i\n",isConnected,WiFi.status(),lastNtpUpdateTime,lastRequestTime );
    if (isConnected && /*WiFi.status() == WL_CONNECTED && */
        lastNtpUpdateTime > 0 && 
        (millis() - lastRequestTime > WIFI_TIMEOUT_MS)) {
        
        Serial.println("[WebMgr] Inactivity timeout. Disabling WiFi to save power...");
        
        WiFi.disconnect();
        stopSoftAP();
        WiFi.mode(WIFI_MODE_NULL); // Полное выключение радиомодуля
        
        isConnected = false;
        currentSSID = "";
        lastRequestTime = 0; // Сбрасываем, чтобы не заходить сюда снова
    }
}

void WebServerManager3::handleClient() {
    server.handleClient();
}

void WebServerManager3::processAsyncConnect() {
    // Serial.printf("WiFiState: %i\n",connectionState);
    if (connectionState != WiFiState::CONNECTING) return;

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WebMgr] Async connection successful!");
        isConnected = true;
        connectionState = WiFiState::CONNECTED;
    } else if (millis() - connectionStartTime > 10000) {
        Serial.println("[WebMgr] Async connection timeout.");
        connectionState = WiFiState::FAILED;
        WiFi.disconnect();
    }
}

void WebServerManager3::checkWiFiConnection() {
    if (isConnected) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WebMgr] WiFi lost. Trying to reconnect...");
            if (!tryAutoConnect()) {
                Serial.println("[WebMgr] Reconnect failed. Restoring AP.");
                isConnected = false;
                currentSSID = "";
                startSoftAP();
            }
        }
    }
}

bool WebServerManager3::tryAutoConnect() {
    if (autoConnectAttempted && isConnected) return true;
    
    autoConnectAttempted = true;
    
    if (storageManager.hasSavedSettings()) {
        WiFiSettings settings = storageManager.loadWiFiSettings();
        if (settings.isValid) {
            Serial.printf("[WebMgr] Auto-connecting to: %s\n", settings.ssid);
            WiFi.begin(settings.ssid, settings.password);
            
            // Для автоподключения в init() мы можем подождать немного, 
            // но в идеале здесь тоже стоит использовать асинхронность.
            unsigned long start = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
                delay(100);
            }
            
            if (WiFi.status() == WL_CONNECTED) {
                Serial.printf("[WebMgr] CONNECTED to: %s\n", settings.ssid);
                isConnected = true;
                currentSSID = String(settings.ssid);
                proj42->LowPowMode();
                return true;
            }
        }
    }else{
        Serial.printf("No stored settings found, start AP Mode\n");
    }
    return false;
}

void WebServerManager3::checkLightSchedule() {
    if (!storageManager.hasLightSchedule()) {
        return;
    }

    LightSchedule schedule = storageManager.loadLightSchedule();
    if (!schedule.enabled) {
        return;
    }

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        int currentHour = timeinfo.tm_hour;
        int currentMinute = timeinfo.tm_min;

        if (currentHour >= schedule.onHour && currentMinute >= schedule.onMinute) {
            Serial.printf("[WebMgr] Light schedule triggered at %02d:%02d\n", currentHour, currentMinute);
            if (proj42 != nullptr) {
                proj42->EnableLight();
            }
        }
    }
}

bool WebServerManager3::getConnectedStatus() {
    return isConnected && (WiFi.status() == WL_CONNECTED);
}