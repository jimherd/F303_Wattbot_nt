//***************************************************************************
// error_codes.h : file to LLcontrol system error codes
// =============
//
// Central collection of system error codes

#ifndef     _ERROR_CODES_H_
#define     _ERROR_CODES_H_

//*********************************************************
 // error codes

typedef enum {
    OK                      =   0, 
    CMD_FAIL                =  -1,
    CMD_STRING_TOO_BIG      =  -2,
// read_from_HLcontrol_task
    LETTER_ERROR            =- 10, 
    DOT_ERROR               = -11,
    PLUSMINUS_ERROR         = -12,
    BAD_COMMAND             = -13,
    BAD_PORT_NUMBER         = -14,
//
} error_codes_t;

#endif