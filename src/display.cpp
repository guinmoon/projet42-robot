#include "display.h"
#include "proj42.hpp"
#include "LuLuEyes.h"
#include "global_def.h"
#include "FFat.h"


// Arduino_DataBus *DisplayHelper::bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
// Arduino_GFX *DisplayHelper::gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
//                                                      1 /* rotation */, true /* IPS */, LCD_HEIGHT, LCD_WIDTH, 0, 20, 0, 0);

LGFX_MyDisplay *DisplayHelper::gfx;
LuLuEyes *DisplayHelper::luluEyes;
LGFX_Sprite *DisplayHelper::eyesSprite;
LGFX_Sprite *DisplayHelper::timeSprite;

AnimatedGIF DisplayHelper::gif;

DigitalRainAnimation<LGFX_MyDisplay> DisplayHelper::matrix_effect = DigitalRainAnimation<LGFX_MyDisplay>();
Proj42 *DisplayHelper::proj42;
int DisplayHelper::xOffset = 0;
int DisplayHelper::yOffset = 0;

uint16_t* DisplayHelper::usTemp;


// DisplayHelper::DisplayHelper()
// {    
//     gfx = new LGFX_MyDisplay();
//     batterySprite = new LGFX_Sprite(gfx);
//     batterySprite->setPsram(true);
//     batterySprite->createSprite(120, 40);
// }

DisplayHelper::DisplayHelper(Proj42 *_proj42)
{
    proj42 = _proj42;
    gfx = new LGFX_MyDisplay();
    

    timeSprite = new LGFX_Sprite(gfx);
    timeSprite->setPsram(false);
    timeSprite->createSprite(240, 100);

}

void DisplayHelper::stopSleepAnimation()
{
    wake = true;
    DisplayOn();
}

void DisplayHelper::showSleepAnimation()
{
    DisplayOff();
}

// void DisplayHelper::DisplayOn()
// {
//     gfx->endWrite();
//     gfx->setBrightness(255);
//     digitalWrite(LCD_BL, HIGH);
//     // DisplayHelper::gfx->displayOn();
// }

// void DisplayHelper::DisplayOff()
// {
//     // DisplayHelper::gfx->displayOff();
//     gfx->endWrite();
//     gfx->setBrightness(0);
//     digitalWrite(LCD_BL, LOW);
// }

// void Arduino_ST7789::displayOn(void)
// {
//   _bus->sendCommand(0x11);
//   delay(120);
// }

// void Arduino_ST7789::displayOff(void)
// {
//   _bus->sendCommand(0x10);
//   delay(120);
// }

void DisplayHelper::InitDisplay()
{
    // gfx->init();
    gfx->setRotation(3);

    if (!gfx->init())
    {
        log_d("gfx->begin() failed!");
    }
    fillScreen();

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    // gfx->setCursor(10, 10);
    // gfx->setTextColor(TFT_RED);
    // gfx->println("Hello World!");

    luluEyes = new LuLuEyes();    
    eyesSprite = new LGFX_Sprite(gfx);
    eyesSprite->setPsram(true);    
    eyesSprite->createSprite(gfx->width(), gfx->height() - EYEBORDER * 2);        
    luluEyes->begin(gfx->width(), gfx->height() - EYEBORDER * 2, eyesSprite); // screen-width, screen-height, max framerate

   

    // eyesSprite->createSprite(gfx->width(), gfx->height() );        
    // luluEyes->begin(gfx->width(), gfx->height() , eyesSprite);

    // Define some automated eyes behaviour
    // luluEyes->anim_hearts();
    // luluEyes->anim_catEyes();
    luluEyes->setAutoblinker(ON, 3, 2); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
    luluEyes->setIdleMode(ON, 2, 2);    
    luluEyes->setSpacebetween(40);
    

    InitMatrixAnimation();
    // showMatrixAnimation = true;
    // luluEyes->setHeartMode(true, 3000);
    // pauseEyes = true;
    xTaskCreatePinnedToCore(
        this->StartEyesUpdateThread, /* Task function. */
        "Task1",                     /* name of task. */
        10000,                       /* Stack size of task */
        this,                        /* parameter of the task */
        2 | portPRIVILEGE_BIT,       /* priority of the task */
        NULL,                      /* Task handle to keep track of created task */
        0);
    

    // luluEyes->setPosition(S);
    // luluEyes->anim_fallingAsleep();
    // delay(10000);
    // luluEyes->anim_wakeUp();
    // showEyes = false;
    // ShowClock(4000);

    // pTurboBuffer = (uint8_t *)heap_caps_malloc(TURBO_BUFFER_SIZE + (280 * 240), MALLOC_CAP_8BIT);
    // pFrameBuffer = (uint8_t *)heap_caps_malloc(280 * 240 * sizeof(uint16_t), MALLOC_CAP_8BIT);
    // pFrameBuffer = (uint8_t *)ps_malloc(280 * 240 * sizeof(uint32_t));
    // usTemp = (uint16_t *)ps_malloc(sizeof(uint16_t)*280);
}

