//***************************************************************************
// Task code
//***************************************************************************
// read_from_HLcontrol_task : Read/execute commands from High Level control
// ========================
//
// Read commands from UART input channel that connects to High Level
// control computer.  Parse and execute.

#include  "sys_routines.h"
#include  "globals.h"

//***************************************************************************
// Globals for this task

char        command[MAX_COMMAND_LENGTH];
uint32_t    character_count;
uint32_t    argc, arg_pt[MAX_COMMAND_PARAMETERS], arg_type[MAX_COMMAND_PARAMETERS];
int         int_parameters[MAX_COMMAND_PARAMETERS];
float       float_parameters[MAX_COMMAND_PARAMETERS];

//***************************************************************************
// read_command_from_HLcontrol : Read ASCII command string from HLcontrol
//

int32_t read_command_from_HLcontrol(void) 
{
uint8_t     character;
int32_t    count, status;

    status = NO_ERROR;
    for(count=0 ; count < MAX_COMMAND_LENGTH ; count++) {
        HLcontrol.read(&character, 1);
        if (character == '\n') {
            command[count++] = '\0';
            command[count]   = '\n';
            break;
        } else {
            if ((character == '\r') || (character == ' ')) {
                command[count] = '\0';
            }else {
                command[count] = character;
            }
        }
    }
    if (count == (MAX_COMMAND_LENGTH-1)) {
        command[count++] = '\n';
        status = CMD_STRING_TOO_BIG;
    } else {
        status = NO_ERROR;
    }
    character_count = count;
    return status;
}

//***************************************************************************
// parse_command : analyse command string and convert into labelled strings
//
// Breaks the command string into a set of token strings that are 
// labelled REAL, INTEGER or STRING.  
//
// Code uses a STATE MACHINE to walkthrough the command string.  Refer
// to associated documentation.  

// defines modes as scan progresses : U=undefined, I=integer, R=real, S=string
//

int32_t parse_command (void) 
{
int32_t    count, mode, status;
uint8_t     character_type;

    argc = 0;
    mode = MODE_U;
    status = NO_ERROR;
    for (count=0 ; count <= character_count ; count++) {
        character_type = char_type[command[count]];
        switch (character_type) {
            case LETTER :
                if ((mode == MODE_I) || (mode == MODE_R)) {
                    status = LETTER_ERROR;
                } else {
                    if (mode == MODE_U) {
                        mode = MODE_S;
                        arg_pt[argc] = count;
                        // argc++;
                    } 
                }
                break;
            case NUMBER :
                if (mode == MODE_U) {
                    mode = MODE_I;
                    arg_pt[argc] = count;
                    // argc++;
                } 
                break;
            case DOT :
                if (mode == MODE_I) {
                    mode = MODE_R;
                } else {
                    if ((mode == MODE_R) || (mode == MODE_U)) {
                        status = DOT_ERROR;  // extra point in real value
                    } 
                }
                break;
            case PLUSMINUS :
                if (mode == MODE_U) {
                    mode = MODE_I;
                    arg_pt[argc] = count;
                    //argc++;
                } else {
                    if ((mode == MODE_I) || (mode == MODE_R)) {
                        status = PLUSMINUS_ERROR;
                    }
                }
                break;
            case NULTERM :
                if (mode != MODE_U) {
                    arg_type[argc++] = mode;
                    mode = MODE_U;
                }
                break;
            case END :
                if (mode != MODE_U) {
                    arg_type[argc++] = mode;
                    mode = MODE_U;
                }
                break;
        }   // end of SWITCH
    }  // end of FOR
    return status;
}

//***************************************************************************
// convert_tokens : convert relevant tokens to numerical values
//
int32_t convert_tokens(void) 
{
    if ((arg_type[0] != MODE_S) || (char_type[command[0]] != LETTER)) {
        return BAD_COMMAND;
    }
    for (uint32_t i=0 ; i <= argc ; i++) {
        switch (arg_type[i]) {
            case MODE_I :  // save as integer and float
                int_parameters[i] = ASCII_to_int(&command[arg_pt[i]]);
                float_parameters[i] = (float)int_parameters[i];
                break;
            case MODE_R :
                float_parameters[i] = ASCII_to_float(&command[arg_pt[i]]);
                break;
        }
    }
    if ((arg_type[1] != MODE_I) || (int_parameters[1] > 63)) {
        return BAD_PORT_NUMBER;
    }
    return NO_ERROR;
}

