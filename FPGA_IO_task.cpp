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
// Globals for this task

//***************************************************************************
// Task code
//***************************************************************************
//
void FPGA_IO_task(void)
{
uint32_t   status, data; 

    status = 0;
    FOREVER {
        osEvent evt = FPGA_cmd_queue.get();  // wait for mail
        if (evt.status == osEventMail) {
            LLcontrol_to_FPGAcontrol_queue_t *FPGA_cmd = (LLcontrol_to_FPGAcontrol_queue_t*)evt.value.p;
            
            bus.do_transaction(FPGA_cmd->command, FPGA_cmd->register_number , FPGA_cmd->data, &data, &status);
            
            reply_t *FPGA_reply = HLcontrol_reply_queue.alloc();
            sprintf(FPGA_reply->reply, "%d 0\r\n", FPGA_cmd->port);
            HLcontrol_reply_queue.put(FPGA_reply);

            FPGA_cmd_queue.free(FPGA_cmd);
            break;
        }
    }
}