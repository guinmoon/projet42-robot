
#include "events.hpp"
#include "proj42.hpp"
#include "LuLuEyes.h"
#include "sensor_handler.hpp"

Proj42 *Proj42Events::proj42;

Proj42Events::Proj42Events(Proj42 *_proj42)
{
    // Сначала сохраняем указатель на proj42
    proj42 = _proj42;
    
    // Создаем обработчики для левого и правого датчиков
    leftSensorHandler = new SensorHandler(this, false);   // false = левый датчик
    rightSensorHandler = new SensorHandler(this, true);   // true = правый датчик
    
    pinMode(TOUCH_PIN, INPUT);
}

void Proj42Events::InitSensors()
{
    // Инициализируем датчики через SensorHandler
    leftSensorHandler->initSensor(VL53L0X_I2C_ADDR, true, &Wire);
    rightSensorHandler->initSensor(VL53L0X_I2C_ADDR, false, &Wire1);
    
    lastAttnT = millis();

    xTaskCreatePinnedToCore(
        this->StartTouchThread, /* Task function. */
        "StartTouchThread",     /* name of task. */
        2048,                  /* Stack size of task */
        this,                   /* parameter of the task */
        2 | portPRIVILEGE_BIT,  /* priority of the task */
        NULL,                   /* Task handle to keep track of created task */
        1);

    xTaskCreatePinnedToCore(
        this->StartLeftSensorsThread, /* Task function. */
        "StartLeftSensorThread",     /* name of task. */
        2048,                    /* Stack size of task */
        this,                     /* parameter of the task */
        tskIDLE_PRIORITY,         /* priority of the task */
        NULL,                     /* Task handle to keep track of created task */
        1);

     xTaskCreatePinnedToCore(
        this->StartRightSensorsThread, /* Task function. */
        "StartRightSensorThread",     /* name of task. */
        2048,                    /* Stack size of task */
        this,                     /* parameter of the task */
        tskIDLE_PRIORITY,         /* priority of the task */
        NULL,                     /* Task handle to keep track of created task */
        1);
    // tskIDLE_PRIORITY
}

void Proj42Events::StartLeftSensorsThread(void *_this)
{
    ((Proj42Events *)_this)->LeftSensorsTask();
    vTaskDelete(NULL);
}

void Proj42Events::LeftSensorsTask()
{
    // Делегируем задачу общему методу SensorHandler
    leftSensorHandler->sensorTask();
}



void Proj42Events::StartRightSensorsThread(void *_this)
{
    ((Proj42Events *)_this)->RightSensorsTask();
    vTaskDelete(NULL);
}

void Proj42Events::RightSensorsTask()
{
    // Делегируем задачу общему методу SensorHandler
    rightSensorHandler->sensorTask();
}


void Proj42Events::StartTouchThread(void *_this)
{
    ((Proj42Events *)_this)->TouchTask();
    vTaskDelete(NULL);
}

void Proj42Events::TouchTask()
{
    delay(1500);
    while (true)
    {
        uint8_t touched = digitalRead(TOUCH_PIN);
        if (touched != 0)
        {
            Serial.println("Touch ");
            this->TouchEvent();
        }
        auto currentTime = millis();
        if (touchTopLastT != 0 && currentTime - touchTopLastT > TOUCH_TOP_LOST_INTERVAL_MS)
        {
            TouchTopLostAttn();
        }
        if (lastAttnT != 0 && currentTime - lastAttnT > LOST_ATTN_INTERVAL_MS)
        {
            LostAttn();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Proj42Events::TouchTopLostAttn()
{
    touchTopLastT = 0;
    if (touchTopCount >= 5 && touchTopCount <= 14)
        proj42->displayHelper->Angry(2000);
    touchTopCount = 0;
    Serial.println("TouchTopLostAttn");
}

void Proj42Events::leftDistanceShortAttn()
{
    HasAttn();   
    proj42->displayHelper->LookLeft();
    proj42->displayHelper->luluEyes->close(false,true);
    delay(500);
    proj42->displayHelper->LookLeft();
    proj42->displayHelper->luluEyes->close(false,true);
}

void Proj42Events::leftDistanceLongAttn()
{
    leftSensorHandler->setLongAttnBegin(true);
    // HasAttn();
    leftDistanceShortAttn();
    if (!proj42->servoHelper->InMove)
        Proj42::runTask(&ServoHelper::LeftAttnAnimMove, proj42->servoHelper, "LeftAttnAnimMove");
    // proj42->servoHelper->LeftAttnAnimMove();
}


void Proj42Events::rightDistanceShortAttn()
{
    HasAttn();   
    proj42->displayHelper->LookRight();
    proj42->displayHelper->luluEyes->close(true,false);
    delay(500);
    proj42->displayHelper->LookRight();
    proj42->displayHelper->luluEyes->close(true,false);
}

void Proj42Events::rightDistanceLongAttn()
{
    rightSensorHandler->setLongAttnBegin(true);
    // HasAttn();
    rightDistanceShortAttn();
    if (!proj42->servoHelper->InMove)
        Proj42::runTask(&ServoHelper::RightAttnAnimMove, proj42->servoHelper, "RightAttnAnimMove");
    // proj42->servoHelper->LeftAttnAnimMove();
}

void Proj42Events::HasAttn()
{

    lastAttnT = millis();
    if (!proj42->displayHelper->showEyes)
        proj42->displayHelper->resumeEyes();
}

void Proj42Events::LostAttn()
{
    // proj42->displayHelper->showTime = true;
    lastAttnT = 0;
    proj42->displayHelper->ShowClock(5000);
    proj42->servoHelper->GoHome();
    // proj42->displayHelper->pauseEyes();
}

void Proj42Events::TouchEvent()
{
    HasAttn();
    touchTopCount++;
    touchTopLastT = millis();
    switch (touchTopCount)
    {
    case 5:
        proj42->displayHelper->Confused();
        break;
    case 14:
        proj42->displayHelper->HeartAnimation();
        break;
    case 25:
        proj42->displayHelper->HeartAnimation();          
        Proj42::runTask(&ServoHelper::HeartAnimMove, proj42->servoHelper, "HeartAnimTask");
        break;
    default:
        proj42->displayHelper->LookUp();
        break;
    }
}

Proj42Events::~Proj42Events()
{
    delete leftSensorHandler;
    delete rightSensorHandler;
}
