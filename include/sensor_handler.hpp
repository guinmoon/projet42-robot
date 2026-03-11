#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include "Arduino.h"
#include "global_def.h"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

#define WINDOW_SIZE 5

class Proj42Events;

class SensorHandler
{
private:
    Adafruit_VL53L0X sensor;
    VL53L0X_RangingMeasurementData_t distanceMeasure;
    
    const uint16_t VLX_THRESHOLD = 80;    // Порог в мм: ниже — считаем "рука близко"
    const unsigned long LONG_DIST_ATTN_DURATION_MS = 400; // Минимальное время поднесения
    const unsigned long SHORT_DIST_ATTN_DURATION_MS = 100; // Минимальное время поднесения

    // Данные для усреднения (скользящее окно)
    uint16_t sensorValues[WINDOW_SIZE];
    int sensorValueIndex = 0;
    bool isHandNear = false;          // Флаг: рука близко
    unsigned long ocupStartTime = 0;  // Время начала поднесения

    // Указатель на Proj42Events для вызова методов обработки
    Proj42Events* events;
    
    // Флаг начала длинного внимания
    bool longAttnBegin = false;

    bool shortAttnBegin = false;
    
    // Идентификатор датчика для отладки (true = правый, false = левый)
    bool isRightSensor;

public:
    SensorHandler(Proj42Events* _events, bool _isRight);
    bool initSensor(uint8_t address = VL53L0X_I2C_ADDR, bool useDefaultWire = true, TwoWire* wire = &Wire);
    void sensorTask();
    bool getLongAttnBegin() const { return longAttnBegin; }
    void setLongAttnBegin(bool value) { longAttnBegin = value; }
};

#endif