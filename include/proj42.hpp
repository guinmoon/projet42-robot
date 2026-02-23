#ifndef PROJ42_H
#define PROJ42_H

#include "global_def.h"
#include "display.h"
#include "events.hpp"
#include "servos.hpp"
#include "WebServerManager.h"


class Proj42
{
private:
    static Proj42* instance;
public:
    
    int topTouchCount = 0;

    DisplayHelper* displayHelper = nullptr;
    Proj42Events* eventsHelper = nullptr;
    ServoHelper* servoHelper = nullptr;
    WebServerManager* webServer = nullptr;
    // GyroHelper* gyroHelper;
    // AudioHelper* audioHelper;
    // LuLuCharacter* luluCharacter;
    // BatteryHelper* batteryHelper;
    // TouchHelper* touchHelper;
    // LVGLHelper* lvglHelper;
    // JSRunner* jsRunner;
    // LuLuWebServer* fsWebServer;
    // static ConfigHelper* configHelper;
    // DogEvents* dogEvents;
    // I2CSlaveHelper* i2cSlaveHelper;

    // int lastAction = -1;
    
    Proj42();
    void Init();
    // void setVoltageBuf(float voltage);
    // void PauseDog();
    // void ResumeDog();
    // void ShowMenu();
    // void Action1();
    // void Action2();
    // void Action3();
    // void ExitMenu();
    // static void DoubleTapCallBack(int x, int y);
    // static void LongPressCallBack(int x, int y);

    // void MemInfo();
    // void LowPowMode();
    // void NormalPowMode();
    // void HighPowMode();

};

#endif