void DisplayHelper::HeartAnimation(){
    luluEyes->setIdleMode(OFF, 2, 2);
    luluEyes->setAutoblinker(OFF, 3, 2);
    luluEyes->cleanEyes();
    luluEyes->anim_hearts();
    luluEyes->cleanEyes();
    luluEyes->setAutoblinker(ON, 3, 2);
    luluEyes->setIdleMode(ON, 2, 2);
    // luluEyes->setAutoblinker(ON, 3, 2); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
    // luluEyes->setIdleMode(ON, 2, 2);
}

void DisplayHelper::Confused(){
    luluEyes->anim_confused();
}

void DisplayHelper::LookUp(){
    luluEyes->setPosition(N);
}



void DisplayHelper::LookLeft(){
    luluEyes->setPosition(W);
}

void DisplayHelper::Laugh(){
    luluEyes->anim_laugh();
}

void DisplayHelper::Angry(int t){
    luluEyes->angry = true;    
    vTaskDelay(pdMS_TO_TICKS(t));
    luluEyes->angry = false;
}

// void DisplayHelper::DrawBatteryThread(void* _this){
//     ((DisplayHelper *)_this)->DrawBatteryTask();
//     vTaskDelete(NULL);
// }

// void DisplayHelper::DrawBatteryTask(){
//     drawBatteryheart();
//     while (1){ 
//         delay(1000);
//         // if (!playGif &&  eyesPaused)
//         //    continue;        
//         drawBatteryheart();
//     }
// }

void DisplayHelper::setIdleMode(bool enable){
    if (enable){
        this->luluEyes->setIdleMode(ON, 2, 2);
    }else{
        this->luluEyes->setIdleMode(OFF, 2, 2);
    }
}


void DisplayHelper::pauseEyes()
{
    showEyes = false;
    gfx->clearDisplay();
}

void DisplayHelper::resumeEyes()
{
    showTime = false;
    showMatrixAnimation = false;       
    gfx->fillRect(0,0, 240, 80, TFT_BLACK);   
    gfx->fillRect(0,160, 240, 240, TFT_BLACK);
    vTaskDelay(pdMS_TO_TICKS(100));
    // this->gfx->clearDisplay();
    showEyes = true;    
    
}



void DisplayHelper::StartEyesUpdateThread(void *_this)
{
    ((DisplayHelper *)_this)->EyesUpdateTask();
    vTaskDelete(NULL);
}

