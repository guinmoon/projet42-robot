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


void Proj42::Init()
{
    // HighPowMode();    
    // configHelper->LoadConfig("/config.json");
    // batteryHelper->InitBattery();
    webServer = new WebServerManager();
    displayHelper->InitDisplay();
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