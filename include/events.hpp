#ifndef EVENTS_H
#define EVENTS_H

#include "Arduino.h"
#include "global_def.h"
// #include <mutex>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "sensor_handler.hpp"

#define TOUCH_TOP_LOST_INTERVAL_MS 2000
#define LOST_ATTN_INTERVAL_MS 5000
// #define LOST_ATTN_INTERVAL_MS 80000

class Proj42;


class Proj42Events
{

private:    
    // Обработчики датчиков (левый и правый)
    SensorHandler* leftSensorHandler;
    SensorHandler* rightSensorHandler;

public:
    static Proj42 *proj42;
    int touchTopCount = 0;
    unsigned long touchTopLastT = 0; 
    unsigned long lastAttnT = 0; 

    Proj42Events(Proj42* _proj42);
    void InitSensors();

    static void StartTouchThread(void *_this);
    void TouchTask();
    void TouchEvent();
    void TouchTopLostAttn();
    void HasAttn();
    void LostAttn();

    // Методы обработки событий левого датчика
    void leftDistanceLongAttn();
    void leftDistanceShortAttn();
    static void StartLeftSensorsThread(void *_this);
    void LeftSensorsTask();

    // Методы обработки событий правого датчика
    void rightDistanceLongAttn();
    void rightDistanceShortAttn();
    static void StartRightSensorsThread(void *_this);
    void RightSensorsTask();

    ~Proj42Events();
};



#endif