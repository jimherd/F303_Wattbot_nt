//***************************************************************************
// Task code
//***************************************************************************
// FPGA_IO_task : manage execution of FPGA commands
// ============
//
// Read commands from queue and generate/run FPGA commands

#include  "Wattbot_nt.h"
#include  "globals.h"

//***************************************************************************
// Task code
//***************************************************************************
//
// Description
//      1. wait on message from FPGA command queue
//      2. execute FPGA command
//      3. create reply string with port, data, and status
//      4. put reply onto reply return queue
//
void FPGA_IO_task(void)
{
uint32_t   status, data; 

    status = 0;
    FOREVER {
        osEvent evt = FPGA_cmd_queue.get();  // wait for command from queue
        if (evt.status == osEventMail) {
            LLcontrol_to_FPGAcontrol_queue_t *FPGA_cmd = (LLcontrol_to_FPGAcontrol_queue_t*)evt.value.p;
            
            bus.do_transaction(FPGA_cmd->command, FPGA_cmd->register_number , FPGA_cmd->data, &data, &status);
            
            reply_t *FPGA_reply = HLcontrol_reply_queue.alloc();
            sprintf(FPGA_reply->reply, "%d %d %d\r\n", FPGA_cmd->port, status, data);
            HLcontrol_reply_queue.put(FPGA_reply);

            FPGA_cmd_queue.free(FPGA_cmd);
            break;
        }
    }
}