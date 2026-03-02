#define SERVOMIN 150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 600    // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2300   // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50

// #define USMIN 600   // Вместо 600 (ближе к стандарту 0 град)
// #define USMAX 2300   // Вместо 2300 (ближе к стандарту 180 град)
// #define SERVOMIN 200 // Оставить или пересчитать под новые US
// #define SERVOMAX 500
// #define SERVO_FREQ 100

#define SERVO_180 170
#define SERVO_90 90
#define SERVO_0 25

#define SERVO_MIN_SPEED 1
#define SERVO_MAX_SPEED 10

#define SER_MAIN 0
// #define SER_LEFT_BACK 1
// #define SER_RIGHT_BACK 2
// #define SER_RIGHT_FRONT 3
// #define SER_TAIL 4

// #define SER_TAIL_MAX 120
// #define SER_TAIL_MIN 60

#define SER_MAIN_PIN 19
// #define SER_RIGHT_FRONT_PIN 1
// #define SER_LEFT_BACK_PIN 4
// #define SER_RIGHT_BACK_PIN 2
// #define SER_TAIL_PIN 5

// #define SER_LEFT_FRONT_PIN 6
// #define SER_RIGHT_FRONT_PIN 2
// #define SER_LEFT_BACK_PIN 1
// #define SER_RIGHT_BACK_PIN 3
// #define SER_TAIL_PIN 7

#define SER_UPDATE_INTERVAL 15

#define TOUCH_UPDATE_INTERVAL 80


#define MSEC_UNTIL_SLEEP 10000
#define RP_SLEEP_DURATION_SEC 5


