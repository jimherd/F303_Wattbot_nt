//***************************************************************************
// sys_routines.h : ASCII conversion routines
//***************************************************************************

 #include    <cstdint>
 
#ifndef  _SYS_ROUTINES_H_
#define  _SYS_ROUTINES_H_

//***************************************************************************
// Function templates

int32_t ASCII_to_int(char *str);
float   ASCII_to_float(const char *char_pt);
char*   int_to_ASCII(int32_t num, char* str);
void    string_to_queue(const char *str);

#endif