#ifndef _CONSTANTS_
#define _CONSTANTS_

enum OUT_STATE {OUT_ON, OUT_OFF};
enum MODE {CC, CV, CP, CR, None};
enum CURRENT_LIMIT {CURR_LOW, CURR_HIGH};


//Setpoint Bounds
#define MAX_CURRENT_LOW_SET   1.50
#define MAX_CURRENT_HIGH_SET  7.50
#define MAX_VOLTAGE_SET       30.00
#define MIN_RESISTANCE_SET    0.50
#define MAX_RESISTANCE_SET    500.00
#define MAX_POWER_SET         100.00

//System Limits
#define CURRENT_LOW_LIMIT     2.00
#define CURRENT_HIGH_LIMIT    8.00
#define VOLTAGE_LIMIT         35.00
#define POWER_LIMIT           120.0

#define FAN_PIN               11
#define PWM_RES               10
#define PWM_FREQ              20000
#define PWM_CHANNEL           0

#define ENABLE_BUTTON_PIN     15
#define CURRENT_BUTTON_PIN    14
#define MODE_BUTTON_PIN       38
#define ENCODER_BUTTON_PIN    16
#define ENCODER_A             3
#define ENCODER_B             4

#define VSENSE_LARGE          13
#define VSENSE_SMALL          12
#define CURRSENSE_LARGE       6
#define CURRSENSE_SMALL       5
#define THERMISTOR_PIN        8

#define DEBOUNCE_TIME_MS      50
#define BUTTON_AVG_COUNT      10

#define V_THRESH_LOW          11.0
#define V_THRESH_HIGH         12.0

#define C_THRESH_LOW          0.7
#define C_THRESH_HIGH         0.8


#define DAC_SDA               10
#define DAC_SCL               9

#define OUT_KILL_TIME_LIMIT   500

#define LETTER_HEIGHT         50
#define LETTER_X              45
#define LETTER_Y              50

#define MEASUREMENT_FILTER    .75
#define FILTER_THRESHOLD      .02


#endif // _CONSTANTS_