void DisplayHelper::EyesUpdateTask()
{
    int dtShows = 0;
    while (true)
    {
        
        if (showEyes)
        {
            luluEyes->update();
            vTaskDelay(pdMS_TO_TICKS(30));                       
        }
        if (showTime){
            // luluEyes->cleanEyes();
            dtShows++;
            if (dtShows >= 10){
                DrawDateTime();
                dtShows=0;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (showMatrixAnimation){
            matrix_effect.loop();
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        if (!showEyes && !showTime && !showMatrixAnimation){
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        
    }
}

void DisplayHelper::DrawDateTime(){
        
    if (proj42->webServer == nullptr)
        return;
    
    int bX = 10;
    int bY = 10;
    int sX = 0;
    int sY = 80;
    // int sY = 0;
    timeSprite->setFont(&fonts::FreeMonoBold24pt7b);
    timeSprite->setCursor(bX, bY);
    timeSprite->setTextColor(TFT_GREEN);
    timeSprite->setTextSize(1.5);
    timeSprite->fillRect(bX,bY, 240-bX, bY+50, TFT_BLACK);    
    timeSprite->println(proj42->webServer->timeStr);
    timeSprite->pushSprite(sX,sY);
    
    
    // gfx->println("00:00");
}

void DisplayHelper::ShowClock(int delay1)
{
    showEyes = false;    
    showTime = false;    
    showMatrixAnimation = true;
    vTaskDelay(pdMS_TO_TICKS(delay1));        
    showMatrixAnimation = false;
    vTaskDelay(pdMS_TO_TICKS(100));
    showTime = true;
}


void DisplayHelper::SetEyePosition(int x, int y)
{
    // eyeLxNext = getScreenConstraint_X() / 2;
    //         eyeLyNext = 0;
    luluEyes->eyeLxNext = x;
    luluEyes->eyeLyNext = y;
}

// void *DisplayHelper::GIFAlloc(uint32_t u32Size)
// {
//     // return heap_caps_malloc(u32Size, MALLOC_CAP_SPIRAM);
//     return (uint8_t *)ps_malloc(u32Size);
// } /* GIFAlloc() */

void DisplayHelper::PlayGif(const char *fname)
{
    playGif = false;
    pauseEyes();
    delay(100);
    if (gifData != nullptr)
        free(gifData);
    // gifData = nullptr;
    // gifSize = 0;

    if (!loadGIFToMemory(fname))
    {
        Serial.printf("Failed to load GIF to memory play = false");
        // drawBatteryheart();
        playGif = false;
        return;
    }

    // gif.begin(GIF_PALETTE_RGB888);
    gif.begin(GIF_PALETTE_RGB565_BE);

    if (!gif.open(gifData, gifSize, GIFDraw))
    {
        Serial.printf("Failed to open GIF from memory");
        free(gifData);
        return;
    }
    // gif.setDrawType(GIF_DRAW_COOKED);
    gif.setFrameBuf(pFrameBuffer); // for Turbo+cooked, we need to supply a full sized output framebuffer
    // gif.setTurboBuf(pTurboBuffer);
    // gif.allocFrameBuf(GIFAlloc);
    // gif.allocTurboBuf(GIFAlloc);

    playGif = true;
    xTaskCreatePinnedToCore(
        this->PlayInfiniteThread, /* Task function. */
        "Task1",                  /* name of task. */
        10000,                    /* Stack size of task */
        this,                     /* parameter of the task */
        2 | portPRIVILEGE_BIT,    /* priority of the task */
        NULL,                   /* Task handle to keep track of created task */
        0);
    // fillScreen();
}



// Turbo COOKED
//  void DisplayHelper::GIFDraw(GIFDRAW *pDraw)
//  {
//      gfx->startWrite();
//      // if (pDraw->y == 0)
//      // { // set the memory window (once per frame) when the first line is rendered
//          gfx->setAddrWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
//      // }
//      // For all other lines, just push the pixels to the display. We requested 'COOKED'big-endian RGB565 and
//      gfx->writePixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
//      gfx->endWrite();
//  }

// //ARUINO_GFX
void DisplayHelper::GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s = pDraw->pPixels;
    uint16_t *usPalette = pDraw->pPalette;
    int y = pDraw->iY + pDraw->y;
    int iWidth = pDraw->iWidth;
    if (iWidth > LCD_WIDTH)
    {
        iWidth = LCD_WIDTH;
    }
    gfx->startWrite();
    // Handle background restoration
    if (pDraw->ucDisposalMethod == 2)
    {
        uint8_t ucBackground = pDraw->ucBackground;
        for (int x = 0; x < iWidth; x++)
        {
            if (s[x] == pDraw->ucTransparent)
                s[x] = ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Optimize transparent handling
    if (pDraw->ucHasTransparency)
    {
        uint8_t ucTransparent = pDraw->ucTransparent;
        uint16_t *d = usTemp;
        int x = 0;
        while (x < iWidth)
        {
            // Count non-transparent pixels
            int count = 0;
            while (x + count < iWidth && s[x + count] != ucTransparent)
            {
                usTemp[count] = usPalette[s[x + count]];
                count++;
            }

            // Draw non-transparent block
            if (count > 0)
            {
                gfx->setAddrWindow((pDraw->iX + x) + xOffset, y + yOffset, count, 1);
                gfx->writePixels((uint16_t *)usTemp, count);
                // gfx->pushImage(pDraw->iX + x, y, count, 1,(uint16_t *)usTemp);
                x += count;
            }

            // Skip transparent pixels
            while (x < iWidth && s[x] == ucTransparent)
                x++;
        }
    }
    else
    {
        // Direct conversion for non-transparent images
        for (int x = 0; x < iWidth; x++)
        {
            usTemp[x] = usPalette[s[x]];
        }
        // gfx->pushImage(pDraw->iX, y,  iWidth, 1,(uint16_t *)usTemp);
        
        gfx->setAddrWindow(pDraw->iX + xOffset, y + yOffset, iWidth, 1);
        gfx->writePixels(usTemp, iWidth);        
    }
    gfx->endWrite();
}



bool DisplayHelper::loadGIFToMemory(const char *filename)
{
    File file = FFat.open(filename, "r");
    if (!file)
    {
        Serial.printf("Failed to open GIF file %s ", filename);
        return false;
    }

    gifSize = file.size();    
    gifData = (uint8_t *)ps_malloc(gifSize);

    // luluDog->MemInfo();
    Serial.printf("GifSize: %d Used PSRAM: %d", gifSize, ESP.getPsramSize() - ESP.getFreePsram());
    
    if (!gifData)
    {
        Serial.printf("Failed to allocate memory for GIF");
        file.close();
        return false;
    }

    file.read(gifData, gifSize);
    file.close();
    return true;
}

void DisplayHelper::StopGif()
{
    playGif = false;
    // luluDog->MemInfo();
}

void DisplayHelper::fillScreen()
{
    // gfx->fillScreen(BLACK);
    gfx->clear();
}

void DisplayHelper::printOnDisplay(char *text, int x, int y)
{
    //
    gfx->setCursor(x, y);
    gfx->setTextColor(TFT_RED);
    gfx->fillRect(x, y, x + 110, y + 20, 0);
    gfx->println(text);
}

// void DisplayHelper::drawHeart(int x, int y, uint16_t color)
// {
//     batterySprite->fillRect(x + 15, y + 15, 10, 10, color);
//     batterySprite->fillRect(x + 28, y + 15, 10, 10, color);
//     batterySprite->fillRect(x + 22, y + 21, 10, 10, color);
// }

// void DisplayHelper::drawBatteryheart()
// {
//     // printOnDisplay(voltageBuf);
//     float volt = luluDog->batteryHelper->get_battery_voltage();
//     int heartColor = TFT_RED;
//     if (luluDog->batteryHelper->isCharging())
//     {
//         heartColor = TFT_GREEN;
//         // printOnDisplay(voltageBuf,10,50);
//     }
//     if (volt <= 3.1)
//         drawHeart(0, 0, TFT_BLACK);
//     else
//         drawHeart(0, 0, heartColor);
//     if (volt <= 3.5)
//         drawHeart(30, 0, TFT_BLACK);
//     else
//         drawHeart(30, 0, heartColor);
//     if (volt <= 3.7)
//         drawHeart(60, 0, TFT_BLACK);
//     else
//         drawHeart(60, 0, heartColor);
    
//     batterySprite->pushSprite(0,0);
//     // batterySprite->deleteSprite();
// }

// void DisplayHelper::setVoltageBuf(float voltage)
// {
//     if (luluDog->batteryHelper->isCharging())
//     {
//         sprintf(voltageBuf, "B: %f V Charging", voltage);
//     }
//     else
//     {
//         sprintf(voltageBuf, "B: %f V", voltage);
//     }
// }

void DisplayHelper::PlayInfiniteThread(void *_this)
{
    ((DisplayHelper *)_this)->PlayInfiniteTask();
    vTaskDelete(NULL);
}

void DisplayHelper::PlayInfiniteTask()
{
    int iter = 0;
    while (playGif /*&& iter<GifPlayTime*/)
    {
        pauseEyes();
        int res = gif.playFrame(true, NULL);
        if (res == -1)
        {
            Serial.printf("play error");
            vTaskDelete(NULL);
            return;
        }
        if (res == 0)
        {
            Serial.printf("play ended reopen");
            iter++;
            gif.close();
            gif.open(gifData, gifSize, GIFDraw);
        }        
    }
    if (playGif){
        resumeEyes();
        playGif = false;
    }
    Serial.printf("play ended");
}

// void DisplayHelper::MatrixAnimationThread(void *_this)
// {
//     while (showMatrixAnimation)
//     {
//         matrix_effect.loop();
//         delay(70);
//     }
//     vTaskDelete(NULL);
// }

// void DisplayHelper::ShowMatrixAnimation()
// {
//     if (!matrixEffectInited)
//     {
//         matrixEffectInited = true;
//         InitMatrixAnimation();
//         // delay(300);
//     }
//     showMatrixAnimation = true;
//     xTaskCreatePinnedToCore(
//         this->MatrixAnimationThread, /* Task function. */
//         "Task1",                     /* name of task. */
//         10000,                       /* Stack size of task */
//         this,                        /* parameter of the task */
//         2 | portPRIVILEGE_BIT,       /* priority of the task */
//         NULL,                      /* Task handle to keep track of created task */
//         0);
// }

void DisplayHelper::StopMatrixAnimation()
{
    showMatrixAnimation = false;
}

void DisplayHelper::InitMatrixAnimation()
{
    // matrix_effect.timeStr = proj42->webServer->timeStr;
    matrix_effect.init(gfx);    
}

// void DisplayHelper::LvglDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
// {
//     if (luluDog->lvglHelper->lvglExit)
//     {
//         Serial.printf("LVGL EXIT");
//         lv_disp_flush_ready(disp);
//         sleep(300);
//         return;
//     }
//     uint32_t w = (area->x2 - area->x1 + 1);
//     uint32_t h = (area->y2 - area->y1 + 1);
//     if (w <= 0 && h <= 0)
//     {
//         lv_disp_flush_ready(disp);
//         return;
//     }
//     gfx->startWrite();
//     gfx->setAddrWindow(area->x1, area->y1, w, h);
//     gfx->writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
//     gfx->endWrite();
//     lv_disp_flush_ready(disp);
// }

// DisplayHelper displayHelper;