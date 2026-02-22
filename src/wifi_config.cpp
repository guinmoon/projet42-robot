#include "wifi_config.h"

const char* loginPage = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Настройка WiFi</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 500px;
            margin: 50px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            background-color: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 12px;
            margin: 10px 0;
            border: 2px solid #ddd;
            border-radius: 5px;
            box-sizing: border-box;
        }
        input[type="submit"] {
            width: 100%;
            background-color: #4CAF50;
            color: white;
            padding: 14px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
        }
        input[type="submit"]:hover {
            background-color: #45a049;
        }
        .status {
            margin-top: 20px;
            padding: 15px;
            border-radius: 5px;
            text-align: center;
        }
        .success {
            background-color: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .error {
            background-color: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Настройка подключения WiFi</h1>
        <form action="/connect" method="POST">
            <label for="ssid">Имя WiFi сети:</label>
            <input type="text" id="ssid" name="ssid" required>
            
            <label for="password">Пароль:</label>
            <input type="password" id="password" name="password">
            
            <input type="submit" value="Подключиться">
        </form>
        
        <div id="statusMessage"></div>
        
        <script>
            // Проверка статуса подключения
            function checkStatus() {
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/status", true);
                xhr.onreadystatechange = function() {
                    if (xhr.readyState === 4 && xhr.status === 200) {
                        var response = JSON.parse(xhr.responseText);
                        var statusDiv = document.getElementById("statusMessage");
                        if (response.connected) {
                            statusDiv.innerHTML = '<div class="status success">Успешно подключено к: ' + response.ssid + '<br>IP: ' + response.ip + '<br>Время: ' + response.time + '</div>';
                        } else {
                            statusDiv.innerHTML = '<div class="status error">Не подключено. Точка доступа активна.</div>';
                        }
                    }
                };
                xhr.send();
            }
            
            // Проверяем статус каждые 3 секунды
            setInterval(checkStatus, 3000);
            checkStatus(); // Проверяем сразу при загрузке
        </script>
    </div>
</body>
</html>
)=====";