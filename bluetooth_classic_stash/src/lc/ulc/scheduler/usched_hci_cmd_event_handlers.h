#ifndef __USCHED_HCI_CMD_EVENT_HANDLERS_H__
#define __USCHED_HCI_CMD_EVENT_HANDLERS_H__
#include <stdint.h>

void usched_api_start_inquiry_periodic_inq();
void usched_tx_pkt_handler(uint8_t *data, uint8_t length);
void usched_api_start_inquiry_scan();
void usched_api_cancel_inquiry_periodic_inq();
void usched_api_stop_periodic_inquiry();

#endif
