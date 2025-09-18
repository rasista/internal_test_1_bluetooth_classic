#ifndef _USCHED_PI_H_
#define _USCHED_PI_H_

#include <stdint.h>
#include "usched_common_role.h"

#define INQUIRY_FLAGS           SL_STATUS_SUCCESS
#define PAGE_FLAGS              SL_STATUS_SUCCESS

void pi_role_initialization(uscheduler_info_t *scheduler_info, usched_pi_params_t *usched_pi_params);
void usched_pi_parameters_init(void *ctx, uint8_t length, uint32_t role);
void pi_reschedule(usch_Task_t *task);
void usched_pi_event_callback(usch_Task_t *task, enum usch_taskEvent event);
void usched_pi_start_handler(void *arg1, void *arg2);
void usched_pi_stop_handler(void *arg1, void *arg2);
void usched_pi_schedule_fail_handler(void *arg1, void *arg2);
void pi_schedule_reschedule(usch_Task_t *task, bool is_finished);
void usched_pi_role_stop(uscheduler_info_t *scheduler_info, uint32_t role);
//xxxx ToDo move to ushed_intf.h
void lpw_start_pi_from_usched (usched_pi_params_t *usched_pi_params);
void lpw_stop_pi_from_usched(usched_pi_params_t *usched_pi_params);
void lpw_pi_stopped(uscheduler_info_t *scheduler_info, uint32_t role);
#endif
