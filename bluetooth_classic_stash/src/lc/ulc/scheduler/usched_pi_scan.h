#ifndef _USCHED_PI_SCAN_H_
#define _USCHED_PI_SCAN_H_

#include <stdint.h>
#include "usched_common_role.h"

#define INQUIRY_SCAN_FLAGS          USCH_TASK_FLAG_IMMOVABLE
#define PAGE_SCAN_FLAGS             USCH_TASK_FLAG_IMMOVABLE

/* Scan Parameters  and variable is used whether scanning should be done after disconnection*/
#define NO_SCAN_EN 0
#define INQUIRY_SCAN_EN 1
#define PAGE_SCAN_EN 2
#define INQ_PAGE_SCAN_EN 3

void pi_scan_role_initialization(uscheduler_info_t *scheduler_info, usched_pi_scan_params_t *usched_pi_scan_params);
void usched_pi_scan_parameters_init(void *ctx, uint32_t window, uint32_t interval, uint32_t role);
void usched_pi_scan_parameters_deinit(void *ctx, uint32_t role);
void pi_scan_reschedule(usched_pi_scan_params_t *pi_scan_params, usch_Task_t *task);
void usched_pi_scan_event_callback(usch_Task_t *task, enum usch_taskEvent event);
void usched_pi_scan_start_handler(void *arg1, void *arg2);
void usched_pi_scan_stop_handler(void *arg1, void *arg2);
void usched_pi_scan_schedule_fail_handler(void *arg1, void *arg2);
void usched_pi_scan_role_stop(uscheduler_info_t *scheduler_info, uint32_t role);
// xxxx ToDo move to ushed_intf.h
void lpw_start_pi_scan_from_usched(usched_pi_scan_params_t *usched_pi_scan_params);
void lpw_stop_pi_scan_from_usched(usched_pi_scan_params_t *usched_pi_scan_params);
void lpw_pi_scan_stopped(uscheduler_info_t *scheduler_info, uint32_t role);
#endif
