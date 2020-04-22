//*********************************************************
// Task code
//*********************************************************
// read_from_HLcontrol_task : Read/execute commands from High Level control
// ========================
//
// Read commands from UART input channel that connects to High Level
// control computer.  Parse and execute.

#include  "globals.h"

//*********************************************************
// Globals for this task

uint8_t     command[MAX_COMMAND_LENGTH], character_count;
uint32_t    argc, arg_pt[MAX_COMMAND_PARAMETERS], arg_type[MAX_COMMAND_PARAMETERS];

//*********************************************************
// read_command_from_HLcontrol : Read ascii command string from HLcontrol
//
// 
uint32_t read_command_from_HLcontrol(void) 
{
uint8_t character;

    for(character_count=0 ; character_count < MAX_COMMAND_LENGTH ; character_count++) {
        character = HLcontrol.getc();
        if (character == '\n') {
            break;
        } else {
            if ((character == '\r') || (character == ' ')) {
                command[character_count] = '\0';
            }else {
                command[character_count] = character;
            }
        }
    }
    if (character_count == (MAX_COMMAND_LENGTH-1)) {
        command[character_count++] = '\0';
        return CMD_STRING_TOO_BIG;
    }
    return SUCCESS;
}

//*********************************************************
// parse_command : analyse command string and convert ant ints/reals
//
// Code uses a STATE MACHINE to walkthrough the command string.  Refer
// to associated documentation.

// defines modes as scan progresses : U=undefined, I=integer, R=real, S=string
//

uint32_t parse_command (void) 
{
uint32_t    count, mode, status;
uint8_t     character_type;

    argc = 0;
    mode = MODE_U;
    status = SUCCESS;
    for (count=0 ; count < character_count ; count++) {
        character_type = char_type[command[count]];
        switch (character_type) {
            case LETTER :
                if ((mode == MODE_I) || (mode == MODE_R)) {
                    status = LETTER_ERROR;
                } else {
                    if (mode == MODE_U) {
                        mode = MODE_S;
                        arg_pt[argc] = count;
                        argc++;
                    } 
                }
                break;
            case NUMBER :
                if (mode == MODE_U) {
                    mode = MODE_I;
                    arg_pt[argc] = count;
                    argc++;
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
                    argc++;
                } else {
                    if ((mode == MODE_I) || (mode == MODE_R)) {
                        status = PLUSMINUS_ERROR;
                    }
                }
                break;
            case NULTERM :
                if (mode != MODE_U) {
                    arg_type[argc] = mode;
                    mode = MODE_U;
                }
                break;
            case END :
                if (mode != MODE_U) {
                    arg_type[argc] = mode;
                    mode = MODE_U;
                }
                break;
        }
    }
    return status;
}

//*********************************************************
uint32_t execute_command()
{
    return 0;
}

//*********************************************************
// Task code
//*********************************************************
//
void read_from_HLcontrol(void const *args)
{
uint32_t   read_from_HLcontrol_error; 

    read_from_HLcontrol_error = 0;
    FOREVER {
    // read command string from HLcontrol and deli
        read_from_HLcontrol_error = read_command_from_HLcontrol();
    // parse command (split into strings)
        read_from_HLcontrol_error = parse_command();
    // implement command
        read_from_HLcontrol_error = execute_command(); 
    }
}