#include  "globals.h"

#include    "ExCmdFromHLcontrol_task.h"

//*********************************************************
// Task code
//*********************************************************
// read_from_HLcontrol_task : Read/execute commands from High Level control
// ========================
//
// Read commands from UART input channel that connects to High Level
// control computer.  Parse and execute.

uint8_t     command[MAX_COMMAND_LENGTH];
uint32_t    argc, arg_pt[MAX_COMMAND_STRINGS];

uint32_t read_command_from_HLcontrol(void) 
{
uint8_t character, char_count;

    for(char_count=0 ; char_count < MAX_COMMAND_LENGTH ; char_count++) {
        character = HLcontrol.getc();
        if (character == '\n') {
            command[char_count] = '\0';
            break;
        } else {
            if ((character == '\r') || (character == ' ')) {
                command[char_count] = '\0';
            }else {
                command[char_count] = character;
            }
        }
    }
    if (char_count == (MAX_COMMAND_LENGTH-1)) {
        command[char_count] = '\0';
        return CMD_STRING_TOO_BIG;
    }
    return SUCCESS;
}

uint32_t parse_command (void) 
{
    return 0;
}

uint32_t execute_command()
{
    return 0;
}


void read_from_HLcontrol(void const *args)
{
uint32_t   read_from_HLcontrol_error; 

    read_from_HLcontrol_error = 0;
    FOREVER {
    // read command from HLcontrol
        read_from_HLcontrol_error = read_command_from_HLcontrol();
    // parse command (split into strings)
        read_from_HLcontrol_error = parse_command();
    // implement command
        read_from_HLcontrol_error = execute_command(); 
    }
}