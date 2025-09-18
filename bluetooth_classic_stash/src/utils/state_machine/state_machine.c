#include "state_machine.h"
#include "procedure_triggers_autogen.h"

uint8_t set_bit_count[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

/**
 * @brief
 * The API is used to take a single step in a state machine.
 * API calls kthe function handler registred for given current_state
 * Vs. trigger combination, if a handler is registerd for given state vs trigger
 * combination, corresponding bit is set in a linear byte array identified as
 * hashed_entries_bitmap. corresponding bit in the linear byte array can be seen
 * as bit(fine_offset) in (coarse_offset)th byte in the linear byte array.
 * @param sm
 * @param trigger
 */
void sm_step(sm_t *sm, uint32_t trigger)
{
  //! handler unique id is current_state*max_num_triggers + current_trigger
  uint32_t handler_id =
    sm->current_state * procedures_max_num_triggers + trigger;
  uint32_t coarse_offset = handler_id >> 3; //! dividing by 8
  uint32_t fine_offset = handler_id & 0x7;  //! Module 8

  hashed_lut_t *hashed_lut = &sm->hashed_lut;
  //! check if the state vs trigger combination have a valid handler registered
  if (hashed_lut->hashed_entries_bitmap[coarse_offset] & (1 << fine_offset)) {
    uint8_t bitmap_entry = hashed_lut->hashed_entries_bitmap[coarse_offset];
    uint8_t fine_bitmap = bitmap_entry & (0xff >> (7 - fine_offset));
    uint8_t fine_bitmap_low = fine_bitmap & 0xf;
    uint8_t fine_bitmap_high = (fine_bitmap & 0xf0) >> 4;
    uint8_t fine_bit_count =
      set_bit_count[fine_bitmap_high] + set_bit_count[fine_bitmap_low];
    uint8_t coarse_bit_count = hashed_lut->hashed_entries_count[coarse_offset];
    sm_trigger_handler_t *sm_trigger_handler =
      &hashed_lut->hashed_entries[coarse_bit_count + fine_bit_count - 1];
    sm_trigger_handler->handler(sm);
  } else {
    sm_assert(sm, trigger);
  }
}
