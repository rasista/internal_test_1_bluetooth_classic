#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "usched_intf.h"
#include "usched_pi.h"
#include "usched_pi_scan.h"
#include "simulation_interface.h"
#include "usched_common_role.h"
#include "usched_port.h"
#include "event_loop.h"
#include "usched_acl.h"
/**
 * @brief All the handlers below take the input_buffer from host and then fill
 * the cmd_id and cmd_type in the payload in the buffer on the basis of which
 * further start or stop handlers of respective state machines are called.
 *
 * @param payload
 * @return void*
 */

uscheduler_info_t scheduler_info;
extern uint32_t timer_invoke_count;
extern uint8_t timer_role;
event_loop_t event_loop_g;
role_pending_t pending_roles;

void usched_init_handler(int argc, char *argv[])
{
  printf("inside usched_init_handler\n");
  pending_roles.non_connected_role_start_pending = INVALID_ROLE;
  pending_roles.non_connected_role_stop_pending = INVALID_ROLE;
  pending_roles.acl_role_start_pending = INVALID_ROLE;
  pending_roles.acl_role_stop_pending = INVALID_ROLE;
  btc_event_loop_init(&event_loop_g);
  btc_event_loop_register_event(BTC_EVENT_SCHEDULER_ACTIVITY_PENDING, scheduler_activity_pending_handler, &event_loop_g);
}

/**
 * @brief start pi command handler from host, calls the fw function for page or inquiry init
 *
 * @param argc
 * @param argv
 */
void start_pi_handler(int argc, char *argv[])
{
  char c_in_payload[1024];
  int i = 0;
  int offset = 0;
  usched_api_start_pi_t usched_pi_params;
  for (i = 0; i < argc; i++) {
    if (offset + strlen(argv[i]) >= 1024) {
      printf("Exceeded maximum payload size.\n");
      return;
    }
    strcpy(c_in_payload + offset, argv[i]);
    offset += strlen(argv[i]);
    if (i < argc - 1) {
      offset++;
    }
  }
  c_in_payload[offset] = '\0';
  usched_api_command_parser((uint8_t *)&usched_pi_params, (uint8_t *)c_in_payload);
  pi_role_initialization(&scheduler_info, (usched_pi_params_t *)&usched_pi_params);

  int8_t cmd_buffer[1024] = { 0 };
  //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
    snprintf((char *)cmd_buffer, 1023, "timer start_timer %d %d\n", timer_invoke_count, usched_pi_params.role);
    cmd_response((uint8_t *)cmd_buffer);
  }

  btc_event_loop(&event_loop_g);
}

/**
 * @brief stop pi command handler from host, calls the fw function for page or inquiry stop
 */
void stop_pi_handler(int argc, char *argv[])
{
  char c_in_payload[1024];
  int i = 0;
  int offset = 0;
  usched_api_stop_pi_t usched_stop_pi_params;
  for (i = 0; i < argc; i++) {
    if (offset + strlen(argv[i]) >= 1024) {
      printf("Exceeded maximum payload size.\n");
      return;
    }
    strcpy(c_in_payload + offset, argv[i]);
    offset += strlen(argv[i]);
    if (i < argc - 1) {
      offset++;
    }
  }
  c_in_payload[offset] = '\0';
  usched_api_command_parser((uint8_t *)&usched_stop_pi_params, (uint8_t *)c_in_payload);
  usched_pi_role_stop(&scheduler_info, usched_stop_pi_params.role);

  // int8_t cmd_buffer[1024] = { 0 };
  // //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  // if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
  //   snprintf((char *)cmd_buffer, 1023, "timer stop_timer %d %d\n", timer_invoke_count, usched_stop_pi_params.role);
  //   cmd_response((uint8_t *)cmd_buffer);
  // }

  btc_event_loop(&event_loop_g);
}

/**
 * @brief start pi scan command handler from host, calls the fw function for pi scan init
 *
 * @param argc
 * @param argv
 */
