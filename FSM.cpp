#include "FSM.h"
#include "IO.h"
#include "helpers.h"

MODE SYSTEM_MODE;
CURRENT_LIMIT SYSTEM_CURRENT;
OUT_STATE SYSTEM_OUTPUT;
float SYSTEM_SETPOINT;

//What the DAC is actually set to. Updated via feedback
float CURRENT_SET;
//Stores the set or calculated current target
float SYSTEM_CURRENT_SETPOINT;
float V_READING;
float I_READING;
long OUT_KILL_TIMER;

float filtered_voltage = 0;
float filtered_current = 0;

void initSystem() {
  SYSTEM_MODE = CC;
  SYSTEM_CURRENT = CURR_HIGH;
  SYSTEM_OUTPUT = OUT_OFF;
  SYSTEM_SETPOINT = 0.0;
  CURRENT_SET = 0.0;
  OUT_KILL_TIMER = millis();
}


void updateSystem() {
  readVandI();
  runFan();
  updateMode();
  updateOutput();
  updateCurrentLimit();
  updateSetpoint();
  runMode();
  filterMeasurements(V_READING, I_READING);
  //updateCurrent();
}

void updateMode() {
  bool update_mode = mode_button.pressed();
  switch (SYSTEM_MODE) {
    case CC:
      if (update_mode) {
        SYSTEM_OUTPUT = OUT_OFF;
        SYSTEM_MODE = CP;
      }
      break;
    case CP:
      if (update_mode) {
        SYSTEM_OUTPUT = OUT_OFF;
        SYSTEM_MODE = CR;
      }
      break;
    case CR:
      if (update_mode) {
        SYSTEM_OUTPUT = OUT_OFF;
        SYSTEM_MODE = CV;
      }
      break;
    case CV:
      if (update_mode) {
        SYSTEM_OUTPUT = OUT_OFF;
        SYSTEM_MODE = CC;
      }
      break;
    default:
      SYSTEM_OUTPUT = OUT_OFF;
      SYSTEM_MODE = CC;
      
  }
}

void updateOutput() {
  if (enable_button.pressed()) {
    if (SYSTEM_OUTPUT == OUT_ON) {
      SYSTEM_OUTPUT = OUT_OFF;
    } else {
      SYSTEM_OUTPUT = OUT_ON;
      OUT_KILL_TIMER = millis();
    }
  }
}

void updateCurrentLimit() {
  if (current_button.pressed()) {
    DEBUG_PRINTLN("CURRENT LIMIT CHANGED");
    if (SYSTEM_CURRENT == CURR_LOW) {
      SYSTEM_CURRENT = CURR_HIGH;
    } else {
      SYSTEM_CURRENT = CURR_LOW;
    }
  }
}

void updateSetpoint() {

  SYSTEM_SETPOINT = updateEncoder(SYSTEM_SETPOINT);
  //Limit values to min/max setpoints
  switch (SYSTEM_MODE) {
    case CC:
      if (SYSTEM_CURRENT == CURR_LOW) {
        SYSTEM_SETPOINT = limitFloat(SYSTEM_SETPOINT, 0, MAX_CURRENT_LOW_SET);
      } else {
        SYSTEM_SETPOINT = limitFloat(SYSTEM_SETPOINT, 0, MAX_CURRENT_HIGH_SET);
      }
      break;
    case CP:
      SYSTEM_SETPOINT = limitFloat(SYSTEM_SETPOINT, 0, MAX_POWER_SET);
      break;
    case CR:
      SYSTEM_SETPOINT = limitFloat(SYSTEM_SETPOINT, MIN_RESISTANCE_SET, MAX_RESISTANCE_SET);
      break;
    case CV:
      SYSTEM_SETPOINT = limitFloat(SYSTEM_SETPOINT, 0, MAX_VOLTAGE_SET);
      break; 
  }
}

void runMode() {
    switch (SYSTEM_MODE) {
      case CC:
        runCC();
        break;
      case CP:
        runCP();
        break;
      case CR:
        runCR();
        break;
      case CV:
        break; 
    }
}

void runCC() {
  SYSTEM_CURRENT_SETPOINT = SYSTEM_SETPOINT;
  updateCurrent();  
}


//************************************************************TODO MAKE CONSTANTS
void runCP() {
  if (V_READING == 0) {
    //Avoid a divide by 0
    SYSTEM_OUTPUT = OUT_OFF;
  } else {
    SYSTEM_CURRENT_SETPOINT = .75*SYSTEM_CURRENT_SETPOINT + .25*SYSTEM_SETPOINT / V_READING;
    updateCurrent();
  }  
}

void runCR() {
  if (SYSTEM_SETPOINT == 0) {
    //should never hit this case bc of the limits on the setpoint, but still good to check 
    SYSTEM_OUTPUT = OUT_OFF;
  } else {
    SYSTEM_CURRENT_SETPOINT = .75*SYSTEM_CURRENT_SETPOINT + .25*V_READING / SYSTEM_SETPOINT;
    updateCurrent();  
  }
}

void updateCurrent() {
  //Serial.println(SYSTEM_OUTPUT);
  Serial.println(millis() - OUT_KILL_TIMER);
  long timer = millis() - OUT_KILL_TIMER;
  if (SYSTEM_CURRENT_SETPOINT < .75*I_READING || SYSTEM_CURRENT_SETPOINT > 1.25*I_READING) {
    if (timer > OUT_KILL_TIME_LIMIT) SYSTEM_OUTPUT = OUT_OFF;
  } else {
    OUT_KILL_TIMER = millis();
  }
  
  if (SYSTEM_OUTPUT == OUT_OFF) {
    CURRENT_SET = 0;
    setDAC(0, SYSTEM_CURRENT);
  } else {
    //Adjusts the value the DAC is set to depending on the measured error
    CURRENT_SET += CURRENT_SET_GAIN*(SYSTEM_CURRENT_SETPOINT - I_READING);
    setCurrent();
  }
}

void readVandI() {
  I_READING = readCurrent(SYSTEM_CURRENT);
  V_READING = readVoltage();

  //There is around 8-12mA of leakage current even when the output is off
  //This is not able to be measured by the current sensor bc it flows through a different path
  //Therefore we don't know if this current is flowing or not (ie if a source is connected)
  //So we assume if there is almost no voltage measured, there is nothing connected
  if (V_READING < .01) I_READING = 0.0;
}

void setCurrent() {
  limitFloat(CURRENT_SET, 0.0, 1.5*(SYSTEM_CURRENT == CURR_HIGH)?CURRENT_HIGH_LIMIT:CURRENT_LOW_LIMIT);
  setDAC(CURRENT_SET, SYSTEM_CURRENT);
  
}

void filterMeasurements(float v, float i) {
  float v_error;
  if (v == 0) v_error = 0;
  else v_error = abs((v - filtered_voltage) / v);
  if (v_error < FILTER_THRESHOLD) {
    filtered_voltage = MEASUREMENT_FILTER*filtered_voltage + (1 - MEASUREMENT_FILTER)*v;
  } else {
    filtered_voltage = v;
  }
  float i_error;
  if (i == 0) i_error = 0;
  else i_error = abs((i - filtered_current) / i);
  if (i_error < FILTER_THRESHOLD) {
    filtered_current = MEASUREMENT_FILTER*filtered_current + (1 - MEASUREMENT_FILTER)*i;
  } else {
    filtered_current = i;
  }
}
