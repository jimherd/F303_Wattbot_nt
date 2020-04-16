/* 
 * Wattbot-nt motion control board uP controller
 *
 * The overall system consists of THREE subsystems
 * Provides an interface between the high-level control computer
 * and the FPGA system.
 *  1.  HLcontrol   : High Level mission control (e.g. Raspberry Pi
 *  2.  LLcontrol   : Low Level uP control of motion and sensing (STM32F303RE)
 *  3.  FPGAcontrol : Low level control of time constrained interfaces
 *
 * The code in this project relates to the LLcontrol subsystem.
 *
 * Processor : STM32F303RE
 * Framework : Mbed V5 (with RTOS)
 * IDE       : Mbed Studio
 * Compiler  : Arm V6
 * SCC       : GIT/GITHUB
 *
 * Author : Jim Herd
 *
 * April 2020
 */

#include "globals.h"

//*******************************************
// Define hardware objects
//
SerialDriver pc(USBTX, USBRX);     // tx, rx - buffered serial port
FPGA_bus  bus(1 /* PWM channel        */,
              1 /* Quadrature channel */,
              8 /* RC servo channels  */);

DigitalOut  led1(LED1);
DigitalOut  debug_pin(D15);

//*******************************************
// Function templates
//
//void         init(void);

//*******************************************
// task templates.
//
//void motor_spd_servo ( void const *args );

//*******************************************
// Mutex semaphores
//
//Mutex   sysdata_mutex;

/*********************************************
 ** init   initialise hardware and system
 */
void init(void)
{
    pc.baud(COM_BAUD);
    bus.initialise();
}   
    
void print_char(char c = '*')
{
    debug_pin = !debug_pin;
//    pc.printf("%c", c);       // 50uS
    pc.putc('*');             // 16uS
    debug_pin = !debug_pin;
    fflush(stdout);
}

Thread thread;


void print_thread() {
uint64_t now;

    while (true) {
        now = Kernel::get_ms_count();   // 2.29uS
        ThisThread::sleep_until(now + 3);
        print_char(); 
    }
}

int main()
{
    init();
    pc.printf("\n\n*** RTOS basic example ***\n");

    thread.start(print_thread);

    while (true) {
        led1 = !led1;
        ThisThread::sleep_for(3000);   // 1 seconds
    }
}

