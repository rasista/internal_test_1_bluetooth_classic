#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "usched_port.h"
#include "usched_common_role.h"
#include "procedure_role_autogen.h"

struct usch_trace_stats_s usch_trace_stats[(USCH_TRACE_MAX_STATS + 1)];
uint16_t usch_trace_stats_count = 0;

/**
 * @brief Checks if a specific bit is set in the flag.
 *
 * @param flags
 * @param value
 */
uint8_t bg_bit_isset(uint32_t flags, uint8_t value)
{
  return (flags & (1 << value)) ? 1 : 0;
}

/**
 * @brief Sets a specific bit in the flag.
 *
 * @param flags
 * @param value
 */
void bg_bit_set(uint32_t *flags, uint8_t value)
{
  *flags |= (1 << value);
}

/**
 * @brief Clears a specific bit in the flag.
 *
 * @param flags
 * @param value
 */
void bg_bit_clr(uint32_t *flags, uint8_t value)
{
  *flags &= ~(1 << value);
}

/**
 * @brief Logs an event as a task in the scheduler.
 *
 * @param event The event to log.
 * @param role_id The role ID associated with the event.
 */
void usch_log_event_to_task(enum usch_taskEvent event, enum usch_role role_id)
{
  usch_trace_stats_count++;
  if (usch_trace_stats_count >= USCH_TRACE_MAX_STATS) {
    usch_trace_stats_count = 0;
  }

  usch_trace_stats[usch_trace_stats_count].event       = event;
  usch_trace_stats[usch_trace_stats_count].role_id     = role_id;
  usch_trace_stats[usch_trace_stats_count].tsf         = usch_GetTimeCB();
}

/**
 * @brief Sends an event to the scheduler.
 *
 * @param event
 * @param task.
 */
void usch_sendEventToTask(uint8_t event, usch_Task_t *task)
{
  role_cb_t *role_cb = task->user_data;
  usch_log_event_to_task(event, task->task_id);
  role_cb->callback_function(task, event);
}

/**
 * @brief Determines if time1 is greater than time2.
 *
 * @param time1
 * @param time2
 */
bool usch_timeGreaterThan(uint32_t time1, uint32_t time2)
{
  return ((time2 - time1) & 0x80000000) ? true : false;
}

/**
 * @brief Sets a specific bit in the flag.
 *
 * @param flags
 * @param value
 */
void bg_bit_set_atomic(uint32_t *flags, uint8_t value)
{
  bg_bit_set(flags, value);
}

/**
 * @brief Clears a specific bit in the flag.
 *
 * @param flags
 * @param value
 */
void bg_bit_clr_atomic(uint32_t *flags, uint8_t value)
{
  bg_bit_clr(flags, value);
}

/**
 * @brief Retrieves time from exec timing structure.
 *
 * @param t
 */
uint16_t ll_execTimingGetCurrentValue(exec_timing_t t)
{
  return t & 0xffff;
}

/**
 * @brief Triggers Scheduling Process Request.
 */
void usch_ScheduleReqCB(void)
{
  usch_ScheduleProcess();
}

/**
 * @brief Dummy function to retrieve the current system time.
 *
 * This function is intended to be used as a placeholder when the simulation
 * environment is not set up. It should compile locally but fail when integrated.
 *
 * @return uint32_t Returns 0 as a dummy value.
 */
uint32_t get_time_dummy()
{
  /*RFU*/
  // This is a dummy implementation. In a real environment, this function
  // should be replaced with a proper implementation that retrieves the
  // current system time.
  return 0;
}
/**
 * @brief Retrieves the current system time.
 */
uint32_t usch_GetTimeCB(void)
{
  return get_time_dummy();
}

/**
 * @brief Assertion function to check for unexpected cases.
 *
 * @param task
 */
int32_t BG_ASSERT(uint32_t task)
{
  if (!task) {
    // unexpected case
    while (1);
  }
  return 0;
}
