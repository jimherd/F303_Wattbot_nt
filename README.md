# uP control of Wattbot-nt robot vehicle

## Overview

The control of the Wattbot-nt robot vehicle consists of three subsystems

1. **HLcontrol**   : **H**igh **L**evel misssion control of the system (e.g.raspberry pi)
2. **LLcontrol**   : **L**ow **L**evel motion and sensor control
3. **FPGAcontrol** : Time critical control of sensor and actuator interfaces

This repository contains the code for the LLcontrol subsystem.

## LLcontrol subsystem hardware

- Based on STM32F303RE microprocessor (72MHz Arm M4)
- 512KB FLASH, 80KB SRAM
- Programmed with the MbedOS/RTOS system (Mbed Studio)
- UART connection to the HLcontrol subsystems
- High speed byte bus to FPGAcontrol subsystems

## LLcontrol subsystem tasks

- Communicate with HLcontrol computer through serial port
- Communicate with FPGA through a fast bidirectional 8-bit bus
- Implement PID contol of PWM/encoder DC motor channels
- Run sequences of command written in ubasic scripting language
- Run real-time clock(RTC) : battery backup system
- interface to CAN bus
- interface to Dynamixel digital servos
- interface to I2C sensors
