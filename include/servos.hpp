#ifndef SERVOS_H
#define SERVOS_H

#include "global_def.h"
#include "servos_def.h"
#include <ESP32Servo.h>

class Proj42;

class ServoHelper
{
private:
    static Proj42* proj42;
    const int SERVOS_COUNT = 1;
    Servo* servo_main;
    // Servo servo_left_back;
    // Servo servo_right_front;
    // Servo servo_right_back;
    // Servo servo_tail;

    // bool touched = false;

    // int tailMovesCount = 0;
    // int tailMaxMovesCount = 6;

    // int currentPos[5] = {SERVO_90, 155, 155, SERVO_90, 0}; //sit position
    // int targetPos[5] = {SERVO_90, 155, 155, SERVO_90, SERVO_90}; //sit position
    int currentPos[5] = {SERVO_90, SERVO_90, SERVO_90, SERVO_90, SERVO_90}; //sit position
    int targetPos[5] = {SERVO_90, SERVO_90, SERVO_90, SERVO_90, SERVO_90}; //sit position
    int servo_speed[5] = {1, 1, 1, 1, 0};
    // bool reverse_tail_move = false;

    // int tail_speed = 1;

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
    // void setTailMaxCount(int count);

    void testServos();
    void HeartAnimMove();
    void LeftAttnAnimMove();

    void WaitAndDetach();
    bool InMove = false;
    static void WaitAndDetachThread(void *_this);

};

#endif