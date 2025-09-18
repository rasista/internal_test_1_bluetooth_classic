#include "procedure_triggers_autogen.h"
#include "inquiry_sm_autogen.h"
#include "state_machine.h"

/*inquiry_sm Handlers*/
sm_trigger_handler_t inquiry_sm_trigger_handler_entries[] = 
{
    {inquiry_start_procedure_handler},
    {inquiry_id_1_tx_done_handler},
    {inquiry_id_1_tx_abort_handler},
    {inquiry_id_2_tx_done_handler},
    {inquiry_id_2_tx_abort_handler},
    {inquiry_rx_1_done_handler},
    {inquiry_rx_1_timeout_handler},
    {inquiry_rx_1_abort_handler},
    {inquiry_rx_2_done_handler},
    {inquiry_rx_2_timeout_handler},
    {inquiry_rx_2_timeout_handler},
    {inquiry_EIR_rx_done_handler},
    {inquiry_EIR_rx_timeout_handler},
    {inquiry_EIR_rx_timeout_handler},
};

uint8_t inquiry_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    6,
    6,
    152,
    152,
    152,
};

uint8_t inquiry_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    3,
    5,
    8,
    11,
};
sm_t inquiry_sm = {
#ifdef SIM
   .sm_name = "inquiry_sm",
#endif
     .max_num_states = inquiry_sm_state_max,
    .current_state = inquiry_sm_idle,
    .current_state_machine = inquiry_sm_context,
    .hashed_lut = {
        .hashed_entries = inquiry_sm_trigger_handler_entries,
        .hashed_entries_bitmap = inquiry_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = inquiry_sm_trigger_handler_entries_count
    }
};