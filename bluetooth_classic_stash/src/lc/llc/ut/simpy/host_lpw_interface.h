#ifndef HSS_COMMANDS_H
#define HSS_COMMANDS_H

#include <stdint.h>
#include "state_machine.h"

void var_status_handler(uint32_t argc, uint8_t *argv[]);
void rail_lpw_procedure_complete_event(uint8_t *buffer, uint16_t buff_len);
void rx_header_received_handler(uint32_t argc, uint8_t *argv[]);
void rx_abort_handler(uint32_t argc, uint8_t *argv[]);
void set_payload_handler(uint32_t argc, uint8_t *argv[]);
void btc_roleswitch_complete_indication_to_ull(sm_t *connection_info_sm, uint8_t *buffer, uint8_t status);
void btc_inquiry_complete_indication_to_ull(sm_t *inquiry_sm, uint8_t *buffer);
void btc_page_complete_indication_to_ull(sm_t *page_sm, uint8_t *buffer);
void btc_inquiry_scan_complete_indication_to_ull(sm_t *inquiry_scan_sm, uint8_t *buffer);
void btc_page_scan_complete_indication_to_ull(sm_t *page_scan_sm, uint8_t *buffer);
void btc_connection_complete_indication_to_ull(sm_t *connection_info_sm, uint8_t *buffer);

#endif
