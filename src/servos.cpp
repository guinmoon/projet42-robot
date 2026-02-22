#include "servos.hpp"
#include "global_def.h"

// #include <Servo.h>
// #include "character.h"
// #include "pico_rtc_utils.h"
Proj42 *ServoHelper::proj42;

ServoHelper::ServoHelper(Proj42* _proj42){
    this->proj42 = _proj42;
    servo_main = new Servo();
    attachServos();
    xTaskCreatePinnedToCore(
        this->StartServosUpdateThread, /* Task function. */
        "TaskServos",                     /* name of task. */
        20000,                       /* Stack size of task */
        this,                        /* parameter of the task */
        2 | portPRIVILEGE_BIT,       /* priority of the task */
        NULL,                      /* Task handle to keep track of created task */
        1);    
}

int ServoHelper::getCurrentServoPos(int servo_ind){
    return(currentPos[servo_ind]);
}

void ServoHelper::attachServos()
{
    servo_main->attach(SER_MAIN_PIN, USMIN, USMAX);
    // servo_main->attach(SER_MAIN_PIN);
    // servo_left_back.attach(SER_LEFT_BACK_PIN, USMIN, USMAX);
    // servo_right_back.attach(SER_RIGHT_BACK_PIN, USMIN, USMAX);
    // servo_right_front.attach(SER_RIGHT_FRONT_PIN, USMIN, USMAX);    
    // servo_tail.attach(SER_TAIL_PIN, USMIN, USMAX);
}

void ServoHelper::detachServos()
{
    servo_main->detach();
    // servo_left_back.detach();
    // servo_right_back.detach();
    // servo_right_front.detach();    
    // servo_tail.detach();
}


void ServoHelper::testServos(){    
    delay(2000);
    this->setTargetPosAndSpeed(SER_MAIN,160,2);
    delay(2000);
    this->setTargetPosAndSpeed(SER_MAIN,90,2);
}

void ServoHelper::HeartAnimMove(){
    
}

void ServoHelper::setTargetPosAndSpeed(int servo_ind, int pos, int _speed)
{
    targetPos[servo_ind] = constrain(pos, 0, 180);
    servo_speed[servo_ind] = constrain(_speed, SERVO_MIN_SPEED, SERVO_MAX_SPEED);
}

void ServoHelper::applyServoPos(int servo_ind, int pos)
{
    // if (servo_speed[servo_ind]==0)
    //     return;
    switch (servo_ind)
    {
    case SER_MAIN:
        // servo_left_front.attach(SER_LEFT_FRONT_PIN, USMIN, USMAX);
        servo_main->write(pos);
        // servo_left_front.detach();
        // servo_left_front.writeMicroseconds(2400);
        break;
    // case SER_LEFT_BACK:
    //     // servo_left_back.attach(SER_LEFT_BACK_PIN, USMIN, USMAX);
    //     servo_left_back.write(pos);
    //     // servo_left_back.detach();
    //     break;
    // case SER_RIGHT_FRONT:
    //     // servo_right_front.attach(SER_RIGHT_FRONT_PIN, USMIN, USMAX);
    //     servo_right_front.write(180 - pos);
    //     // servo_right_front.detach();
    //     break;
    // case SER_RIGHT_BACK:
    //     // servo_right_back.attach(SER_RIGHT_BACK_PIN, USMIN, USMAX);
    //     servo_right_back.write(180 - pos);
    //     // servo_right_back.detach();
    //     break;
    // case SER_TAIL:
    //     // servo_tail.attach(SER_TAIL_PIN, USMIN, USMAX);
    //     servo_tail.write(pos);
    //     // servo_tail.detach();
    //     // Serial.printf("Tail pos: %i\n",pos);
    //     break;
    }
}






// Функция обновления сервоприводов
void ServoHelper::updateServos()
{
    unsigned long currentTime = millis(); 
        
    // if (currentTime - lastUpdateTime >= SER_UPDATE_INTERVAL)
    // {
    //     touched = digitalRead(TOUCH_PIN);
    //     if (touched != 0)
    //     {
    //         Serial.println("Touch ");            
    //     }
    // }
    if (currentTime - lastUpdateTime >= SER_UPDATE_INTERVAL)
    {
        // if (currentTime-getLastPingTime()>=MSEC_UNTIL_SLEEP){
        //     setLastPingTime(currentTime);
        //     // pico_sleep(RP_SLEEP_DURATION_SEC);
        // }
        lastUpdateTime = currentTime;
        // updateTail();
        for (int i = 0; i < 5; i++)
        {
            if (currentPos[i] < targetPos[i])
            {
                currentPos[i] += servo_speed[i];
                if (currentPos[i] > targetPos[i])
                    currentPos[i] = targetPos[i];
            }
            else if (currentPos[i] > targetPos[i])
            {
                currentPos[i] -= servo_speed[i];
                if (currentPos[i] < targetPos[i])
                    currentPos[i] = targetPos[i];
            }

            switch (i)
            {
            case SER_MAIN:
                this->applyServoPos(SER_MAIN, currentPos[i]);
                break;
            // case SER_LEFT_BACK:
            //     applyServoPos(SER_LEFT_BACK, currentPos[i]);
            //     break;
            // case SER_RIGHT_BACK:
            //     applyServoPos(SER_RIGHT_BACK, currentPos[i]);
            //     break;
            // case SER_RIGHT_FRONT:
            //     applyServoPos(SER_RIGHT_FRONT, currentPos[i]);
            //     break;
            // case SER_TAIL:
            //     applyServoPos(SER_TAIL, currentPos[i]);
            //     break;
            }
        }
    }else{
        // delay(1);
    }
}

void ServoHelper::StartServosUpdateThread(void *_this){
     ((ServoHelper *)_this)->ServosUpdateTask();
    vTaskDelete(NULL);
}

// Функция, работающая на втором ядре
void ServoHelper::ServosUpdateTask()
{
    while (true)
    {
        // Обновление сервоприводов выполняется на втором ядре        
        this->updateServos();
        delay(5);
    }
}
