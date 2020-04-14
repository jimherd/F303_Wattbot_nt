//
// globals.h : file to hold some general global defines, 
// =========
//

#include    "mbed.h"
#include    "rtos.h"
#include    "Wattbot_nt.h"
#include    "FPGA_bus.h"
#include    "SerialDriver.h"

#ifndef     GLOBALS_H
#define     GLOBALS_H

#define     FOREVER     for(;;)
#define     HANG        for(;;)

extern DigitalOut  led1;

#endif