#ifndef esco_central_sm_AUTOGEN_H_
#define esco_central_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t esco_central_sm;

extern sm_trigger_handler_t esco_central_sm_trigger_handler_entries[];
extern uint8_t esco_central_sm_trigger_handler_entries_bitmap[];
extern uint8_t esco_central_sm_trigger_handler_entries_count[];

typedef enum esco_central_sm_states_s {
    esco_central_sm_idle,
    ESCO_CENTRAL_TX_SCHEDULED,
    ESCO_CENTRAL_RX_HEADER_SCHEDULED,
    ESCO_CENTRAL_RX_COMPLETE_SCHEDULED,
    ESCO_RETRANSMIT_CENTRAL_TX_SCHEDULED,
    esco_central_sm_state_max
} esco_central_sm_states_t;



void esco_central_start_procedure_handler(sm_t *esco_central_sm);
void esco_central_tx_done_handler(sm_t *esco_central_sm);
void esco_central_tx_abort_handler(sm_t *esco_central_sm);
void esco_central_rx_timeout_handler(sm_t *esco_central_sm);
void esco_central_rx_header_done_handler(sm_t *esco_central_sm);
void esco_central_rx_timeout_handler(sm_t *esco_central_sm);
void esco_central_rx_complete_handler(sm_t *esco_central_sm);
void esco_central_rx_timeout_handler(sm_t *esco_central_sm);
void esco_central_rx_timeout_handler(sm_t *esco_central_sm);
void esco_central_tx_done_handler(sm_t *esco_central_sm);
void esco_central_tx_abort_handler(sm_t *esco_central_sm);
#endif