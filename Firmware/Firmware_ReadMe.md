# Error/Warning Codes
The system features a handful of warnings and errors that will be displayed on the screen. These are as follows:
* No message (NO_ERROR): No error present
* "I Sense Fail" (C_SENSE_ERROR): Discrepancy between low and high current sensors. Disables device, cannot be overwritten
* "Over Current" (OVER_C_ERROR): Current went over system limit
* "Over Voltage" (OVER_V_ERROR): Voltage went over system limit
* "Over Power" (OVER_P_ERROR): Power went over system limit
* "Over Temp" (OVER_T_ERROR): Heatsink temp went over system limit
* "I_Set!=I_Meas" (MISMATCH_ERROR): Set current is not being achieved within required margin of error

When triggered, a warning/error will turn off the output. Of these, all but C_SENSE_ERROR can be reset by simply re-enabling the output. C_SENSE_ERROR could indicate a failure of one of the power MOSFETs and can only be disabled via a power cycle. You should proceed with caution by testing the device at ~1A using a current limited supply.

# Library Version Info
* Arduino ESP32: 2.0.18-arduino.5
* ESP32_FastPWM: v1.1.0
* Adafruit_MCP4725: v2.0.2
* ESP32Encoder: v0.11.6