void start_pi_scan_handler(int argc, char *argv[])
{
  printf("Starting from intf");
  char c_in_payload[1024];
  int i = 0;
  int offset = 0;
  usched_api_start_pi_scan_t usched_pi_scan_params;
  for (i = 0; i < argc; i++) {
    if (offset + strlen(argv[i]) >= 1024) {
      printf("Exceeded maximum payload size.\n");
      return;
    }
    strcpy(c_in_payload + offset, argv[i]);
    offset += strlen(argv[i]);
    if (i < argc - 1) {
      offset++;
    }
  }
  c_in_payload[offset] = '\0';
  usched_api_command_parser((uint8_t *)&usched_pi_scan_params, (uint8_t *)c_in_payload);
  pi_scan_role_initialization(&scheduler_info, (usched_pi_scan_params_t *)&usched_pi_scan_params);

  int8_t cmd_buffer[1024] = { 0 };
  //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
    snprintf((char *)cmd_buffer, 1023, "timer start_timer %d %d\n", timer_invoke_count, usched_pi_scan_params.role);
    cmd_response((uint8_t *)cmd_buffer);
  }

  btc_event_loop(&event_loop_g);
}

/**
 * @brief stop pi scan command handler from host, calls the fw function for page scan/inquiry scan stop
 */
void stop_pi_scan_handler(int argc, char *argv[])
{
  char c_in_payload[1024];
  int i = 0;
  int offset = 0;
  usched_api_stop_pi_scan_t usched_stop_pi_scan_params;
  for (i = 0; i < argc; i++) {
    if (offset + strlen(argv[i]) >= 1024) {
      printf("Exceeded maximum payload size.\n");
      return;
    }
    strcpy(c_in_payload + offset, argv[i]);
    offset += strlen(argv[i]);
    if (i < argc - 1) {
      offset++;
    }
  }
  c_in_payload[offset] = '\0';
  usched_api_command_parser((uint8_t *)&usched_stop_pi_scan_params, (uint8_t *)c_in_payload);
  usched_pi_scan_role_stop(&scheduler_info, usched_stop_pi_scan_params.role);

  // int8_t cmd_buffer[1024] = { 0 };
  //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  // if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
  //   snprintf((char *)cmd_buffer, 1023, "timer stop_timer %d\n", timer_invoke_count);
  //   cmd_response((uint8_t *)cmd_buffer);
  // }

  btc_event_loop(&event_loop_g);
}

/**
 * @brief start pi scan command handler from host, calls the fw function for acl init
 *
 * @param argc
 * @param argv
 */
void start_acl_handler(int argc, char *argv[])
{
  printf("\n\n%d Starting from intf\n\n", usch_GetTimeCB());
  char c_in_payload[1024];
  int i = 0;
  int offset = 0;
  usched_api_start_acl_t usched_api_start_acl_params;
  for (i = 0; i < argc; i++) {
    if (offset + strlen(argv[i]) >= 1024) {
      printf("Exceeded maximum payload size.\n");
      return;
    }
    strcpy(c_in_payload + offset, argv[i]);
    offset += strlen(argv[i]);
    if (i < argc - 1) {
      offset++;
    }
  }
  c_in_payload[offset] = '\0';
  usched_api_command_parser((uint8_t *)&usched_api_start_acl_params, (uint8_t *)c_in_payload);
  acl_role_initialization(&scheduler_info, (usched_acl_params_t *)&usched_api_start_acl_params);

  int8_t cmd_buffer[1024] = { 0 };
  //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
    snprintf((char *)cmd_buffer, 1023, "timer start_timer %d %d\n", timer_invoke_count, usched_api_start_acl_params.role);
    cmd_response((uint8_t *)cmd_buffer);
  }

  btc_event_loop(&event_loop_g);
}

/**
 * @brief stop pi scan command handler from host, calls the fw function for acl stop
 */
void stop_acl_handler(int argc, char *argv[])
{
  char c_in_payload[1024];
  int i = 0;
  int offset = 0;
  usched_api_stop_acl_t usched_api_stop_acl_params;
  for (i = 0; i < argc; i++) {
    if (offset + strlen(argv[i]) >= 1024) {
      printf("Exceeded maximum payload size.\n");
      return;
    }
    strcpy(c_in_payload + offset, argv[i]);
    offset += strlen(argv[i]);
    if (i < argc - 1) {
      offset++;
    }
  }
  c_in_payload[offset] = '\0';
  usched_api_command_parser((uint8_t *)&usched_api_stop_acl_params, (uint8_t *)c_in_payload);
  usched_acl_role_stop(&scheduler_info, usched_api_stop_acl_params.conn_info_index);

  // int8_t cmd_buffer[1024] = { 0 };
  // //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  // if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
  //   snprintf((char *)cmd_buffer, 1023, "timer stop_timer %d\n", timer_invoke_count);
  //   cmd_response((uint8_t *)cmd_buffer);
  // }

  btc_event_loop(&event_loop_g);
}

