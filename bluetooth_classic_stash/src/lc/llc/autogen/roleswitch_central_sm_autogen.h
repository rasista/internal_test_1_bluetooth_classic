#ifndef roleswitch_central_sm_AUTOGEN_H_
#define roleswitch_central_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t roleswitch_central_sm;

extern sm_trigger_handler_t roleswitch_central_sm_trigger_handler_entries[];
extern uint8_t roleswitch_central_sm_trigger_handler_entries_bitmap[];
extern uint8_t roleswitch_central_sm_trigger_handler_entries_count[];

typedef enum roleswitch_central_sm_states_s {
    roleswitch_central_sm_idle,
    WAITING_FOR_FHS_RX_PKT_HEADER,
    WAITING_FOR_FHS_RX_PKT_COMPLETE,
    ID_PKT_TX_SCHEDULED,
    NEW_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER,
    NEW_PERIPHERAL_WAITING_FOR_RX_PKT_COMPLETE,
    NEW_PERIPHERAL_TX_SCHEDULED,
    roleswitch_central_sm_state_max
} roleswitch_central_sm_states_t;



void roleswitch_central_start_procedure_handler(sm_t *roleswitch_central_sm);
void rs_central_fhs_rx_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_rx_header_received_handler(sm_t *roleswitch_central_sm);
void rs_central_fhs_rx_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_fhs_rx_complete_handler(sm_t *roleswitch_central_sm);
void rs_central_fhs_rx_complete_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_fhs_rx_complete_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_id_tx_done_handler(sm_t *roleswitch_central_sm);
void rs_central_id_tx_abort_handler(sm_t *roleswitch_central_sm);
void rs_central_rx_poll_null_done_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_rx_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_rx_header_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_rx_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_rx_complete_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_rx_complete_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_rx_complete_timeout_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_tx_scheduled_done_handler(sm_t *roleswitch_central_sm);
void rs_central_new_peripheral_tx_scheduled_abort_handler(sm_t *roleswitch_central_sm);
#endif