#include "Arduino.h"

//This version of button is modified. I should put the code into this folder
#include "Button.h"
#include "constants.h"
#include "IO.h"
#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include "debug_utils.h"
#include <ESP32Encoder.h>
#include "helpers.h"

Adafruit_MCP4725 dac_low;
Adafruit_MCP4725 dac_high;
Button enable_button(ENABLE_BUTTON_PIN, DEBOUNCE_TIME_MS, BUTTON_AVG_COUNT);
Button current_button(CURRENT_BUTTON_PIN, DEBOUNCE_TIME_MS, BUTTON_AVG_COUNT);
Button mode_button(MODE_BUTTON_PIN, DEBOUNCE_TIME_MS, BUTTON_AVG_COUNT);
Button encoder_button(ENCODER_BUTTON_PIN, DEBOUNCE_TIME_MS, BUTTON_AVG_COUNT);
ESP32Encoder encoder;

ESP32_FAST_PWM* coolingFan;

uint8_t digit = 0;
bool useLargeVoltage = true;
bool useLargeCurrent = true;
bool digit_updated = false;
int prev_encoder = 0;

int safety_count = 0;

void writeEncoder(int val) {
  encoder.setCount(4*val);
}

int readEncoder() {
  int temp = encoder.getCount();
  if (temp >= 0)return (2+temp)/4;
  else return (-2+temp)/4;
}

float updateEncoder(float setpoint) {
  bool buttonstate = encoder_button.read();
  float increment;
  float prev_setpoint = setpoint;
  int temp = readEncoder();
  
  if (buttonstate == Button::PRESSED && encoder_button.has_changed()) {
    writeEncoder(digit);
    digit_updated = false;
  } else if (buttonstate == Button::RELEASED && encoder_button.has_changed()) {
    if (!digit_updated) {
       if (digit == 0) {
          digit = 3;
       } else {
          digit -= 1;
       }
    }
    writeEncoder(0);
    prev_encoder = 0;
  } else if (buttonstate == Button::PRESSED) {
    if (temp != int(digit)) {
      digit_updated = true;
      if (temp > 3) writeEncoder(3);
      if (temp < 0) writeEncoder(0);
      digit = readEncoder();
    }
  } else {
 
    increment = pow(10, modifiedLog(setpoint) - 3 + digit);

    if (setpoint - increment * (temp-prev_encoder) > -.001) setpoint -= increment * (temp - prev_encoder);
    prev_encoder = temp;

    int log_new = modifiedLog(setpoint);
    int log_prev = modifiedLog(prev_setpoint);  
    if (log_new > log_prev) digit = (digit > 0) ? digit - 1 : 0;
    else if (log_new < log_prev) digit = (digit < 3) ? digit + 1 : 3;
  }

  if (setpoint < 0.0) setpoint = 0;
  return setpoint;  
}


void setDAC(float current, CURRENT_LIMIT SYSTEM_CURRENT) {
  if (SYSTEM_CURRENT == CURR_HIGH){
    //Roughly estimate DAC setpoint from desired current, feedback loop handles exact value
    int val = current*3.8543E02;
    if (val < 0) {
      val = 0;
    } else if (val > 4095) {
      val = 4095;
    }
    dac_low.setVoltage((uint16_t)val, false);
    dac_high.setVoltage((uint16_t)val, false);
  } else {
    dac_low.setVoltage(0, false);
    dac_high.setVoltage(0, false);
  }
}

void setFan(float dutyCycle) {
  if (dutyCycle > 98.0) {
    dutyCycle = 98.0; 
  } else if (dutyCycle < 1.5) {
    dutyCycle = 1.5;
  }
  coolingFan->setPWM(FAN_PIN, PWM_FREQ, dutyCycle);
}

void runFan(float current, float temperature) {
  DEBUG_PRINTLN(temperature);

  //25C = 0% fan, 40C = 100% fan
  float dutyCycle1 = (temperature - 25.0) * 100.0/15.0;
  dutyCycle1 = limitFloat(dutyCycle1, 0.0, 100.0);


  //3.0A = 0% fan, 7.0A = 100% fan
  float dutyCycle2 = (current - 3.0) * 100.0 / (7.0 - 3.0);
  dutyCycle2 = limitFloat(dutyCycle2, 0.0, 100.0);
  
  float dutyCycle = max(dutyCycle1, dutyCycle2);
  
  setFan(dutyCycle);
}

float readVoltage() {
  
  float v;
  if (useLargeVoltage) {
    v = readVoltageLarge();
    if (v < V_THRESH_LOW) {
      useLargeVoltage = false;
      v = readVoltageSmall();
    }
  } else {
    v = readVoltageSmall();
    if (v > V_THRESH_HIGH) {
      useLargeVoltage = true;
      v = readVoltageLarge();
    }
  }
  
  if (v < 0) v = 0;
  return v;
}

