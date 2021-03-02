#include "mbed.h"

#ifndef  COMPILE_CONFIG_H
#define  COMPILE_CONFIG_H

//////////////////////////////////////////////////////////////////////////
// Compile time configuration parameters
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 1.
// System can be configured to return ONE or TWO 32-bit values from the FPGA.
//
//      first value  : 32-bit data value
//      second value : 32-bit status value
//
// In practice, the status word carries little or no information but consumes
// four 8-bit transactions between the FPGA and the uP.
//
// Uncomment following #define to enable status word to be returned.

//#define INCLUDE_32_BIT_STATUS_RETURN

//////////////////////////////////////////////////////////////////////////
// 2.
// Hand coded optimisations for handshake with FPGA

#define   OPT_1      // Implement handshake I/O with direct access to BSRR register
#define   OPT_2      // in-line "read_byte" and "write_byte" functions
#define   OPT_3      // in-line "do_read" and "do_write" functions
 
#endif