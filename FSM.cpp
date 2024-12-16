#include "FSM.h"
#include "IO.h"
#include "helpers.h"

MODE SYSTEM_MODE;
CURRENT_LIMIT SYSTEM_CURRENT;
OUT_STATE SYSTEM_OUTPUT;
float SYSTEM_SETPOINT;
float SETPOINTS[5];
ERROR_CODES ERRORS;

//What the DAC is actually set to. Updated via feedback
float CURRENT_SET;
//Stores the set or calculated current target
float SYSTEM_CURRENT_SETPOINT;
float V_READING;
float I_READING;
long OUT_KILL_TIMER;

//Records when the current for BAT mode was last dropped
long current_last_updated = 0;

long prev_time = 0;
float TOTAL_MAH = 0;

float filtered_voltage = 0;
float filtered_current = 0;


void initSetpoints() {
  SETPOINTS[CC] = DEFAULT_CC;
  SETPOINTS[CV] = DEFAULT_CV;
  SETPOINTS[CP] = DEFAULT_CP;
  SETPOINTS[CR] = DEFAULT_CR;
  SETPOINTS[BAT] = DEFAULT_BAT;
}

void initSystem() {
  ERRORS = NO_ERROR;
  initSetpoints();
  SYSTEM_MODE = CC;
  SYSTEM_CURRENT = CURR_HIGH;
  SYSTEM_OUTPUT = OUT_OFF;
  SYSTEM_SETPOINT = SETPOINTS[SYSTEM_MODE];
  CURRENT_SET = 0.0;
  OUT_KILL_TIMER = millis();
}


void updateSystem() {
  readVandI();
  checkLimits();
  runFan(I_READING);
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
  if (!update_mode) return;
  SETPOINTS[SYSTEM_MODE] = SYSTEM_SETPOINT;
  switch (SYSTEM_MODE) {
    case CC:
      SYSTEM_OUTPUT = OUT_OFF;
      SYSTEM_MODE = CP;
      break;
    case CP:
      SYSTEM_OUTPUT = OUT_OFF;
      SYSTEM_MODE = CR;
      break;
    case CR:
      SYSTEM_OUTPUT = OUT_OFF;
      SYSTEM_MODE = CV;
      break;
    case CV:
      SYSTEM_OUTPUT = OUT_OFF;
      TOTAL_MAH = 0;
      prev_time = millis();
      SYSTEM_MODE = BAT;
      break;
    case BAT:
      SYSTEM_OUTPUT = OUT_OFF;
      SYSTEM_MODE = CC;
      break;
    default:
      SYSTEM_OUTPUT = OUT_OFF;
      SYSTEM_MODE = CC;
  }
  SYSTEM_SETPOINT = SETPOINTS[SYSTEM_MODE];
}

void updateOutput() {
  if (enable_button.pressed()) {
    if (SYSTEM_OUTPUT == OUT_ON) {
      SYSTEM_OUTPUT = OUT_OFF;
    } else {
      if (ERRORS == C_SENSE_ERROR) return;
      SYSTEM_OUTPUT = OUT_ON;
      OUT_KILL_TIMER = millis();
      ERRORS = NO_ERROR;
    }
  }
}

void updateCurrentLimit() {
  //DISABLED. LOW CURRENT MODE NOT ENABLED
  return;
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
    case BAT:
      SYSTEM_SETPOINT = limitFloat(SYSTEM_SETPOINT, 0, MAX_VOLTAGE_SET);
      break;
  }
}

void checkLimits() {
  if (I_READING > CURRENT_HIGH_LIMIT) {
    SYSTEM_OUTPUT = OUT_OFF;
    ERRORS = OVER_C_ERROR;
  }
  if (V_READING > VOLTAGE_LIMIT) {
    SYSTEM_OUTPUT = OUT_OFF;
    ERRORS = OVER_V_ERROR;
  }
  if (V_READING*I_READING > POWER_LIMIT) {
    SYSTEM_OUTPUT = OUT_OFF;
    ERRORS = OVER_P_ERROR;
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
        runCV();
        break; 
      case BAT:
        runBAT();
        break;
    }
}

void runCC() {
  SYSTEM_CURRENT_SETPOINT = SYSTEM_SETPOINT;
  updateCurrent();  
}


void runCP() {
  if (V_READING == 0) {
    //Avoid a divide by 0
    SYSTEM_OUTPUT = OUT_OFF;
  } else {
    SYSTEM_CURRENT_SETPOINT = CURRENT_SET_FILTER*SYSTEM_CURRENT_SETPOINT + (1-CURRENT_SET_FILTER)*SYSTEM_SETPOINT / V_READING;
  }  
  updateCurrent();
}

void runCR() {
  if (SYSTEM_SETPOINT == 0) {
    //should never hit this case bc of the limits on the setpoint, but still good to check 
    SYSTEM_OUTPUT = OUT_OFF;
  } else {
    SYSTEM_CURRENT_SETPOINT = CURRENT_SET_FILTER*SYSTEM_CURRENT_SETPOINT + (1-CURRENT_SET_FILTER)*V_READING / SYSTEM_SETPOINT;
  }
  updateCurrent();  
}

void runCV() {
  SYSTEM_OUTPUT = OUT_OFF;
  updateCurrent();
}

void runBAT() {
  if (SYSTEM_OUTPUT == OUT_OFF) {
    SYSTEM_CURRENT_SETPOINT = 5.00;
    current_last_updated = millis();
  } else {
    if (V_READING < SYSTEM_SETPOINT - I_READING * BAT_RESISTANCE && millis() - current_last_updated > BAT_UPDATE_TIME) {
      SYSTEM_CURRENT_SETPOINT -= BAT_CURRENT_STEP;
      if (SYSTEM_CURRENT_SETPOINT <= BAT_END_CURRENT) SYSTEM_OUTPUT = OUT_OFF;
    }
  }
  long curr_time = millis();
  float loop_time = float(curr_time - prev_time); 
  TOTAL_MAH += loop_time / 3600.0 * I_READING;
  prev_time = curr_time;
  updateCurrent();  
}

void updateCurrent() {;
  long timer = millis() - OUT_KILL_TIMER;
  if (SYSTEM_CURRENT_SETPOINT < .75*I_READING || SYSTEM_CURRENT_SETPOINT > 1.25*I_READING) {
    if (timer > OUT_KILL_TIME_LIMIT) {
      SYSTEM_OUTPUT = OUT_OFF;
      ERRORS = MISMATCH_ERROR;
    }
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
  if (I_READING < 0) {
    SYSTEM_OUTPUT = OUT_OFF;
    ERRORS = C_SENSE_ERROR;
  }
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
