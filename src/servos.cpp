#include "servos.hpp"
#include "global_def.h"
#include "proj42.hpp"

// #include <Servo.h>
// #include "character.h"
// #include "pico_rtc_utils.h"
Proj42 *ServoHelper::proj42;

ServoHelper::ServoHelper(Proj42* _proj42){
    this->proj42 = _proj42;
    ESP32PWM::allocateTimer(0);
    servo_main = new Servo();
    servo_main->setPeriodHertz(SERVO_FREQ);
    attachServos();
    xTaskCreatePinnedToCore(
        this->StartServosUpdateThread, /* Task function. */
        "TaskServos",                     /* name of task. */
        2048,                       /* Stack size of task */
        this,                        /* parameter of the task */
        3 | portPRIVILEGE_BIT,       /* priority of the task */
        NULL,                      /* Task handle to keep track of created task */
        1);    
    GoHome();
    // delay(1500);
    // detachServos();
}

int ServoHelper::getCurrentServoPos(int servo_ind){
    return(currentPos[servo_ind]);
}

void ServoHelper::GoHome(){
    setTargetPosAndSpeed(SER_MAIN,90,4);
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
    servo_speed[SER_MAIN] = 0;
    servo_main->detach();
    // servo_left_back.detach();
    // servo_right_back.detach();
    // servo_right_front.detach();    
    // servo_tail.detach();
}


void ServoHelper::testServos(){    
    attachServos();
    delay(2000);
    this->setTargetPosAndSpeed(SER_MAIN,160,2);
    delay(2000);
    this->setTargetPosAndSpeed(SER_MAIN,90,2);
}

void ServoHelper::HeartAnimMove(){
    Serial.println("HeartAnimMove");
    // attachServos();
    InMove = true;
    auto curPos = currentPos[SER_MAIN];
    while (proj42->eventsHelper->touchTopCount > 14){
        this->setTargetPosAndSpeed(SER_MAIN,curPos-20,2);
        delay(1500);
        this->setTargetPosAndSpeed(SER_MAIN,curPos+20,2);
        delay(1500);
        this->setTargetPosAndSpeed(SER_MAIN,SERVO_90,3);
        delay(500);
    }
    WaitAndDetach();
}


void ServoHelper::BorringAnimMove(){
    Serial.println("BorringAnimMove");
    // attachServos();
    InMove = true;
    auto curPos = currentPos[SER_MAIN];
    proj42->displayHelper->LookLeft();
    proj42->displayHelper->luluEyes->close(false,true);
    this->setTargetPosAndSpeed(SER_MAIN,curPos-50,6);
    delay(5000);
    this->setTargetPosAndSpeed(SER_MAIN,curPos,3);            
    delay(2000);
    proj42->displayHelper->LookRight();
    proj42->displayHelper->luluEyes->close(true,false);
    this->setTargetPosAndSpeed(SER_MAIN,curPos+50,6);
    delay(5000);
    this->setTargetPosAndSpeed(SER_MAIN,curPos,3);
    WaitAndDetach();
    GoHome();
}

void ServoHelper::LeftAttnAnimMove(){
    Serial.println("LeftAttnAnimMove");
    // attachServos();
    InMove = true;
    auto curPos = currentPos[SER_MAIN];
    this->setTargetPosAndSpeed(SER_MAIN,curPos-80,4);
    delay(3000);
    this->setTargetPosAndSpeed(SER_MAIN,curPos,3);        
    WaitAndDetach();
}

void ServoHelper::RightAttnAnimMove(){
    Serial.println("RightAttnAnimMove");
    // attachServos();
    InMove = true;
    auto curPos = currentPos[SER_MAIN];
    this->setTargetPosAndSpeed(SER_MAIN,curPos+80,4);
    delay(3000);
    this->setTargetPosAndSpeed(SER_MAIN,curPos,3);        
    WaitAndDetach();
}

void ServoHelper::WaitAndDetach(){
        xTaskCreatePinnedToCore(
            this->WaitAndDetachThread, /* Task function. */
            "TaskServos",                     /* name of task. */
            2048,                       /* Stack size of task */
            this,                        /* parameter of the task */
            tskIDLE_PRIORITY,       /* priority of the task */
            NULL,                      /* Task handle to keep track of created task */
            1); 
}

void ServoHelper::WaitAndDetachThread(void *_this){     
    delay(1000);
    int servCount  = SERVOS_COUNT;
    while (true){
        int okServosCount = 0;
        for (int i=0;i<servCount;i++){
            if (((ServoHelper *)_this)->targetPos[i] == ((ServoHelper *)_this)->currentPos[i]){
                okServosCount++;
                Serial.println("Servo reach target");
            }            
        }
        if (okServosCount==servCount){
            ((ServoHelper *)_this)->InMove = false;
            // delay(1000);
            // ((ServoHelper *)_this)->detachServos();
            break;
        }
        // delay(1000);
    }
    // Serial.println("Servos detached");
    vTaskDelete(NULL);
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
        // Serial.printf("Servo pos: %i\n",pos);
        // servo_left_front.detach();
        // servo_left_front.writeMicroseconds(2400);
        break;
    // case SER_LEFT_BACK:
    //     // servo_left_back.attach(SER_LEFT_BACK_PIN, USMIN, USMAX);
    //     servo_left_back.write(pos);
    //     // servo_left_back.detach();
    //     break;
    }
}






// Функция обновления сервоприводов
void ServoHelper::updateServos()
{
    // unsigned long currentTime = millis(); 
    // if (currentTime - lastUpdateTime >= SER_UPDATE_INTERVAL)
    // {
        
        // lastUpdateTime = currentTime;
        
        for (int i = 0; i < SERVOS_COUNT; i++)
        {
            if (currentPos[i] == targetPos[i])
                continue;

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
                // Serial.printf("C: %i T: %i\n",currentPos[i] , targetPos[i]);
                applyServoPos(SER_MAIN, currentPos[i]);
                break;
            // case SER_LEFT_BACK:
            //     applyServoPos(SER_LEFT_BACK, currentPos[i]);
            //     break;
            }
      }
    // }
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
        delay(SER_UPDATE_INTERVAL);
    }
}
