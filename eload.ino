#include "Arduino.h"
#include <cstdint>
#include "drawing.h"
#include "IO.h"
#include "helpers.h"
#include "FSM.h"

void setup() {
  
  pinMode(VSENSE_LARGE, INPUT);
  pinMode(VSENSE_SMALL, INPUT);
  pinMode(CURRSENSE_LARGE, INPUT);
  pinMode(CURRSENSE_SMALL, INPUT);
  pinMode(THERMISTOR_PIN, INPUT);
  
  encoder.attachFullQuad (ENCODER_A, ENCODER_B);
  
  Wire.begin(DAC_SDA, DAC_SCL);
  dac_low.begin(0x60, &Wire);
  dac_high.begin(0x61, &Wire);
  dac_low.setVoltage(0, false);
  dac_high.setVoltage(0, false);

  Serial.begin(115200);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setFreeFont(&FreeMonoBold24pt7b);

  encoder_button.begin();
  enable_button.begin();
  mode_button.begin();
  current_button.begin();

  initSystem();
  SYSTEM_CURRENT = CURR_HIGH;
  SYSTEM_OUTPUT = OUT_ON;
  
  coolingFan = new ESP32_FAST_PWM(FAN_PIN, PWM_FREQ, 0.0f, PWM_CHANNEL, PWM_RES);
  
}

void loop() {
  
  drawOutputState();
  readVandI();
  printSetpoint(SYSTEM_SETPOINT, digit);
  printVoltage(V_READING);
  //setFan(25.0);
  runFan();
  printCurrent(I_READING);

  updateSystem();


  
}
