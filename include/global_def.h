
#define WIRE Wire

#define TOUCH_PIN 35

#define LULU_SLAVE_ADDRESS 8

#define AUDIO_ON 1
#define MOVE_ON 1
#define SLEEP_ON 1
#define WIFI_ON 0
#define DEEP_SLEEP_ON 1

#define IMG_WIDTH 240
#define IMG_HEIGHT 240

#define DISPLAY_WIDTH 240

#define EYEBORDER 10

#define SLEEP_AFTER 45

#define DEEP_SLEEP_AFTER 180

// GYRO CONFIG
 // Пороговое значение изменений ускорения (низкое из-за минимальных изменений)
#define IMPACT_THRESHHOLD_ACC_X 0.12
#define IMPACT_THRESHHOLD_ACC_Y 0.12
#define IMPACT_THRESHHOLD_ACC_Z 0.1

#define IMPACT_THRESHHOLD_GYR_X 36
#define IMPACT_THRESHHOLD_GYR_Y 36
#define IMPACT_THRESHHOLD_GYR_Z 12 // Пороговое значение изменений угловой скорости
#define LAST_IMPACT_MIN_PERIOD 2000

#define GYRO_ACTION_PERIOD 3000 // Время в миллисекундах


// BATTERY CONFIG

#define VREF  3.3    // Power supply voltage of ESP32-S3 (unit: volts)
#define BATTERY_R1  200000.0 // Resistance value of the first resistor (unit: ohms)
#define BATTERY_R2  100000.0
#define BATTERY_CHARGING_THR 0.1
#define BATTERY_CHARGING_V 3.95
#define BATTERY_CHARGING_V_MIN 3.9



// PIN CONFIG

#define XPOWERS_CHIP_AXP2101

#define LCD_DC 14
// #define LCD_CS 25 
#define LCD_SCK 25
#define LCD_MOSI 26
#define LCD_RST 27
#define LCD_BL 12
#define LCD_WIDTH 240
#define LCD_HEIGHT 240

//  -D TFT_MISO=19
//     ; -D TFT_MOSI=14
//     ; -D TFT_SCLK=12
//     ; -D TFT_CS=15
//     ; -D TFT_DC=26
//     ; -D TFT_RST=27


#define IIC_SDA 11
#define IIC_SCL 10

#define TP_RST 13
#define TP_INT 14
#define LONG_PRESS_T_COUNT 33
#define LONG_PRESS_T_END_COUNT 34

// #define BUZZER_PIN 33
#define BUZZER_PIN 42
#define SYS_EN_PIN 41
#define SYS_OUT_PIN 40

// #define I2S_DOUT 17 
// #define I2S_BCLK 18 
// #define I2S_LRC 16  

#define I2S_DOUT 18 
#define I2S_BCLK 3 
#define I2S_LRC 17  

#define VOLTAGE_DIVEDER_PIN 1


// TOUCH CONFIG
#define LONGPRESSTHRESHOLD 500
#define DOUBLETAPTHRESHOLD 800