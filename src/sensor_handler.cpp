#include "sensor_handler.hpp"
#include "proj42.hpp"
#include "events.hpp"

SensorHandler::SensorHandler(Proj42Events* _events, bool _isRight)
    : events(_events), isRightSensor(_isRight)
{
    for (int i = 0; i < WINDOW_SIZE; i++) {
        sensorValues[i] = 180.0;
    }
}

bool SensorHandler::initSensor(uint8_t address, bool useDefaultWire, TwoWire* wire)
{
    int sensorBeginIterCount = 0;
    
    if (isRightSensor) {
        // Правый датчик использует Wire1
        while (!sensor.begin(address, false, wire) && sensorBeginIterCount < 5)
        {
            sensorBeginIterCount++;
            Serial.println(F("Failed to boot VL53L0X rightDistanceSensor"));
            delay(1000);
        }
    } else {
        // Левый датчик использует Wire по умолчанию
        while (!sensor.begin() && sensorBeginIterCount < 5)
        {
            sensorBeginIterCount++;
            Serial.println(F("Failed to boot VL53L0X leftDistanceSensor"));
            delay(1000);
        }
    }
    
    return sensorBeginIterCount < 5;
}

void SensorHandler::sensorTask()
{
    const char* sensorName = isRightSensor ? "Right" : "Left";
    
    while (true)
    {
        // Выполняем измерение расстояния
        sensor.rangingTest(&distanceMeasure, false);

        // Записываем значение в скользящее окно
        sensorValues[sensorValueIndex] = distanceMeasure.RangeMilliMeter;
        sensorValueIndex = (sensorValueIndex + 1) % WINDOW_SIZE;

        // Вычисляем среднее значение
        float avgDist = 0.0;
        for (int i = 0; i < WINDOW_SIZE; i++)
        {
            avgDist += sensorValues[i];
        }
        avgDist /= WINDOW_SIZE;

        // Проверка: рука далеко или нет данных
        if (avgDist >= VLX_THRESHOLD || distanceMeasure.RangeStatus == 4)
        {
            if (distanceMeasure.RangeStatus == 4)
            {
                Serial.print(sensorName);
                Serial.println(F(" Out of range"));
            }

            if (isHandNear)
            {
                longAttnBegin = false;
                // Рука ушла — проверяем, сколько длилось
                unsigned long duration = millis() - ocupStartTime;
                if (duration < LONG_DIST_ATTN_DURATION_MS)
                {
                    Serial.print("⚠️");
                    Serial.print(sensorName);
                    Serial.print(F(" Кратковременное приближение: "));
                    Serial.print(duration);
                    Serial.println(F(" мс — пропущено"));
                }
                isHandNear = false;
            }
        }
        // Проверка: близко ли рука?
        else if (avgDist < VLX_THRESHOLD)
        {
            if (!isHandNear)
            {
                // Рука только что поднеслась — фиксируем старт
                isHandNear = true;
                ocupStartTime = millis();
            }
            else
            {
                unsigned long duration = millis() - ocupStartTime;
                
                // Короткое внимание
                if (duration >= SHORT_DIST_ATTN_DURATION_MS && 
                    !events->proj42->servoHelper->InMove && 
                    !longAttnBegin)
                {
                    if (isRightSensor) {
                        events->rightDistanceShortAttn();
                    } else {
                        events->leftDistanceShortAttn();
                    }
                }

                // Длинное внимание
                if (duration >= LONG_DIST_ATTN_DURATION_MS && 
                    !events->proj42->servoHelper->InMove)
                {
                    if (!longAttnBegin) {
                        Serial.print("✅");
                        Serial.print(sensorName);
                        Serial.print(F(" Рука поднесена на "));
                        Serial.print(duration);
                        Serial.print(F(" мс (≥ "));
                        Serial.print(LONG_DIST_ATTN_DURATION_MS);
                        Serial.println(F(" мс)"));
                        
                        if (isRightSensor) {
                            events->rightDistanceLongAttn();
                        } else {
                            events->leftDistanceLongAttn();
                        }
                    }
                }
            }
        }
        
        delay(30);
    }
}