#include "proj42.hpp"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

static Proj42 proj42;


void setup()
{

    Wire.begin(18, 19);    
    Wire1.begin(32,33);
    Serial.begin(115200);
    Serial.println("alive");
    proj42.Init();
    proj42.NormalPowMode();
}

void loop()
{
    // Serial.println("alive");
    proj42.MemInfo();
    delay(10000);
}
