#ifndef _FSM_
#define _FSM_

#include "IO.h"

extern MODE SYSTEM_MODE;
extern CURRENT_LIMIT SYSTEM_CURRENT;
extern OUT_STATE SYSTEM_OUTPUT;
extern float SYSTEM_SETPOINT;
extern float CURRENT_SET;

extern float V_READING;
extern float I_READING;

void initSystem();
void updateSystem();

void updateMode();
void updateOutput();
void updateCurrentLimit();
void updateSetpoint();
void updateCurrent();

float limitFloat(float val, float val_min, float val_max);

void readVandI();
void setCurrent();

#endif // _FSM_
