/*
 * FPGA_bus : 8-bit bi-directional bus between uP and FPGA
 */
#include "mbed.h"
#include "FPGA_bus.h"

/** create a FPGA_bus object connecting uP to FPGA
 *
 * @param  nos_PWM      Number of PWM channels (default = 4)
 * @param  nos_QE       Number of Quadrature Encoder channels (default = 4)
 * @param  nos_servo    Number of RC servo channels (default = 8)
 *
 * Notes
 *      You can only change the defaults by recompiling the SystemVerilog code
 *      on the FPGA.
 */ 
 
//////////////////////////////////////////////////////////////////////////

FPGA_bus::FPGA_bus(int nos_PWM   /* = NOS_PWM_CHANNELS */, 
                   int nos_QE    /* = NOS_QE_CHANNELS  */, 
                   int nos_servo /* = NOS_RC_CHANNELS  */ )
                   
                :   
                    _nos_PWM_units(nos_PWM),
                    _nos_QE_units(nos_QE),
                    _nos_servo_units(nos_servo),   
                                            
                    async_uP_start(ASYNC_UP_START_PIN), 
                    async_uP_handshake_1(ASYNC_UP_HANDSHAKE_1_PIN),
                    async_uP_RW(ASYNC_UP_RW_PIN),
                    async_uP_reset(ASYNC_UP_RESET_PIN),
                    uP_ack(ASYNC_UP_ACK_PIN),
                    uP_handshake_2(ASYNC_UP_HANDSHAKE_2_PIN),
                    log_pin(LOG_PIN)
                    
        {
        async_uP_start   = LOW;
        PWM_base         = 0;
        QE_base          = 0;
        RC_base          = 0;
}

//////////////////////////////////////////////////////////////////////////
// do_reset : reset FPGA system
// ========
//
// Generate a 20uS LOW going pulse to set FPGA into initial state

void FPGA_bus:: do_reset(void)
{
    async_uP_reset = LOW;       // generate low reset pulse
    wait_us(20);
    async_uP_reset = HIGH;
    wait_us(20);
}
 
//////////////////////////////////////////////////////////////////////////
// initialise : Configure uP to FPGA data bus
// ==========
//
// Do the following
//  1. Set output signals to known values
//  2. Prod FPGA to check that it is responding correctly
//          Code  will make 3 attempts before returning with an ERROR    
//  3. Read register ZERO to get number of units in FPGA
//          This data is used to initialise the correct register pointers

