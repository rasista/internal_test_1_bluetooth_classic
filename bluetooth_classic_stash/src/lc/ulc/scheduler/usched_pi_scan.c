#include "usched_pi_scan.h"
#include "usched_hci_cmd_event_handlers.h"
/**
 * @brief this function is the Look Up Table for pi scan related callbacks
 */
const usched_handler_entry_t usched_pi_scan_callback[] = {
  { usched_pi_scan_start_handler },
  { usched_pi_scan_stop_handler },
  { usched_pi_scan_schedule_fail_handler },
};

/**
 * @brief this function triggers the corresponding pi scan callback
 *
 * @param task
 * @param event enum
 */
void usched_pi_scan_event_callback(usch_Task_t *task, enum usch_taskEvent event)
{
  //usched_pi_scan_callback[event].handler(&scheduler_info, task);
  usched_pi_scan_callback[event].handler(NULL, task);
}

void usched_pi_scan_parameters_init(void *ctx, uint32_t window, uint32_t interval, uint32_t role)
{
  /*Filling usched_pi_scan_paramas*/
  uscheduler_info_t *scheduler_info = (uscheduler_info_t *)ctx;
  usched_pi_scan_params_t usched_pi_scan_params;
  usched_pi_scan_params.role = role;
  usched_pi_scan_params.pi_scan_window = window;
  usched_pi_scan_params.pi_scan_interval = interval;
  usched_pi_scan_params.priority_step_size = USCH_PRIORITY_STEP;
  if (role == INQUIRY_SCAN_ROLE) {
    usched_pi_scan_params.start_priority = INQUIRY_SCAN_START_PRIORITY;
    usched_pi_scan_params.end_priority = INQUIRY_SCAN_END_PRIORITY;
    usched_pi_scan_params.curr_priority = usched_pi_scan_params.start_priority;
  } else {
    usched_pi_scan_params.start_priority = PAGE_SCAN_START_PRIORITY;
    usched_pi_scan_params.end_priority = PAGE_SCAN_END_PRIORITY;
    usched_pi_scan_params.curr_priority = usched_pi_scan_params.start_priority;
  }
  pi_scan_role_initialization(scheduler_info, &usched_pi_scan_params);
}

void usched_pi_scan_parameters_deinit(void *ctx, uint32_t role)
{
  uscheduler_info_t *scheduler_info = (uscheduler_info_t *)ctx;
  if (role == INQUIRY_SCAN_ROLE) {
    usched_pi_scan_stop_handler(NULL, &scheduler_info->inquiry_scan_role_cb.task);
  } else {
    usched_pi_scan_stop_handler(NULL, &scheduler_info->page_scan_role_cb.task);
  }
}
/**
 * @brief this function is used by host to trigger pi scan role
 *
 * @param scheduler_info
 * @param usched_pi_scan_params
 */
void pi_scan_role_initialization(uscheduler_info_t *scheduler_info, usched_pi_scan_params_t *usched_pi_scan_params)
{
  role_cb_t *pi_scan_role_cb;
  if (usched_pi_scan_params->role == PAGE_SCAN_ROLE) {
    pi_scan_role_cb         = &scheduler_info->page_scan_role_cb;
    pi_scan_role_cb->task.flags |= PAGE_SCAN_FLAGS;
  } else {
    pi_scan_role_cb         = &scheduler_info->inquiry_scan_role_cb;
    pi_scan_role_cb->task.flags |= INQUIRY_SCAN_FLAGS;
  }

  /*Filling pi_scan_role_cb params*/
  pi_scan_role_cb->end_tsf = usch_GetTimeCB() + usched_pi_scan_params->pi_scan_interval;
  pi_scan_role_cb->task.minRuntime    = usched_pi_scan_params->pi_scan_window;
  pi_scan_role_cb->task.maxRuntime    = usched_pi_scan_params->pi_scan_window;
  pi_scan_role_cb->callback_function = usched_pi_scan_event_callback;
  pi_scan_role_cb->start_priority = usched_pi_scan_params->start_priority;
  pi_scan_role_cb->end_priority = usched_pi_scan_params->end_priority;
  pi_scan_role_cb->priority_step_size = usched_pi_scan_params->priority_step_size;
  pi_scan_role_cb->occupied = ROLE_OCCUPIED;

  memcpy(&pi_scan_role_cb->usched_private_data.usched_pi_scan_params, usched_pi_scan_params, sizeof(usched_pi_scan_params_t));

  /*Filling task variables*/
  pi_scan_role_cb->task.task_id = usched_pi_scan_params->role;
  pi_scan_role_cb->task.priority = pi_scan_role_cb->start_priority;
  pi_scan_role_cb->task.next = NULL;
  pi_scan_role_cb->task.start = usch_GetTimeCB();
  pi_scan_role_cb->task.user_data = pi_scan_role_cb;
#ifdef SCHED_SIM
  usched_api_start_inquiry_scan();
#else
  /*Add task to queue*/
  usch_AddTask(&pi_scan_role_cb->task);
#endif
}

/**
 * @brief this function checks for invalid flag and then reschedules the role of inquiry scan or page_scan
 *
 * @param task
 */
