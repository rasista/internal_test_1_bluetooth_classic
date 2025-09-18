#ifndef inquiry_scan_sm_AUTOGEN_H_
#define inquiry_scan_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t inquiry_scan_sm;

extern sm_trigger_handler_t inquiry_scan_sm_trigger_handler_entries[];
extern uint8_t inquiry_scan_sm_trigger_handler_entries_bitmap[];
extern uint8_t inquiry_scan_sm_trigger_handler_entries_count[];

typedef enum inquiry_scan_sm_states_s {
    inquiry_scan_sm_idle,
    INQUIRY_SCAN_ID_RX_SCHEDULED,
    INQUIRY_SCAN_FHS_TX_CONFIGURED,
    INQUIRY_SCAN_EIR_TX_CONFIGURED,
    inquiry_scan_sm_state_max
} inquiry_scan_sm_states_t;



void inquiry_scan_start_procedure_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_rx_done_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_rx_timeout_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_rx_timeout_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_fhs_tx_done_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_fhs_tx_abort_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_eir_tx_done_handler(sm_t *inquiry_scan_sm);
void inquiry_scan_eir_tx_abort_handler(sm_t *inquiry_scan_sm);
#endif