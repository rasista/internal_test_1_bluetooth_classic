#ifndef ROLESWITCH_CENTRAL_H
#define ROLESWITCH_CENTRAL_H
#include "state_machine.h"
#include "btc_common_defines.h"
#include "acl_central.h"

//Temporary structure to store fhs data, will be used to update fhs details.
typedef struct fhs_data_s {
  uint64_t parity_bits : 34;
  uint32_t lap : 24;
  uint8_t eir : 1;
  uint8_t reserved : 1;
  uint8_t sr : 2;
  uint8_t sp : 2;
  uint8_t uap : 8;
  uint16_t nap : 16;
  uint32_t class_of_device : 24;
  uint8_t lt_addr : 3;
  uint32_t clk : 26;
  uint8_t previously_used : 3;
} fhs_data_t;

void roleswitch_central_start_procedure_handler(sm_t *connection_info_sm_p);
void rs_central_rx_header_received_handler(sm_t *connection_info_sm);
void rs_central_fhs_rx_complete_handler(sm_t *connection_info_sm);
void rs_central_fhs_rx_timeout_handler(sm_t *connection_info_sm);
void rs_central_id_tx_abort_handler(sm_t *connection_info_sm);
void rs_central_new_peripheral_rx_header_handler(sm_t *connection_info_sm);
void rs_central_new_peripheral_rx_complete_handler(sm_t *connection_info_sm);
void rs_central_id_tx_done_handler(sm_t *connection_info_sm);
void rs_central_new_peripheral_rx_timeout_handler(sm_t *connection_info_sm);
void rs_central_new_peripheral_tx_scheduled_done_handler(sm_t *connection_info_sm);
void rs_central_new_peripheral_tx_scheduled_abort_handler(sm_t *connection_info_sm);
void rs_central_rx_poll_null_done_handler(sm_t *connection_info_sm);
void update_rcvd_fhs_data_for_new_role(roleswitch_data_t *roleswitch_data, connection_info_t *connection_info_new, uint8_t *data, connection_info_t *connection_info_old);
void roleswitch_activity_complete(sm_t *connection_info_sm, uint32_t conn_status);
void roleswitch_central_fhs_rx_retry_and_rx_timeout(sm_t *connection_info_sm, uint8_t state);
void roleswitch_enable_new_role_params(btc_dev_t *btc_dev, connection_info_t *connection_info_p);
#endif
