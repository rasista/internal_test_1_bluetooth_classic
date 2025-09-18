#include "procedure_triggers_autogen.h"
#include "acl_central_sm_autogen.h"
#include "state_machine.h"

/*acl_central_sm Handlers*/
sm_trigger_handler_t acl_central_sm_trigger_handler_entries[] = 
{
    {acl_central_start_procedure_handler},
    {acl_central_tx_done_handler},
    {acl_central_tx_abort_handler},
    {acl_central_rx_poll_null_done_handler},
    {acl_central_header_received_timeout_handler},
    {acl_central_header_received_done_handler},
    {acl_central_rx_abort_handler},
    {acl_central_rx_done_handler},
    {acl_central_rx_abort_handler},
    {acl_central_rx_abort_handler},
};

uint8_t acl_central_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    6,
    216,
    152,
};

uint8_t acl_central_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    3,
    7,
};
sm_t acl_central_sm = {
#ifdef SIM
   .sm_name = "acl_central_sm",
#endif
     .max_num_states = acl_central_sm_state_max,
    .current_state = acl_central_sm_idle,
    .current_state_machine = acl_central_sm_context,
    .hashed_lut = {
        .hashed_entries = acl_central_sm_trigger_handler_entries,
        .hashed_entries_bitmap = acl_central_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = acl_central_sm_trigger_handler_entries_count
    }
};