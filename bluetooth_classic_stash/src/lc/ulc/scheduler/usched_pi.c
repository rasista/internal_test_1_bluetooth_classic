#include "usched_pi.h"
#include "usched.h"
#include "usched_hci_cmd_event_handlers.h"

extern uint32_t timer_invoke_count;
extern struct usch_s usch;

/**
 * @brief this function is the Look Up Table for pi related callbacks
 */
const usched_handler_entry_t usched_pi_callback[] = {
  { usched_pi_start_handler },
  { usched_pi_stop_handler },
  { usched_pi_schedule_fail_handler },
};

/**
 * @brief this function triggers the corresponding pi callback
 *
 * @param task
 * @param event enum
 */
void usched_pi_event_callback(usch_Task_t *task, enum usch_taskEvent event)
{
  usched_pi_callback[event].handler(NULL, task);
}

void usched_pi_parameters_init(void *ctx, uint8_t length, uint32_t role)
{
  /*Filling usched_pi_paramas*/
  uscheduler_info_t *scheduler_info = (uscheduler_info_t *)ctx;
  usched_pi_params_t usched_pi_params;
  usched_pi_params.role = role;
  if (role == INQUIRY_ROLE) {
    usched_pi_params.start_priority = INQUIRY_START_PRIORITY;
    usched_pi_params.end_priority = INQUIRY_END_PRIORITY;
    usched_pi_params.priority_step_size = USCH_PRIORITY_STEP;
    usched_pi_params.min_runtime = INQUIRY_MIN_RUN_TIME;
    usched_pi_params.max_runtime = INQUIRY_MAX_RUN_TIME;
    usched_pi_params.pi_total_duration = (uint32_t)(length << 11);
    usched_pi_params.curr_priority = usched_pi_params.start_priority;
  } else {
    usched_pi_params.start_priority = PAGE_START_PRIORITY;
    usched_pi_params.end_priority = PAGE_END_PRIORITY;
    usched_pi_params.priority_step_size = USCH_PRIORITY_STEP;
    usched_pi_params.min_runtime = PAGE_MIN_RUN_TIME;
    usched_pi_params.max_runtime = PAGE_MAX_RUN_TIME;
    usched_pi_params.curr_priority = usched_pi_params.start_priority;
  }
  pi_role_initialization(scheduler_info, &usched_pi_params);
}

/**
 * @brief this function is used by host to trigger page/inquiry role
 *
 * @param scheduler_info
 * @param usched_pi_params
 */
void pi_role_initialization(uscheduler_info_t *scheduler_info, usched_pi_params_t *usched_pi_params)
{
  role_cb_t *pi_role_cb;
  if (usched_pi_params->role == PAGE_ROLE) {
    pi_role_cb         = &scheduler_info->page_role_cb;
    pi_role_cb->task.flags |= PAGE_FLAGS;
    BTC_PRINT("\nInit Page %d\n", pi_role_cb);
  } else {
    pi_role_cb         = &scheduler_info->inquiry_role_cb;
    pi_role_cb->task.flags |= INQUIRY_FLAGS;
    BTC_PRINT("\nInit Inquiry %d\n", pi_role_cb);
  }

  /*Filling pi_role_cb params*/
  pi_role_cb->end_tsf = usch_GetTimeCB() + usched_pi_params->pi_total_duration;
  pi_role_cb->task.minRuntime    = usched_pi_params->min_runtime;
  pi_role_cb->task.maxRuntime    = usched_pi_params->max_runtime;
  pi_role_cb->callback_function  = usched_pi_event_callback;
  pi_role_cb->start_priority = usched_pi_params->start_priority;
  pi_role_cb->end_priority = usched_pi_params->end_priority;
  pi_role_cb->priority_step_size = usched_pi_params->priority_step_size;
  pi_role_cb->occupied = ROLE_OCCUPIED;

  memcpy(&pi_role_cb->usched_private_data.usched_pi_params, usched_pi_params, sizeof(usched_pi_params_t));

  /*Filling task variables*/
  pi_role_cb->task.task_id = usched_pi_params->role;
  pi_role_cb->task.priority = pi_role_cb->start_priority;
  pi_role_cb->task.next = NULL;
  pi_role_cb->task.start =  usch_GetTimeCB();
  pi_role_cb->task.user_data = pi_role_cb;

  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), usched_pi_params->role, 1, "pi_role_initialization");
  BTC_PRINT("Time: %d, pi_role_initialization %d\n", usch_GetTimeCB(), pi_role_cb);
#ifdef SCHED_SIM
  usched_api_start_inquiry_periodic_inq();
#else
  /*Add task to queue*/
  usch_AddTask(&pi_role_cb->task);
#endif
}

/**
 * @brief this function checks for invalid flag and then reschedules the role of page or inquiry
 *
 * @param task
 */
