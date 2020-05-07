//***************************************************************************
// ASCII_convert.h : ASCII conversion routines
//***************************************************************************

 #include    <cstdint>
 
#ifndef  ASCII_CONVERT_H
#define  ASCII_CONVERT_H

//***************************************************************************
// Function templates

int32_t ASCII_to_int(char *str);
float ASCII_to_float(const char *char_pt);
char* int_to_ASCII(int32_t num, char* str);

#endif