int32_t  FPGA_bus::initialise(void)
{
int32_t     status;

    status = NO_ERROR;
    
    // GPIOC Periph clock enable
    
    ENABLE_GPIO_SUBSYSTEM;
    wait_us(2);
    
    async_uP_start       = LOW;
    async_uP_reset       = HIGH;
    async_uP_handshake_1 = LOW;
    async_uP_RW          = LOW;
    
    do_reset();
//
// Test to see if FPGA is alive and well
//    
    status = hard_check_bus();
    if (status != NO_ERROR) {
        global_FPGA_unit_error_flag = status;
        return status;
    }  
//
// Seems OK, now read register 0 and get basic system parameters
//
    wait_us(1000);
    get_SYS_data();
    
    if (global_FPGA_unit_error_flag != NO_ERROR){
        return global_FPGA_unit_error_flag;
    }
    global_FPGA_unit_error_flag = NO_ERROR;
    log_pin = LOW;
    return NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////  
// do_start : generate signals to start of bus transaction 
// ========

void FPGA_bus::do_start(void)
{
    async_uP_start       = HIGH;
    async_uP_handshake_1 = LOW;
    async_uP_start       = LOW;
}

//////////////////////////////////////////////////////////////////////////
// do_end : do actions to complete bus transaction
// ======

void FPGA_bus::do_end(void)
{
    while (uP_ack == HIGH)
        ;
    async_uP_start = LOW;
}

//////////////////////////////////////////////////////////////////////////
// write_byte : write a byte of data to the FPGA
// ==========

void FPGA_bus::write_byte(uint32_t byte_value)
{
    SET_BUS_OUTPUT;
    OUTPUT_BYTE_TO_BUS(byte_value);
    async_uP_RW = WRITE_BUS;
    async_uP_handshake_1 = HIGH;
    while (uP_handshake_2 == LOW)
        ;
    async_uP_handshake_1 = LOW;
    while (uP_handshake_2 == HIGH)
        ;
}

//////////////////////////////////////////////////////////////////////////
// read_byte : read a byte of data from the FPGA
// =========

uint32_t FPGA_bus::read_byte(void)
{
    SET_BUS_INPUT;
    async_uP_RW = READ_BUS;
    while (uP_handshake_2 == LOW)
        ;
    data = INPUT_BYTE_FROM_BUS;
    async_uP_handshake_1 = HIGH;
    while (uP_handshake_2 == HIGH)
        ;
    async_uP_handshake_1 = LOW;
    return data;
}

//////////////////////////////////////////////////////////////////////////
// do write : write a SIX byte command to FPGA
// ========

void FPGA_bus::do_write(uint32_t command, 
              uint32_t register_address, 
              uint32_t register_data)
{
    write_byte(command);
    write_byte(register_address);
    write_byte(register_data);
    write_byte(register_data>>8);
    write_byte(register_data>>16);
    write_byte(register_data>>24);
}

//////////////////////////////////////////////////////////////////////////
// do_read : Read the results of FPGA command execution
// =======

void FPGA_bus::do_read(received_packet_t   *buffer)
{
    for (int i=0; i < (NOS_RECEIVED_PACKET_WORDS<<2) ; i++) {
        buffer->byte_data[i] = (uint8_t)read_byte();
    }
}

//////////////////////////////////////////////////////////////////////////
// do_transaction : execute complete command and get results
// ==============

void FPGA_bus::do_transaction(uint32_t command, 
                    uint32_t register_address, 
                    uint32_t register_data,
                    uint32_t *data,
                    uint32_t *status)
{
    log_pin = HIGH;
    do_start();
    do_write(command, register_address, register_data);
    do_read(&in_pkt);
    do_end();
    log_pin = LOW;
    *data = in_pkt.word_data[0];
    *status = in_pkt.word_data[1];
}

////////////////////////////////////////////////////////////////////////// 
// set_PWM_period : set PWM period given frequency
// ==============
   
void FPGA_bus::set_PWM_period(uint32_t channel, float frequency)
{
    uint32_t register_address = ((PWM_base + (channel * NOS_PWM_REGISTERS)) + PWM_PERIOD);
    uint32_t period_value = (uint32_t)(1000000/(20 * frequency));
    do_transaction(WRITE_REGISTER_CMD, register_address, period_value, &data, &status); 
    sys_data.PWM_period_value[channel] = period_value;
    do_transaction(READ_REGISTER_CMD, (1 + (channel * NOS_PWM_REGISTERS)) , NULL, &data, &status);
    global_FPGA_unit_error_flag = status;
}

//////////////////////////////////////////////////////////////////////////
// set_PWM_duty : set puty time at % of period
// ============

void FPGA_bus::set_PWM_duty(uint32_t channel, float percentage)
{
    uint32_t register_address = ((PWM_base + (channel * NOS_PWM_REGISTERS)) + PWM_ON_TIME);
    uint32_t duty_value = (uint32_t)((sys_data.PWM_period_value[channel] * percentage) / 100);
    do_transaction(WRITE_REGISTER_CMD, register_address , duty_value, &data, &status);
    sys_data.PWM_duty_value[channel] = duty_value;
    global_FPGA_unit_error_flag = status;;
}

//////////////////////////////////////////////////////////////////////////
// PWM_enable : enable PWM output
// ==========

void FPGA_bus::PWM_enable(uint32_t channel)
{
    uint32_t register_address = ((PWM_base + (channel * NOS_PWM_REGISTERS)) + PWM_CONFIG);
    do_transaction(WRITE_REGISTER_CMD, register_address , 1, &data, &status);
    global_FPGA_unit_error_flag = status;;
}

//////////////////////////////////////////////////////////////////////////
// PWM_config : program PWM configuration register
// ==========

void FPGA_bus::PWM_config(uint32_t channel, uint32_t config_value)
{
    uint32_t register_address = ((PWM_base + (channel * NOS_PWM_REGISTERS)) + PWM_CONFIG);
    do_transaction(WRITE_REGISTER_CMD, register_address , config_value, &data, &status);
    sys_data.PWM_duty_value[channel] = config_value;
    global_FPGA_unit_error_flag = status;;
}

//////////////////////////////////////////////////////////////////////////
// set_RC_period : set RC period to 20mS for all servos
// =============
//
// No parameter therefore assume 20mS period
//
// FPGA clock = 20nS => 20mS = 1,000,000 clocks counts

void FPGA_bus::set_RC_period(void)
{
    do_transaction(WRITE_REGISTER_CMD, (RC_base + RC_SERVO_PERIOD), 1000000, &data, &status);
    global_FPGA_unit_error_flag = status;
}

//////////////////////////////////////////////////////////////////////////
// set_RC_period : set RC pulse in units of uS for all servos
// =============
//

void FPGA_bus::set_RC_period(uint32_t duty_uS)
{
    uint32_t nos_20nS_ticks = ((duty_uS * nS_IN_uS)/FPGA_CLOCK_PERIOD_nS);
    do_transaction(WRITE_REGISTER_CMD, (RC_base + RC_SERVO_PERIOD), nos_20nS_ticks, &data, &status);
    global_FPGA_unit_error_flag = status;
}

//////////////////////////////////////////////////////////////////////////
// set_RC_pulse : set RC pulse width in uS for a particular servo
// ============
//

void FPGA_bus :: set_RC_pulse(uint32_t channel, uint32_t pulse_uS)
{
    uint32_t nos_20nS_ticks = ((pulse_uS * nS_IN_uS)/FPGA_CLOCK_PERIOD_nS);
    do_transaction(WRITE_REGISTER_CMD, (RC_base + RC_SERVO_ON_TIME + channel), nos_20nS_ticks, &data, &status);
    global_FPGA_unit_error_flag = status;;    
}

//////////////////////////////////////////////////////////////////////////
// enable_RC_channel : enable RC servo channel output
// =================

void FPGA_bus::enable_RC_channel(uint32_t channel)
{
    do_transaction(READ_REGISTER_CMD, (RC_base + RC_SERVO_CONFIG), NULL, &data, &status);
    int32_t config = (data || (0x01 << channel)) + GLOBAL_RC_ENABLE;
    do_transaction(WRITE_REGISTER_CMD, (RC_base + RC_SERVO_CONFIG), config, &data, &status);
    global_FPGA_unit_error_flag = status;
}

//////////////////////////////////////////////////////////////////////////
// disable_RC_channel : disable RC servo channel output
// ==================

void FPGA_bus::disable_RC_channel(uint32_t channel)
{
    do_transaction(READ_REGISTER_CMD, (RC_base + RC_SERVO_CONFIG), NULL, &data, &status);
    uint32_t config = data && ~(0x01 << channel);
    do_transaction(WRITE_REGISTER_CMD, (RC_base + RC_SERVO_CONFIG), config, &data, &status);
    global_FPGA_unit_error_flag = status;
}

//////////////////////////////////////////////////////////////////////////
// QE_config : configure quadrature encoder unit
// =========

void FPGA_bus::QE_config(uint32_t channel, uint32_t config_value)
{
    uint32_t register_address = ((PWM_base + (channel * NOS_PWM_REGISTERS)) + PWM_CONFIG);
    do_transaction(WRITE_REGISTER_CMD, register_address , config_value, &data, &status);
    global_FPGA_unit_error_flag = status;;
}

//////////////////////////////////////////////////////////////////////////
// enable_speed_measure : enable speed calculation of encoder channel
// ====================

void FPGA_bus::enable_speed_measure(uint32_t channel, uint32_t config_value, uint32_t phase_time)
{
    uint32_t register_base = (QE_base + (channel * NOS_QE_REGISTERS));
    do_transaction(WRITE_REGISTER_CMD, (register_base + QE_SIM_PHASE_TIME), phase_time, &data, &status);
    global_FPGA_unit_error_flag = status;
    if (status != NO_ERROR) {
        return;
    }
    do_transaction(WRITE_REGISTER_CMD, (register_base + QE_CONFIG), config_value, &data, &status);
    global_FPGA_unit_error_flag = status;
}

//////////////////////////////////////////////////////////////////////////
// read_speed_measure : read current speed measurement
// ==================

uint32_t FPGA_bus::read_speed_measure(uint32_t channel)
{
    uint32_t register_address = ((QE_base + (channel * NOS_QE_REGISTERS)) + QE_SPEED_BUFFER);
    do_transaction(READ_REGISTER_CMD, register_address, NULL, &data, &status);
    global_FPGA_unit_error_flag = status;
    return data;
}

//////////////////////////////////////////////////////////////////////////
// read_count_measure : read encoder counter register
// ==================

uint32_t FPGA_bus::read_count_measure(uint32_t channel)
{
    uint32_t register_address = ((QE_base + (channel * NOS_QE_REGISTERS)) + QE_COUNT_BUFFER);
    do_transaction(READ_REGISTER_CMD, register_address, NULL, &data, &status);
    global_FPGA_unit_error_flag = status;
    return data;
}

//////////////////////////////////////////////////////////////////////////
// get_SYS_data : read system data from register 0
// ============
//
// Notes
//      Register data has information to define where the subsystem registers
//      are located.  The software will therefore be independent of the 
//      number of subsystems in the FPGA

uint32_t FPGA_bus::get_SYS_data(void)
{
    do_transaction(READ_REGISTER_CMD, SYS_DATA_REG_ADDR, NULL, &data, &status);
    sys_data.major_version          = (data & 0x0000000F);
    sys_data.minor_version          = (data >> 4)  & 0x0000000F;
    sys_data.number_of_PWM_channels = (data >> 8)  & 0x0000000F;
    sys_data.number_of_QE_channels  = (data >> 12) & 0x0000000F;
    sys_data.number_of_RC_channels  = (data >> 16) & 0x0000000F;
    
    update_FPGA_register_pointers();
    
    global_FPGA_unit_error_flag = status;
    return data;
}
//////////////////////////////////////////////////////////////////////////
// update_FPGA_register_pointers : set pointer to FPGA register bank
// =============================
//
// Accessing the units within the FPGA is by reading and writing to registers.
// This block of registers is a contiguous block starting at ZERO, up to
// a maximum of 255.  This implies that if the FPGA is configured with more
// PWM units (say), then the addresses of the later units are changed.  
//
// To overcome this problem, register ZERO contains a note of the number of
// PWM, QE, abd RC servo units.  This address is fixed and is read to 
// calculate the relevant register address pointers.
//

void FPGA_bus::update_FPGA_register_pointers(void) {
    
    PWM_base = 1;
    QE_base  = ((NOS_PWM_REGISTERS * sys_data.number_of_PWM_channels) + PWM_base);
    RC_base  = ((NOS_QE_REGISTERS * sys_data.number_of_QE_channels) + QE_base);   
}

//////////////////////////////////////////////////////////////////////////
// hard_check_bus : verify that connection to FPGA is working (uses RESET)
// ==============
//
// Notes
//      Timeout on a first handshake after a RESET. Uses first part of
//      'write_byte' handshake to test bus
// Return
//      NO_ERROR   = bus active and replying correctly
//      BUS_FAIL_1 = unable to get response from FPGA through bus
//      BUS_FAIL_2 = unable to get response from FPGA through bus
//
int32_t FPGA_bus::hard_check_bus(void)
{
    do_reset();
    wait_us(uS_DELAY_BEFORE_TEST_HANDSHAKE);
    
    if (uP_handshake_2 == HIGH) {
        return  BUS_FAIL_1;       // handshake line in wrong init state
    }
    
    do_start();
    SET_BUS_INPUT;
    async_uP_RW = READ_BUS;
    SET_BUS_OUTPUT;
    OUTPUT_BYTE_TO_BUS(0);
    async_uP_RW = WRITE_BUS;
    async_uP_handshake_1 = HIGH;
    wait_us(uS_DELAY_BEFORE_TEST_HANDSHAKE);
    
    if (uP_handshake_2 == LOW) {
        return BUS_FAIL_2;
    }  
//
// set system back to original state
//
    async_uP_start       = LOW;
    async_uP_reset       = HIGH;
    async_uP_handshake_1 = LOW;
    async_uP_RW          = LOW;
    do_reset();
    return NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////
// soft_check_bus : verify that connection to FPGA is working (no RESET)
// ==============
//
// Notes
//      FPGA-uP bus FSM special cased to accept a handshake_2 signal BEFORE
//      a START signal.  If handshake replies are not recieved in a timely
//      manner, error codes are returned.
// Return
//      NO_ERROR   = bus active and replying correctly
//      BUS_FAIL_1 = handshake line in wrong init state
//      BUS_FAIL_2 = no handshake response
//      BUS_FAIL_3 = handshake failed to terminate correctly
//

int32_t FPGA_bus::soft_check_bus(void)
{
uint32_t  timeout_counter;

    timeout_counter = HANDSHAKE_TIMEOUT_COUNT;
    while (uP_handshake_2 == HIGH) {
        timeout_counter--;
        if (timeout_counter == 0) {
            return  BUS_FAIL_1;       // handshake line in wrong init state
        }
    }
    async_uP_handshake_1 = HIGH;
    timeout_counter = HANDSHAKE_TIMEOUT_COUNT;
    while (uP_handshake_2 == LOW) {
        timeout_counter--;
        if (timeout_counter == 0) {
            async_uP_handshake_1 = LOW;
            return  BUS_FAIL_1;       // no handshake response
        }
    }
    async_uP_handshake_1 = LOW;
    timeout_counter = HANDSHAKE_TIMEOUT_COUNT;
    while (uP_handshake_2 == HIGH) {
        timeout_counter--;
        if (timeout_counter == 0) {
            return  BUS_FAIL_2;       // handshake failed to terminate correctly
        }
    }    
    return NO_ERROR;
}

    