void pi_reschedule(usch_Task_t *task)
{
  role_cb_t *role_cb                 = (role_cb_t *)task->user_data;
  uint32_t start;
  uint32_t pi_start_tsf;
  uint32_t curr_time;

  usch_RemoveTask(&role_cb->task, true);    //remove the completed task and initialize the next task
  /*Checking if there is enough time to serve at least the minimum runtime*/
  curr_time = usch_GetTimeCB();
  if (check_task_end_based_on_time(task->minRuntime, role_cb->end_tsf, curr_time)) {
    role_cb->task.status_flags = INVALID_TASK;
    clear_role(role_cb);
  } else {
    task->status_flags &= ~REMOVE_TASK;
    pi_start_tsf = usch_GetTimeCB();
    start               = find_white_spaces(pi_start_tsf,
                                            (role_cb->end_tsf),
                                            (role_cb->task.minRuntime),
                                            (role_cb->task.maxRuntime),
                                            0,
                                            &role_cb->task);
    role_cb->task.start = start;
    role_cb->task.next  = NULL;
    usch_AddTask(task);
    BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), role_cb->usched_private_data.usched_pi_params.role, 1, "pi_role_initialization");

    // if(task->start > usch_GetTimeCB()+2){
    //   timer_invoke_count = task->start;
    //   // set the pending flag
    //   bg_bit_set(&task->flags, USCH_FLAG_PENDING);
    // }
  }
}

/**
 * @brief Callback handler for pi start triggered by uscheduler
 */
void usched_pi_start_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *pi_role_cb               = (role_cb_t *)task->user_data;
  BTC_PRINT("Time: %d, usched_pi_start_handler %d\n", usch_GetTimeCB(), pi_role_cb);

  pi_role_cb->usched_private_data.usched_pi_params.end_tsf = pi_role_cb->end_tsf; // Updating end_tsf here to send to lower layer
  pi_role_cb->usched_private_data.usched_pi_params.curr_priority = task->priority;
  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), pi_role_cb->usched_private_data.usched_pi_params.role, 1, "usched_pi_start_handler");
#ifndef SCHED_SIM
  lpw_start_pi_from_usched((usched_pi_params_t *)&pi_role_cb->usched_private_data.usched_pi_params);
#endif
}

/**
 * @brief Callback handler for pi stop triggered by uscheduler
 */
void usched_pi_stop_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *pi_role_cb               = (role_cb_t *)task->user_data;
  // ToDo xxxx check if we need to update priority here
  BTC_PRINT("Time: %d, usched_pi_stop_handler %d\n", usch_GetTimeCB(), pi_role_cb);
  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), pi_role_cb->usched_private_data.usched_pi_params.role, 0, "usched_pi_stop_handler");
#ifdef SCHED_SIM
  usched_api_cancel_inquiry_periodic_inq();
#else
  lpw_stop_pi_from_usched((usched_pi_params_t *)&pi_role_cb->usched_private_data.usched_pi_params);
#endif
}

/**
 * @brief Callback handler for pi schedule fail triggered by uscheduler
 */
void usched_pi_schedule_fail_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *pi_role_cb               = (role_cb_t *)task->user_data;
  pi_role_cb->task.priority  = usched_increase_priority(pi_role_cb->task.priority, pi_role_cb->end_priority, pi_role_cb->priority_step_size);
  BTC_PRINT("Time: %d, usched_pi_fail_handler %d\n", usch_GetTimeCB(), pi_role_cb);
  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), pi_role_cb->usched_private_data.usched_pi_params.role, 0, "usched_pi_schedule_fail_handler");

  pi_reschedule(task);
}

/**
 * @brief Callback handler for pi stop triggered by host
 *
 * @param scheduler_info
 */
void usched_pi_role_stop(uscheduler_info_t *scheduler_info, uint32_t role)
{
  // if(!scheduler_info->inquiry_role_cb.occupied) {
  //   BTC_PRINT("Invalid role\n");
  //   return;
  // }
  role_cb_t *pi_role_cb;
  if (role == PAGE_ROLE) {
    pi_role_cb         = &scheduler_info->page_role_cb;
  } else if (role == INQUIRY_ROLE) {
    pi_role_cb         = &scheduler_info->inquiry_role_cb;
  } else {
    return;
  }

  pi_role_cb->end_tsf = usch_GetTimeCB();
#ifdef SCHED_SIM
  usched_api_cancel_inquiry_periodic_inq();
#else
  lpw_stop_pi_from_usched((usched_pi_params_t *)&pi_role_cb->usched_private_data.usched_pi_params);
#endif
  BTC_PRINT("%d %d set_state %d %d\n", usch_GetTimeCB(), pi_role_cb->usched_private_data.usched_pi_params.role, 0, "usched_pi_role_stop");

  /* xxxx ToDo - do we call LPW here instead?*/
}

/**
 * @brief Final pi end handler, called after acknowledgment from lpw that role is stopped
 */
void lpw_pi_stopped(uscheduler_info_t *scheduler_info, uint32_t role)
{
  /* xxxx ToDo to keep a flag here for race condition - if both lpw and scheduler simultaneously call this*/
  role_cb_t *pi_role_cb;
  if (role == PAGE_ROLE) {
    pi_role_cb         = &scheduler_info->page_role_cb;
  } else {
    pi_role_cb         = &scheduler_info->inquiry_role_cb;
  }
  BTC_PRINT("Time: %d, lpw_pi_stopped \n", usch_GetTimeCB());
  BTC_PRINT("%d %d set_state %d %s\n", usch_GetTimeCB(), pi_role_cb->usched_private_data.usched_pi_params.role, 0, "lpw_pi_stopped");

  pi_reschedule(&pi_role_cb->task);
}
