#ifndef ESCO_CENTRAL_H
#define ESCO_CENTRAL_H
#include "btc_common_defines.h"
#include "state_machine.h"
#include "btc_rail.h"

/*
   Structure to store sco and esco details[hybrid]
 */
typedef struct esco_info_s {
  uint8_t is_esco;  //1 in case of esco , 0 in case of sco
  uint8_t is_edr;   //1 in case of edr , 0 in case of basic rate
  uint8_t valid_esco_pkt_rcvd; //  Resets at every Window start
  // This Variable is Zero at the start of the window and once it is being accepted , it value is set to one, So that it will help us to perform other services in retransmission slots
  uint8_t tx_pkt_type;
  uint8_t rx_pkt_type;
  uint8_t crc_status;  //variable to check crc_status
  uint32_t esco_handle;
  uint32_t esco_window_start; // in BT clock values (312.5 us, half slots)
  uint32_t d_esco;
  uint32_t t_esco;
  uint32_t w_esco;
  sm_t esco_sm;
} esco_info_t;

typedef struct connection_info_s connection_info_t;

void esco_central_start_procedure_handler(sm_t *connection_info_sm);
void esco_central_tx_done_handler(sm_t *connection_info_sm);
void esco_central_tx_abort_handler(sm_t *connection_info_sm);
void esco_central_rx_timeout_handler(sm_t *connection_info_sm);
void esco_central_rx_header_done_handler(sm_t *connection_info_sm);
void esco_central_rx_complete_handler(sm_t *connection_info_sm);
void esco_central_retransmit(sm_t *connection_info_sm);
void esco_central_calculate_next_esco_window_start(sm_t *connection_info_sm);
uint8_t check_slot_is_esco_retransmit(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs);

void esco_peripheral_start_procedure_handler(sm_t *connection_info_peripheral_sm);
void esco_peripheral_rx_header_done_handler(sm_t *connection_info_sm);
void esco_peripheral_rx_complete_handler(sm_t *connection_info_sm);
void esco_peripheral_rx_timeout_handler(sm_t *connection_info_sm);
void esco_peripheral_tx_done_handler(sm_t *connection_info_sm);
void esco_peripheral_tx_abort_handler(sm_t *connection_info_sm);
void esco_peripheral_retransmit(sm_t *esco_sm, bool is_rx_timeout);
void esco_peripheral_calculate_next_esco_window_start(sm_t *connection_info_sm);

void update_esco_parameters(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs);
void calculate_first_esco_window_start(connection_info_t *connection_info_p);
#endif
