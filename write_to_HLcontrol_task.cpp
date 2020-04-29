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
        osEvent evt = HLcontrol_reply_queue.get();  // wait for mail
        if (evt.status == osEventMail) {
            reply_t *mail = (reply_t*)evt.value.p;

            HLcontrol.puts(mail->reply);

            HLcontrol_reply_queue.free(mail);
        }
    }
}
