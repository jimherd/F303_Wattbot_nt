/* 
 * Wattbot-nt motion control board uP controller
 *
 * The overall system consists of THREE subsystems
 * Provides an interface between the high-level control computer
 * and the FPGA system.
 *
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

//*********************************************************
// Define hardware objects
//
SerialDriver HLcontrol(USBTX, USBRX);     // tx, rx - buffered serial port
FPGA_bus  bus(1 /* PWM channel        */,
              1 /* Quadrature channel */,
              8 /* RC servo channels  */);

DigitalOut  led1(LED1);
DigitalOut  debug_pin(D15);

//*********************************************************
// Function templates
//
void         init(void);

///*********************************************************
// task templates.
//
void read_from_HLcontrol_task ( void const *args );

//*********************************************************
// Mutex semaphores
//
//Mutex   sysdata_mutex;

//*********************************************************
// Task code
//*********************************************************
// read_from_HLcontrol_task : Read commands from High Level control
// ========================
//
// Read commands from UART input channel that connects to High Level
// control computer.

#define     HLCONTROL_IN_PACKET_HEADER_SIZE     3
#define     HLCONTROL_IN_PACKET_MAX_DATA_SIZE  16

#define     HLCONTROL_IN_PACKET_COMMAND         0
#define     HLCONTROL_IN_PACKET_PORT            1
#define     HLCONTROL_IN_PACKET_DATA_BYTES      2  

#define     HLCONTROL_IN_PACKET_DATA_SIZE_TOO_BIG       -1

struct  {
    uint8_t     header[HLCONTROL_IN_PACKET_HEADER_SIZE];
    uint8_t     data[HLCONTROL_IN_PACKET_MAX_DATA_SIZE];
} HLcontol_in_packet;

void read_from_HLcontrol(void const *args)
{
uint32_t   read_from_HLcontrol_error; 

    read_from_HLcontrol_error = 0;
    FOREVER {
    // read packet header from HLcontrol
        HLcontrol.read(&HLcontol_in_packet.header[0], HLCONTROL_IN_PACKET_HEADER_SIZE);
        if (HLcontol_in_packet.header[HLCONTROL_IN_PACKET_DATA_BYTES] > HLCONTROL_IN_PACKET_MAX_DATA_SIZE) {
            read_from_HLcontrol_error = HLCONTROL_IN_PACKET_DATA_SIZE_TOO_BIG;
        } else {
    // read packet data from HLcontrol
            HLcontrol.read(&HLcontol_in_packet.data[0], HLcontol_in_packet.header[HLCONTROL_IN_PACKET_DATA_BYTES]);
        }
    }
}

//*********************************************************
// ** init   initialise hardware and system
//
void init(void)
{
    HLcontrol.baud(COM_BAUD);
    bus.initialise();
}   
    
void print_char(char c = '*')
{
    debug_pin = !debug_pin;
//    pc.printf("%c", c);       // 50uS
    HLcontrol.putc('*');             // 16uS
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
    HLcontrol.printf("\n\n*** RTOS basic example ***\n");

    thread.start(print_thread);

    while (true) {
        led1 = !led1;
        ThisThread::sleep_for(3000);   // 1 seconds
    }
}