float readVoltageLarge() {
  float raw = analogReadAverage(VSENSE_LARGE, 200);
  return pow(raw, 10)*-1.84897E-33 + \
  pow(raw, 9) *-1.62828E-29 + \
  pow(raw, 8) * 4.62921E-25 + \
  pow(raw, 7) *-3.08937E-21 + \
  pow(raw, 6) * 9.51169E-18 + \
  pow(raw, 5) *-1.45470E-14 + \
  pow(raw, 4) * 9.54453E-12 + \
  pow(raw, 3) * 6.47609E-11 + \
  pow(raw, 2) *-2.84146E-6  + \
  raw         * 1.04979E-2  + \
                4.40063E-2;
  
}


float readVoltageSmall() {
  float raw = analogReadAverage(VSENSE_SMALL, 200);
  return pow(raw, 10)*-8.61993E-34 + \
  pow(raw, 9) * 1.88324E-29 + \
  pow(raw, 8) *-1.79139E-25 + \
  pow(raw, 7) * 9.73128E-22 + \
  pow(raw, 6) *-3.31056E-18 + \
  pow(raw, 5) * 7.22482E-15 + \
  pow(raw, 4) *-9.91446E-12 + \
  pow(raw, 3) * 8.05764E-09 + \
  pow(raw, 2) *-3.53311E-6  + \
  raw         * 4.13327E-3  + \
                1.71605E-4;
}

float readCurrent(CURRENT_LIMIT SYSTEM_CURRENT) {
  float c;
  float safety_check;
  if (useLargeCurrent) {
    c = readCurrentLarge(SYSTEM_CURRENT, SAMPLE_COUNT);
    if (c < C_THRESH_LOW) {
      useLargeCurrent = false;
      c = readCurrentSmall(SYSTEM_CURRENT, SAMPLE_COUNT);
    }
  } else {
    c = readCurrentSmall(SYSTEM_CURRENT, SAMPLE_COUNT);
    safety_check = readCurrentLarge(SYSTEM_CURRENT, 20);
    if (safety_check > c + SAFETY_OFFSET) {
      safety_count +=1;
      if (safety_count == SAFETY_COUNT) return -1;
    } else {
      safety_count = 0;
    }
    if (c > C_THRESH_HIGH) {
      useLargeCurrent = true;
      c = readCurrentLarge(SYSTEM_CURRENT, SAMPLE_COUNT);
    }
  }
  
  if (c < 0) c = 0;
  return c;
}


float readCurrentLarge(CURRENT_LIMIT SYSTEM_CURRENT, int samples) {
  float raw = analogReadAverage(CURRSENSE_LARGE, samples);
  if (SYSTEM_CURRENT == CURR_HIGH) {
    float val =  pow(raw, 10)*-1.63034E-32 + \
    pow(raw, 9) * 2.60202E-28 + \
    pow(raw, 8) *-1.77464E-24 + \
    pow(raw, 7) * 6.76420E-21 + \
    pow(raw, 6) *-1.58241E-17 + \
    pow(raw, 5) * 2.34779E-14 + \
    pow(raw, 4) *-2.19820E-11 + \
    pow(raw, 3) * 1.24020E-08 + \
    pow(raw, 2) *-3.80117E-06  + \
    raw         * 3.35855E-03  + \
                  1.06078E-01;
  
    if (raw == 0) return 0.0;
    if (val <= 0) val = 0.0;
    return val;
  } else {
    return 0.0;
  }
}



float readCurrentSmall(CURRENT_LIMIT SYSTEM_CURRENT, int samples) {
  float raw = analogReadAverage(CURRSENSE_SMALL, samples);
  if (SYSTEM_CURRENT == CURR_HIGH) {
    float val =  pow(raw, 10)*-1.57915E-34 + \
    pow(raw, 9) * 2.72220E-30 + \
    pow(raw, 8) *-2.03675E-26 + \
    pow(raw, 7) * 8.82230E-23 + \
    pow(raw, 6) *-2.49323E-19 + \
    pow(raw, 5) * 4.84118E-16 + \
    pow(raw, 4) *-6.39218E-13 + \
    pow(raw, 3) * 5.30865E-10 + \
    pow(raw, 2) *-2.39639E-07  + \
    raw         * 3.04127E-04  + \
                  9.63830E-03;
    if (val <= 0) val = 0.0;
    return val;
  } else {
    return 0.0;
  }
}

float readThermistor() {
  float raw = analogReadAverage(THERMISTOR_PIN, 20);
  return pow(raw, 3) * 6.57E-9 + \
         pow(raw, 2) *-4.54E-5 + \
         raw         * 0.127   + \
                       -97.4;
}
