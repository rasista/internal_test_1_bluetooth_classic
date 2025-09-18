#ifndef acl_central_H_
#define acl_central_H_
#include "btc_device.h"
#include "state_machine.h"
#include "acl_common.h"

void acl_central_start_procedure_handler(sm_t *connection_info_sm_p);
void acl_central_tx_done_handler(sm_t *connection_info_sm_p);
void acl_central_header_received_done_handler(sm_t *connection_info_sm_p);
void acl_central_header_received_timeout_handler(sm_t *connection_info_sm_p);
void acl_central_rx_done_handler(sm_t *connection_info_sm_p);
void acl_central_rx_abort_handler(sm_t *connection_info_sm_p);
#endif
