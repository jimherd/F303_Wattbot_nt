//***************************************************************************
// globals.h : file to hold some general system defines
// =========
//***************************************************************************
//
// Specific defines are in file "Wattbot_nt.h"

#ifndef     GLOBALS_H
#define     GLOBALS_H

//***************************************************************************
// Necessary include files

#include    "compile_config.h"

#include    <cstdint>
#include    <cstdlib>

#include    "mbed.h"
#include    "rtos.h"

#include    "FPGA_bus.h"

#include    "Wattbot_nt.h"
#include    "error_codes.h"
#include    "read_from_HLcontrol_task.h"
#include    "write_to_HLcontrol_task.h"
#include    "FPGA_IO_task.h"
#include    "ROM_data.h"
#include    "sys_routines.h"

//***************************************************************************
// Macros

#define     FOREVER     for(;;)
#define     HANG        for(;;)

//***************************************************************************
// Extern references to hardware interfaces

extern FPGA_bus  bus;
extern DigitalOut  led1;
extern BufferedSerial HLcontrol;

//***************************************************************************
// Extern references to system queues

extern  Mail<reply_t,8> HLcontrol_reply_queue;  // holds replies being sent to HLcontrol
extern  Mail<LLcontrol_to_FPGAcontrol_queue_t,8> FPGA_cmd_queue;

#endif