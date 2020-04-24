//
// globals.h : file to hold some general system defines
// =========
//
// Specific defines are in file "Wattbot_nt.h"

#ifndef     GLOBALS_H
#define     GLOBALS_H

//***************************************************************************
// Necessary include files

#include    <cstdint>
#include    <cstdlib>

#include    "mbed.h"
#include    "rtos.h"

#include    "FPGA_bus.h"
#include    "SerialDriver.h"

#include    "Wattbot_nt.h"
#include    "read_from_HLcontrol_task.h"
#include    "write_to_HLcontrol_task.h"
#include    "ROM_data.h"

//***************************************************************************
// Macros

#define     FOREVER     for(;;)
#define     HANG        for(;;)

//***************************************************************************
// Extern references to hardware interfaces

extern FPGA_bus  bus;
extern DigitalOut  led1;
extern SerialDriver HLcontrol;

extern  Mail<reply_t,8> HLcontrol_reply_queue;  // holds replies being sent to HLcontrol

#endif