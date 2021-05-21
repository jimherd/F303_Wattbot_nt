//***************************************************************************
// Wattbot_nt.h : Constants and typedef structres for Wattbot-nt robot
//***************************************************************************

#include    <cstdint>
#include    "mbed.h"
#include    "rtos.h"
#include    "globals.h"

#ifndef  WATTBOT_NT_H
#define  WATTBOT_NT_H
/*
//***************************************************************************
// COM port data
//
// JTH Laptop <--> F303 working baud rates
//      115200, 128000, 230400, 256000, 460800
//
#define     COM_BAUD  256000      


//***************************************************************************
// System data structures
//***************************************************************************
//
// Packet structures for queues to connection to external hardware
//
// 1. Structure of command sent to FPGA_IO task

typedef struct {
    uint8_t    port;
    uint8_t    command;
    uint8_t    register_number;
    uint32_t    data;
} LLcontrol_to_FPGAcontrol_queue_t;

//
// 2. Structure of response sent to HLcontrol

typedef struct {
    char    reply[80];
} reply_t;

//
// 3. Structure of command sent to sequencer task 

typedef struct {
    uint8_t    port;
    uint8_t    sequence_number;
    int32_t    parameters[NOS_SEQUENCE_PARAMETERS];
} sequence_command_queue_t;


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

*/

#endif