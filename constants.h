#ifndef _CONSTANTS_
#define _CONSTANTS_

enum OUT_STATE {OUT_ON, OUT_OFF};
enum MODE {CC, CV, CP, CR, BAT};
enum CURRENT_LIMIT {CURR_LOW, CURR_HIGH};

//Setpoint Bounds
#define MAX_CURRENT_LOW_SET   1.50
#define MAX_CURRENT_HIGH_SET  7.50
#define MAX_VOLTAGE_SET       30.00
#define MIN_RESISTANCE_SET    0.50
#define MAX_RESISTANCE_SET    500.00
#define MAX_POWER_SET         100.00

//Configure default setpoint values
#define DEFAULT_CC            1.00
#define DEFAULT_CV            5.00
#define DEFAULT_CP            10.00
#define DEFAULT_CR            10.00
#define DEFAULT_BAT           15.30


//System Limits
#define CURRENT_LOW_LIMIT     2.00
#define CURRENT_HIGH_LIMIT    8.00
#define VOLTAGE_LIMIT         35.00
#define POWER_LIMIT           135.0



//Pins
#define FAN_PIN               11
#define PWM_RES               10
#define PWM_FREQ              20000
#define PWM_CHANNEL           0

#define ENABLE_BUTTON_PIN     38
#define CURRENT_BUTTON_PIN    14
#define MODE_BUTTON_PIN       39
#define ENCODER_BUTTON_PIN    16
#define ENCODER_A             3
#define ENCODER_B             4

#define VSENSE_LARGE          13
#define VSENSE_SMALL          12
#define CURRSENSE_LARGE       6
#define CURRSENSE_SMALL       5
#define THERMISTOR_PIN        8

#define DAC_SDA               10
#define DAC_SCL               9

//debounce time for all I/O buttons
#define DEBOUNCE_TIME_MS      50
//Takes N digital readings of each button, all must agree for output state to toggle
//This was implemented to solve an infrequent issue where the current limit would randomly update due to
//digitalRead of the button returning the wrong value
#define BUTTON_AVG_COUNT      10




//Below this voltage the low voltage sensor will be used
#define V_THRESH_LOW          11.0
//Above this voltage the high voltage sensor will be used
//Between them the previous sensor is used
#define V_THRESH_HIGH         12.0

//On the high current setting, below this current the low current sensor is used
#define C_THRESH_LOW          0.7
//On the high current setting, above this current, the high current sensor is used
#define C_THRESH_HIGH         0.8

//defines how long the current can be outside of 25% of the setpoint before being cut (ms)
#define OUT_KILL_TIME_LIMIT   500




//Vertical spacing between lines
#define LETTER_HEIGHT         50
//defines the x coordinate of the left of the numbers
#define LETTER_X              45
//defines the y coordiante of the top of the numbers
#define LETTER_Y              50

//time in ms between screen updates
#define SCREEN_REFRESH        75

//sets the IIR filter used on values before displaying them. 
//Only impacts the displayed values, not the values used in control loops and such
//0 = no filter, -> 1 = high filtering
#define MEASUREMENT_FILTER    .75
//Will skip filtering and directly set the measured value if the filtered value has a percent error
//greater than the below value. Makes output responsive to large steps
#define FILTER_THRESHOLD      .02




//sets the gain of the integral controller for current setpoint
//1 or above likely to cause oscillations
#define CURRENT_SET_GAIN      .8

//Filters how fast the set current can change for CR, CP, CV
#define CURRENT_SET_FILTER    .75



//time in ms between current decreases in battery discharge mode
#define BAT_UPDATE_TIME       100
//Value of current steps in battery discharge mode
#define BAT_CURRENT_STEP      .05
//Speeds up discharging process by allowing measured voltage to drop below desired end voltage when
//at high currents, since resistive losses result in voltage drop
//Must be below actual resistance to avoid over discharging
#define BAT_RESISTANCE        0.02
//Cut off current for battery discharging
#define BAT_END_CURRENT       0.2

#endif // _CONSTANTS_
