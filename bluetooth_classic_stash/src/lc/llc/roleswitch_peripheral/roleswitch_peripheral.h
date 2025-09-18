#ifndef ROLESWITCH_PERIPHERAL_H
#define ROLESWITCH_PERIPHERAL_H
#include "state_machine.h"
#include "btc_common_defines.h"
#include "roleswitch_peripheral_sm_autogen.h"
#include "roleswitch_central.h"
#include "acl_central.h"

void roleswitch_peripheral_start_procedure_handler(sm_t *connection_info_sm_p);
void rs_peripheral_fhs_tx_done_handler(sm_t *connection_info_sm);
void rs_peripheral_new_central_rx_header_timeout_handler(sm_t *connection_info_sm);
void rs_peripheral_new_central_rx_complete_handler(sm_t *connection_info_sm);
void rs_peripheral_new_central_rx_header_timeout_handler(sm_t *connection_info_sm);
void rs_peripheral_new_central_rx_complete_timeout_handler(sm_t *connection_info_sm);
void rs_peripheral_new_central_rx_complete_handler(sm_t *connection_info_sm);
void roleswitch_new_central_poll_retransmit(sm_t *connection_info_sm);
void roleswitch_peripheral_retransmit_fhs(sm_t *connection_info_sm);
void roleswitch_enable_new_role_params_peri(connection_info_t *connection_info_p, btc_dev_t *btc_dev);
void rs_peripheral_new_central_rx_complete_handler(sm_t *connection_info_sm);
#endif
