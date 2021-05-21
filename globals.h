//***************************************************************************
// globals.h : file to hold some global system defines/data structures/macros
// =========
//***************************************************************************
//

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

//***************************************************************************
// Constants relevant to overall system operation

#define     NOS_SEQUENCE_PARAMETERS     4       // number of int parameters passed to sequencer task


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



//***************************************************************************
// Global Macros

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