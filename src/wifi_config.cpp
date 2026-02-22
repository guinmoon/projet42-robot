#include "wifi_config.h"

const char loginPage[] PROGMEM = R"=====(
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
            margin: 20px auto;
            padding: 15px;
            background-color: #f5f5f5;
        }
        .container {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
            font-size: 24px;
        }
        label {
            display: block;
            margin: 10px 0 5px 0;
            font-weight: bold;
        }
        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 10px;
            margin: 5px 0 15px 0;
            border: 2px solid #ddd;
            border-radius: 5px;
            box-sizing: border-box;
        }
        input[type="submit"] {
            width: 100%;
            background-color: #4CAF50;
            color: white;
            padding: 12px;
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
            padding: 10px;
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
        #statusMessage {
            min-height: 60px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Настройка WiFi</h1>
        <form id="wifiForm">
            <label for="ssid">Имя сети (SSID):</label>
            <input type="text" id="ssid" name="ssid" required placeholder="Введите имя WiFi сети">
            
            <label for="password">Пароль:</label>
            <input type="password" id="password" name="password" placeholder="Введите пароль (если есть)">
            
            <input type="submit" value="Подключиться">
        </form>
        
        <div id="statusMessage"></div>
    </div>

    <script>
        document.getElementById('wifiForm').addEventListener('submit', function(e) {
            e.preventDefault();
            
            var ssid = document.getElementById('ssid').value;
            var password = document.getElementById('password').value;
            
            var xhr = new XMLHttpRequest();
            xhr.open("POST", "/connect", true);
            xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            
            xhr.onreadystatechange = function() {
                if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        // Обновляем страницу для отображения результата
                        document.write(xhr.responseText);
                    } else {
                        document.getElementById('statusMessage').innerHTML = 
                            '<div class="status error">Ошибка отправки данных</div>';
                    }
                }
            };
            
            var data = "ssid=" + encodeURIComponent(ssid) + "&password=" + encodeURIComponent(password);
            xhr.send(data);
        });
        
        // Проверка статуса подключения
        function checkStatus() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/status", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    try {
                        var response = JSON.parse(xhr.responseText);
                        var statusDiv = document.getElementById("statusMessage");
                        if (response.connected) {
                            statusDiv.innerHTML = '<div class="status success">Подключено к: ' + 
                                response.ssid + '<br>IP: ' + response.ip + '<br>Время: ' + response.time + '</div>';
                        } else {
                            statusDiv.innerHTML = '<div class="status error">Точка доступа активна. Ожидание подключения.</div>';
                        }
                    } catch(e) {
                        console.log("Ошибка парсинга JSON: " + e);
                    }
                }
            };
            xhr.send();
        }
        
        // Проверяем статус каждые 3 секунды
        setInterval(checkStatus, 3000);
        // Проверяем сразу при загрузке
        setTimeout(checkStatus, 1000);
    </script>
</body>
</html>
)=====";