#include "usched_acl.h"
#include "usched_common_role.h"
/**
 * @brief this function is the Look Up Table for acl related callbacks
 */
const usched_handler_entry_t usched_acl_callback[] = {
  { usched_acl_start_handler },
  { usched_acl_stop_handler },
  { usched_acl_fail_handler },
};

/**
 * @brief this function triggers the corresponding acl  callback
 *
 * @param task
 * @param event enum
 */
void usched_acl_event_callback(usch_Task_t *task, enum usch_taskEvent event)
{
  usched_acl_callback[event].handler(&scheduler_info, task);
}

/**
 * @brief this function is used by host to trigger page/inquiry role
 *
 * @param scheduler_info
 * @param usched_pi_params
 */
void acl_role_initialization(uscheduler_info_t *scheduler_info, usched_acl_params_t *usched_acl_params)
{
  role_cb_t *acl_role_cb = &scheduler_info->connection_role_cb[usched_acl_params->conn_info_index];
  /*Filling acl_role_cb params*/
  acl_role_cb->end_tsf = usch_GetTimeCB() + usched_acl_params->duration;
  acl_role_cb->task.minRuntime    = usched_acl_params->min_runtime;
  acl_role_cb->task.maxRuntime    = usched_acl_params->max_runtime;
  acl_role_cb->callback_function  = usched_acl_event_callback;
  acl_role_cb->start_priority = usched_acl_params->start_priority;
  acl_role_cb->end_priority = usched_acl_params->end_priority;
  acl_role_cb->priority_step_size = usched_acl_params->priority_step_size;
  acl_role_cb->occupied = ROLE_OCCUPIED;
  memcpy(&acl_role_cb->usched_private_data.usched_acl_params, usched_acl_params, sizeof(usched_acl_params_t));
  /*Filling task variables*/
  acl_role_cb->task.task_id = NON_ACL_ROLES_COUNT + usched_acl_params->conn_info_index; // Taskid Range 5-7, conn index range 0-2
  acl_role_cb->task.priority = usched_acl_params->start_priority;
  acl_role_cb->task.next = NULL;
  acl_role_cb->task.start =  usch_GetTimeCB();
  acl_role_cb->task.user_data = acl_role_cb;
  acl_role_cb->task.flags |= CONNECTED_FLAGS;

  /*Add task to queue*/
  usch_AddTask(&acl_role_cb->task);
  BTC_PRINT("Time: %d, acl_role_initialization\n", usch_GetTimeCB());
}

/* xxxx ToDo Dummy function for compilation, add implementation later*/
uint32_t get_max_runtime_based_on_pending_packets(role_cb_t *role_cb)
{
  /* xxxx ToDo Considering Max Slot Packet. Can make it specific to the current max slot */
  return ((role_cb->usched_private_data.usched_acl_params.is_tx_pending + role_cb->usched_private_data.usched_acl_params.is_rx_pending) * 6 * BT_SLOT_WIDTH);
}

/*ToDo xxxx to be edited to have a random number generator*/
uint32_t generate_random(uint32_t lower_end, uint32_t higher_end)
{
  return ((lower_end + higher_end) / 2); /*ToDo Create Random logic here*/
}

/**
 * @brief this function checks for invalid flag and then reschedules acl central
 *
 * @param task
 * @param is_fail
 */
void acl_central_reschedule(usch_Task_t *task, uint32_t is_fail)
{
  (void)is_fail;
  role_cb_t *role_cb                 = (role_cb_t *)task->user_data;
  uint32_t start;
  uint32_t acl_start_tsf;

  usch_RemoveTask(&role_cb->task, true);

  if (role_cb->usched_private_data.usched_acl_params.acl_priority == ACL_HIGH_PRIORITY) {
    role_cb->task.flags = 0;
    role_cb->task.flags |= ACL_HIGH_PRIORITY_FLAGS;
  }

  /*Checking if there is enough time to serve at least the minimum runtime*/
  if (role_cb->task.status_flags & INVALID_TASK) {
    clear_role(role_cb);
    // role_cb->task.status_flags &= ~INVALID_TASK;
  } else {
    task->status_flags &= ~REMOVE_TASK;
    if ((role_cb->usched_private_data.usched_acl_params.is_tx_pending) || (role_cb->usched_private_data.usched_acl_params.is_rx_pending)) {
      acl_start_tsf = usch_GetTimeCB();
      role_cb->task.minRuntime = CONNECTED_HIGH_PRIORITY_SLOTS;
      role_cb->task.maxRuntime = get_max_runtime_based_on_pending_packets(role_cb);
      role_cb->task.maxRuntime = MAX(role_cb->task.minRuntime, role_cb->task.maxRuntime);
    } else {
      if (role_cb->usched_private_data.usched_acl_params.acl_priority == ACL_LOW_PRIORITY) {
        acl_start_tsf = usch_GetTimeCB() + role_cb->usched_private_data.usched_acl_params.poll_interval;
      } else { /*If ACL_HIGH_PRIORITY*/
        acl_start_tsf = usch_GetTimeCB();
        /*ToDo xxxx this check for a2dp sink / high priority needs to be discussed*/
      }
      role_cb->task.minRuntime = CONNECTED_NORMAL_PRIORITY_SLOTS;
    }
    // if (is_fail) {
    //   acl_start_tsf += generate_random(MIN_RANDOM_VALUE, MAX_RANDOM_VALUE);
    // }
    role_cb->end_tsf = acl_start_tsf + role_cb->task.maxRuntime;
    start = find_white_spaces(acl_start_tsf,
                              (role_cb->end_tsf),
                              (role_cb->task.minRuntime),
                              (role_cb->task.maxRuntime),
                              0,
                              &role_cb->task);
    role_cb->task.start = start;
    role_cb->task.next  = NULL;
    usch_AddTask(task);
  }
  BTC_PRINT("Time: %d, acl_central_reschedule\n", usch_GetTimeCB());
}