void pi_scan_reschedule(usched_pi_scan_params_t *pi_scan_params, usch_Task_t *task)
{
  role_cb_t *role_cb                 = (role_cb_t *)task->user_data;
  uint32_t start;
  uint32_t curr_time;

  //Removing the task
  usch_RemoveTask(&role_cb->task, true);
  curr_time = usch_GetTimeCB();
  if (check_task_end_based_on_time(task->minRuntime, role_cb->end_tsf, curr_time)) {
    task->status_flags = INVALID_TASK;
    clear_role(role_cb);
  }
  // else if (task->status_flags & REMOVE_TASK) {
  else {
    task->status_flags &= ~REMOVE_TASK;
    role_cb->task.start = role_cb->task.start + pi_scan_params->pi_scan_interval;
    if (role_cb->task.start < usch_GetTimeCB()) {
      /*update new start*/
      role_cb->task.start = (((usch_GetTimeCB() - role_cb->task.start) / pi_scan_params->pi_scan_window + 1) * pi_scan_params->pi_scan_window) + role_cb->task.start;
    }

    start = find_white_spaces(role_cb->task.start,
                              (role_cb->task.start + pi_scan_params->pi_scan_window),
                              (role_cb->task.minRuntime),
                              (role_cb->task.maxRuntime),
                              0,
                              &role_cb->task);
    if (start < role_cb->task.start) {
      start = role_cb->task.start;
    }
    //! Updating the final start slot
    role_cb->task.next = NULL;
    role_cb->task.start = start;
    usch_AddTask(task);
  }
}

/**
 * @brief Callback handler for pi scan start triggered by uscheduler
 */
void usched_pi_scan_start_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *pi_scan_role_cb                         = (role_cb_t *)task->user_data;

  pi_scan_role_cb->usched_private_data.usched_pi_scan_params.end_tsf = pi_scan_role_cb->end_tsf; // Updating end_tsf here to send to lower layer
  pi_scan_role_cb->usched_private_data.usched_pi_scan_params.curr_priority = task->priority;
#ifndef SCHED_SIM
  lpw_start_pi_scan_from_usched((usched_pi_scan_params_t *)&pi_scan_role_cb->usched_private_data.usched_pi_scan_params);
#endif
}

/**
 * @brief Callback handler for pi scan stop triggered by uscheduler
 */
void usched_pi_scan_stop_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *pi_scan_role_cb               = (role_cb_t *)task->user_data;
  // ToDo xxxx check if we need to update priority here
  BTC_PRINT("Time: %d, usched_pi_stop_handler %d\n", usch_GetTimeCB(), pi_scan_role_cb);
  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), pi_scan_role_cb->usched_private_data.usched_pi_scan_params.role, 0, "usched_pi_scan_stop_handler");
#ifdef SCHED_SIM
  usched_api_cancel_inquiry_periodic_inq();
#else
  usch_RemoveTask(&pi_scan_role_cb->task, true);
  lpw_stop_pi_scan_from_usched((usched_pi_scan_params_t *)&pi_scan_role_cb->usched_private_data.usched_pi_scan_params);
#endif
}

/**
 * @brief Callback handler for pi scan schedule fail triggered by uscheduler
 */
void usched_pi_scan_schedule_fail_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *pi_scan_role_cb               = (role_cb_t *)task->user_data;

  pi_scan_role_cb->task.priority  = usched_increase_priority(pi_scan_role_cb->task.priority, pi_scan_role_cb->end_priority, pi_scan_role_cb->priority_step_size);

  pi_scan_reschedule((usched_pi_scan_params_t *)&pi_scan_role_cb->usched_private_data.usched_pi_scan_params, &pi_scan_role_cb->task);
}

/**
 * @brief Callback handler for pi scan stop triggered by host
 *
 * @param scheduler_info
 */
void usched_pi_scan_role_stop(uscheduler_info_t *scheduler_info, uint32_t role)
{
  // if(!scheduler_info->page_role_cb.occupied) {
  //   BTC_PRINT("Invalid role\n");
  //   return;
  // }
  role_cb_t *pi_scan_role_cb;
  if (role == PAGE_SCAN_ROLE) {
    pi_scan_role_cb         = &scheduler_info->page_scan_role_cb;
  } else if (role == INQUIRY_SCAN_ROLE) {
    pi_scan_role_cb         = &scheduler_info->inquiry_scan_role_cb;
  } else {
    return;
  }
  pi_scan_role_cb->end_tsf = usch_GetTimeCB();
#ifdef SCHED_SIM
  usched_api_cancel_inquiry_periodic_inq();
#else
  lpw_stop_pi_scan_from_usched((usched_pi_scan_params_t *)&pi_scan_role_cb->usched_private_data.usched_pi_scan_params);
#endif
}

/**
 * @brief Final pi scan end handler, called after acknowledgment from lpw that role is stopped
 */
void lpw_pi_scan_stopped(uscheduler_info_t *scheduler_info, uint32_t role)
{
  /* xxxx ToDo to keep a flag here for race condition - if both lpw and scheduler simultaneously call this*/
  role_cb_t *pi_scan_role_cb;
  if (role == PAGE_SCAN_ROLE) {
    pi_scan_role_cb         = &scheduler_info->page_scan_role_cb;
  } else {
    pi_scan_role_cb         = &scheduler_info->inquiry_scan_role_cb;
  }
  BTC_PRINT("Time: %d, lpw_pi_scan_stopped \n", usch_GetTimeCB());
  pi_scan_reschedule((usched_pi_scan_params_t *)&pi_scan_role_cb->usched_private_data.usched_pi_scan_params, &pi_scan_role_cb->task);
}
