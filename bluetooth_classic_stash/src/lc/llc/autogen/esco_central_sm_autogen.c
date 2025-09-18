#include "procedure_triggers_autogen.h"
#include "esco_central_sm_autogen.h"
#include "state_machine.h"

/*esco_central_sm Handlers*/
sm_trigger_handler_t esco_central_sm_trigger_handler_entries[] = 
{
    {esco_central_start_procedure_handler},
    {esco_central_tx_done_handler},
    {esco_central_tx_abort_handler},
    {esco_central_rx_timeout_handler},
    {esco_central_rx_header_done_handler},
    {esco_central_rx_timeout_handler},
    {esco_central_rx_complete_handler},
    {esco_central_rx_timeout_handler},
    {esco_central_rx_timeout_handler},
    {esco_central_tx_done_handler},
    {esco_central_tx_abort_handler},
};

uint8_t esco_central_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    6,
    208,
    152,
    6,
};

uint8_t esco_central_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    3,
    6,
    9,
};
sm_t esco_central_sm = {
#ifdef SIM
   .sm_name = "esco_central_sm",
#endif
     .max_num_states = esco_central_sm_state_max,
    .current_state = esco_central_sm_idle,
    .current_state_machine = esco_central_sm_context,
    .hashed_lut = {
        .hashed_entries = esco_central_sm_trigger_handler_entries,
        .hashed_entries_bitmap = esco_central_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = esco_central_sm_trigger_handler_entries_count
    }
};