//***************************************************************************
// reply_to_HLcontrol : send reply
//
//void reply_to_HLcontrol(int32_t status)
//{
//    return;
//}

//***************************************************************************
// execute_command : run processed command
//
//  S - RC servo commands
//      Sc : config
//      Sp : set pulse period in uS
//      Sw : set width of pulse in uS
//
//  P - ping commands
//      Pu : ping microcontroller (no contact with FPGA)
//      Pf : ping FPGA 

int32_t execute_command(void) {

    uint32_t data, status;

    switch (command[0]) {
        case 'P' : {
            switch (command[1]) {
                case 'u' :  {        // PING microcontroller
                    reply_t *mail = HLcontrol_reply_queue.try_alloc_for(Kernel::wait_for_u32_forever);
                    sprintf(mail->reply, "%d 0\n", int_parameters[1]);
                    HLcontrol_reply_queue.put(mail);
                    break;
                }  // end case 'Pu'
                case 'f' :  {        // PING FPGA
                    LLcontrol_to_FPGAcontrol_queue_t *FPGA_command = FPGA_cmd_queue.try_alloc_for(Kernel::wait_for_u32_forever);
                    FPGA_command->port            = int_parameters[1];
                    FPGA_command->command         = SOFT_PING_FPGA;
                    FPGA_command->register_number = NULL;
                    FPGA_command->data            = NULL;            
                    FPGA_cmd_queue.put(FPGA_command);
                    break;
                }  // end case 'Pf'
            }
            break;
        }   // end case 'P'

        case 'r'  :   {   //read from FPGA register
            LLcontrol_to_FPGAcontrol_queue_t *FPGA_command = FPGA_cmd_queue.try_alloc_for(Kernel::wait_for_u32_forever);
            FPGA_command->port            = int_parameters[1];
            FPGA_command->command         = READ_REGISTER_CMD;
            FPGA_command->register_number = int_parameters[2];
            FPGA_command->data            = NULL; 
            //    string_to_queue((char *)"D: Received r CMD\n");
            FPGA_cmd_queue.put(FPGA_command);
            break;
        }

        case 'w'    :  {  // write to FPGA register
            LLcontrol_to_FPGAcontrol_queue_t *FPGA_command = FPGA_cmd_queue.try_alloc_for(Kernel::wait_for_u32_forever);
            FPGA_command->port            = int_parameters[1];
            FPGA_command->command         = WRITE_REGISTER_CMD;
            FPGA_command->register_number = int_parameters[2];
            FPGA_command->data            = int_parameters[3];
            FPGA_cmd_queue.put(FPGA_command);
            break;
        }
        case 'S' : {
            LLcontrol_to_FPGAcontrol_queue_t *FPGA_command = FPGA_cmd_queue.try_alloc_for(Kernel::wait_for_u32_forever);
            FPGA_command->port            = int_parameters[1];
            FPGA_command->command         = WRITE_REGISTER_CMD;
            switch (command[1]) {
                case 'c' :  {     
                    FPGA_command->register_number = int_parameters[2];
                    FPGA_command->data            = int_parameters[3];
                    break;
                }
                case 'p' : {
                    FPGA_command->register_number = bus.RC_base + RC_SERVO_PERIOD;
                    FPGA_command->data            = int_parameters[2] * 50;
                    break;
                }
                case 'w' : {
                    FPGA_command->register_number = int_parameters[2];
                    FPGA_command->data            = int_parameters[3];
                    break;
                }
            }
            FPGA_cmd_queue.put(FPGA_command);
            break;
        }  // end case 'S'   
    }  // end of main switch
    return NO_ERROR;
}

//***************************************************************************
// Task code
//***************************************************************************
//
void read_from_HLcontrol_task(void)
{
int32_t   status; 

    status = NO_ERROR;
    FOREVER {
        status = read_command_from_HLcontrol();
        if (status == NO_ERROR) {
            status = parse_command();
            if (status ==  NO_ERROR) {
                status = convert_tokens();
            }
        }
        if(status == NO_ERROR) {  
            status = execute_command(); 
        } else {  // return error message
            reply_t *mail = HLcontrol_reply_queue.try_alloc_for(Kernel::wait_for_u32_forever);
            sprintf(mail->reply, "%d %d\n", int_parameters[1], status);
            HLcontrol_reply_queue.put(mail);
        } 
    }
}