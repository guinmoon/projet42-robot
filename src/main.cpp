// #include <ESP32Servo.h>
#include "proj42.hpp"

// static const int servoPin = 19;
// static const int servoPin2 = 33;

// Servo servo1;
// Servo servo2;
static Proj42 proj42;

// void ServoInit()
// {
//     for (int posDegrees = 90; posDegrees <= 180; posDegrees++)
//     {
//         servo1.write(posDegrees);        
//         Serial.println(posDegrees);
//         delay(20);
//     }

//     for (int posDegrees = 180; posDegrees >= 90; posDegrees--)
//     {
//         servo1.write(posDegrees);        
//         Serial.println(posDegrees);
//         delay(20);
//     }
//     servo1.detach();
// }

void setup()
{

    Serial.begin(115200);

    // servo1.attach(servoPin);
    // ServoInit();

    proj42.Init();
    // displayHelper = new DisplayHelper();
    // displayHelper->InitDisplay();
    
    //   servo2.attach(servoPin2);
}

void loop()
{
    Serial.println("alive");

    delay(1000);
}
