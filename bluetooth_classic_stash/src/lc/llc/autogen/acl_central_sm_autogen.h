#ifndef acl_central_sm_AUTOGEN_H_
#define acl_central_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t acl_central_sm;

extern sm_trigger_handler_t acl_central_sm_trigger_handler_entries[];
extern uint8_t acl_central_sm_trigger_handler_entries_bitmap[];
extern uint8_t acl_central_sm_trigger_handler_entries_count[];

typedef enum acl_central_sm_states_s {
    acl_central_sm_idle,
    ACL_CENTRAL_TX_SCHEDULED,
    ACL_CENTRAL_WAITING_FOR_RX_PKT_HEADER,
    ACL_CENTRAL_WAITING_FOR_RX_COMPLETE,
    acl_central_sm_state_max
} acl_central_sm_states_t;



void acl_central_start_procedure_handler(sm_t *acl_central_sm);
void acl_central_tx_done_handler(sm_t *acl_central_sm);
void acl_central_tx_abort_handler(sm_t *acl_central_sm);
void acl_central_rx_poll_null_done_handler(sm_t *acl_central_sm);
void acl_central_header_received_timeout_handler(sm_t *acl_central_sm);
void acl_central_header_received_done_handler(sm_t *acl_central_sm);
void acl_central_rx_abort_handler(sm_t *acl_central_sm);
void acl_central_rx_done_handler(sm_t *acl_central_sm);
void acl_central_rx_abort_handler(sm_t *acl_central_sm);
void acl_central_rx_abort_handler(sm_t *acl_central_sm);
#endif