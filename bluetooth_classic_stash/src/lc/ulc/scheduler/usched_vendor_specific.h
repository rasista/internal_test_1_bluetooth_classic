#ifndef __USCHED_VENDOR_SPECIFIC_H__
#define __USCHED_VENDOR_SPECIFIC_H__
#include <stdint.h>

#define VENDOR_COMMAND_INTERNAL_PKT_LEN(p)  (p)->data[3]

/* VENDOR INTERNAL TEST OPCODES*/
#define PAGE_SUCCESS_NOTIFICATION_FROM_LPW 0x01
#define PAGE_SCAN_SUCCESS_NOTIFICATION_FROM_LPW 0x02
#define INQUIRY_COMPLETE_INDICATION_FROM_LPW 0x03
#define CONNECTION_COMPLETE_NOTIFICATION_FROM_LPW 0x04
#define POWER_CONTROL_RSSI_FROM_LPW 0x05
#define DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW 0x06
#define SNIFF_MODE_TRANSITION_COMPLETE 0x07
#define SNIFF_SUBR_PARAMS_SET 0x08

void usched_api_start_vendor_page_scan(uint16_t port);
void usched_api_cancel_vendor_page_scan();
void usched_api_start_vendor_page(uint8_t ip_0, uint8_t ip_1, uint8_t ip_2,
                                  uint8_t ip_3, uint16_t port);
void usched_api_cancel_vendor_page();
#endif
