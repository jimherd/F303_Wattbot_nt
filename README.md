# uP control of Wattbot-nt robot vehicle

## Overview

The control of the Wattbot-nt robot vehicle consists of three subsystems

1. **HLcontrol**   : **H**igh **L**evel misssion control of the system (e.g.raspberry pi)
2. **LLcontrol**   : **L**ow **L**evel motion and sensor control
3. **FPGAcontrol** : Time critical control of sensor and actuator interfaces

The LLcontrol system has the following characteristics

- Based on STM32F303RE microprocessor
- Programmed with the MbedOS/RTOS system (Mbed Studio)
- UART connection to the HLcontrol subsystems
- High speed byte bus to FPGAcontrol subsystems

##Tasks

- Communicate with HLcontrol computer through serial port
- Communicate with FPGA through a fast bidirectional 8-bit bus
- Implement PID contol of PWM/encoder DC motor channels
- Run real-time clock(RTC) : battery backup system
- interface to CAN bus
- interface to Dynamixel digital servos
- interface to I2C sensors
