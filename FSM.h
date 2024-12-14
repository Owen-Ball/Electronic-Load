#ifndef _FSM_
#define _FSM_

#include "IO.h"
#include "helpers.h"

extern MODE SYSTEM_MODE;
extern CURRENT_LIMIT SYSTEM_CURRENT;
extern OUT_STATE SYSTEM_OUTPUT;
extern float SYSTEM_SETPOINT;
extern float CURRENT_SET;

extern float V_READING;
extern float I_READING;

extern float TOTAL_MAH;

extern float filtered_voltage;
extern float filtered_current;

void initSystem();
void updateSystem();
void initSetpoints();

void updateMode();
void updateOutput();
void updateCurrentLimit();
void updateSetpoint();
void updateCurrent();
void checkLimits();

void runMode();
void runCC();
void runCP();
void runCR();
void runCV();
void runBAT();

void readVandI();
void setCurrent();

void filterMeasurements(float v, float i);

#endif // _FSM_