// xxxx ToDo - to be updated
/**
 * @brief this function checks for invalid flag and then reschedules acl peripheral
 *
 * @param task
 * @param is_fail
 */
void acl_peripheral_reschedule(usch_Task_t *task, uint32_t is_fail)
{
  role_cb_t *role_cb                 = (role_cb_t *)task->user_data;
  uint32_t start;
  uint32_t acl_start_tsf;

  usch_RemoveTask(&role_cb->task, true);

  uint32_t conn_index = role_cb->usched_private_data.usched_acl_params.conn_info_index;

  if (role_cb->usched_private_data.usched_acl_params.acl_priority == ACL_HIGH_PRIORITY) {
    role_cb->task.flags = 0;
    role_cb->task.flags |= ACL_HIGH_PRIORITY_FLAGS;
  }

  /*Checking if there is enough time to serve at least the minimum runtime*/
  if (role_cb->task.status_flags & INVALID_TASK) {
    clear_role(role_cb);
    role_cb->task.status_flags &= ~INVALID_TASK;
  } else {
    task->status_flags &= ~REMOVE_TASK;
    if ((role_cb->usched_private_data.usched_acl_params.is_tx_pending) || (role_cb->usched_private_data.usched_acl_params.is_rx_pending)) {
      acl_start_tsf = usch_GetTimeCB();
      role_cb->task.minRuntime = CONNECTED_HIGH_PRIORITY_SLOTS;
      role_cb->task.maxRuntime = get_max_runtime_based_on_pending_packets(role_cb);
      role_cb->task.maxRuntime = MAX(role_cb->task.minRuntime, role_cb->task.maxRuntime);
    } else {
      if (role_cb->usched_private_data.usched_acl_params.acl_priority == ACL_LOW_PRIORITY) {
        acl_start_tsf = usch_GetTimeCB() + role_cb->usched_private_data.usched_acl_params.poll_interval;
      } else { /*If ACL_HIGH_PRIORITY*/
        acl_start_tsf = usch_GetTimeCB();
      }
      role_cb->task.minRuntime = CONNECTED_NORMAL_PRIORITY_SLOTS;
    }
    if (acl_runtime_stats[conn_index].last_successful_rx_tsf < role_cb->task.start) {
      role_cb->task.priority  = usched_increase_priority(role_cb->task.priority, role_cb->end_priority, role_cb->priority_step_size);
      role_cb->task.minRuntime = CONNECTED_HIGH_PRIORITY_SLOTS;
      role_cb->task.maxRuntime = MAX(role_cb->task.minRuntime, role_cb->task.maxRuntime);
    } else {
      role_cb->task.priority  = role_cb->start_priority;
      /* xxxx Might end up overwriting above priority/window bump ups - ToBeDiscussed*/
      //role_cb->task.minRuntime = CONNECTED_NORMAL_PRIORITY_SLOTS;
    }
    if (is_fail) {
      acl_start_tsf += generate_random(MIN_RANDOM_VALUE, MAX_RANDOM_VALUE);
    }
    role_cb->end_tsf = acl_start_tsf + role_cb->task.maxRuntime;
    start = find_white_spaces(acl_start_tsf,
                              (role_cb->end_tsf),
                              (role_cb->task.minRuntime),
                              (role_cb->task.maxRuntime),
                              0,
                              &role_cb->task);
    role_cb->task.start = start;
    role_cb->task.next  = NULL;
    usch_AddTask(task);
  }
  BTC_PRINT("Time: %d, acl_peripheral_reschedule\n", usch_GetTimeCB());
}

/**
 * @brief this function is used to initialize acl
 *
 * @param scheduler_info
 * @param usched_acl_params
 */