void lpw_stop_callback_handler(int argc, char *argv[])
{
  //role decides what to stop based on the role and call stop handler ushed_pi_scan or ushed_pi file accordingly
  uint32_t role = atoi(argv[0]);
  if (role == 0) {
    lpw_pi_stopped(&scheduler_info, role);
  } else if (role == 1) {
    lpw_pi_stopped(&scheduler_info, role);
  } else if (role == 2) {
    lpw_pi_scan_stopped(&scheduler_info, role);
  } else if (role == 3) {
    lpw_pi_scan_stopped(&scheduler_info, role);
  } else {
    printf("Invalid role.\n");
    return;
  }

  int8_t cmd_buffer[1024] = { 0 };
  //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
    snprintf((char *)cmd_buffer, 1023, "timer start_timer %d %d\n", timer_invoke_count, role);
    cmd_response((uint8_t *)cmd_buffer);
  }

  btc_event_loop(&event_loop_g);
}

void lpw_stop_acl_callback_handler(int argc, char *argv[])
{
  uint32_t role = atoi(argv[0]);
  printf("inside lpw_stop_acl_callback_handler role: %d\n", role);
  lpw_acl_stopped(&scheduler_info, role);

  int8_t cmd_buffer[1024] = { 0 };
  //if USCH_FLAG_PENDING set, then start timer by sending a command to lpw with start_tsf
  if (bg_bit_isset(usch.flags, USCH_FLAG_PENDING)) {
    snprintf((char *)cmd_buffer, 1023, "timer start_timer %d %d\n", timer_invoke_count, role);
    cmd_response((uint8_t *)cmd_buffer);
  }

  btc_event_loop(&event_loop_g);
}

void timer_stop_callback_handler(void)
{
  usch_future_task();
  btc_event_loop(&event_loop_g);
}

void timer_expire_callback_handler(void)
{
  usch_remove_future_task();
  btc_event_loop(&event_loop_g);
}

/**
 * @brief scheduler activity pending handler, calls the fw function for pi stop or start
 *
 * @param usched_pi_params
 */
