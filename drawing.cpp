#include "Arduino.h"
#include "drawing.h"
#include "helpers.h"
#include "FSM.h"
#include "symbols.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite wifi_sprite = TFT_eSprite(&tft);
TFT_eSprite output_sprite = TFT_eSprite(&tft);
char f_to_s_buffer[20];
FlickerFreePrint<TFT_eSPI> Voltage(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Current(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Setpoint(&tft, TFT_WHITE, TFT_BLACK);
FlickerFreePrint<TFT_eSPI> Cursor(&tft, TFT_WHITE, TFT_BLACK);

bool isdef_wifi_sprite = false;
bool omega_drawn = false;

void floatToString(float f, char* s) {  
  dtostrf(f, -1, 3-modifiedLog(f), s);
  if (modifiedLog(f) >= 3) s[4] = '.';
  s[5] = '\0';
}

void printSetpoint(float s, uint8_t digit) {
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
  }
  if (SYSTEM_MODE == CR) {
    tft.drawXBitmap(254, 40, omega, ohmWidth, ohmHeight, TFT_YELLOW);
    omega_drawn = true;
  }
  else if (omega_drawn == true) {
    tft.drawXBitmap(254, 40, omega, ohmWidth, ohmHeight, TFT_BLACK);
    omega_drawn = false;
  }
  mode_text[4] = '\0';
  //if (s < 0) s = 0.0;
  tft.setTextColor(TFT_YELLOW);
  Setpoint.setTextColor(TFT_YELLOW, TFT_BLACK);
  floatToString(s, f_to_s_buffer);
  f_to_s_buffer[5] = unit;
  f_to_s_buffer[6] = '\0';
  
  tft.setCursor(2, 70);
  strncat(mode_text, f_to_s_buffer, 6);
  Setpoint.print(mode_text);

  int blanks;
  int modlog = modifiedLog(s);
  if (3 - digit > modlog) blanks = 3 - digit + 1;
  else blanks = 3 - digit;
  for (int i = 0; i < 9; i++) {
    if (i == blanks+4) f_to_s_buffer[i] = '_';
    else f_to_s_buffer[i] = ' ';
  }
  f_to_s_buffer[9] = '\0'; 
  Cursor.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(2, 70);
  Cursor.print(f_to_s_buffer);

}

void printVoltage(float v) {
  if (v < 0) v = 0.0;
  tft.setTextColor(TFT_YELLOW);
  Voltage.setTextColor(TFT_YELLOW, TFT_BLACK);
  floatToString(v, f_to_s_buffer);
  tft.setCursor(30, 120);
  tft.print("V: ");
  Voltage.print(f_to_s_buffer);
  tft.print("V");
}

void printCurrent(float i) {
  if (i < 0) i = 0.0;
  Current.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextColor(TFT_RED);
  floatToString(i, f_to_s_buffer);
  tft.setCursor(30, 170);
  tft.print("I: ");
  Current.print(f_to_s_buffer);
  tft.print("A");
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
