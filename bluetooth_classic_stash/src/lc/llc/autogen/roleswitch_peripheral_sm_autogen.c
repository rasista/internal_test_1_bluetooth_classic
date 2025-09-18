#include "procedure_triggers_autogen.h"
#include "roleswitch_peripheral_sm_autogen.h"
#include "state_machine.h"

/*roleswitch_peripheral_sm Handlers*/
sm_trigger_handler_t roleswitch_peripheral_sm_trigger_handler_entries[] = 
{
    {roleswitch_peripheral_start_procedure_handler},
    {rs_peripheral_fhs_tx_done_handler},
    {rs_peripheral_fhs_tx_abort_handler},
    {rs_peripheral_id_rx_done_handler},
    {rs_peripheral_id_rx_timeout_handler},
    {rs_peripheral_new_central_tx_done_handler},
    {rs_peripheral_new_central_tx_abort_handler},
    {rs_peripheral_rx_poll_null_done_handler},
    {rs_peripheral_new_central_rx_header_timeout_handler},
    {rs_peripheral_new_central_rx_header_received_handler},
    {rs_peripheral_new_central_rx_header_timeout_handler},
    {rs_peripheral_new_central_rx_complete_handler},
    {rs_peripheral_new_central_rx_complete_timeout_handler},
    {rs_peripheral_new_central_rx_complete_timeout_handler},
};

uint8_t roleswitch_peripheral_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    6,
    24,
    6,
    216,
    152,
};

uint8_t roleswitch_peripheral_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    3,
    5,
    7,
    11,
};
sm_t roleswitch_peripheral_sm = {
#ifdef SIM
   .sm_name = "roleswitch_peripheral_sm",
#endif
     .max_num_states = roleswitch_peripheral_sm_state_max,
    .current_state = roleswitch_peripheral_sm_idle,
    .current_state_machine = roleswitch_peripheral_sm_context,
    .hashed_lut = {
        .hashed_entries = roleswitch_peripheral_sm_trigger_handler_entries,
        .hashed_entries_bitmap = roleswitch_peripheral_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = roleswitch_peripheral_sm_trigger_handler_entries_count
    }
};