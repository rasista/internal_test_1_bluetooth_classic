#ifndef USCHED_PORT_H
#define USCHED_PORT_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "usched.h"
#include "debug.h"
#define BIT(x) (1 << x)

#ifdef SIM
extern uint32_t glbl_sim_sys_tick;
#endif
enum usch_role {
  usch_inq_role,
  usch_page_role,
  usch_inq_scan_role,
  usch_page_scan_role,
};

struct usch_trace_stats_s {
  enum usch_taskEvent event;
  enum usch_role role_id;
  uint32_t tsf;
};

#define USCH_TRACE_MAX_STATS 25

uint8_t bg_bit_isset(uint32_t flags, uint8_t value);
void bg_bit_set(uint32_t *flags, uint8_t value);
void bg_bit_clr(uint32_t *flags, uint8_t value);
void usch_log_event_to_task(enum usch_taskEvent event, enum usch_role role_id);
void usch_sendEventToTask(uint8_t event, struct usch_Task *task);
bool usch_timeGreaterThan(uint32_t time1, uint32_t time2);
void bg_bit_set_atomic(uint32_t *flags, uint8_t value);
void bg_bit_clr_atomic(uint32_t *flags, uint8_t value);
uint16_t ll_execTimingGetCurrentValue(exec_timing_t t);
uint32_t usch_GetTimeCB(void);
int32_t BG_ASSERT(uint32_t task);
void usch_ScheduleReqCB(void);
#endif
