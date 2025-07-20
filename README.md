# Pan-Tilt System with Joystick Control :: ATMega328P

## Project Overview

This project implements a simple **pan-tilt mechanism** driven by two stepper motors, allowing manual control via a **2-axis analog joystick**. Users can adjust the **azimuth (pan)** and **elevation (tilt)** using joystick movements. A button on the joystick resets the system to its centered position. Additionally, orientation can be controlled via command-line input through a serial interface.

The system is built around an **ATmega32 microcontroller** and uses a custom hardware abstraction layer (**avrhal**) to manage stepper motors, ADC, USART communication, and joystick input.

---

## Features

- **Hardware-level control of stepper motors** using A4988 stepper drivers, managed through custom abstraction
- **Precise positioning** of the pan-tilt mechanism with an accuracy of 1° in both azimuth and elevation axes
- **Manual control** via a 2D analog joystick (X/Y axes) for intuitive real-time positioning
- **Position reset** function via joystick button to return to neutral (centered) orientation
- **USART interface** for remote control and feedback, compatible with EasyComm2-style commands
- **Interoperability with rotctl and Gpredict**, allowing for automatic satellite tracking
- **Low CPU overhead**, with motor stepping handled by hardware timers (no pin toggling in interrupts)
- **Command-line control** via serial terminal (e.g., `minicom`) to issue commands or monitor system status

---

## Hardware Setup

- **Microcontroller:** ATmega328P
- **Motor Control:** 2x stepper motors with A4988 drivers  
- **Joystick:** 2-axis analog joystick with integrated pushbutton  
- **Communication:** USART @ 9600 Baud, 8N1  
- **Custom HAL:** 
  - Stepper motor control
  - ADC input for joystick
  - USART serial communication

---

## Dependencies

- AVR Libc (`avr/io.h`)
- Custom AVR Hardware Abstraction Layer:
  - `avrhal/stepper.h`
  - `avrhal/joystick.h`
  - `avrhal/usart.h`
  - `avrhal/adc.h`
  - `avrhal/commandParser.h`
  - `utils/bit.h`

---

## Usage Instructions

### Manual Control (via Joystick)
1. **Move joystick left/right** – controls horizontal panning (azimuth)
2. **Move joystick up/down** – controls vertical tilting (elevation)
3. **Press joystick button** – resets both motors to their zero (center) position

### Remote Control (via USART)
Connect to the device using a serial terminal (e.g., `minicom`) at **9600 Baud, 8N1**. The following commands are supported:

#### Query Commands
- `STATUS`  
  Prints detailed stepper status and current angles in the format:  
  `P:<taken>/<remaining> T:<taken>/<remaining> p:<azimuth> t:<elevation>`
  
- `AZ`  
  Returns the current azimuth angle (in degrees):  
  `AZ<value>.0`

- `EL`  
  Returns the current elevation angle (in degrees):  
  `EL<value>.0`

#### Position Commands
- `AZ<value>`  
  Sets the azimuth to the specified angle (integer degrees).  
  Example: `AZ45`

- `EL<value>`  
  Sets the elevation to the specified angle.  
  Example: `EL30`

- `AZ<value> EL<value>`  
  Sets both azimuth and elevation in one command.  
  Example: `AZ90 EL45`

> **Note:** Commands are case-insensitive and must be followed by a newline (`\n`) or carriage return (`\r\n`) when sent over serial.


---

## TODO

- Implement **different microstepping modes** (currently only full-step is supported) for finer orientation control
- Refactor stepper logic to use **two independent hardware timers** for better per-motor speed control and increased flexibility
- Add **limit switch support** for physical range detection and homing
- Add EEPROM storage to **retain last known position** across power cycles
- Optional: Integrate **OLED or LCD** for local display of position or status

---

## Credits

- **CAD Model Design:**  
  Original pan-tilt CAD model by [Scienteer](https://www.youtube.com/watch?v=Su6O6155UJM)

- **FreeCAD Conversion:**  
  Converted to FreeCAD format by [Eyal Abraham](https://www.youtube.com/watch?v=LF1JT0Pmddc)


