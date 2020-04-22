/*
 * General constants and typedef structres for Wattbot-nt robot
 */

 #include    <cstdint>
 

#ifndef  ExCmdFromHLcontrol_task_H
#define  ExCmdFromHLcontrol_task_H

//*********************************************************
// Function templates

uint32_t read_command_from_HLcontrol(void);
uint32_t parse_command (void);
uint32_t execute_command(void);

//*********************************************************
// Constants relevant to this task

enum {MODE_U, MODE_I, MODE_R, MODE_S} modes;  // defines modes as scan progresses

#define   LETTER_ERROR     -12;  
#define   DOT_ERROR      -13;
#define   PLUSMINUS_ERROR   -14


#define     MAX_COMMAND_LENGTH         100
#define     MAX_COMMAND_PARAMETERS      16

#endif

