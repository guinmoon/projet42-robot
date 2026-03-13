#include "proj42.hpp"

Proj42 *Proj42::instance;

Proj42::Proj42()
{
    // configHelper = new ConfigHelper(this);
    // dogEvents = new DogEvents(this);
    // i2cSlaveHelper = new I2CSlaveHelper(this);    
    // batteryHelper = new BatteryHelper(this);
    // audioHelper = new AudioHelper(this);    
    displayHelper = new DisplayHelper(this);
    eventsHelper = new Proj42Events(this);
    servoHelper = new ServoHelper(this);
   
    // luluCharacter = new LuLuCharacter(this);
    // gyroHelper = new GyroHelper(this);
    // touchHelper = new TouchHelper(this);
    // lvglHelper = new LVGLHelper(this);
    // jsRunner = new JSRunner(this);
    // fsWebServer = new LuLuWebServer(this);    
    instance = this;
    
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
    Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
    // uint32_t getXtalFrequencyMhz();
}


void Proj42::LowPowMode(){
    setCpuFrequencyMhz(80);
    // MemInfo();
}

void Proj42::NormalPowMode(){
    setCpuFrequencyMhz(160);
    // MemInfo();
}

void Proj42::HighPowMode(){
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
    digitalWrite(BUILTIN_LED, HIGH);
    webServer = new WebServerManager();
    displayHelper->InitDisplay();
    eventsHelper->InitSensors();
    delay(1000);    
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