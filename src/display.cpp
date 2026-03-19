#include "display.h"
#include "proj42.hpp"
#include "global_def.h"
#include "FFat.h"


// Arduino_DataBus *DisplayHelper::bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI);
// Arduino_GFX *DisplayHelper::gfx = new Arduino_ST7789(bus, LCD_RST /* RST */,
//                                                      1 /* rotation */, true /* IPS */, LCD_HEIGHT, LCD_WIDTH, 0, 20, 0, 0);

LGFX_MyDisplay *DisplayHelper::gfx;
LuLuEyes *DisplayHelper::luluEyes;
LGFX_Sprite *DisplayHelper::gfxSprite;
// LGFX_Sprite *DisplayHelper::timeSprite;

// AnimatedGIF DisplayHelper::gif;

DigitalRainAnimation<LGFX_Sprite> DisplayHelper::matrix_effect = DigitalRainAnimation<LGFX_Sprite>();
Proj42 *DisplayHelper::proj42;
int DisplayHelper::xOffset = 0;
int DisplayHelper::yOffset = 0;

uint16_t* DisplayHelper::usTemp;



DisplayHelper::DisplayHelper(Proj42 *_proj42)
{
    proj42 = _proj42;
    gfx = new LGFX_MyDisplay();
    
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

void DisplayHelper::DisplayOn()
{
    digitalWrite(LCD_BL, HIGH);    
}

void DisplayHelper::DisplayOff()
{     
    // gfx->setBrightness(0);
    digitalWrite(LCD_BL, LOW);
}



void DisplayHelper::InitDisplay()
{
    // gfx->init();
    gfx->setRotation(3);

    if (!gfx->init())
    {
        log_d("gfx->begin() failed!");
    }
    fillScreen();

        

    // gfx->setCursor(10, 10);
    // gfx->setTextColor(TFT_RED);
    // gfx->println("Hello World!");

    luluEyes = new LuLuEyes();   
    luluEyes->SpriteY =  EYEBORDER;
    gfxSprite = new LGFX_Sprite(gfx);
    gfxSprite->setPsram(false);    
    gfxSprite->createSprite(gfx->width(), gfx->height() - EYEBORDER * 2);      
    
    InitMatrixAnimation();
    
    luluEyes->begin(gfx->width(), gfx->height() - EYEBORDER * 8, gfxSprite); // screen-width, screen-height, max framerate
    luluEyes->setAutoblinker(ON, 3, 2); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
    luluEyes->setIdleMode(ON, 2, 2);    
    luluEyes->setSpacebetween(40);
    
    // luluEyes->setMood(HAPPY);
    
    xTaskCreatePinnedToCore(
        this->StartEyesUpdateThread, /* Task function. */
        "Task1",                     /* name of task. */
        4096,                       /* Stack size of task */
        this,                        /* parameter of the task */
        2 | portPRIVILEGE_BIT,       /* priority of the task */
        NULL,                      /* Task handle to keep track of created task */
        0);    

    DisplayOn();
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

void DisplayHelper::LookDown(){
    luluEyes->setPosition(S);
} 

void DisplayHelper::LookLeft(){
    luluEyes->setPosition(W);
}

void DisplayHelper::LookRight(){
    luluEyes->setPosition(E);
}

void DisplayHelper::Laugh(){
    luluEyes->anim_laugh();
}

void DisplayHelper::Happy(){
    luluEyes->setMood(HAPPY);
    delay(3000);
    luluEyes->setMood(0);
}

void DisplayHelper::Angry(int t){
    luluEyes->angry = true;    
    vTaskDelay(pdMS_TO_TICKS(t));
    luluEyes->angry = false;
}



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

void DisplayHelper::inAttn(){
    showMatrixAnimation = false; 
    if (luluEyes->fallingAsleep != 0){
        luluEyes->fallingAsleep = 0;
        luluEyes->fallingAsleepStage = 0;    
        luluEyes->eyeLheightNext = luluEyes->eyeLheightDefault;
        luluEyes->eyeRheightNext = luluEyes->eyeRheightDefault;
        luluEyes-> eyeL_open = 1;
        luluEyes-> eyeR_open = 1;
    }     
}

void DisplayHelper::resumeEyes()
{
    inAttn();
    if (showEyes)
        return;    
    gfxSprite->fillRect(0,0, 240, 240, TFT_BLACK);     
    luluEyes->setAutoblinker(ON, 3, 2); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
    setIdleMode(true);   
    // vTaskDelay(pdMS_TO_TICKS(100));
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
        if (showMatrixAnimation){
            matrix_effect.loop();
            gfxSprite->pushSprite(0,EYEBORDER);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        if (!showEyes  && !showMatrixAnimation){
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        
    }
}


void DisplayHelper::ShowClock(int delay1)
{
    showEyes = false;        
    showMatrixAnimation = true;
    // vTaskDelay(pdMS_TO_TICKS(delay1));        
    // showMatrixAnimation = false;
    // vTaskDelay(pdMS_TO_TICKS(100));
    // showTime = true;
}


void DisplayHelper::SetEyePosition(int x, int y)
{
    luluEyes->eyeLxNext = x;
    luluEyes->eyeLyNext = y;
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

void DisplayHelper::StopMatrixAnimation()
{
    showMatrixAnimation = false;
}

void DisplayHelper::InitMatrixAnimation()
{
    matrix_effect.drawDateTime = true;
    matrix_effect.timeStr = proj42->webServer->timeStr;
    matrix_effect.init(gfxSprite);    
}

// void *DisplayHelper::GIFAlloc(uint32_t u32Size)
// {
//     // return heap_caps_malloc(u32Size, MALLOC_CAP_SPIRAM);
//     return (uint8_t *)ps_malloc(u32Size);
// } /* GIFAlloc() */

// void DisplayHelper::PlayGif(const char *fname)
// {
//     playGif = false;
//     pauseEyes();
//     delay(100);
//     if (gifData != nullptr)
//         free(gifData);
//     // gifData = nullptr;
//     // gifSize = 0;

//     if (!loadGIFToMemory(fname))
//     {
//         Serial.printf("Failed to load GIF to memory play = false");
//         // drawBatteryheart();
//         playGif = false;
//         return;
//     }

//     // gif.begin(GIF_PALETTE_RGB888);
//     gif.begin(GIF_PALETTE_RGB565_BE);

//     if (!gif.open(gifData, gifSize, GIFDraw))
//     {
//         Serial.printf("Failed to open GIF from memory");
//         free(gifData);
//         return;
//     }
//     // gif.setDrawType(GIF_DRAW_COOKED);
//     gif.setFrameBuf(pFrameBuffer); // for Turbo+cooked, we need to supply a full sized output framebuffer
//     // gif.setTurboBuf(pTurboBuffer);
//     // gif.allocFrameBuf(GIFAlloc);
//     // gif.allocTurboBuf(GIFAlloc);

//     playGif = true;
//     xTaskCreatePinnedToCore(
//         this->PlayInfiniteThread, /* Task function. */
//         "Task1",                  /* name of task. */
//         4096,                    /* Stack size of task */
//         this,                     /* parameter of the task */
//         2 | portPRIVILEGE_BIT,    /* priority of the task */
//         NULL,                   /* Task handle to keep track of created task */
//         0);
//     // fillScreen();
// }



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
// void DisplayHelper::GIFDraw(GIFDRAW *pDraw)
// {
//     uint8_t *s = pDraw->pPixels;
//     uint16_t *usPalette = pDraw->pPalette;
//     int y = pDraw->iY + pDraw->y;
//     int iWidth = pDraw->iWidth;
//     if (iWidth > LCD_WIDTH)
//     {
//         iWidth = LCD_WIDTH;
//     }
//     gfx->startWrite();
//     // Handle background restoration
//     if (pDraw->ucDisposalMethod == 2)
//     {
//         uint8_t ucBackground = pDraw->ucBackground;
//         for (int x = 0; x < iWidth; x++)
//         {
//             if (s[x] == pDraw->ucTransparent)
//                 s[x] = ucBackground;
//         }
//         pDraw->ucHasTransparency = 0;
//     }

//     // Optimize transparent handling
//     if (pDraw->ucHasTransparency)
//     {
//         uint8_t ucTransparent = pDraw->ucTransparent;
//         uint16_t *d = usTemp;
//         int x = 0;
//         while (x < iWidth)
//         {
//             // Count non-transparent pixels
//             int count = 0;
//             while (x + count < iWidth && s[x + count] != ucTransparent)
//             {
//                 usTemp[count] = usPalette[s[x + count]];
//                 count++;
//             }

//             // Draw non-transparent block
//             if (count > 0)
//             {
//                 gfx->setAddrWindow((pDraw->iX + x) + xOffset, y + yOffset, count, 1);
//                 gfx->writePixels((uint16_t *)usTemp, count);
//                 // gfx->pushImage(pDraw->iX + x, y, count, 1,(uint16_t *)usTemp);
//                 x += count;
//             }

//             // Skip transparent pixels
//             while (x < iWidth && s[x] == ucTransparent)
//                 x++;
//         }
//     }
//     else
//     {
//         // Direct conversion for non-transparent images
//         for (int x = 0; x < iWidth; x++)
//         {
//             usTemp[x] = usPalette[s[x]];
//         }
//         // gfx->pushImage(pDraw->iX, y,  iWidth, 1,(uint16_t *)usTemp);
        
//         gfx->setAddrWindow(pDraw->iX + xOffset, y + yOffset, iWidth, 1);
//         gfx->writePixels(usTemp, iWidth);        
//     }
//     gfx->endWrite();
// }



// bool DisplayHelper::loadGIFToMemory(const char *filename)
// {
//     File file = FFat.open(filename, "r");
//     if (!file)
//     {
//         Serial.printf("Failed to open GIF file %s ", filename);
//         return false;
//     }

//     gifSize = file.size();    
//     gifData = (uint8_t *)ps_malloc(gifSize);

//     // luluDog->MemInfo();
//     Serial.printf("GifSize: %d Used PSRAM: %d", gifSize, ESP.getPsramSize() - ESP.getFreePsram());
    
//     if (!gifData)
//     {
//         Serial.printf("Failed to allocate memory for GIF");
//         file.close();
//         return false;
//     }

//     file.read(gifData, gifSize);
//     file.close();
//     return true;
// }

// void DisplayHelper::StopGif()
// {
//     playGif = false;
//     // luluDog->MemInfo();
// }



// void DisplayHelper::PlayInfiniteThread(void *_this)
// {
//     ((DisplayHelper *)_this)->PlayInfiniteTask();
//     vTaskDelete(NULL);
// }

// void DisplayHelper::PlayInfiniteTask()
// {
//     int iter = 0;
//     while (playGif /*&& iter<GifPlayTime*/)
//     {
//         pauseEyes();
//         int res = gif.playFrame(true, NULL);
//         if (res == -1)
//         {
//             Serial.printf("play error");
//             vTaskDelete(NULL);
//             return;
//         }
//         if (res == 0)
//         {
//             Serial.printf("play ended reopen");
//             iter++;
//             gif.close();
//             gif.open(gifData, gifSize, GIFDraw);
//         }        
//     }
//     if (playGif){
//         resumeEyes();
//         playGif = false;
//     }
//     Serial.printf("play ended");
// }




