
#include "events.hpp"
#include "proj42.hpp"
#include "LuLuEyes.h"

Proj42 *Proj42Events::proj42;
Adafruit_VL53L0X Proj42Events::leftDistanceSensor = Adafruit_VL53L0X();
Adafruit_VL53L0X Proj42Events::rightDistanceSensor = Adafruit_VL53L0X();

Proj42Events::Proj42Events(Proj42 *_proj42)
{
    for (int i = 0; i < WINDOW_SIZE; i++)
        leftSensorValues[i] = 180.0;
    pinMode(TOUCH_PIN, INPUT);
    proj42 = _proj42;
}

void Proj42Events::InitSensors()
{
    // Adafruit_VL53L0X sensor = Adafruit_VL53L0X();
    // leftDistanceSensor = new Adafruit_VL53L0X();
    // delay(1000);
    int sensorBeginIterCount = 0;
    while (!leftDistanceSensor.begin() && sensorBeginIterCount<5)
    // if (!sensor.begin())
    {
        sensorBeginIterCount++;
        Serial.println(F("Failed to boot VL53L0X leftDistanceSensor"));
        delay(1000);
    }

    sensorBeginIterCount = 0;
    while (!rightDistanceSensor.begin(VL53L0X_I2C_ADDR, false,&Wire1) && sensorBeginIterCount<5)    
    {
        sensorBeginIterCount++;
        Serial.println(F("Failed to boot VL53L0X rightDistanceSensor"));
        delay(1000);
    }
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

    while (true)
    {

        // Выполняем измерение расстояния
        leftDistanceSensor.rangingTest(&leftDistanceMeasure, false);

            leftSensorValues[leftSensorValueIndex] = leftDistanceMeasure.RangeMilliMeter;
            leftSensorValueIndex = (leftSensorValueIndex + 1) % WINDOW_SIZE;

            float avgDist = 0.0;
            for (int i = 0; i < WINDOW_SIZE; i++)
            {
                avgDist += leftSensorValues[i];
            }
            avgDist /= WINDOW_SIZE;

            if (avgDist >= VLX_THRESHOLD || leftDistanceMeasure.RangeStatus == 4)
            {
                if(leftDistanceMeasure.RangeStatus == 4)
                    Serial.println(F("Left Out of range")); // Если расстояние недоступно, выводим сообщение

                if (isHandNearLeft)
                {
                    leftDistanceLongAttnBegin = false;
                    // Рука ушла — проверяем, сколько длилось
                    unsigned long duration = millis() - leftOcupStartTime;
                    if (duration < LONG_DIST_ATTN_DURATION_MS)
                    {
                        Serial.print("⚠️Left Кратковременное приближение: ");
                        Serial.print(duration);
                        Serial.println(" мс — пропущено");
                        leftDistanceShortAttn();
                    }
                    isHandNearLeft = false;
                }
            }
            // Проверка: близко ли рука?
            if (avgDist < VLX_THRESHOLD)
            {
                if (!isHandNearLeft)
                {
                    // Рука только что поднеслась — фиксируем старт
                    isHandNearLeft = true;
                    leftOcupStartTime = millis();
                }else{
                    
                    unsigned long duration = millis() - leftOcupStartTime;
                    if (duration >= SHORT_DIST_ATTN_DURATION_MS)
                    {

                    }

                    if (duration >= LONG_DIST_ATTN_DURATION_MS)
                    {
                        Serial.print("✅Left Рука поднесена на ");
                        Serial.print(duration);
                        Serial.println(" мс (≥ " + String(LONG_DIST_ATTN_DURATION_MS) + " мс)");
                        if (!leftDistanceLongAttnBegin)
                            leftDistanceLongAttn();

                    }
                }
            }
        delay(30);
    }
}



void Proj42Events::StartRightSensorsThread(void *_this)
{
    ((Proj42Events *)_this)->RightSensorsTask();
    vTaskDelete(NULL);
}

void Proj42Events::RightSensorsTask()
{

    while (true)
    {

        // Выполняем измерение расстояния
        rightDistanceSensor.rangingTest(&rightDistanceMeasure, false);

            rightSensorValues[rightSensorValueIndex] = rightDistanceMeasure.RangeMilliMeter;
            rightSensorValueIndex = (rightSensorValueIndex + 1) % WINDOW_SIZE;

            float avgDist = 0.0;
            for (int i = 0; i < WINDOW_SIZE; i++)
            {
                avgDist += rightSensorValues[i];
            }
            avgDist /= WINDOW_SIZE;

            if (avgDist >= VLX_THRESHOLD || rightDistanceMeasure.RangeStatus == 4)
            {
                if(rightDistanceMeasure.RangeStatus == 4)
                    Serial.println(F("Right Out of range")); // Если расстояние недоступно, выводим сообщение

                if (isHandNearRight)
                {
                    rightDistanceLongAttnBegin = false;
                    // Рука ушла — проверяем, сколько длилось
                    unsigned long duration = millis() - rightOcupStartTime;
                    if (duration < LONG_DIST_ATTN_DURATION_MS)
                    {
                        Serial.print("Right Кратковременное приближение: ");
                        Serial.print(duration);
                        Serial.println(" мс — пропущено");
                        leftDistanceShortAttn();
                    }
                    isHandNearRight = false;
                }
            }
            // Проверка: близко ли рука?
            if (avgDist < VLX_THRESHOLD)
            {
                if (!isHandNearRight)
                {
                    // Рука только что поднеслась — фиксируем старт
                    isHandNearRight = true;
                    leftOcupStartTime = millis();
                }else{
                    
                    unsigned long duration = millis() - rightOcupStartTime;
                    if (duration >= SHORT_DIST_ATTN_DURATION_MS)
                    {

                    }

                    if (duration >= LONG_DIST_ATTN_DURATION_MS)
                    {
                        Serial.print("✅Left Рука поднесена на ");
                        Serial.print(duration);
                        Serial.println(" мс (≥ " + String(LONG_DIST_ATTN_DURATION_MS) + " мс)");
                        if (!rightDistanceLongAttnBegin)
                            rightDistanceLongAttn();

                    }
                }
            }
        delay(30);
    }
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
    leftDistanceLongAttnBegin = true;
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
    rightDistanceLongAttnBegin = true;
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
