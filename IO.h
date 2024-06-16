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

extern Adafruit_MCP4725 dac_low;
extern Adafruit_MCP4725 dac_high;
extern Button enable_button;
extern Button current_button;
extern Button mode_button;
extern Button encoder_button;
extern ESP32Encoder encoder;

extern ESP32_FAST_PWM* coolingFan;

extern uint8_t digit;
extern bool useLargeVoltage;
extern bool useLargeCurrent;

float updateEncoder(float setpoint);
int readEncoder();

void setDAC(float current, CURRENT_LIMIT SYSTEM_CURRENT);
void setFan(float dutyCycle);
void runFan();

float readVoltage();
float readVoltageLarge();
float readVoltageSmall();

float readCurrent(CURRENT_LIMIT SYSTEM_CURRENT);
float readCurrentLarge(CURRENT_LIMIT SYSTEM_CURRENT);
float readCurrentSmall(CURRENT_LIMIT SYSTEM_CURRENT);

float readThermistor();

#endif // _IO_
