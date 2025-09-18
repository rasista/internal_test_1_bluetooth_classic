#ifndef roleswitch_peripheral_sm_AUTOGEN_H_
#define roleswitch_peripheral_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t roleswitch_peripheral_sm;

extern sm_trigger_handler_t roleswitch_peripheral_sm_trigger_handler_entries[];
extern uint8_t roleswitch_peripheral_sm_trigger_handler_entries_bitmap[];
extern uint8_t roleswitch_peripheral_sm_trigger_handler_entries_count[];

typedef enum roleswitch_peripheral_sm_states_s {
    roleswitch_peripheral_sm_idle,
    FHS_TX_SCHEDULED,
    ID_RX_SCHEDULED,
    NEW_CENTRAL_TX_SCHEDULED,
    NEW_CENTRAL_WAITING_FOR_RX_PACKET_HEADER,
    NEW_CENTRAL_WAITING_FOR_RX_PKT_COMPLETE,
    roleswitch_peripheral_sm_state_max
} roleswitch_peripheral_sm_states_t;



void roleswitch_peripheral_start_procedure_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_fhs_tx_done_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_fhs_tx_abort_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_id_rx_done_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_id_rx_timeout_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_tx_done_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_tx_abort_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_rx_poll_null_done_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_rx_header_timeout_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_rx_header_received_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_rx_header_timeout_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_rx_complete_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_rx_complete_timeout_handler(sm_t *roleswitch_peripheral_sm);
void rs_peripheral_new_central_rx_complete_timeout_handler(sm_t *roleswitch_peripheral_sm);
#endif