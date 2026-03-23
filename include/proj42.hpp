#ifndef PROJ42_H
#define PROJ42_H

#include "global_def.h"
#include "display.h"
#include "events.hpp"
#include "servos.hpp"
#include "WebServerManager.h"
#include "RtcManager.hpp"

class Proj42
{
private:
    static Proj42 *instance;

    template <typename T>
    struct TaskParam
    {
        T *instance;
        void (T::*method)();
        TaskParam(T *inst, void (T::*meth)()) : instance(inst), method(meth) {}
    };

    // Статическая обёртка, которая вызывает метод у объекта
    template <typename T>
    static void taskWrapper(void *param)
    {
        auto task = static_cast<TaskParam<T> *>(param);
        (task->instance->*(task->method))(); // Вызов метода
        delete task;
        vTaskDelete(NULL);
    }

public:
    
    char timeStr[15] = "";
    DisplayHelper *displayHelper = nullptr;
    Proj42Events *eventsHelper = nullptr;
    ServoHelper *servoHelper = nullptr;
    WebServerManager *webServer = nullptr;
    RtcManager *rtcManager = nullptr;
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

    void MemInfo();
    void LowPowMode();
    void NormalPowMode();
    void HighPowMode();

    template <typename T>
    static void runTaskPriotity(void (T::*method)(), T *instance, const char *name,
                        uint32_t stackSize = 4096, uint32_t priority = 2, int core = 1)
    {
        auto task = new TaskParam<T>{instance, method};
        if (xTaskCreatePinnedToCore(taskWrapper<T>, name, stackSize, task,
                                    priority | portPRIVILEGE_BIT, NULL, core) != pdPASS)
        {
            delete task; // Освобождаем память при ошибке создания задачи
            Serial.println("Failed to create task");
        }
    }


    template <typename T>
    static void runTaskIdle(void (T::*method)(), T *instance, const char *name,
                        uint32_t stackSize = 4096, uint32_t priority = 2, int core = 1)
    {
        auto task = new TaskParam<T>{instance, method};
        if (xTaskCreatePinnedToCore(taskWrapper<T>, name, stackSize, task,
                                    tskIDLE_PRIORITY, NULL, core) != pdPASS)
        {
            delete task; // Освобождаем память при ошибке создания задачи
            Serial.println("Failed to create task");
        }
    }
};

#endif