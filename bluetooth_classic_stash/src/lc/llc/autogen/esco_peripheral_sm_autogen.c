#include "procedure_triggers_autogen.h"
#include "esco_peripheral_sm_autogen.h"
#include "state_machine.h"

/*esco_peripheral_sm Handlers*/
sm_trigger_handler_t esco_peripheral_sm_trigger_handler_entries[] = 
{
    {esco_peripheral_start_procedure_handler},
    {esco_peripheral_rx_timeout_handler},
    {esco_peripheral_rx_header_done_handler},
    {esco_peripheral_rx_timeout_handler},
    {esco_peripheral_rx_complete_handler},
    {esco_peripheral_rx_timeout_handler},
    {esco_peripheral_rx_timeout_handler},
    {esco_peripheral_tx_done_handler},
    {esco_peripheral_tx_abort_handler},
    {esco_peripheral_rx_timeout_handler},
    {esco_peripheral_rx_header_done_handler},
    {esco_peripheral_rx_timeout_handler},
};

uint8_t esco_peripheral_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    208,
    152,
    6,
    208,
};

uint8_t esco_peripheral_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    4,
    7,
    9,
};
sm_t esco_peripheral_sm = {
#ifdef SIM
   .sm_name = "esco_peripheral_sm",
#endif
     .max_num_states = esco_peripheral_sm_state_max,
    .current_state = esco_peripheral_sm_idle,
    .current_state_machine = esco_peripheral_sm_context,
    .hashed_lut = {
        .hashed_entries = esco_peripheral_sm_trigger_handler_entries,
        .hashed_entries_bitmap = esco_peripheral_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = esco_peripheral_sm_trigger_handler_entries_count
    }
};