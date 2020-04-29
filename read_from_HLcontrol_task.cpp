//***************************************************************************
// Task code
//***************************************************************************
// read_from_HLcontrol_task : Read/execute commands from High Level control
// ========================
//
// Read commands from UART input channel that connects to High Level
// control computer.  Parse and execute.

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

uint32_t read_command_from_HLcontrol(void) 
{
uint8_t     character;
uint32_t    count, status;

    for(count=0 ; count < MAX_COMMAND_LENGTH ; count++) {
        character = HLcontrol.getc();
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
        status = SUCCESS;
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

uint32_t parse_command (void) 
{
uint32_t    count, mode, status;
uint8_t     character_type;

    argc = 0;
    mode = MODE_U;
    status = SUCCESS;
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
uint32_t convert_tokens(void) 
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
    return SUCCESS;
}

//***************************************************************************
// reply_to_HLcontrol : send reply
//
void reply_to_HLcontrol(uint32_t status)
{
    return;
}

//***************************************************************************
// execute_command : run processed command
//
uint32_t execute_command()
{
    switch (command[0]) {
        case 'p' :          // PING
            reply_t *mail = HLcontrol_reply_queue.alloc();
            sprintf(mail->reply, "%d 0\n", int_parameters[1]);
            HLcontrol_reply_queue.put(mail);
            break;
    }
    return 0;
}

//***************************************************************************
// ASCII_to_int : local version of atoi()
//
// String has already been checked therefore no need to test for errors

int32_t ASCII_to_int(char *str) {

int32_t result = 0;    // Initialize result
int32_t sign = 1;      // Initialize sign as positive
uint32_t char_pt = 0;  // Initialize index of first digit

    if (*str == '\0') {
        return 0;
    }
    if (str[0] == '-') {
        sign = -1;
        char_pt++; // Also update index of first digit
    }
     for (; str[char_pt] != '\0'; ++char_pt) {
        result = (result * 10) + (str[char_pt] - '0');
    }
    return sign * result;
}

//***************************************************************************
// ASCII_to_float : local version of atof()   *** TO BE CHECKED  ***
//
// String has already been checked therefore no need to test for errors
// Saves 5K bytes of FLASH and therefore should be faster.

float ASCII_to_float(const char *char_pt) {
  float result = 0, fact = 1;
    if (*char_pt == '-') {
        char_pt++;
        fact = -1;
    };
    for (uint32_t point_seen = 0; *char_pt == '\0'; char_pt++) {
        if (*char_pt == '.') {
            point_seen = 1;
            continue;
        }
        uint32_t d = *char_pt - '0';
//        if (d >= 0 && d <= 9) {
            if (point_seen) {
                fact = fact / 10.0f;
            }
        result = (result * 10.0f) + (float)d;
//        };
    };
    return result * fact;
};

//***************************************************************************
// Task code
//***************************************************************************
//
void read_from_HLcontrol_task(void)
{
uint32_t   status; 

    status = 0;
    FOREVER {
        status = read_command_from_HLcontrol();
        status = parse_command();
        status = convert_tokens();
        status = execute_command(); 
    }
}