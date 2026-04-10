#include "proj42.hpp"

Proj42 *Proj42::instance;

Proj42::Proj42()
{
   
    displayHelper = new DisplayHelper(this);
    eventsHelper = new Proj42Events(this);
    servoHelper = new ServoHelper(this);
    // rtcManager = new RtcManager(this);
   
     
    instance = this;
    sprintf(timeStr, "Hi!");
    // touchHelper = new TouchHelper();
}


void Proj42::MemInfo()
{    
    Serial.printf("\nTotal heap: %d\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());    
    Serial.printf("CPU F: %d\n", getCpuFrequencyMhz());
    Serial.printf("Xtal F: %d\n", getXtalFrequencyMhz());    
    Serial.printf("Apb F: %d\n", getApbFrequency());
    
    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());    
    Serial.printf("Used PSRAM: %d\n\n", ESP.getPsramSize() - ESP.getFreePsram());
    // uint32_t getXtalFrequencyMhz();
}


void Proj42::LowPowMode(){
    analogWrite(LCD_BL, 40);
    setCpuFrequencyMhz(80);
    // MemInfo();
}

void Proj42::NormalPowMode(){
    analogWrite(LCD_BL, 255);   
    setCpuFrequencyMhz(160);
    // MemInfo();
}

void Proj42::HighPowMode(){
    analogWrite(LCD_BL, 255);     
    setCpuFrequencyMhz(240);
    // MemInfo();
}

void Proj42::Init()
{
    // HighPowMode();    
    // configHelper->LoadConfig("/config.json");
    // batteryHelper->InitBattery();   
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(LCD_BL, OUTPUT);
    pinMode(LED_PHILAMENT_LEFT_PIN, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    // rtcManager->setup();
    // runTaskPriotity(&Proj42::LightLikeAnimation, this, "LightLikeAnimation");
    webServer = new WebServerManager3(this);
    displayHelper->InitDisplay();
    eventsHelper->InitSensors();
    delay(200);    
    webServer->init();
    

    // servoHelper->testServos();    
    // touchHelper->InitTouch();    
    // gyroHelper->InitGyro();
    // audioHelper->InitAudio();
    // jsRunner->jsInit();
    // audioHelper->PlayWav("/audio/woof1.wav");
    // // luluCharacter->doRandomReact(-1,false);
    // dogEvents->StartDogActivitiWatcher();
    // dogEvents->StartSlavePingThread();
    // i2cSlaveHelper->StartReciveThread();
    // // ShowMenu();
    // // if (configHelper->EnableWifi)
    //     fsWebServer->Init();

    // NormalPowMode();
    // jsRunner->jsEvalFile("/js/script1.js");
    // jsRunner->jsEvalFile("/js/demo.js");
}

void Proj42::LightLikeAnimation(){
    auto curBrightness = GetLightBrightness();
    analogWrite(LED_PHILAMENT_LEFT_PIN, 0); 
    delay(500);
    byte maxB  = 30;
    for (byte brightness = 0; brightness <= maxB; brightness++) {
        analogWrite(LED_PHILAMENT_LEFT_PIN, brightness);
        analogWrite(LED_PHILAMENT_RIGHT_PIN, brightness);   // Устанавливаем PWM (0-255)
        delay(50);  // Задержка для плавности
    }

    for (byte brightness = maxB; brightness > 1; brightness--) {
        analogWrite(LED_PHILAMENT_LEFT_PIN, brightness); 
        analogWrite(LED_PHILAMENT_RIGHT_PIN, brightness);    // Устанавливаем PWM (0-255)
        delay(50);  // Задержка для плавности
    }
    SetLightBrightness(curBrightness);
}