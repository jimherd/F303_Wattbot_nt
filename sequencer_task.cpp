//***************************************************************************
// Task code
//***************************************************************************
// sequencer_task : execute a ubasic coded program
// ==============
//

#include  "sequencer_task.h"
#include  "globals.h"


//***************************************************************************
//
// Description
//      1. wait for message from sequence command queue
//      2. 


void sequencer_task  (void)
{
uint32_t   status, data; 

    status = 0;
    FOREVER {
        sequence_command_packet_t *sequence_cmd = sequence_command_queue.try_get_for(Kernel::wait_for_u32_forever); 
    }
}

