/*
 * General constants for read_from_HLcontrol_task.cpp
 */

 #include    <cstdint>
 
#ifndef  _FPGA_IO_TASK_H_
#define  _FPGA_IO_TASK_H_

//***************************************************************************
// Function templates

//***************************************************************************
// Constants relevant to this task

enum {READ_REGISTER=0, WRITE_REGISTER, SOFT_PING_FPGA, HARD_PING_FPGA, FPGA_RESTART};

#endif