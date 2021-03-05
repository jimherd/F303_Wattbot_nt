/*
 * General constants for read_from_HLcontrol_task.cpp
 */

 #include    <cstdint>
 
#ifndef  FPGA_IO_task_H
#define  FPGA_IO_task_H

//***************************************************************************
// Function templates

//***************************************************************************
// Constants relevant to this task

enum {READ_REGISTER=0, WRITE_REGISTER, SOFT_PING_FPGA, HARD_PING_FPGA, FPGA_RESTART};

#endif