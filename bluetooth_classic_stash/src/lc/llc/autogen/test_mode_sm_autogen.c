#include "procedure_triggers_autogen.h"
#include "test_mode_sm_autogen.h"
#include "state_machine.h"

/*test_mode_sm Handlers*/
sm_trigger_handler_t test_mode_sm_trigger_handler_entries[] = 
{
    {test_mode_start_procedure_handler},
    {test_mode_tx_done_handler},
    {test_mode_tx_abort_handler},
    {test_mode_rx_done_handler},
    {test_mode_rx_timeout_handler},
    {test_mode_rx_abort_handler},
};

uint8_t test_mode_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    6,
    152,
};

uint8_t test_mode_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    3,
};
sm_t test_mode_sm = {
#ifdef SIM
   .sm_name = "test_mode_sm",
#endif
     .max_num_states = test_mode_sm_state_max,
    .current_state = test_mode_sm_idle,
    .current_state_machine = test_mode_sm_context,
    .hashed_lut = {
        .hashed_entries = test_mode_sm_trigger_handler_entries,
        .hashed_entries_bitmap = test_mode_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = test_mode_sm_trigger_handler_entries_count
    }
};