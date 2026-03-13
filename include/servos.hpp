#ifndef SERVOS_H
#define SERVOS_H

#include "global_def.h"
#include "servos_conf.h"
#include <ESP32Servo.h>



class Proj42;

class ServoHelper
{
private:
    static Proj42* proj42;    
    Servo* servo_main;
    // Servo servo_left_back;

    // int currentPos[5] = {SERVO_90, 155, 155, SERVO_90, 0}; //sit position
    int currentPos[SERVOS_COUNT] = {SERVO_90}; //sit position
    int targetPos[SERVOS_COUNT] = {SERVO_90}; //sit position
    int servo_speed[SERVOS_COUNT] = {1};


    unsigned long lastUpdateTime = 0;
public:

    ServoHelper(Proj42* _proj42);

    static void StartServosUpdateThread(void *_this);
    void ServosUpdateTask();    

    void attachServos();
    void detachServos();
    void setTargetPosAndSpeed(int servo_ind, int pos, int _speed);
    void applyServoPos(int servo_ind, int pos);              
    
    int getCurrentServoPos(int servo_ind);    
    void updateServos();    

    void testServos();
    void HeartAnimMove();
    void LeftAttnAnimMove();
    void RightAttnAnimMove();
    void GoHome();

    void WaitAndDetach();
    bool InMove = false;
    static void WaitAndDetachThread(void *_this);

};

#endif