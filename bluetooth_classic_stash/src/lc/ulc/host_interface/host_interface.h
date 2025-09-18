#ifndef __HOST_INTERFACE_H__
#define __HOST_INTERFACE_H__
#include <stdint.h>
#include "hci.h"
#include "btc_queue.h"
#include "ulc.h"

void hci_pending_handler(void *ctx);
void hci_event_pending_handler(void *ctx);
void hci_event_sent_handler(void *ctx);
extern void schedule_hci_events_to_host();
#endif
