#ifndef esco_peripheral_sm_AUTOGEN_H_
#define esco_peripheral_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t esco_peripheral_sm;

extern sm_trigger_handler_t esco_peripheral_sm_trigger_handler_entries[];
extern uint8_t esco_peripheral_sm_trigger_handler_entries_bitmap[];
extern uint8_t esco_peripheral_sm_trigger_handler_entries_count[];

typedef enum esco_peripheral_sm_states_s {
    esco_peripheral_sm_idle,
    ESCO_PERIPHERAL_RX_HEADER_SCHEDULED,
    ESCO_PERIPHERAL_RX_COMPLETE_SCHEDULED,
    ESCO_PERIPHERAL_TX_SCHEDULED,
    ESCO_RETRANSMIT_PERIPHERAL_WAITING_FOR_RX_HEADER,
    esco_peripheral_sm_state_max
} esco_peripheral_sm_states_t;



void esco_peripheral_start_procedure_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_timeout_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_header_done_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_timeout_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_complete_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_timeout_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_timeout_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_tx_done_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_tx_abort_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_timeout_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_header_done_handler(sm_t *esco_peripheral_sm);
void esco_peripheral_rx_timeout_handler(sm_t *esco_peripheral_sm);
#endif