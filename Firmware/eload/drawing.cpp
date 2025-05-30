#include "Arduino.h"
#include "drawing.h"
#include "helpers.h"
#include "FSM.h"
#include "symbols.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite wifi_sprite = TFT_eSprite(&tft);
TFT_eSprite output_sprite = TFT_eSprite(&tft);
TFT_eSprite degree_sprite = TFT_eSprite(&tft);
TFT_eSprite blank_sprite = TFT_eSprite(&tft);

char f_to_s_buffer[20];
FlickerFreePrint<TFT_eSPI> Voltage(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Current(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Setpoint(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Power(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Cursor(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> DebugLine(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Temp(&tft, TFT_WHITE, TFT_BLACK);

bool isdef_wifi_sprite = false;
bool omega_drawn = false;
bool displayingPower = false;
bool isdef_degree_sprite = false;
long lastRefresh = 0;

void floatToString(float f, char* s) {  
  dtostrf(f, -1, 3-modifiedLog(f), s);
  if (modifiedLog(f) >= 3) s[4] = '.';
  s[5] = '\0';
}

void printSetpoint(float setpoint, uint8_t digit) {
  char mode_text[20];
  char unit;
  switch (SYSTEM_MODE) {
  case CC:
    strncpy(mode_text, "CC: ", 4);
    unit = 'A';
    break;
  case CP:
    strncpy(mode_text, "CP: ", 4);
    unit = 'W';
    break;
  case CR:
    strncpy(mode_text, "CR: ", 4);
    unit = ' ';
    break;
  case CV:
    strncpy(mode_text, "CV: ", 4);
    unit = 'V';
    break;
  case BAT:
    strncpy(mode_text, "BT: ", 4);
    unit = 'V';
    break;
  case R_CAL:
    strncpy(mode_text, "CL: ", 4);
    unit = ' ';
    break;
  }
  if (SYSTEM_MODE == CR || SYSTEM_MODE == R_CAL) {
    tft.drawXBitmap(LETTER_X + 225, LETTER_Y - 30, omega, ohmWidth, ohmHeight, TFT_YELLOW);
    omega_drawn = true;
  }
  else if (omega_drawn == true) {
    tft.drawXBitmap(LETTER_X + 225, LETTER_Y - 30, omega, ohmWidth, ohmHeight, TFT_BLACK);
    omega_drawn = false;
  }
  mode_text[4] = '\0';
  tft.setTextColor(TFT_YELLOW);
  Setpoint.setTextColor(TFT_YELLOW, TFT_BLACK);
  floatToString(setpoint, f_to_s_buffer);
  f_to_s_buffer[5] = unit;
  f_to_s_buffer[6] = '\0';
  
  tft.setCursor(LETTER_X - 28, LETTER_Y);
  strncat(mode_text, f_to_s_buffer, 6);
  Setpoint.print(mode_text);

  int blanks;
  int modlog = modifiedLog(setpoint);
  if (3 - digit > modlog) blanks = 3 - digit + 1;
  else blanks = 3 - digit;
  for (int i = 0; i < 9; i++) {
    if (i == blanks+4) f_to_s_buffer[i] = '_';
    else f_to_s_buffer[i] = ' ';
  }
  f_to_s_buffer[9] = '\0'; 
  Cursor.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(LETTER_X - 28, LETTER_Y);
  Cursor.print(f_to_s_buffer);

}

void printVoltage(float v) {
  if (v < 0) v = 0.0;
  tft.setTextColor(TFT_RED);
  Voltage.setTextColor(TFT_RED, TFT_BLACK);
  floatToString(v, f_to_s_buffer);
  tft.setCursor(LETTER_X, LETTER_Y + LETTER_HEIGHT);
  tft.print("V: ");
  Voltage.print(f_to_s_buffer);
  tft.print("V");
}

void printCurrent(float i) {
  if (i < 0) i = 0.0;
  Current.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  floatToString(i, f_to_s_buffer);
  tft.setCursor(LETTER_X, LETTER_Y + 2*LETTER_HEIGHT);
  tft.print("I: ");
  Current.print(f_to_s_buffer);
  tft.print("A");
}

void printPower(float p) {
  if (p < 0) p = 0.0;
  Power.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextColor(TFT_BLUE);
  floatToString(p, f_to_s_buffer);
  tft.setCursor(LETTER_X, LETTER_Y + 3*LETTER_HEIGHT);
  tft.print("P: ");
  Power.print(f_to_s_buffer);
  tft.print("W");
}

void printMAH(float m) {
  tft.drawXBitmap(LETTER_X+225, LETTER_Y + 3*LETTER_HEIGHT-30, mah, mahWidth, mahHeight, TFT_BLUE);
  if (m < 0) m = 0.0;
  Power.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextColor(TFT_BLUE);
  floatToString(m, f_to_s_buffer);
  tft.setCursor(LETTER_X, LETTER_Y + 3*LETTER_HEIGHT);
  tft.print("C: ");
  Power.print(f_to_s_buffer);
}

void printDebug(String s) {
  char text[DEBUG_LEN+1];
  for (uint8_t i = 0; i<DEBUG_LEN; i++) {
    text[i]=' ';
  }
  tft.setFreeFont(&FreeMonoBold12pt7b);
  s.toCharArray(text, DEBUG_LEN);
  text[DEBUG_LEN] = '\0';
  tft.setCursor(15, 230);
  DebugLine.print(text);
  tft.setFreeFont(&FreeMonoBold24pt7b);
}

void printTemp(float t) {
  tft.setFreeFont(&FreeMonoBold12pt7b);
  Temp.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  sprintf(f_to_s_buffer, "%02d C", int(t));
  f_to_s_buffer[4] = '\0';
  drawDegreeSymbol(255, 212);
  tft.setCursor(225, 226);
  Temp.print(f_to_s_buffer);
  tft.setFreeFont(&FreeMonoBold24pt7b);
}

void drawDegreeSymbol(int x, int y) {
  if (!isdef_degree_sprite) {
    isdef_degree_sprite = true;
    degree_sprite.setColorDepth(16);
    degree_sprite.createSprite(10, 10);
    degree_sprite.fillSprite(TFT_TRANSPARENT);
    degree_sprite.fillCircle(5, 5, 4, TFT_WHITE);
    degree_sprite.fillCircle(5, 5, 2, TFT_BLACK);
  }
  degree_sprite.pushSprite(x, y, TFT_TRANSPARENT);
}

void drawWifi() {
  if (!isdef_wifi_sprite) {
    isdef_wifi_sprite = true;
    wifi_sprite.setColorDepth(16);
    wifi_sprite.createSprite(40, 40);
    wifi_sprite.fillSprite(TFT_TRANSPARENT);
  
    wifi_sprite.drawSmoothArc(20, 20, 5,  7, 141, 221, TFT_YELLOW, TFT_TRANSPARENT, false);
    wifi_sprite.drawSmoothArc(20, 20, 10,  12, 141, 221, TFT_YELLOW, TFT_TRANSPARENT, false);
    wifi_sprite.drawSmoothArc(20, 20, 15,  17, 141, 221, TFT_YELLOW, TFT_TRANSPARENT, false);
    wifi_sprite.drawSmoothArc(20, 20, 0,  2, 141, 221, TFT_YELLOW, TFT_TRANSPARENT, false);
  }
  wifi_sprite.pushSprite(100, 100, TFT_TRANSPARENT);
}

void drawOutputState() {
  output_sprite.setColorDepth(16);
  output_sprite.createSprite(40, 40);
  output_sprite.fillSprite(TFT_TRANSPARENT);
  
  output_sprite.fillCircle(20, 20, 10, TFT_WHITE);
  if (SYSTEM_OUTPUT == OUT_ON) {
    output_sprite.fillCircle(20, 20, 9, TFT_GREEN);
  } else {
    output_sprite.fillCircle(20, 20, 9, TFT_RED);
  }
  
  output_sprite.pushSprite(280, 200, TFT_TRANSPARENT);
  output_sprite.deleteSprite();
  
}

void clearPower() {
  blank_sprite.setColorDepth(16);
  blank_sprite.createSprite(280, LETTER_HEIGHT);
  blank_sprite.fillSprite(TFT_BLACK);
  blank_sprite.pushSprite(30, 160);
  blank_sprite.deleteSprite();
}


void printErrorMsg() {
  String msg;
  switch (ERRORS) {
    case NO_ERROR:
      msg = " ";
      break;
    case C_SENSE_ERROR:
      msg = "I Sense Fail";
      break;
    case OVER_C_ERROR:
      msg = "Over Current";
      break;
    case OVER_V_ERROR:
      msg = "Over Voltage";
      break;
    case OVER_P_ERROR:
      msg = "Over Power";
      break;
    case OVER_T_ERROR:
      msg = "Over Temp";
      break;
    case MISMATCH_ERROR:
      msg = "I_Set!=I_Meas";
      break;
  }
  printDebug(msg);
}

void drawAll() {
  if (millis() - lastRefresh < SCREEN_REFRESH) return;
  drawOutputState();
  printSetpoint(SYSTEM_SETPOINT, digit);
  printVoltage(filtered_voltage);
  printCurrent(filtered_current);
  printTemp(TEMPERATURE);
  if (SYSTEM_MODE == BAT) {
    if (displayingPower) clearPower();
    displayingPower = false;
    printMAH(TOTAL_MAH);
  } else {
    if (!displayingPower) clearPower();
    displayingPower = true;
    printPower(filtered_voltage*filtered_current);
  }
  #ifndef DEBUG
    printErrorMsg();
  #endif
  lastRefresh = millis();
}