void scheduler_activity_pending_handler(void *param)
{
  printf("inside scheduler_activity_pending_handler\n");
  int8_t cmd_buffer[1024] = { 0 };
  // int32_t role = pending_roles.non_connected_role_stop_pending;
  int32_t connected_role = pending_roles.acl_role_stop_pending;
  // usched_pi_params_t *usched_pi_params = NULL;
  // usched_pi_scan_params_t *usched_pi_scan_params = NULL;
  usched_acl_params_t *usched_acl_params = NULL;
  // if (role != INVALID_ROLE) {
  //   if (role == INQUIRY_ROLE || role == PAGE_ROLE) {
  //     usched_pi_params = role == INQUIRY_ROLE ? (usched_pi_params_t *)&scheduler_info.inquiry_role_cb.usched_private_data.usched_pi_params : (usched_pi_params_t *)&scheduler_info.page_role_cb.usched_private_data.usched_pi_params;
  //     if (role == timer_role) {
  //       snprintf((char *)cmd_buffer, 1023, "timer timer_expired %d %d\n", timer_invoke_count, role);
  //       cmd_response((uint8_t *)cmd_buffer);
  //     }
  //     snprintf((char *)cmd_buffer, 1023, "btc_llc_interface_%d stop_procedure %d %d %d\n",
  //              pending_roles.non_connected_role_stop_pending + 1,
  //              usched_pi_params->end_tsf,
  //              pending_roles.non_connected_role_stop_pending,
  //              usched_pi_params->max_runtime);
  //     cmd_response((uint8_t *)cmd_buffer);
  //     pending_roles.non_connected_role_stop_pending = INVALID_ROLE;
  //     printf("cmd_buffer_stop: %s\n", cmd_buffer);
  //   } else if (role == PAGE_SCAN_ROLE || role == INQUIRY_SCAN_ROLE) {
  //     usched_pi_scan_params = role == PAGE_SCAN_ROLE ? (usched_pi_scan_params_t *)&scheduler_info.page_scan_role_cb.usched_private_data.usched_pi_scan_params : (usched_pi_scan_params_t *)&scheduler_info.inquiry_scan_role_cb.usched_private_data.usched_pi_scan_params;
  //     if (role == timer_role) {
  //       snprintf((char *)cmd_buffer, 1023, "timer timer_expired %d %d\n", timer_invoke_count, role);
  //       cmd_response((uint8_t *)cmd_buffer);
  //     }

  //     snprintf((char *)cmd_buffer, 1023, "btc_llc_interface_%d stop_procedure %d %d %d\n",
  //              pending_roles.non_connected_role_stop_pending + 1,
  //              usched_pi_scan_params->pi_scan_window + usch_GetTimeCB(),
  //              pending_roles.non_connected_role_stop_pending,
  //              usched_pi_scan_params->pi_scan_window + usch_GetTimeCB());
  //     cmd_response((uint8_t *)cmd_buffer);
  //     pending_roles.non_connected_role_stop_pending = INVALID_ROLE;
  //   } else {
  //     printf("Invalid role.\n");
  //     while (1);
  //   }
  // }

  // memset(cmd_buffer, 0, 1024);
  // role = pending_roles.non_connected_role_start_pending;
  // if (role != INVALID_ROLE) {
  //   if (role == INQUIRY_ROLE || role == PAGE_ROLE) {
  //     usched_pi_params = role == INQUIRY_ROLE ? (usched_pi_params_t *)&scheduler_info.inquiry_role_cb.usched_private_data.usched_pi_params : (usched_pi_params_t *)&scheduler_info.page_role_cb.usched_private_data.usched_pi_params;
  //     snprintf((char *)cmd_buffer, 1023, "btc_llc_interface_%d start_procedure %d %d %d\n",
  //              pending_roles.non_connected_role_start_pending + 1,
  //              usched_pi_params->end_tsf,
  //              pending_roles.non_connected_role_start_pending,
  //              usched_pi_params->max_runtime);
  //     cmd_response((uint8_t *)cmd_buffer);
  //     pending_roles.non_connected_role_start_pending = INVALID_ROLE;
  //   } else if (role == PAGE_SCAN_ROLE || role == INQUIRY_SCAN_ROLE) {
  //     usched_pi_scan_params = role == PAGE_SCAN_ROLE ? (usched_pi_scan_params_t *)&scheduler_info.page_scan_role_cb.usched_private_data.usched_pi_scan_params : (usched_pi_scan_params_t *)&scheduler_info.inquiry_scan_role_cb.usched_private_data.usched_pi_scan_params;
  //     snprintf((char *)cmd_buffer, 1023, "btc_llc_interface_%d start_procedure %d %d %d\n",
  //              pending_roles.non_connected_role_start_pending + 1,
  //              usched_pi_scan_params->pi_scan_window + usch_GetTimeCB(),
  //              pending_roles.non_connected_role_start_pending,
  //              usched_pi_scan_params->pi_scan_window + usch_GetTimeCB());
  //     cmd_response((uint8_t *)cmd_buffer);
  //     pending_roles.non_connected_role_start_pending = INVALID_ROLE;
  //   } else {
  //     printf("Invalid role.\n");
  //     while (1);
  //   }
  // }

  memset(cmd_buffer, 0, 1024);
  if (connected_role != INVALID_ROLE) {
    usched_acl_params = (usched_acl_params_t *)&scheduler_info.connection_role_cb[connected_role].usched_private_data.usched_acl_params;
    snprintf((char *)cmd_buffer, 1023, "btc_llc_interface_%d stop_procedure %d %d %d\n",
             pending_roles.acl_role_stop_pending + 1,
             usched_acl_params->duration,
             pending_roles.acl_role_stop_pending,
             usched_acl_params->max_runtime);
    cmd_response((uint8_t *)cmd_buffer);
    pending_roles.acl_role_stop_pending = INVALID_ROLE;
    printf("cmd_buffer_stop: %s\n", cmd_buffer);
  }

  memset(cmd_buffer, 0, 1024);
  connected_role = pending_roles.acl_role_start_pending;
  if (connected_role != INVALID_ROLE) {
    // usched_pi_params = (usched_pi_params_t *)&scheduler_info.inquiry_role_cb.usched_private_data.usched_pi_params;
    usched_acl_params = (usched_acl_params_t *)&scheduler_info.connection_role_cb[connected_role].usched_private_data.usched_acl_params;
    snprintf((char *)cmd_buffer, 1023, "btc_llc_interface_%d start_procedure %d %d %d\n",
             pending_roles.acl_role_start_pending + 1,
             usched_acl_params->duration,
             pending_roles.acl_role_start_pending,
             usched_acl_params->max_runtime);
    cmd_response((uint8_t *)cmd_buffer);
    pending_roles.acl_role_start_pending = INVALID_ROLE;
    printf("cmd_buffer_start: %s\n", cmd_buffer);
  }

  btc_event_loop_clear_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}

