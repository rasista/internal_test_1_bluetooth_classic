#ifndef test_mode_sm_AUTOGEN_H_
#define test_mode_sm_AUTOGEN_H_
#include "state_machine.h"

extern sm_t test_mode_sm;

extern sm_trigger_handler_t test_mode_sm_trigger_handler_entries[];
extern uint8_t test_mode_sm_trigger_handler_entries_bitmap[];
extern uint8_t test_mode_sm_trigger_handler_entries_count[];

typedef enum test_mode_sm_states_s {
    test_mode_sm_idle,
    TEST_MODE_TX_SCHEDULED,
    TEST_MODE_WAITING_FOR_RX_COMPLETE,
    test_mode_sm_state_max
} test_mode_sm_states_t;



void test_mode_start_procedure_handler(sm_t *test_mode_sm);
void test_mode_tx_done_handler(sm_t *test_mode_sm);
void test_mode_tx_abort_handler(sm_t *test_mode_sm);
void test_mode_rx_done_handler(sm_t *test_mode_sm);
void test_mode_rx_timeout_handler(sm_t *test_mode_sm);
void test_mode_rx_abort_handler(sm_t *test_mode_sm);
#endif