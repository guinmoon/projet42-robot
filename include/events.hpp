#ifndef EVENTS_H
#define EVENTS_H

#include "Arduino.h"
#include "global_def.h"
// #include <mutex>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>


#define WINDOW_SIZE 5
#define TOUCH_TOP_LOST_INTERVAL_MS 2000
#define LOST_ATTN_INTERVAL_MS 80000

class Proj42;


class Proj42Events
{

private:    
    // std::mutex i2c_mutex;
    // bool _reciveSuspended = false;
    // bool _isCommandFinished = true;
    static Proj42 *proj42;    
    static Adafruit_VL53L0X leftDistanceSensor ;
    static Adafruit_VL53L0X rightDistanceSensor ;
    VL53L0X_RangingMeasurementData_t leftDistanceMeasure;
    VL53L0X_RangingMeasurementData_t rightDistanceMeasure;


    const uint16_t VLX_THRESHOLD = 180;    // Порог в мм: ниже — считаем "рука близко"
    const unsigned long LONG_DIST_ATTN_DURATION_MS = 400; // Минимальное время поднесения (например, 500 мс)
    const unsigned long SHORT_DIST_ATTN_DURATION_MS = 399; // Минимальное время поднесения (например, 500 мс)

    // Данные для усреднения (скользящее окно)
    
    uint16_t leftSensorValues[WINDOW_SIZE];
    int leftSensorValueIndex = 0;
    bool isHandNearLeft = false;          // Флаг: рука близко
    unsigned long leftOcupStartTime = 0;      // Время начала поднесения

    uint16_t rightSensorValues[WINDOW_SIZE];
    int rightSensorValueIndex = 0;
    bool isHandNearRight = false;          // Флаг: рука близко
    unsigned long rightOcupStartTime = 0;      // Время начала поднесения

public:
    int touchTopCount = 0;
    unsigned long touchTopLastT = 0; 
    unsigned long lastAttnT = 0; 
    bool leftDistanceLongAttnBegin = false;
    bool rightDistanceLongAttnBegin = false;

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
    static void StartLeftSensorsThread(void *_this);
    void LeftSensorsTask();

    void rightDistanceLongAttn();
    void rightDistanceShortAttn();
    static void StartRightSensorsThread(void *_this);
    void RightSensorsTask();

    ~Proj42Events();
};



#endif