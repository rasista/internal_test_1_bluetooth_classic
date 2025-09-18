#ifndef acl_peripheral_sm_AUTOGEN_H_
#define acl_peripheral_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t acl_peripheral_sm;

extern sm_trigger_handler_t acl_peripheral_sm_trigger_handler_entries[];
extern uint8_t acl_peripheral_sm_trigger_handler_entries_bitmap[];
extern uint8_t acl_peripheral_sm_trigger_handler_entries_count[];

typedef enum acl_peripheral_sm_states_s {
    acl_peripheral_sm_idle,
    ACL_PERIPHERAL_WAITING_FOR_RX_PKT_HEADER,
    ACL_PERIPHERAL_WAITING_FOR_RX_COMPLETE,
    ACL_PERIPHERAL_TX_SCHEDULED,
    acl_peripheral_sm_state_max
} acl_peripheral_sm_states_t;



void acl_peripheral_start_procedure_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_rx_poll_null_done_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_header_received_timeout_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_header_received_done_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_header_received_timeout_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_rx_done_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_rx_timeout_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_rx_timeout_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_tx_done_handler(sm_t *acl_peripheral_sm);
void acl_peripheral_tx_abort_handler(sm_t *acl_peripheral_sm);
#endif