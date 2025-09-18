#ifndef USCHED_ACL_H
#define USCHED_ACL_H

#include <stdint.h>
#include "usched_common_role.h"

//new defines
#define ACL_LOW_PRIORITY 0
#define ACL_HIGH_PRIORITY 1

#define ACL_CENTRAL 0
#define ACL_PERIPHERAL 1

#define MIN_RANDOM_VALUE 2000
#define MAX_RANDOM_VALUE 5000

#define CONNECTED_NORMAL_PRIORITY_SLOTS (4 * 625)  // 6 SLOTS
#define CONNECTED_HIGH_PRIORITY_SLOTS (10 * 625)  // 10 SLOTS

#define CONNECTED_FLAGS                        SL_STATUS_SUCCESS
#define ACL_HIGH_PRIORITY_FLAGS                USCH_TASK_FLAG_IMMOVABLE

role_cb_t *get_next_free_acl_role_cb(uscheduler_info_t *scheduler_info, uint32_t *conn_info_index);
uint32_t generate_random(uint32_t lower_end, uint32_t higher_end);
uint32_t get_max_runtime_based_on_pending_packets(role_cb_t *role_cb);
void acl_role_initialization(uscheduler_info_t *scheduler_info, usched_acl_params_t *usched_acl_params);
void usched_acl_role_stop(uscheduler_info_t *scheduler_info, uint32_t conn_info_index);
void usched_acl_event_callback(usch_Task_t *task, enum usch_taskEvent event);
void acl_central_reschedule(usch_Task_t *task, uint32_t is_fail);
void acl_peripheral_reschedule(usch_Task_t *task, uint32_t is_fail);
void usched_acl_start_handler(void *arg1, void *arg2);
void usched_acl_stop_handler(void *arg1, void *arg2);
void usched_acl_fail_handler(void *arg1, void *arg2);
void lpw_acl_stopped(uscheduler_info_t *scheduler_info, uint32_t role);

//xxxx ToDo move to ushed_intf.h
void lpw_start_acl_from_usched (usched_acl_params_t *usched_acl_params);
void lpw_stop_acl_from_usched (usched_acl_params_t *usched_acl_params);

#endif
