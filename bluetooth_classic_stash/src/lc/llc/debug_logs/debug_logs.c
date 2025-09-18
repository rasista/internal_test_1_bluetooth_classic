/**
 * @file debug_logs.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
 #include "debug_logs.h"
 #ifndef SIM
 #include "rail_seq.h"
 #endif

void debug_log_event(debug_log_type_t event_type, debug_log_event_t event)
{
  switch (event_type) {
    case DEBUG_GPIO:
      break;
    case DEBUG_PTI:
#ifndef SIM
      RAIL_SEQ_SetPtiAuxdataOutputMasked(RAIL_PTI_MASK_AUXDATA, event);
#endif
      break;
    case DEBUG_GLOBAL_MEM:
      break;
    default:
      break;
  }
}
