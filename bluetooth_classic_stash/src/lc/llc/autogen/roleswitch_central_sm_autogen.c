#include "procedure_triggers_autogen.h"
#include "roleswitch_central_sm_autogen.h"
#include "state_machine.h"

/*roleswitch_central_sm Handlers*/
sm_trigger_handler_t roleswitch_central_sm_trigger_handler_entries[] = 
{
    {roleswitch_central_start_procedure_handler},
    {rs_central_fhs_rx_timeout_handler},
    {rs_central_rx_header_received_handler},
    {rs_central_fhs_rx_timeout_handler},
    {rs_central_fhs_rx_complete_handler},
    {rs_central_fhs_rx_complete_timeout_handler},
    {rs_central_fhs_rx_complete_timeout_handler},
    {rs_central_id_tx_done_handler},
    {rs_central_id_tx_abort_handler},
    {rs_central_rx_poll_null_done_handler},
    {rs_central_new_peripheral_rx_timeout_handler},
    {rs_central_new_peripheral_rx_header_handler},
    {rs_central_new_peripheral_rx_timeout_handler},
    {rs_central_new_peripheral_rx_complete_handler},
    {rs_central_new_peripheral_rx_complete_timeout_handler},
    {rs_central_new_peripheral_rx_complete_timeout_handler},
    {rs_central_new_peripheral_tx_scheduled_done_handler},
    {rs_central_new_peripheral_tx_scheduled_abort_handler},
};

uint8_t roleswitch_central_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    208,
    152,
    6,
    216,
    152,
    6,
};

uint8_t roleswitch_central_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    4,
    7,
    9,
    13,
    16,
};
sm_t roleswitch_central_sm = {
#ifdef SIM
   .sm_name = "roleswitch_central_sm",
#endif
     .max_num_states = roleswitch_central_sm_state_max,
    .current_state = roleswitch_central_sm_idle,
    .current_state_machine = roleswitch_central_sm_context,
    .hashed_lut = {
        .hashed_entries = roleswitch_central_sm_trigger_handler_entries,
        .hashed_entries_bitmap = roleswitch_central_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = roleswitch_central_sm_trigger_handler_entries_count
    }
};