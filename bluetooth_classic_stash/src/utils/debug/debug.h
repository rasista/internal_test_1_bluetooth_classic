#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "stdint.h"
#include "stdio.h"

enum BTC_ASSERT_TYPE {
  NULL_POINTER,
  EVENT_LOOP_UNREGISTERED_HANDLER,
  EVENT_LOOP_ERR_OVERWRITING_REGISTERED_EVENT,
  EVENT_LOOP_MAX_NUM_EVENTS_CROSSED_IN_REGISTER_EVENTS,
  HCI_UNSUPPORTED_COMMAND,
  INCORRECT_HCI_PACKET_TYPE,
  INVALID_PARAM,
  QUEUE_EMPTY_ERROR,
  ADHOC_ABORT,
  INVALID_INPUT,
  IMPLEMENTATION_PENDING,
  BT_UNEXPECTED_HCI_EVENT_FLT_TYPE_RCVD,
  CODE_PENDING,
  FRAG_SELECTION_FAILED,
  CHECK_BITMAP,
  PERIODIC_QUEUE_EMPTY,
  PERIODIC_ROLE_NOT_FOUND,
  INVALID_CMD_TYPE,
  INVALID_CMD_ID,
  INVALID_CMD_PROCEDURE,
  BT_INVALID_PTT,
  NO_LT_ADDR,
  AFH_NOT_ENABLED,
  THIS_SHOULD_NEVER_HAPPEN_SINCE_THIS_PACKET_SHOULD_NOT_BE_FORMED,
  UNKOWN_SLOT_TYPE,
  DMA_TRANSFER_FAILED,
  AUTO_RATE_NOT_ENABLED
};

void btc_assert(enum BTC_ASSERT_TYPE msg, const char *file, int line);
void btc_print(const char *fmt, ...);
void btc_hex_dump(char *msg, uint8_t *bytes, uint16_t len);
//These functions are used in log_to_vcd
#ifdef LINUX_UTILS
#include "stdio.h"
#define BTC_PRINT(a, ...) btc_print(a, ##__VA_ARGS__)
#define BTC_SIM_PRINT(...) BTC_PRINT("%d\t", glbl_sim_sys_tick); BTC_PRINT(__VA_ARGS__)
#define BTC_ASSERT(msg) btc_assert(msg, __FILE__, __LINE__)
#define BTC_HEX_DUMP(msg, bytes, len) btc_hex_dump(msg, bytes, len);
#else
#define BTC_PRINT(...) while (0)
#define BTC_SIM_PRINT(...) while (0)
#define BTC_ASSERT(msg) while (0)
#define BTC_HEX_DUMP(msg, bytes, len) while (0);
#endif
void debug_init(void);
#endif
