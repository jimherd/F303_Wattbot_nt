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
//      2. get reply message storage
//      3. execute FPGA command
//      4. create reply string with port, data, and status
//      5. put reply onto reply return queue
//      6. free command queue message memory
//
void FPGA_IO_task(void)
{
uint32_t   status, data; 

char str[80];
    status = 0;
    FOREVER {
        LLcontrol_to_FPGAcontrol_queue_t *FPGA_cmd = FPGA_cmd_queue.try_get_for(Kernel::wait_for_u32_forever);  // wait for mail

        reply_t *FPGA_reply = HLcontrol_reply_queue.try_alloc_for(Kernel::wait_for_u32_forever);

        switch (FPGA_cmd->command) {
            case READ_REGISTER_CMD : {
                string_to_queue((char *)"D: READ_REGISTER_CMD\n");
                //bus.do_transaction(READ_REGISTER_CMD, 0 , 0, &bus.data, &bus.status);
                bus.do_transaction(FPGA_cmd->command, FPGA_cmd->register_number , FPGA_cmd->data, &data, &status);
                break;
            }
            case WRITE_REGISTER_CMD : {
                bus.do_transaction(FPGA_cmd->command, FPGA_cmd->register_number , FPGA_cmd->data, &data, &status);
                break;
            }
            case SOFT_PING_FPGA : {
                string_to_queue((char *)"D: to do soft FPGA ping\n");
                status = bus.soft_check_bus();
                string_to_queue((char *)"D: soft FPGA ping complete\n");
                data = NULL;
                break;
            }
            case HARD_PING_FPGA : {
                status = bus.hard_check_bus();
                data = NULL;
                break;
            }
        } // end switch
        
        sprintf(FPGA_reply->reply, "%d %d %d\n", FPGA_cmd->port, status, data);
        HLcontrol_reply_queue.put(FPGA_reply);

        FPGA_cmd_queue.free(FPGA_cmd);

    }
}
