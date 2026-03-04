
#ifndef DISLPAY_HELPER_H
#define DISLPAY_HELPER_H

// #include "Arduino_GFX_Library.h"

// #include <lvgl.h>
// #include <TFT_eSPI.h>
// #include <Wire.h>
#include <AnimatedGIF.h>
// #include "LittleFS.h"
#include "global_def.h"
// #include "battery_helper.h"
#include <LovyanGFX.hpp>
#include <DigitalRainAnimation.hpp>




class Proj42;
class LuLuEyes;


class LGFX_MyDisplay : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    LGFX_MyDisplay()
    {
        // Configure SPI bus settings
        auto cfg = _bus_instance.config();
        cfg.spi_host = SPI2_HOST; // Select HSPI or VSPI
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.spi_mode = 3;          // SPI mode (0-3)
        // cfg.freq_write = 80000000; // Write frequency
        cfg.pin_sclk = LCD_SCK;    // SCK pin
        cfg.pin_mosi = LCD_MOSI;   // MOSI pin
        cfg.pin_miso = -1;         // MISO pin, usually not used by LCD
        cfg.pin_dc = LCD_DC;       // DC pin

        _bus_instance.config(cfg); // Apply the configuration
        _panel_instance.setBus(&_bus_instance);

        // Set panel configuration
        auto panel_cfg = _panel_instance.config();
        // panel_cfg.pin_cs = LCD_CS;     // CS pin
        panel_cfg.pin_cs = -1;     // CS pin
        panel_cfg.pin_rst = LCD_RST;   // RST pin
        panel_cfg.pin_busy = -1;       // BUSY pin if used
        panel_cfg.memory_width = 240;  // Physical width of the panel
        panel_cfg.memory_height = 240; // Physical height of the panel
        panel_cfg.panel_width = 240;   // Resolution width
        panel_cfg.panel_height = 240;  // Resolution height
        panel_cfg.offset_y = -80;            
        panel_cfg.readable = true;
        panel_cfg.invert = true;
        panel_cfg.rgb_order = false;
        panel_cfg.dlen_16bit = false;
        panel_cfg.bus_shared = false;
        

        _panel_instance.config(panel_cfg);

        setPanel(&_panel_instance);
    }

};

class DisplayHelper
{
private:
    // File gifFile;
    static uint16_t* usTemp;
    uint8_t *gifData = nullptr;
    int32_t gifSize = 0;
    // TaskHandle_t Task1;
    char voltageBuf[15];

    bool playGif = true;
    bool wake = false;

    uint8_t *pTurboBuffer;
    uint8_t *pFrameBuffer;

    // BatteryHelper* batteryHelper;
    static Proj42 *proj42;

    static int xOffset;
    static int yOffset;

    bool matrixEffectInited = false;

public:
    // static Arduino_DataBus *bus;
    // static Arduino_GFX *gfx;

    
    int weatherType;
    bool showTime = false;
    bool showEyes = true;
    bool showMatrixAnimation = false;

    const int GifPlayTime = 2;    
    
    static LGFX_Sprite *eyesSprite;
    static LGFX_Sprite *timeSprite;
    // static LGFX_Sprite *matrixSprite;
    static LGFX_MyDisplay* gfx;
    static LuLuEyes* luluEyes;
    static AnimatedGIF gif;
    static DigitalRainAnimation<LGFX_Sprite> matrix_effect;    
    DisplayHelper(Proj42 *_proj42);

    void DrawDateTime();
    void HeartAnimation();
    void LookUp();
    void LookLeft();
    void LookRight();
    void LookDown();
    void Borring();
    void Confused();
    void Laugh();
    void Angry(int t);
    void ShowClock(int delay1);

    // DisplayHelper();
    static void GIFDraw(GIFDRAW *pDraw);
    static void TFTDraw(int x, int y, int w, int h, uint16_t *lBuf);
    static void DrawBatteryThread(void* _this);
    void DrawBatteryTask();
    // static void *GIFAlloc(uint32_t u32Size);
    // static void GIFDraw_24bit(GIFDRAW *pDraw);
    bool loadGIFToMemory(const char *filename);
    static void PlayInfiniteThread(void *pvParameters);
    static void StartEyesUpdateThread(void *_this);    
    void pauseEyes();    
    void resumeEyes();    
    void EyesUpdateTask();
    void PlayInfiniteTask();
    void InitDisplay();
    void PlayGif(const char *fname);
    void printOnDisplay(char *text, int x, int y);
    void setVoltageBuf(float voltage);
    void showSleepAnimation();
    void StopGif();
    void stopSleepAnimation();
    static void drawBatteryheart();
    void fillScreen();
    void DisplayOff();
    void DisplayOn();
    void ShowMatrixAnimation();
    void InitMatrixAnimation();
    void StopMatrixAnimation();
    // static void MatrixAnimationThread(void *_this);
    // static void drawHeart(int x, int y, uint16_t color);
    
    // static void LvglDispFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
    void SetEyePosition(int x, int y);
    void setIdleMode(bool enable);
    
};

// extern DisplayHelper displayHelper;
#endif