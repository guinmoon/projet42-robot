#include "proj42.hpp"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

static Proj42 proj42;

Adafruit_VL53L0X sensor = Adafruit_VL53L0X();

void setup()
{

    Wire.begin(17, 5);    
    Wire1.begin(32,33);
    // delay(1000);    
    // if (!sensor.begin())
    // {
    //     Serial.println(F("Failed to boot VL53L0X"));
    //     delay(1000);
    // }
    Serial.begin(115200);
    Serial.println("alive");
    proj42.Init();
}

void loop()
{
    Serial.println("alive");
    delay(10000);
}
