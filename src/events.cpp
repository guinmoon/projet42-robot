
#include "events.hpp"
#include "proj42.hpp"

Proj42 *Proj42Events::proj42;



Proj42Events::Proj42Events(Proj42* _proj42){
    pinMode(TOUCH_PIN, INPUT);
    proj42 = _proj42;
    xTaskCreatePinnedToCore(
        this->StartTouchThread, /* Task function. */
        "Task1",                     /* name of task. */
        10000,                       /* Stack size of task */
        this,                        /* parameter of the task */
        2 | portPRIVILEGE_BIT,       /* priority of the task */
        NULL,                      /* Task handle to keep track of created task */
        1);
}

void Proj42Events::StartTouchThread(void *_this)
{
    ((Proj42Events *)_this)->TouchTask();
    vTaskDelete(NULL);
}

void Proj42Events::TouchTask(){

    while (true){
        uint8_t touched = digitalRead(TOUCH_PIN);        
        if (touched != 0)
        {
            Serial.println("Touch ");
            this->TouchEvent();
        }
        auto currentTime = millis();
        if (touchTopLast!= 0  && currentTime - touchTopLast > TOUCH_TOP_LOST_INTERVAL_MS){
            touchTopLast = 0;
            touchTopCount = 0;
        }
        delay(100);
    }
}


void Proj42Events::TouchEvent(){
    touchTopCount++;    
    touchTopLast = millis();
    switch (touchTopCount)
    {
    case 5:
        proj42->displayHelper->Confused();
        break;
    case 14:
        touchTopCount = 0;
        proj42->displayHelper->HeartAnimation();
        break;
    default:
        proj42->displayHelper->LookUp();
        break;
    }
}
