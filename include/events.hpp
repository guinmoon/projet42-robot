#ifndef EVENTS_H
#define EVENTS_H

#include "Arduino.h"
#include "global_def.h"
#include <mutex>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>


#define WINDOW_SIZE 5
#define TOUCH_TOP_LOST_INTERVAL_MS 2000
#define LOST_ATTN_INTERVAL_MS 80000

class Proj42;


class Proj42Events
{

private:    
    std::mutex i2c_mutex;
    bool _reciveSuspended = false;
    bool _isCommandFinished = true;
    static Proj42 *proj42;    
    static Adafruit_VL53L0X leftDistanceSensor ;
    VL53L0X_RangingMeasurementData_t leftDistanceMeasure;


    const uint16_t THRESHOLD = 180;    // Порог в мм: ниже — считаем "рука близко"
    const unsigned long LONG_DIST_ATTN_DURATION_MS = 1000; // Минимальное время поднесения (например, 500 мс)
    const unsigned long SHORT_DIST_ATTN_DURATION_MS = 300; // Минимальное время поднесения (например, 500 мс)

    // Данные для усреднения (скользящее окно)
    
    uint16_t sensorValues[WINDOW_SIZE];
    int valueIndex = 0;

    bool isHandNear = false;          // Флаг: рука близко
    unsigned long startTime = 0;      // Время начала поднесения

public:
    int touchTopCount = 0;
    unsigned long touchTopLastT = 0; 
    unsigned long lastAttnT = 0; 
    bool     leftDistanceLongAttnBegin = false;

    Proj42Events(Proj42* _proj42);
    void InitSensors();

    static void StartTouchThread(void *_this);
    void TouchTask();
    void TouchEvent();
    void TouchTopLostAttn();
    void HasAttn();
    void LostAttn();
    void leftDistanceLongAttn();
    void leftDistanceShortAttn();

    static void StartSensorsThread(void *_this);
    void SensorsTask();

    ~Proj42Events();
};



#endif