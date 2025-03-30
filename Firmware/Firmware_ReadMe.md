# Error/Warning Codes
The system features a handful of warnings and errors that will be displayed on the screen. These are as follows:
* No message (NO_ERROR): No error present
* "I Sense Fail" (C_SENSE_ERROR): Discrepancy between low and high current sensors. Disables device, cannot be overwritten
* "Over Current" (OVER_C_ERROR): Current went over system limit
* "Over Voltage" (OVER_V_ERROR): Voltage went over system limit
* "Over Power" (OVER_P_ERROR): Power went over system limit
* "Over Temp" (OVER_T_ERROR): Heatsink temp went over system limit
* "I_Set!=I_Meas" (MISMATCH_ERROR): Set current is not being achieved within required margin of error

When triggered, a warning/error will turn off the output. Of these, all but C_SENSE_ERROR can be reset by simply re-enabling the output. 
C_SENSE_ERROR could indicate a failure of one of the power MOSFETs and can only be disabled via a power cycle. You should proceed with 
caution by testing the device at ~1A using a current limited supply.

# Library & Board Version Info
* Espressif Systems esp32: v2.0.14
    * Board: Adafruit Feather ESP32-S3 2MB PSRAM
    * Add https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json in _Additional Boards Manager_
* Adafruit BusIO: v1.16.2
* Adafruit_MCP4725: v2.0.2
* ESP32_FastPWM: v1.1.0
* ESP32Encoder: v0.11.6
* [FlickerFreePrint](https://github.com/KrisKasprzak/FlickerFreePrint) 
* TFT_eSPI: v2.5.43
    * Need to overwrite _Arduino\libraries\TFT_eSPI\User_Setup.h_ with the following: [User_Setup.h](User_Setup.h)


# Modes of Operation
All modes operate around setting a current on the device. Depending on the mode, this may be a constant current or it can be a function of voltage.

## Constant Current
Constant current mode directly sets a current output and attempts to keep it stable. 
The DACs in the device are only loosely calibrated, so when CC mode is started, 
the DACs are set roughly to the required voltage. The actual current is monitored 
and an integral controller is used to update the set DAC value until the measured 
current matches the user setpoint. This allows the device to maintain a constant 
current no matter what the applied voltage, temperature of FETs, etc is. Sanity 
checks must be performed to ensure the DAC setpoint and the measured current are
within some tolerance of each other to prevent runaway of the integral controller.

## Constant Power
Under the hood, constant power method uses the feedback loop from constant current mode. 
The only change is that in constant power mode, the target current value is calculated by
dividing the set power by the measured voltage. Because the device does not have sense wires,
the voltage drops of wires and connectors will impact the measured voltage. The result of this is
that slightly more power will be drawn from the connected supply than measured by the device.

## Constant Resistance
This mode also uses constant current mode, but the target current is set to the measured voltage divided
by the set resistance. Due to wire and connector drops, the actual current drawn will be slightly
lower than if the supply was connected to some ideal resistor of the set value.

## Battery Discharge Mode
In battery discharging mode, the device starts by pulling 5A from the connected supply. This
is achieved using the same constant current feedback loop as all other modes.
The measured voltage is compared to the user set end voltage. If the voltage drops below the end voltage
(some battery and E-load internal resistance is accounted for), the current setpoint will be 
dropped by a set value (currently 0.05A). This will occur until a minimum current is reached, at
which point the device will turn off. The battery voltage should now be quite close to the
set end voltage.

In battery discharge mode, the power reading is replaced by a mAh reading that indicates the total
number of mAh consumed from the battery. This value will be cleared when the mode is changed,
but will not change when you toggle the output on and off. When turned off and back on, the
current will reset to 5A

## Battery Discharge Mode
This mode is used to account for resistive drops in the Eload and wires. This mode will set the current to 1A 
when the output is enabled. The resistance value shown can be changed until the voltage measured by the Eload 
matches with the voltage measured at the desired location with a kelvin connected voltmeter. The resistance value
set in this mode will be used in all other modes to update the measured voltage in accordance with the measured current.
