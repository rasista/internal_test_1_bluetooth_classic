#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "usched_port.h"
#include "procedure_role_autogen.h"

#ifndef _USCHED_COMMON_ROLE_H_
#define _USCHED_COMMON_ROLE_H_

#define MAX(x, y) ((x) > (y) ? (x) : (y)) // xxxx ToDo verify if this is ok

#define BT_SLOT_WIDTH 625

#define BT_TRUE 1
#define BT_FALSE 0
/*ToDo:Check the role macros */
#define INVALID_ROLE -1
#define INQUIRY_ROLE 0
#define PAGE_ROLE 1
#define INQUIRY_SCAN_ROLE 2
#define PAGE_SCAN_ROLE 3
#define NON_ACL_ROLES_COUNT 4
#define TIMER_ROLE 10

#define MAX_ACL_CONNECTIONS_SUPPORTED 3

#define INQ_SCAN_IS_SET  BIT(0)
#define PAGE_SCAN_IS_SET BIT(1)

enum procedure_params {
  INQUIRY_START_PRIORITY = 245,
  INQUIRY_END_PRIORITY = 200,
  INQUIRY_MAX_RUN_TIME = (16 * 625), // 14 SLOTS
  INQUIRY_MIN_RUN_TIME = (16 * 625), // 16 SLOTS
  USCH_PRIORITY_STEP = 5,
  INQUIRY_SCAN_START_PRIORITY = 240,
  INQUIRY_SCAN_END_PRIORITY = 210,
  PAGE_SCAN_START_PRIORITY = 180,
  PAGE_SCAN_END_PRIORITY = 160,
  PAGE_START_PRIORITY = 205,
  PAGE_END_PRIORITY = 170,
  PAGE_MAX_RUN_TIME = (16 * 625),
  PAGE_MIN_RUN_TIME = (16 * 625),
};
typedef struct {
  uint32_t non_connected_role_start_pending;
  uint32_t non_connected_role_stop_pending;
  uint32_t acl_role_start_pending;
  uint32_t acl_role_stop_pending;
} role_pending_t;

typedef union usched_private_data_s {
  usched_pi_params_t usched_pi_params;
  usched_pi_scan_params_t usched_pi_scan_params;
  usched_acl_params_t usched_acl_params;
} usched_private_data_t;

typedef struct role_cb_s {
  uint32_t end_tsf;
  int16_t start_priority;
  int16_t end_priority;
#define ROLE_AVAILABLE 0
#define ROLE_OCCUPIED  1
  usch_Task_t task;
  uint32_t priority_step_size;
  usched_private_data_t usched_private_data;
  uint32_t occupied;
  void (*callback_function)(usch_Task_t *task, enum usch_taskEvent);
} role_cb_t;

typedef struct uscheduler_info_s {
  role_cb_t inquiry_role_cb;
  role_cb_t inquiry_scan_role_cb;
  role_cb_t page_role_cb;
  role_cb_t page_scan_role_cb;
  role_cb_t connection_role_cb[MAX_ACL_CONNECTIONS_SUPPORTED];
} uscheduler_info_t;

//ToDo should be moved to other layer
/*Shared structure between Host and LPW*/
typedef struct acl_runtime_stats_s{
  uint32_t last_successful_rx_tsf;
} acl_runtime_stats_t;

typedef struct usched_handler_entry_s {
  void (*handler)(void *arg1, void *arg2);
} usched_handler_entry_t;

extern uscheduler_info_t scheduler_info;
extern acl_runtime_stats_t acl_runtime_stats[MAX_ACL_CONNECTIONS_SUPPORTED];

void clear_role(role_cb_t *role_cb);
uint32_t usched_increase_priority(uint32_t curr_priority, uint32_t end_priority, uint32_t step_size);
bool check_task_end_based_on_time(uint32_t min_runtime, uint32_t end_tsf, uint32_t curr_time);
uint32_t time_diff(uint32_t curr_time, uint32_t end_time);
#endif
