#include "procedure_triggers_autogen.h"
#include "page_scan_sm_autogen.h"
#include "state_machine.h"

/*page_scan_sm Handlers*/
sm_trigger_handler_t page_scan_sm_trigger_handler_entries[] = 
{
    {page_scan_start_procedure_handler},
    {page_scan_id_rx_done_handler},
    {page_scan_id_rx_timeout_handler},
    {page_scan_id_rx_timeout_handler},
    {page_scan_id_tx_done_handler},
    {page_scan_id_tx_abort_handler},
    {page_scan_fhs_rx_done_handler},
    {page_scan_fhs_rx_1_timeout_handler},
    {page_scan_fhs_rx_1_timeout_handler},
    {page_scan_fhs_rx_done_handler},
    {page_scan_fhs_rx_2_timeout_handler},
    {page_scan_fhs_rx_2_timeout_handler},
    {page_scan_fhs_response_tx_done_handler},
    {page_scan_fhs_response_tx_abort_handler},
    {page_scan_central_poll_rx_done_handler},
    {page_scan_central_poll_rx_timeout_handler},
    {page_scan_central_poll_rx_timeout_handler},
    {page_scan_peripheral_null_tx_done_handler},
    {page_scan_peripheral_null_tx_abort_handler},
};

uint8_t page_scan_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    152,
    6,
    152,
    152,
    6,
    152,
    6,
};

uint8_t page_scan_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    4,
    6,
    9,
    12,
    14,
    17,
};
sm_t page_scan_sm = {
#ifdef SIM
   .sm_name = "page_scan_sm",
#endif
     .max_num_states = page_scan_sm_state_max,
    .current_state = page_scan_sm_idle,
    .current_state_machine = page_scan_sm_context,
    .hashed_lut = {
        .hashed_entries = page_scan_sm_trigger_handler_entries,
        .hashed_entries_bitmap = page_scan_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = page_scan_sm_trigger_handler_entries_count
    }
};