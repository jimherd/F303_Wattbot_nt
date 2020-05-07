/*
 * General constants for read_from_HLcontrol_task.cpp
 */

 #include    <cstdint>
 
#ifndef  read_from_HLcontrol_task_H
#define  read_from_HLcontrol_task_H

//***************************************************************************
// Function templates

uint32_t read_command_from_HLcontrol(void);
uint32_t parse_command (void);
uint32_t convert_tokens(void);
uint32_t execute_command(void);
int32_t  ASCII_to_int(char *str);
float    ASCII_to_float(const char *char_pt);

//***************************************************************************
// Constants relevant to this task

enum {MODE_U, MODE_I, MODE_R, MODE_S} modes;  // defines modes as scan progresses

#define     MAX_COMMAND_LENGTH         100
#define     MAX_COMMAND_PARAMETERS      16

#endif

