
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
        if (touchTopLastT!= 0  && currentTime - touchTopLastT > TOUCH_TOP_LOST_INTERVAL_MS){
            TouchTopLostAttn();
        }
        if (lastAttnT!= 0  && currentTime - lastAttnT > LOST_ATTN_INTERVAL_MS){
            LostAttn();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Proj42Events::TouchTopLostAttn(){
    touchTopLastT = 0;
    if (touchTopCount>=5 && touchTopCount<=14)
        proj42->displayHelper->Angry(2000);
    touchTopCount = 0;
}

void Proj42Events::HasAttn(){
    
    lastAttnT = millis();
    if (!proj42->displayHelper->showEyes)
        proj42->displayHelper->resumeEyes();    
}


void Proj42Events::LostAttn(){
    // proj42->displayHelper->showTime = true;
    lastAttnT = 0;
    proj42->displayHelper->ShowClock(5000);
    // proj42->displayHelper->pauseEyes();    
}

void Proj42Events::TouchEvent(){
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
        proj42->servoHelper->HeartAnimMove();
        break;
    default:
        proj42->displayHelper->LookUp();
        break;
    }
}
