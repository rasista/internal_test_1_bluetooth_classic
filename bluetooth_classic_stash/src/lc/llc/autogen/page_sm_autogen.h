#ifndef page_sm_AUTOGEN_H_
#define page_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t page_sm;

extern sm_trigger_handler_t page_sm_trigger_handler_entries[];
extern uint8_t page_sm_trigger_handler_entries_bitmap[];
extern uint8_t page_sm_trigger_handler_entries_count[];

typedef enum page_sm_states_s {
    page_sm_idle,
    PAGE_SM_FIRST_ID_TX_SCHEDULED,
    PAGE_SM_SECOND_ID_TX_SCHEDULED,
    PAGE_SM_FIRST_ID_RX_SCHEDULED,
    PAGE_SM_SECOND_ID_RX_SCHEDULED,
    PAGE_SM_FHS_TX_SCHEDULED,
    PAGE_SM_PERIPHERAL_RESPONSE_ID_RX_SCHEDULED,
    PAGE_SM_CENTRAL_RESPONSE_POLL_TX_SCHEDULED,
    PAGE_SM_PERIPHERAL_RESPONSE_NULL_RX_SCHEDULED,
    page_sm_state_max
} page_sm_states_t;



void page_start_procedure_handler(sm_t *page_sm);
void page_first_id_tx_done_handler(sm_t *page_sm);
void page_first_id_tx_abort_handler(sm_t *page_sm);
void page_second_id_tx_done_handler(sm_t *page_sm);
void page_second_id_tx_abort_handler(sm_t *page_sm);
void page_first_id_rx_done_handler(sm_t *page_sm);
void page_first_id_rx_timeout_handler(sm_t *page_sm);
void page_first_id_rx_timeout_handler(sm_t *page_sm);
void page_second_id_rx_done_handler(sm_t *page_sm);
void page_second_id_rx_timeout_handler(sm_t *page_sm);
void page_second_id_rx_timeout_handler(sm_t *page_sm);
void page_fhs_tx_done_handler(sm_t *page_sm);
void page_fhs_tx_abort_handler(sm_t *page_sm);
void page_peripheral_response_id_rx_done_handler(sm_t *page_sm);
void page_peripheral_response_id_rx_timeout_handler(sm_t *page_sm);
void page_peripheral_response_id_rx_timeout_handler(sm_t *page_sm);
void page_central_response_poll_tx_done_handler(sm_t *page_sm);
void page_central_response_poll_tx_abort_handler(sm_t *page_sm);
void page_peripheral_response_null_rx_done_handler(sm_t *page_sm);
void page_peripheral_response_null_rx_timeout_handler(sm_t *page_sm);
void page_peripheral_response_null_rx_timeout_handler(sm_t *page_sm);
#endif