/*LPW Apis*/
/*Incomplete, to be completed*/
/**
 * @brief start inquiry command from usched sent to lpw
 *
 * @param lpw_start_inquiry
 */
void lpw_start_pi_from_usched(usched_pi_params_t *usched_pi_params)
{
  printf("%s for %d\n", __FUNCTION__, usched_pi_params->role);

  // Check if any start is pending
  if (pending_roles.non_connected_role_start_pending != INVALID_ROLE) {
    printf("Can't start, role already started  %d\n", pending_roles.non_connected_role_start_pending);
  }
  // Check if stop of same type is pending, then reset the stop pending and set pending start
  else if (pending_roles.non_connected_role_stop_pending == pending_roles.non_connected_role_start_pending) {
    printf("Role stop for self is pending, reset it\n");
    pending_roles.non_connected_role_stop_pending = INVALID_ROLE;
  }
  printf("Setting role start pending for %d\n", usched_pi_params->role);
  pending_roles.non_connected_role_start_pending = usched_pi_params->role;
  btc_event_loop_set_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}
/*xxxx ToDo create a new pythong - btc_llc_interface.py -> copy rail inits, create start_procedure_handler
   -> starts a timer for xyz time

 */
/**
 * @brief stop inquiry command handler, tells the lpw to stop inquiry
 */
void lpw_stop_pi_from_usched(usched_pi_params_t *usched_pi_params)
{
  printf("%s for %d\n", __FUNCTION__, usched_pi_params->role);
  //!If we are trying to stop a role, we expect the start for either the same role to be pending
  //! or no start to be pending
  if ((pending_roles.non_connected_role_start_pending != usched_pi_params->role)
      && (pending_roles.non_connected_role_start_pending != INVALID_ROLE)) {
    printf("Trying to Stop role %d, while %d is already running\n", usched_pi_params->role, pending_roles.non_connected_role_start_pending);
    while (1);
  } else {
    // if start of same role is pending, then clear
    if (pending_roles.non_connected_role_start_pending == usched_pi_params->role) {
      printf("Start for this role was previously pending, reseting it now\n");
      pending_roles.non_connected_role_start_pending = INVALID_ROLE;
    }
  }
  printf("Setting role stop pending for hi %d\n", usched_pi_params->role);
  pending_roles.non_connected_role_stop_pending = usched_pi_params->role;
  btc_event_loop_set_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}

/**
 * @brief start inquiry scan command from usched sent to lpw
 *
 * @param lpw_start_inquiry_scan
 */
void lpw_start_pi_scan_from_usched(usched_pi_scan_params_t *usched_pi_scan_params)
{
  printf("%s for %d\n", __FUNCTION__, usched_pi_scan_params->role);
  // Check if any start is pending
  if (pending_roles.non_connected_role_start_pending != INVALID_ROLE) {
    printf("Can't start, role already started  %d\n", pending_roles.non_connected_role_start_pending);
  }
  // Check if stop of same type is pending, then reset the stop pending and set pending start
  else if (pending_roles.non_connected_role_stop_pending == pending_roles.non_connected_role_start_pending) {
    printf("Role stop for self is pending, reset it\n");
    pending_roles.non_connected_role_stop_pending = INVALID_ROLE;
  }
  printf("Setting role start pending for %d\n", usched_pi_scan_params->role);
  pending_roles.non_connected_role_start_pending = usched_pi_scan_params->role;
  btc_event_loop_set_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}

/**
 * @brief stop inquiry scan command handler, tells the lpw to stop inquiry scan
 */
