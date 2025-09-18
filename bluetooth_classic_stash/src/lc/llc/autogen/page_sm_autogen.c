#include "procedure_triggers_autogen.h"
#include "page_sm_autogen.h"
#include "state_machine.h"

/*page_sm Handlers*/
sm_trigger_handler_t page_sm_trigger_handler_entries[] = 
{
    {page_start_procedure_handler},
    {page_first_id_tx_done_handler},
    {page_first_id_tx_abort_handler},
    {page_second_id_tx_done_handler},
    {page_second_id_tx_abort_handler},
    {page_first_id_rx_done_handler},
    {page_first_id_rx_timeout_handler},
    {page_first_id_rx_timeout_handler},
    {page_second_id_rx_done_handler},
    {page_second_id_rx_timeout_handler},
    {page_second_id_rx_timeout_handler},
    {page_fhs_tx_done_handler},
    {page_fhs_tx_abort_handler},
    {page_peripheral_response_id_rx_done_handler},
    {page_peripheral_response_id_rx_timeout_handler},
    {page_peripheral_response_id_rx_timeout_handler},
    {page_central_response_poll_tx_done_handler},
    {page_central_response_poll_tx_abort_handler},
    {page_peripheral_response_null_rx_done_handler},
    {page_peripheral_response_null_rx_timeout_handler},
    {page_peripheral_response_null_rx_timeout_handler},
};

uint8_t page_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    6,
    6,
    152,
    152,
    6,
    152,
    6,
    152,
};

uint8_t page_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    3,
    5,
    8,
    11,
    13,
    16,
    18,
};
sm_t page_sm = {
#ifdef SIM
   .sm_name = "page_sm",
#endif
     .max_num_states = page_sm_state_max,
    .current_state = page_sm_idle,
    .current_state_machine = page_sm_context,
    .hashed_lut = {
        .hashed_entries = page_sm_trigger_handler_entries,
        .hashed_entries_bitmap = page_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = page_sm_trigger_handler_entries_count
    }
};