/**
 * @file debug_logs.h
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#ifndef __BTC_DEBUG_LOGS_H__
#define __BTC_DEBUG_LOGS_H__

typedef enum debug_log_type_s {
  DEBUG_GPIO,
  DEBUG_PTI,
  DEBUG_GLOBAL_MEM
} debug_log_type_t;

typedef enum debug_log_evet_s {
  PTI_SCHEDULED_ID_TX_CALL = 65,   // Charater 'A' ASCII value. Debug events are logged from this value onwards.
  PTI_SCHEDULED_ID_RX_CALL, //B
  PTI_SCHEDULED_TX_CALL, //C
  PTI_SCHEDULED_RX_CALL, //D
  PTI_TX_DONE_CB,//E
  PTI_INQUIRY_COMPLETED,//F
  PTI_TX_DONE,//G
  PTI_TX_ABORT,//H
  PTI_RX_DONE_CB,//I
  PTI_RX_TIMEOUT_CB,//J
  PTI_RX_DONE,//K
  PTI_RX_ID_PKT,//L
  PTI_RX_ABORT_HEC_ERROR,//M
  PTI_RX_TIMEOUT, //N
  PTI_SCHEDULED_TX_FHS, // O
  PTI_RX_ABORT, // P
  PTI_INQUIRY_SCAN_COMPLETED, // Q
  PTI_PKT_HDR_EVENT_CB, // R
  PTI_RX_WINDOW_END, // S
  PTI_SCHEDULED_TX_EIR, // T
  PTI_PAGE_COMPLETED, // U
  PTI_PAGE_SCAN_COMPLETED, // V
  PTI_CONNECTION_COMPLETED, // W
  PTI_CONNECTION_FAILED, // X
  PTI_PROCEDURE_ACTIVITY_COMPLETED, // Y
  PTI_ACL_LT_ADDR_MISMATCH // Z
} debug_log_event_t;

void debug_log_event(debug_log_type_t event_type, debug_log_event_t event);

#endif
