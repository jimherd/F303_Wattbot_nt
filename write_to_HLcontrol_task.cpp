//***************************************************************************
// Task code
//***************************************************************************
// write_to_HLcontrol_task : send results back to HLcontrol
// =======================
//

#include  "Wattbot_nt.h"
#include  "globals.h"


//***************************************************************************
// write_to_HLcontrol_task : write stings to HLcontrol interface
//
// Strings pointers are passed to this routine through the HLcontol
// reply queue.  Strings are handled on a first-come-first-served
// basis.

void write_to_HLcontrol_task  (void)
{
    FOREVER {
        reply_t *mail = HLcontrol_reply_queue.try_get_for(Kernel::wait_for_u32_forever);  // wait for mail
            HLcontrol.write(mail->reply, strlen(mail->reply));
            HLcontrol_reply_queue.free(mail);
//        }
    }
}