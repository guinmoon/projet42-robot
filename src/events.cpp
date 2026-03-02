
#include "events.hpp"
#include "proj42.hpp"

Proj42 *Proj42Events::proj42;
Adafruit_VL53L0X Proj42Events::leftDistanceSensor = Adafruit_VL53L0X();

Proj42Events::Proj42Events(Proj42 *_proj42)
{
    for (int i = 0; i < WINDOW_SIZE; i++)
        sensorValues[i] = 180.0;
    pinMode(TOUCH_PIN, INPUT);
    proj42 = _proj42;
}

void Proj42Events::InitSensors()
{
    // Adafruit_VL53L0X sensor = Adafruit_VL53L0X();
    // leftDistanceSensor = new Adafruit_VL53L0X();
    // delay(1000);
    while (!leftDistanceSensor.begin())
    // if (!sensor.begin())
    {
        Serial.println(F("Failed to boot VL53L0X"));
        delay(1000);
    }

    xTaskCreatePinnedToCore(
        this->StartTouchThread, /* Task function. */
        "StartTouchThread",     /* name of task. */
        10000,                  /* Stack size of task */
        this,                   /* parameter of the task */
        2 | portPRIVILEGE_BIT,  /* priority of the task */
        NULL,                   /* Task handle to keep track of created task */
        1);

    xTaskCreatePinnedToCore(
        this->StartSensorsThread, /* Task function. */
        "StartSensorsThread",     /* name of task. */
        10000,                    /* Stack size of task */
        this,                     /* parameter of the task */
        tskIDLE_PRIORITY,         /* priority of the task */
        NULL,                     /* Task handle to keep track of created task */
        1);
    // tskIDLE_PRIORITY
}

void Proj42Events::StartSensorsThread(void *_this)
{
    ((Proj42Events *)_this)->SensorsTask();
    vTaskDelete(NULL);
}

void Proj42Events::SensorsTask()
{

    while (true)
    {

        // Выполняем измерение расстояния
        leftDistanceSensor.rangingTest(&leftDistanceMeasure, false);

        if (leftDistanceMeasure.RangeStatus != 4)
        { // Проверяем, что измерение действительно
            // Выводим измеренное расстояние в миллиметрах
            // Serial.print(F("Range (mm): "));
            // Serial.println(leftDistanceMeasure.RangeMilliMeter);
            sensorValues[valueIndex] = leftDistanceMeasure.RangeMilliMeter;
            valueIndex = (valueIndex + 1) % WINDOW_SIZE;

            float avgDist = 0.0;
            for (int i = 0; i < WINDOW_SIZE; i++)
            {
                avgDist += sensorValues[i];
            }
            avgDist /= WINDOW_SIZE;

            // Проверка: близко ли рука?
            if (avgDist < THRESHOLD)
            {
                if (!isHandNear)
                {
                    // Рука только что поднеслась — фиксируем старт
                    isHandNear = true;
                    startTime = millis();
                }else{
                    
                    unsigned long duration = millis() - startTime;
                    if (duration >= SHORT_DIST_ATTN_DURATION_MS)
                    {

                    }

                    if (duration >= LONG_DIST_ATTN_DURATION_MS)
                    {
                        Serial.print("✅ Рука поднесена на ");
                        Serial.print(duration);
                        Serial.println(" мс (≥ " + String(LONG_DIST_ATTN_DURATION_MS) + " мс)");
                        if (!leftDistanceLongAttnBegin)
                            leftDistanceLongAttn();
                        leftDistanceLongAttnBegin = true;
                        // Здесь можно вывести сигнал, включить LED и т.д.
                    }
                }
            }
            else
            {
                if (isHandNear)
                {
                    leftDistanceLongAttnBegin = false;
                    // Рука ушла — проверяем, сколько длилось
                    unsigned long duration = millis() - startTime;
                    if (duration < LONG_DIST_ATTN_DURATION_MS)
                    {
                        Serial.print("⚠️ Кратковременное приближение: ");
                        Serial.print(duration);
                        Serial.println(" мс — пропущено");
                    }
                    isHandNear = false;
                }
            }
        }
        else
        {
            Serial.println(F("Out of range")); // Если расстояние недоступно, выводим сообщение
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
}

void Proj42Events::leftDistanceLongAttn()
{

    HasAttn();
    Proj42::runTask(&ServoHelper::LeftAttnAnimMove, proj42->servoHelper, "LeftAttnAnimMove");
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
