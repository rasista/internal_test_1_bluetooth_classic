#include "usched_port.h"
#include "usched_common_role.h"

// Define the acl_runtime_stats array with the specified size
acl_runtime_stats_t acl_runtime_stats[MAX_ACL_CONNECTIONS_SUPPORTED];

/**
 * @brief this function clears the role instance and memsets the structure to 0
 *
 * @param role_cb
 */
inline void clear_role(role_cb_t *role_cb)
{
  memset(role_cb, 0, sizeof(role_cb_t));
  role_cb->task.user_data = role_cb;
}

/**
 * @brief this function checks the current priority and updates it based on the step size
 *
 * @param curr_priority
 * @param end_priority
 * @param step_size
 */
uint32_t usched_increase_priority(uint32_t curr_priority, uint32_t end_priority, uint32_t step_size)
{
  curr_priority = curr_priority - step_size;
  if (curr_priority < end_priority) {
    curr_priority = end_priority;
  }
  return curr_priority;
}

/**
 * @brief this function checks if the task end condition has been reached
 * True for end task and false if not
 *
 * @param min_runtime
 * @param end_tsf
 * @param curr_time
 */
bool check_task_end_based_on_time(uint32_t min_runtime, uint32_t end_tsf, uint32_t curr_time)
{
  if (min_runtime > (time_diff(curr_time, end_tsf))) {
    return true;
  }
  return false;
}

uint32_t time_diff(uint32_t curr_time, uint32_t end_time)
{
  return (end_time > curr_time) ? end_time - curr_time : curr_time - end_time;
}