void usched_acl_start_handler(void *arg1, void *arg2)
{
  (void)arg1;
  BTC_PRINT("Time: %d, usched_acl_start_handler\n", usch_GetTimeCB());
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *acl_role_cb         = (role_cb_t *)task->user_data;

  acl_role_cb->usched_private_data.usched_acl_params.duration = acl_role_cb->end_tsf;
  lpw_start_acl_from_usched((usched_acl_params_t *)&acl_role_cb->usched_private_data.usched_acl_params);
}

/**
 * @brief Callback handler for acl stop triggered by uscheduler xxxx ToDo
 */
void usched_acl_stop_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task                = (usch_Task_t *)arg2;
  role_cb_t *acl_role_cb               = (role_cb_t *)task->user_data;
  // ToDo xxxx indicate lower layer to stop?

  lpw_stop_acl_from_usched((usched_acl_params_t *)&acl_role_cb->usched_private_data.usched_acl_params);
  BTC_PRINT("! Time: %d, usched_acl_stop_handler\n", usch_GetTimeCB());
}

/**
 * @brief Callback handler for acl  schedule fail triggered by uscheduler
 */
void usched_acl_fail_handler(void *arg1, void *arg2)
{
  (void)arg1;
  usch_Task_t *task = (usch_Task_t *)arg2;
  role_cb_t *acl_role_cb = (role_cb_t *)task->user_data;
  acl_role_cb->task.priority = usched_increase_priority(acl_role_cb->task.priority, acl_role_cb->end_priority, acl_role_cb->priority_step_size);

  if (acl_role_cb->usched_private_data.usched_acl_params.role == ACL_CENTRAL) {
    acl_central_reschedule(task, BT_TRUE);
  } else if (acl_role_cb->usched_private_data.usched_acl_params.role == ACL_PERIPHERAL) {
    acl_peripheral_reschedule(task, BT_TRUE);
  } else {
    return;
  }
  BTC_PRINT("! Time: %d, usched_acl_fail_handler\n", usch_GetTimeCB());
}
/**
 * @brief Callback handler for acl stop triggered by host
 *
 * @param scheduler_info
 */
void usched_acl_role_stop(uscheduler_info_t *scheduler_info, uint32_t conn_info_index)
{
  if (!scheduler_info->connection_role_cb[conn_info_index].occupied) {
    BTC_PRINT("Invalid conn_info_index\n");
    return;
  }
  role_cb_t *acl_role_cb = &scheduler_info->connection_role_cb[conn_info_index];
  acl_role_cb->end_tsf = usch_GetTimeCB();
  acl_role_cb->task.status_flags |= INVALID_TASK;
  lpw_stop_acl_from_usched((usched_acl_params_t *)&acl_role_cb->usched_private_data.usched_acl_params);
  /* xxxx ToDo - do we call LPW here instead?*/
}

/**
 * @brief Final acl end handler, called after acknowledgment from lpw that role is stopped
 * xxxx ToDo Reschedule based on connection info index??? Why do we need roles as this function
 * is called on the basis of connection info and not role
 */
void lpw_acl_stopped(uscheduler_info_t *scheduler_info, uint32_t role)
{
  BTC_PRINT("! Time: %d, usched_acl_stopped_handler\n", usch_GetTimeCB());
  // reschedule based on role
  role_cb_t *acl_role_cb = &scheduler_info->connection_role_cb[role];

  uint32_t conn_info_index = acl_role_cb->usched_private_data.usched_acl_params.conn_info_index;
  role_cb_t *target_role_cb = &scheduler_info->connection_role_cb[conn_info_index];
  if (target_role_cb->usched_private_data.usched_acl_params.role == ACL_CENTRAL) {
    acl_central_reschedule(&acl_role_cb->task, BT_FALSE);
  } else if (target_role_cb->usched_private_data.usched_acl_params.role == ACL_PERIPHERAL) {
    acl_peripheral_reschedule(&acl_role_cb->task, BT_FALSE);
  } else {
    return;
  }
}

/*Not being used for now, can be leveraged in other layer to find conn_info_index and the next free role*/
/**
 * @brief this function is used to get the next free acl_role_cb and also update the connection info index
 *
 * @param scheduler_info
 * @param idx
 */
role_cb_t *get_next_free_acl_role_cb(uscheduler_info_t *scheduler_info, uint32_t *conn_info_index)
{
  uint32_t idx;
  for (idx = 0; idx < MAX_ACL_CONNECTIONS_SUPPORTED; idx++) {
    if (scheduler_info->connection_role_cb[idx].occupied != ROLE_OCCUPIED) {
      role_cb_t *temp_acl_role_cb = &scheduler_info->connection_role_cb[idx];
      *conn_info_index = idx; //update connection info index
      return temp_acl_role_cb;
    }
  }
  BTC_PRINT("Time: %d, get_next_free_acl_role_cb\n", usch_GetTimeCB());
  //ToDo throw assert, max connections reached
  return NULL;
}
