/*
* General constants and typedef structres for Wattbot-nt robot
 */

#include    <cstdint>

#ifndef  WATTBOT_NT_H
#define  WATTBOT_NT_H

//********************
// COM port data
//
#define     COM_BAUD    921600

#define     MAX_DATA_BYTES_HLCONTROL_TO_LLCONTROL       16
#define     MAX_DATA_BYTES_LLCONTROL_TO_HLCONTROL       16

//*********************************************************
// System data structures
//
// Queues for connection to external hardware
//
// Queue of command being sent to FPGA from uP

typedef struct {
    uint8_t     register_number;
    uint8_t     command;
    uint32_t    data;
} LLcontrol_to_FPGAcontrol_queue_t;

//
// queue of responses being sent to HLcontrol

typedef struct {
    uint8_t     port; 
    uint8_t     nos_bytes; 
    uint8_t     data[MAX_DATA_BYTES_LLCONTROL_TO_HLCONTROL];
} LLcontrol_to_HLcontrol_queue_t;


//*********************************************************
// format of data packets from HLcontrol

typedef struct {
    uint8_t     command;
    uint8_t     port;
    uint8_t     nos_bytes;
    uint8_t     data[MAX_DATA_BYTES_HLCONTROL_TO_LLCONTROL];
}  HLcontrol_LLcontrol_packet_t;

//*********************************************************
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
 
 //*********************************************************
 // error codes

typedef enum {
    CMD_SUCCESS                    =  0, 
    CMD_FAIL                       = -1,
} error_codes_t;

//********************
// macros
//
#define SET_PROBE_1     probe_1_pin=1
#define CLR_PROBE_1     probe_1_pin=0

#endif