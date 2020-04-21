/*
 * General constants and typedef structres for Wattbot-nt robot
 */
 
 //#include    <cstdint>
 

#ifndef  ExCmdFromHLcontrol_task_H
#define  ExCmdFromHLcontrol_task_H

#include  "globals.h"

//*********************************************************
// Function templates

uint32_t read_command_from_HLcontrol(void);

#define     HLCONTROL_IN_PACKET_COMMAND         0
#define     HLCONTROL_IN_PACKET_PORT            1
#define     HLCONTROL_IN_PACKET_DATA_BYTES      2  

#define     HLCONTROL_IN_PACKET_DATA_SIZE_TOO_BIG       -1

#define     MAX_COMMAND_LENGTH      100
#define     MAX_COMMAND_STRINGS      16
#define     CMD_STRING_TOO_BIG       -6

#endif

