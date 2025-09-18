#ifndef page_scan_sm_AUTOGEN_H_
#define page_scan_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t page_scan_sm;

extern sm_trigger_handler_t page_scan_sm_trigger_handler_entries[];
extern uint8_t page_scan_sm_trigger_handler_entries_bitmap[];
extern uint8_t page_scan_sm_trigger_handler_entries_count[];

typedef enum page_scan_sm_states_s {
    page_scan_sm_idle,
    PAGE_SCAN_ID_RX_SCHEDULED,
    PAGE_SCAN_ID_TX_SCHEDULED,
    PAGE_SCAN_FHS_RX_1_SCHEDULED,
    PAGE_SCAN_FHS_RX_2_SCHEDULED,
    PAGE_SCAN_FHS_RESPONSE_TX_SCHEDULED,
    PAGE_SCAN_CENTRAL_POLL_RX_SCHEDULED,
    PAGE_SCAN_PERIPHERAL_NULL_TX_SCHEDULED,
    page_scan_sm_state_max
} page_scan_sm_states_t;



void page_scan_start_procedure_handler(sm_t *page_scan_sm);
void page_scan_id_rx_done_handler(sm_t *page_scan_sm);
void page_scan_id_rx_timeout_handler(sm_t *page_scan_sm);
void page_scan_id_rx_timeout_handler(sm_t *page_scan_sm);
void page_scan_id_tx_done_handler(sm_t *page_scan_sm);
void page_scan_id_tx_abort_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_done_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_1_timeout_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_1_timeout_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_done_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_2_timeout_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_2_timeout_handler(sm_t *page_scan_sm);
void page_scan_fhs_response_tx_done_handler(sm_t *page_scan_sm);
void page_scan_fhs_response_tx_abort_handler(sm_t *page_scan_sm);
void page_scan_central_poll_rx_done_handler(sm_t *page_scan_sm);
void page_scan_central_poll_rx_timeout_handler(sm_t *page_scan_sm);
void page_scan_central_poll_rx_timeout_handler(sm_t *page_scan_sm);
void page_scan_peripheral_null_tx_done_handler(sm_t *page_scan_sm);
void page_scan_peripheral_null_tx_abort_handler(sm_t *page_scan_sm);
#endif