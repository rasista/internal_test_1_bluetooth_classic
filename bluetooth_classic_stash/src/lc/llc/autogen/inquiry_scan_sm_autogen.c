#include "procedure_triggers_autogen.h"
#include "inquiry_scan_sm_autogen.h"
#include "state_machine.h"

/*inquiry_scan_sm Handlers*/
sm_trigger_handler_t inquiry_scan_sm_trigger_handler_entries[] = 
{
    {inquiry_scan_start_procedure_handler},
    {inquiry_scan_rx_done_handler},
    {inquiry_scan_rx_timeout_handler},
    {inquiry_scan_rx_timeout_handler},
    {inquiry_scan_fhs_tx_done_handler},
    {inquiry_scan_fhs_tx_abort_handler},
    {inquiry_scan_eir_tx_done_handler},
    {inquiry_scan_eir_tx_abort_handler},
};

uint8_t inquiry_scan_sm_trigger_handler_entries_bitmap[] = 
{
    1,
    152,
    6,
    6,
};

uint8_t inquiry_scan_sm_trigger_handler_entries_count[] = 
{
    0,
    1,
    4,
    6,
};
sm_t inquiry_scan_sm = {
#ifdef SIM
   .sm_name = "inquiry_scan_sm",
#endif
     .max_num_states = inquiry_scan_sm_state_max,
    .current_state = inquiry_scan_sm_idle,
    .current_state_machine = inquiry_scan_sm_context,
    .hashed_lut = {
        .hashed_entries = inquiry_scan_sm_trigger_handler_entries,
        .hashed_entries_bitmap = inquiry_scan_sm_trigger_handler_entries_bitmap,
        .hashed_entries_count = inquiry_scan_sm_trigger_handler_entries_count
    }
};