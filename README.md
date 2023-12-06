# Mbed Desktop Fan
#### Lelah Cochran
#### ECE 4180 B
#### Fall 2023

## Description
This desktop fan device takes input from the user, either through the free "Adafruit Bluefruit BLE" App or on-board pushbuttons, to select one of three modes: Motion Detection Mode, Temperature Mode, and Manual Mode.

- Motion Detection Mode: Fan will turn on at an inputted speed (potentiometer/app) when an object is detected 
  within range (<500mm).

- Temperature Mode: Fan will turn on at an inputted speed (potentiometer/app) at temperatures above 75°F.

- Manual Mode: User may manually turn fan on/off by adjust speed using an on-board potentiometer or up/down arrows in app.

The temperature as well as current mode selected will be displayed on an LCD screen.

## Components
- User-Interface:
  -  uLCD-144-G2 128 by 128 Smart Color LCD
  -  10k Trimpot
  -  3 small SPST Pushbuttons
  -  Adafruit Bluefruit BLE UART 
- Sensors:
  -  VL53L0X LIDAR TOF Sensor
  -  TMP36 Analog Temperature Sensor
- Fan:
  -  Dual H-bridge breakout
  -  DC Motor
  -  Trifoil Propeller Fan 40mm
- Power:
  -  5VDC 2A AC wall adapter
  -  Barrel jack for breadboard power hookup

## Wiring

![image](https://github.com/lelahc/MbedDesktopFan/assets/153053788/9ac31ef4-c87d-4835-98ee-904a866f41d6)

![image](https://github.com/lelahc/MbedDesktopFan/assets/153053788/f3bb1ee4-7b3e-455b-a7f9-ef7131af3086)

![image](https://github.com/lelahc/MbedDesktopFan/assets/153053788/195ab374-be9a-4c20-bc0d-e745b55195eb)

## Photos

![image](https://github.com/lelahc/MbedDesktopFan/assets/153053788/f3a14c8f-1063-41bf-9788-b02f05db6107)

![image](https://github.com/lelahc/MbedDesktopFan/assets/153053788/59198e8a-fb5a-4494-a421-6ff5e012ea19)

## Demo

