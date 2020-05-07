//***************************************************************************
// Wattbot_nt.h : Constants and typedef structres for Wattbot-nt robot
//***************************************************************************

#include    <cstdint>
#include    "mbed.h"
#include    "rtos.h"

#ifndef  WATTBOT_NT_H
#define  WATTBOT_NT_H

//********************
// COM port data
//
#define     COM_BAUD    115200


//***************************************************************************
// System data structures
//***************************************************************************
//
// Queues for connection to external hardware
//
// 1. Queue of command being sent to FPGA from uP

typedef struct {
    uint8_t     port;
    uint8_t     command;
    uint8_t     register_number;
    uint32_t    data;
} LLcontrol_to_FPGAcontrol_queue_t;

//
// 2. queue of responses being sent to HLcontrol

typedef struct {
    char    reply[80];
} reply_t;


//***************************************************************************
// Set of commands that can be sent fro HLcontrol to uP

typedef enum {
    PING = 1,
    FPGA_WRITE,
    FPGA_READ,
    SET_PID,
    READ_PID,
    DYNAMIXEL_WRITE,
    DYNAMIXEL_READ,
    uP_CONFIG,
    uP_STATUS
} LLcontrol_commands_t;

//***************************************************************************
// macros
//
#define SET_PROBE_1     probe_1_pin=1
#define CLR_PROBE_1     probe_1_pin=0



#endif