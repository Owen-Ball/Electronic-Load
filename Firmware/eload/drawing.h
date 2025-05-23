#ifndef _DRAWING_
#define _DRAWING_

#include <TFT_eSPI.h>
#include <SPI.h>
#include "Free_Fonts.h"
#include <FlickerFreePrint.h>
#include "Arduino.h"
#include "FSM.h"
#include "symbols.h"



extern TFT_eSPI tft;
extern TFT_eSprite wifi_sprite;
extern TFT_eSprite output_sprite;
extern FlickerFreePrint<TFT_eSPI> Voltage;
extern FlickerFreePrint<TFT_eSPI> Current;
extern FlickerFreePrint<TFT_eSPI> Setpoint;
extern FlickerFreePrint<TFT_eSPI> Power;
extern FlickerFreePrint<TFT_eSPI> Cursor;
extern FlickerFreePrint<TFT_eSPI> DebugLine;
extern char f_to_s_buffer[20];

//Draw the wifi symbol at provided coordinates
void drawWifi();

//Puts a float into the provided char*
//Will be one of the following forms: XXXX.  XXX.X  XX.XX  X.XXX
//Adds null terminator
void floatToString(float f, char* s);

//Prints the system setpoint as well as the indicator under the currently selected digit
//digit is a value from 0-3 indicating the selected digit
void printSetpoint(float s, uint8_t digit);

//Print the measured voltage
void printVoltage(float v);

//Print the measured current
void printCurrent(float i);

//Print the measured power
void printPower(float i);

//Print the heatsink temperature
void printTemp(float t);

//Display a degree symbol for the temperature readout
void drawDegreeSymbol(int x, int y);

//Used for printing debug values
//OR for printing error messages
void printDebug(String s);

//When debug is disabled, use printDebug to display error message
//If it is enabled, it gets printed via the serial monitor
void printErrorMsg();

//Draws an indicator of if the system output is on or off
void drawOutputState();

//Runs all drawing functions
void drawAll();

#endif // _DRAWING_
