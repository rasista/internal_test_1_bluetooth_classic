#ifndef __STATE_MACHINE_H_
#define __STATE_MACHINE_H_
#include "btc_common_defines.h"
#include "stdint.h"
#include "stdio.h"

#define SM_STATE_ROLE_IDLE 0

// This is just a forward declaration to breaj cyclic dependency
struct sm_trigger_handler_s;
typedef struct sm_trigger_handler_s sm_trigger_handler_t;

typedef struct hashed_lut_s {
  sm_trigger_handler_t *hashed_entries; //! Function pointers to handlers
  uint8_t
  *hashed_entries_bitmap;     //! bitmap for valid state Vs. trigger combimation
  uint8_t *
    hashed_entries_count;   //! count array for cumulative valid state Vs
                            //! trigger combimations before a given coarse index
} hashed_lut_t;

typedef struct sm_s {
#ifdef SIM
  char sm_name[24]; //! informative, used in asserts
#endif
  uint8_t current_state_machine; //current state machine running
  uint8_t max_num_states;  //! max number of states in state machine
  uint8_t current_state;   //! current state of the state machine
  hashed_lut_t hashed_lut; //! Hashed LUT for state machine handlers
  uint8_t last_rx_slots;  // Number of slots required for the last received packet
  uint8_t last_tx_slots;  // Number of slots required for the last transmitted packet
  uint8_t current_device_index; // Current device index
} sm_t;

typedef struct sm_trigger_handler_s {
  void (*handler)(sm_t *sm);
} sm_trigger_handler_t;

static inline void SM_STATE(sm_t *sm, uint8_t state)
{
  sm->current_state = state;
}

static inline void sm_assert(sm_t *sm, uint8_t trigger)
{
  (void) trigger;
  (void) sm;
#ifdef SIM
  printf("\nNo handler for state %d and trigger %d\n", sm->current_state, trigger);
#endif
  while (1) {
  }
}
void sm_step(sm_t *sm, uint32_t trigger);
#define SM_STEP(sm_a, sm, trigger) sm_step(sm, trigger)
#endif
