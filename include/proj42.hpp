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
    byte lightBrightness = 0;
    

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
    bool lightEnabled = false;
    DisplayHelper *displayHelper = nullptr;
    Proj42Events *eventsHelper = nullptr;
    ServoHelper *servoHelper = nullptr;
    WebServerManager *webServer = nullptr;
    RtcManager *rtcManager = nullptr;
   
   

    Proj42();
    void Init();
  

    void MemInfo();
    void LowPowMode();
    void NormalPowMode();
    void HighPowMode();
    void LightLikeAnimation();

    void EnableLight(){
        lightEnabled = true;
        if (lightBrightness == 0)
            lightBrightness = 1;
        analogWrite(LED_PHILAMENT_LEFT_PIN, lightBrightness); 
        analogWrite(LED_PHILAMENT_RIGHT_PIN, lightBrightness);
    }

    void DisabelLight(){
        lightEnabled = false;
        analogWrite(LED_PHILAMENT_LEFT_PIN, 0); 
        analogWrite(LED_PHILAMENT_RIGHT_PIN, 0);
    }

    void ToogleLight(){
        if (!lightEnabled)
            EnableLight();
        else
            DisabelLight();        
    }

    void SetLightBrightness(byte brightness){
        if (brightness>MAX_PHILAMENT_BRGIHT)
            return;
        lightBrightness = brightness;        
        analogWrite(LED_PHILAMENT_LEFT_PIN, lightBrightness);   
        analogWrite(LED_PHILAMENT_RIGHT_PIN, lightBrightness);     
    }

    byte GetLightBrightness(){
        return lightBrightness;
    }

    void IncreaseLightBrightness(){
        lightBrightness++;
        if (lightBrightness>MAX_PHILAMENT_BRGIHT)
            lightBrightness = 0;
        analogWrite(LED_PHILAMENT_LEFT_PIN, lightBrightness);     
        analogWrite(LED_PHILAMENT_RIGHT_PIN, lightBrightness);
    }

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