#include "procedure_triggers_autogen.h"
#include "acl_peripheral_sm_autogen.h"
#include "state_machine.h"

/*acl_peripheral_sm Handlers*/
sm_trigger_handler_t acl_peripheral_sm_trigger_handler_entries[] = 
{
    {acl_peripheral_start_procedure_handler},
    {acl_peripheral_rx_poll_null_done_handler},
    {acl_peripheral_header_received_timeout_handler},
    {acl_peripheral_header_received_done_handler},
    {acl_peripheral_header_received_timeout_handler},
    {acl_peripheral_rx_done_handler},
    {acl_peripheral_rx_timeout_handler},
    {acl_peripheral_rx_timeout_handler},
    {acl_peripheral_tx_done_handler},
    {acl_peripheral_tx_abort_handler},
};

uint8_t acl_peripheral_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    216,
    152,
    6,
};

uint8_t acl_peripheral_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    5,
    8,
};
sm_t acl_peripheral_sm = {
#ifdef SIM
   .sm_name = "acl_peripheral_sm",
#endif
     .max_num_states = acl_peripheral_sm_state_max,
    .current_state = acl_peripheral_sm_idle,
    .current_state_machine = acl_peripheral_sm_context,
    .hashed_lut = {
        .hashed_entries = acl_peripheral_sm_trigger_handler_entries,
        .hashed_entries_bitmap = acl_peripheral_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = acl_peripheral_sm_trigger_handler_entries_count
    }
};