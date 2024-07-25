#ifndef _IO_
#define _IO_


#include "Arduino.h"
#include <Button.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include "constants.h"
#include "debug_utils.h"
#include <ESP32Encoder.h>
#include "ESP32_FastPWM.h"

//DAC that controls the rightmost MOSFET
extern Adafruit_MCP4725 dac_low;
//DAC that controls the other 3 MOSFETs
extern Adafruit_MCP4725 dac_high;
extern Button enable_button;
extern Button current_button;
extern Button mode_button;
extern Button encoder_button;
extern ESP32Encoder encoder;

extern ESP32_FAST_PWM* coolingFan;

//stores what of the setpoint is being changed. 0-3
extern uint8_t digit;
//stores whether the system is currently using the large or small voltage sensor
//will update according to values in constants.h
extern bool useLargeVoltage;
//same as above
extern bool useLargeCurrent;

//Keeps track of the current digit the cursor is on and will update the setpoint provided according to the encoder
float updateEncoder(float setpoint);
//reads the current state of the encoder to the nearest full step
int readEncoder();

//Roughly sets the DAC to the required value. Feedback loop handles precise setting
void setDAC(float current, CURRENT_LIMIT SYSTEM_CURRENT);
//sets the fan to the provided duty cycle. 0-100 but will properly handle <0 and >100
void setFan(float dutyCycle);
//runs a fan curve that sets the fan speed based on MOSFET temperature
void runFan();

float readVoltage();
float readVoltageLarge();
float readVoltageSmall();

float readCurrent(CURRENT_LIMIT SYSTEM_CURRENT);
float readCurrentLarge(CURRENT_LIMIT SYSTEM_CURRENT);
float readCurrentSmall(CURRENT_LIMIT SYSTEM_CURRENT);

//Measure the temperature of the MOSFET heatsink
float readThermistor();

#endif // _IO_
