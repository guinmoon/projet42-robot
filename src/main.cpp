#include "proj42.hpp"

static Proj42 proj42;

void setup()
{
    Serial.begin(115200);
    proj42.Init();
}

void loop()
{
    Serial.println("alive");
    delay(10000);
}
