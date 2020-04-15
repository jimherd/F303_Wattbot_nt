/* 
 * Wattbot-nt motion control board uP controller
 *
 * Provides an interface between the high-level control computer
 * and the FPGA system.
 *
 * Author : Jim Herd
 */

#include "globals.h"

//*******************************************
// Define hardware objects
//
SerialDriver pc(USBTX, USBRX);     // tx, rx - buffered serial port
FPGA_bus  bus(1,1,8);

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