void lpw_stop_pi_scan_from_usched(usched_pi_scan_params_t *usched_pi_scan_params)
{
  printf("%s for %d\n", __FUNCTION__, usched_pi_scan_params->role);
  //!If we are trying to stop a role, we expect the start for either the same role to be pending
  //! or no start to be pending
  if ((pending_roles.non_connected_role_start_pending != usched_pi_scan_params->role)
      && (pending_roles.non_connected_role_start_pending != INVALID_ROLE)) {
    printf("Trying to Stop role %d, while %d is already running\n", usched_pi_scan_params->role, pending_roles.non_connected_role_start_pending);
    while (1);
  } else {
    // if start of same role is pending, then clear
    if (pending_roles.non_connected_role_start_pending == usched_pi_scan_params->role) {
      printf("Start for this role was previously pending, reseting it now\n");
      pending_roles.non_connected_role_start_pending = INVALID_ROLE;
    }
  }
  printf("Setting role stop pending for hi %d\n", usched_pi_scan_params->role);
  pending_roles.non_connected_role_stop_pending = usched_pi_scan_params->role;
  btc_event_loop_set_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}

/**
 * @brief start acl command from usched sent to lpw
 *
 * @param lpw_start_inquiry
 */
void lpw_start_acl_from_usched(usched_acl_params_t *usched_acl_params)
{
  printf("%s for %d\n", __FUNCTION__, usched_acl_params->conn_info_index);

  // Check if any other start is pending
  if (pending_roles.acl_role_start_pending != INVALID_ROLE && (pending_roles.acl_role_start_pending != usched_acl_params->conn_info_index)) {
    printf("Can't start, role already started  %d\n", pending_roles.acl_role_start_pending);
    while (1);
  }
  // Check if stop of same type is pending, then reset the stop pending and set pending start
  else if (pending_roles.acl_role_stop_pending == usched_acl_params->conn_info_index) {
    printf("Role stop for self is pending, reset it\n");
    pending_roles.acl_role_stop_pending = INVALID_ROLE;
    pending_roles.acl_role_start_pending = usched_acl_params->conn_info_index;
  } else {
    pending_roles.acl_role_start_pending = usched_acl_params->conn_info_index;
  }

  // printf("Setting role start pending for %d\n", usched_acl_params->conn_info_index);
  btc_event_loop_set_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}
/*xxxx ToDo create a new pythong - btc_llc_interface.py -> copy rail inits, create start_procedure_handler
   -> starts a timer for xyz time

 */
/**
 * @brief stop acl command handler, tells the lpw to stop acl
 */
void lpw_stop_acl_from_usched(usched_acl_params_t *usched_acl_params)
{
  printf("%s for %d\n", __FUNCTION__, usched_acl_params->conn_info_index);
  //!If we are trying to stop a role, we expect the start for either the same role to be pending
  //! or no start to be pending
  if ((pending_roles.acl_role_start_pending != usched_acl_params->conn_info_index) && (pending_roles.acl_role_start_pending != INVALID_ROLE)) {
    printf("Trying to Stop role %d, while %d is already running\n", usched_acl_params->conn_info_index, pending_roles.acl_role_start_pending);
    while (1);
  }
  // if start of same role is pending, then clear
  else if (pending_roles.acl_role_start_pending == usched_acl_params->conn_info_index) {
    printf("Start for this role was previously pending, reseting it now\n");
    pending_roles.acl_role_start_pending = INVALID_ROLE;
  } else {
    pending_roles.acl_role_stop_pending = usched_acl_params->conn_info_index;
  }
  // printf("Setting role stop pending for hi %d\n", usched_acl_params->conn_info_index);
  btc_event_loop_set_event(&event_loop_g, BTC_EVENT_SCHEDULER_ACTIVITY_PENDING);
}

/*Stop Apis from LPW*/

/**
 * @brief stop pi sent from lpw, calls the fw function for pi stop
 */
void usched_stop_pi_from_lpw()
{
}

/**
 * @brief stop pi scan sent from lpw, calls the fw function for pi scan stop
 */
void usched_stop_pi_scan_from_lpw()
{
}

/**
 * @brief stop pi scan sent from lpw, calls the fw function for pi scan stop
 */
void usched_stop_acl_from_lpw()
{
}
