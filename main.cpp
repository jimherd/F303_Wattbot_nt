//***************************************************************************
//             Wattbot-nt motion control board uP controller
//***************************************************************************
//
// The robot vehicle system being developed consists of THREE subsystems
//
//  1.  HLcontrol   : High Level mission control (e.g. Raspberry Pi)
//  2.  LLcontrol   : Low Level uP control of motion and sensing (STM32F303RE)
//  3.  FPGAcontrol : Low level control of time constrained interfaces
//
// The code in this project relates to the LLcontrol subsystem and provides
// an interface between the high-level control computer and the FPGA system.
//
// Processor : STM32F303RE
// Framework : Mbed V5 (with RTOS)
// IDE       : Mbed Studio
// Compiler  : Arm V6
// SCC       : GIT/GITHUB
//
// Author : Jim Herd
//
// April 2020
//

#include "globals.h"

//***************************************************************************
// Define hardware objects
//
SerialDriver HLcontrol(USBTX, USBRX);     // tx, rx - buffered serial port
FPGA_bus  bus(1 /* PWM channel        */,
              1 /* Quadrature channel */,
              8 /* RC servo channels  */);

DigitalOut  led1(LED1);
DigitalOut  debug_pin(D15);

//***************************************************************************
// Function templates
//
void         init(void);

//***************************************************************************
// task templates.
//
void read_from_HLcontrol_task (void);
void write_to_HLcontrol_task  (void);

Thread read_from_HLcontrol(osPriorityNormal, OS_STACK_SIZE, NULL, NULL);
Thread write_to_HLcontrol(osPriorityNormal, OS_STACK_SIZE, NULL, NULL);

//***************************************************************************
// Mutex semaphores
//
//Mutex   sysdata_mutex;

//***************************************************************************
// Queues

Queue<char*, 16> HLcontrol_queue;  // holds replies being sent to HLcontrol

//***************************************************************************
// ** init   initialise hardware and system
//
void init(void)
{
//
// initialise hardware
//
    HLcontrol.baud(COM_BAUD);
    bus.initialise();
//
// start system tasks
//
    read_from_HLcontrol.start(callback(read_from_HLcontrol_task));
    write_to_HLcontrol.start(callback(write_to_HLcontrol_task));
}

//***************************************************************************
// ** main   
//
int main() {
  init();
  HLcontrol.printf("\n\n*** RTOS basic example ***\n");

  while (true) {
    led1 = !led1;
    ThisThread::sleep_for(3000); // 1 seconds
  }
}

//void print_char(char c = '*') {
//  debug_pin = !debug_pin;
//  //    pc.printf("%c", c);       // 50uS
//  HLcontrol.putc('*'); // 16uS
//  debug_pin = !debug_pin;
//  fflush(stdout);
//}

//Thread thread;

//void print_thread() {
//uint64_t now;
//
//    while (true) {
//        now = Kernel::get_ms_count();   // 2.29uS
//        ThisThread::sleep_until(now + 3);
//        print_char(); 
//    }
//}

