#include "proj42.hpp"
// #include <Wire.h>
// #include <Adafruit_VL53L0X.h>

// // Создаем объект для работы с модулем VL53L0X
// Adafruit_VL53L0X sensor = Adafruit_VL53L0X();

static Proj42 proj42;

void setup()
{
    // Serial.begin(115200);

    // Wire.begin(22, 21);
    // while (!sensor.begin())
    // {
    //     Serial.println(F("Failed to boot VL53L0X"));
    //     delay(1000);
    // }

    Serial.println("alive");
    proj42.Init();
}

void loop()
{

    // VL53L0X_RangingMeasurementData_t measure;

    // // Выполняем измерение расстояния
    // sensor.rangingTest(&measure, false);

    // if (measure.RangeStatus != 4)
    // { // Проверяем, что измерение действительно
    //     // Выводим измеренное расстояние в миллиметрах
    //     Serial.print(F("Range (mm): "));
    //     Serial.println(measure.RangeMilliMeter);
    // }
    // else
    // {
    //     Serial.println(F("Out of range")); // Если расстояние недоступно, выводим сообщение
    // }

    Serial.println("alive");
    delay(10000);
}
