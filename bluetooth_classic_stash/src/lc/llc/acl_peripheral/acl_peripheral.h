#ifndef acl_peripheral_H_
#define acl_peripheral_H_
#include "state_machine.h"
#include "btc_common_defines.h"
#include "acl_common.h"

void acl_peripheral_start_procedure_handler(sm_t *connection_info_sm_p);
void acl_peripheral_header_received_done_handler(sm_t *connection_info_sm_p);
void acl_peripheral_header_received_timeout_handler(sm_t *connection_info_sm_p);
void acl_peripheral_rx_done_handler(sm_t *connection_info_sm_p);
void acl_peripheral_rx_timeout_handler(sm_t *connection_info_sm_p);
void acl_peripheral_tx_done_handler(sm_t *connection_info_sm_p);
void acl_peripheral_tx_abort_handler(sm_t *connection_info_sm_p);
void loopback_testmode_add_rx_packet_to_tx_queue(sm_t *connection_info_sm_p);
void testmode_rx_done_handler(sm_t *connection_info_sm_p);
#endif
