#ifndef EVENTS_H
#define EVENTS_H

#include "Arduino.h"
#include "global_def.h"
#include <mutex>
// #include <Wire.h>

class Proj42;

class Proj42Events
{

private:    
    std::mutex i2c_mutex;
    bool _reciveSuspended = false;
    bool _isCommandFinished = true;
    static Proj42 *proj42;    

public:
    int touchTopCount = 0;
    unsigned long touchTopLastT = 0; 
    unsigned long lastAttnT = 0; 
 

    Proj42Events(Proj42* _proj42);

    static void StartTouchThread(void *_this);
    void TouchTask();
    void TouchEvent();
    void TouchTopLostAttn();
    void HasAttn();
    void LostAttn();
 

    ~Proj42Events();
};


#define TOUCH_TOP_LOST_INTERVAL_MS 2000
#define LOST_ATTN_INTERVAL_MS 25000

#endif