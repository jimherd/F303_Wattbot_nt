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
// Framework : Mbed V6 (with RTOS)
// IDE       : Mbed Studio
// Compiler  : Arm V6
// SCC       : GIT/GITHUB
//
// Author : Jim Herd
//
// April 2020
//

#include    "mbed.h"
#include    "rtos.h"
#include    "sys_routines.h"
#include    "FPGA_bus.h"
#include    "error_codes.h"
#include    "globals.h"

//***************************************************************************
// Define hardware objects
//
//BufferedSerial Sys_Debug(USBTX, USBRX);     // tx, rx - buffered serial port
BufferedSerial HLcontrol(PB_6, PB_7);
//FileHandle *mbed::mbed_override_console(int fd) {
//
//    return &Sys_Debug;
//}

FPGA_bus  bus(1 /* PWM channel        */,
              1 /* Quadrature channel */,
              8 /* RC servo channels  */);

DigitalOut  led1(LED1);
DigitalOut  debug_pin(D15);

char str[80];

//***************************************************************************
// Function templates
//
uint32_t         init(void);

//***************************************************************************
// task templates.
//
void read_from_HLcontrol_task (void);
void write_to_HLcontrol_task  (void);
void FPGA_IO_task(void);

Thread read_from_HLcontrol(osPriorityNormal, 8192, NULL, NULL);
Thread write_to_HLcontrol(osPriorityNormal, OS_STACK_SIZE, NULL, NULL);
Thread FPGA_IO(osPriorityBelowNormal, 8192, NULL, NULL);

//***************************************************************************
// Mutex semaphores
//
//Mutex   sysdata_mutex;

//***************************************************************************
// Mail queues
//
// Queue 1 : FPGA commands
//
Mail<LLcontrol_to_FPGAcontrol_packet_t,8> FPGA_cmd_queue;

// Queue 2 : replies to high level controller
//
Mail<reply_packet_t, 8> HLcontrol_reply_queue; 

// Queue 3 : commands to sequencer task
//
Mail<sequence_command_packet_t, 4> sequence_command_queue;

// Queue 4 : FPGA replies from sequencer commands
//
Mail<reply_packet_t, 8> sequencer_reply_queue; 

//***************************************************************************
// ** init   initialise hardware and system
//
uint32_t init(void)
{
//
// initialise hardware
//
    HLcontrol.set_baud(COM_BAUD);
    bus.initialise();
    if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
        sprintf(str, "D: bus init error : %d\r\n", bus.global_FPGA_unit_error_flag);
        HLcontrol.write(str, strlen(str));
        return bus.global_FPGA_unit_error_flag;
    }
    bus.soft_check_bus();

    sprintf(str, "D: PWM channels : %d\n", bus.sys_data.number_of_PWM_channels);
    HLcontrol.write(str, strlen(str));
//
// start system tasks
//
    read_from_HLcontrol.start(callback(read_from_HLcontrol_task));
    write_to_HLcontrol.start(callback(write_to_HLcontrol_task));
    FPGA_IO.start(callback(FPGA_IO_task));

    return NO_ERROR;
}

//***************************************************************************
// ** main   
//
int main() 
{
uint32_t status;

    status = init();
    if (status != NO_ERROR) {
        led1 = 1;
        HANG;
    }
    string_to_queue((char *)"D:  System started\n");
  

  while (true) {
    led1 = !led1;
    ThisThread::sleep_for(3000ms); // 1 seconds
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

