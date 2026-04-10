#include "proj42.hpp"
#include <Wire.h>

static Proj42 proj42;



void setup()
{
    Serial.begin(115200);        
    Wire.begin(18, 19);    
    Wire1.begin(32,33);   
    

    Serial.println("alive");     
    
    proj42.Init();
    // proj42.NormalPowMode();
    proj42.HighPowMode();
}

void loop()
{
    // Serial.println("alive");
    proj42.MemInfo();
    // proj42.rtcManager->loop();
    delay(10000);
}



