#include "WebServerManager.h"
#include "wifi_config.h"
#include "time.h"

Proj42 *WebServerManager::proj42;

WebServerManager::WebServerManager() : server(80),
                                       isConnected(false),
                                       ntpServer("pool.ntp.org"),
                                       gmtOffset_sec(10800), // +3 часа (Москва)
                                       daylightOffset_sec(0)
{
}

WebServerManager::WebServerManager(Proj42* _proj42) : server(80),
                                       isConnected(false),
                                       ntpServer("pool.ntp.org"),
                                       gmtOffset_sec(10800), // +3 часа (Москва)
                                       daylightOffset_sec(0){    
    proj42 = _proj42;
    init();   
}

void WebServerManager::StartWebServerThread(void *_this)
{
    ((WebServerManager *)_this)->WebServerTask();
    vTaskDelete(NULL);
}

void WebServerManager::WebServerTask()
{
    while (true){
        handleClient();                
        checkWiFiConnection();        
        delay(10);
    }
}

void WebServerManager::init()
{
    // Запуск точки доступа
    WiFi.mode(WIFI_MODE_AP);
    delay(100);
    WiFi.softAP("Proj42_Config", "12345678", 1, 0, 4);

    // Настройка маршрутов
    server.on("/", [this]()
              { this->handleRoot(); });
    server.on("/connect", [this]()
              { this->handleConnect(); });
    server.on("/status", [this]()
              { this->handleStatus(); });

    server.begin();

    // Инициализация NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    Serial.println("Web server initialized");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

     xTaskCreatePinnedToCore(
        this->StartWebServerThread, /* Task function. */
        "Task1",                     /* name of task. */
        50000,                       /* Stack size of task */
        this,                        /* parameter of the task */
        2 | portPRIVILEGE_BIT,       /* priority of the task */
        NULL,                      /* Task handle to keep track of created task */
        1);
}

void WebServerManager::handleRoot()
{
    server.send(200, "text/html", loginPage);
}

void WebServerManager::handleConnect()
{
    if (server.method() == HTTP_POST)
    {
        String newSSID = server.arg("ssid");
        String newPass = server.arg("password");

        if (newSSID.length() > 0)
        {
            Serial.println("Попытка подключения к: " + newSSID);

            WiFi.begin(newSSID.c_str(), newPass.c_str());

            // Ждем подключения (максимум 10 секунд)
            unsigned long startTime = millis();
            while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
            {
                delay(500);
                Serial.print(".");
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                isConnected = true;
                currentSSID = newSSID;
                Serial.println("Успешно подключено к WiFi");
                server.send(200, "text/html",
                            "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Успех</title></head>"
                            "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                            "<h2 style='color:green;'>Успешно подключено!</h2>"
                            "<p>Подключено к: " +
                                newSSID + "</p>"
                                          "<p>IP адрес: " +
                                WiFi.localIP().toString() + "</p>"
                                                            "<a href='/'>Назад</a>"
                                                            "</body></html>");
            }
            else
            {
                isConnected = false;
                WiFi.disconnect();
                Serial.println("Не удалось подключиться к WiFi");
                server.send(200, "text/html",
                            "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Ошибка</title></head>"
                            "<body style='font-family:Arial;text-align:center;padding:50px;'>"
                            "<h2 style='color:red;'>Ошибка подключения!</h2>"
                            "<p>Не удалось подключиться к сети: " +
                                newSSID + "</p>"
                                          "<a href='/'>Попробовать снова</a>"
                                          "</body></html>");
            }
        }
        else
        {
            server.send(400, "text/plain", "SSID не может быть пустым");
        }
    }
}

void WebServerManager::handleStatus()
{
    String jsonResponse = getJsonStatus();
    server.send(200, "application/json", jsonResponse);
}

String WebServerManager::getJsonStatus()
{
    String jsonResponse;
    if (isConnected && WiFi.status() == WL_CONNECTED)
    {
        String currentTime = getCurrentTime();
        jsonResponse = "{\"connected\":true,\"ssid\":\"" + currentSSID + "\",\"ip\":\"" +
                       WiFi.localIP().toString() + "\",\"time\":\"" + currentTime + "\"}";
    }
    else
    {
        jsonResponse = "{\"connected\":false,\"ssid\":\"\",\"ip\":\"\",\"time\":\"\"}";
        isConnected = false;
    }
    return jsonResponse;
}

String WebServerManager::getCurrentTime()
{
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        char timeStringBuff[50];
        strftime(timeStringBuff, sizeof(timeStringBuff), "%d.%m.%Y %H:%M:%S", &timeinfo);
        return String(timeStringBuff);
    }
    else
    {
        return "Не удалось получить время";
    }
}

void WebServerManager::handleClient()
{
    server.handleClient();
}

bool WebServerManager::getConnectedStatus()
{
    return isConnected && (WiFi.status() == WL_CONNECTED);
}

void WebServerManager::checkWiFiConnection()
{
    if (isConnected)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("Потеряно подключение WiFi. Переход в режим AP");
            isConnected = false;
            currentSSID = "";
        }
    }
}


