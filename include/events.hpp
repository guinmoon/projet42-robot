#ifndef EVENTS_H
#define EVENTS_H

#include "Arduino.h"
#include "global_def.h"
#include <mutex>
#include <Wire.h>



class Proj42Events
{

private:    
    std::mutex i2c_mutex;
    bool _reciveSuspended = false;
    bool _isCommandFinished = true;


public:

    // bool IsCommandFinished()
    // {
    //     return _isCommandFinished;
    // }

    // Proj42Events()
    // {
        
    // }

    // void sendCommand(int command);

    // void SendCommand(int command, int arg1);

    // void ConfirmCommand(int command, int arg1);

    // static void commandConfirmThread(void *_this);

    // // uint8_t requestI2CByte();

    // void commandConfirmTask();

    // void StartReciveThread();
    // static void I2CReciveThread(void *_this);
    // void I2CReciveTask();

    // void OnSlaveTouchEvent();

    ~I2CSlaveHelper();
};

#endif