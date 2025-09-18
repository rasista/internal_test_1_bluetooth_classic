#ifndef inquiry_sm_AUTOGEN_H_
#define inquiry_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t inquiry_sm;

extern sm_trigger_handler_t inquiry_sm_trigger_handler_entries[];
extern uint8_t inquiry_sm_trigger_handler_entries_bitmap[];
extern uint8_t inquiry_sm_trigger_handler_entries_count[];

typedef enum inquiry_sm_states_s {
    inquiry_sm_idle,
    INQUIRY_ID_1_TX_SCHEDULED,
    INQUIRY_ID_2_TX_SCHEDULED,
    INQUIRY_RX_1_SCHEDULED,
    INQUIRY_RX_2_SCHEDULED,
    INQUIRY_EIR_RX_SCHEDULED,
    inquiry_sm_state_max
} inquiry_sm_states_t;



void inquiry_start_procedure_handler(sm_t *inquiry_sm);
void inquiry_id_1_tx_done_handler(sm_t *inquiry_sm);
void inquiry_id_1_tx_abort_handler(sm_t *inquiry_sm);
void inquiry_id_2_tx_done_handler(sm_t *inquiry_sm);
void inquiry_id_2_tx_abort_handler(sm_t *inquiry_sm);
void inquiry_rx_1_done_handler(sm_t *inquiry_sm);
void inquiry_rx_1_timeout_handler(sm_t *inquiry_sm);
void inquiry_rx_1_abort_handler(sm_t *inquiry_sm);
void inquiry_rx_2_done_handler(sm_t *inquiry_sm);
void inquiry_rx_2_timeout_handler(sm_t *inquiry_sm);
void inquiry_rx_2_timeout_handler(sm_t *inquiry_sm);
void inquiry_EIR_rx_done_handler(sm_t *inquiry_sm);
void inquiry_EIR_rx_timeout_handler(sm_t *inquiry_sm);
void inquiry_EIR_rx_timeout_handler(sm_t *inquiry_sm);
#endif