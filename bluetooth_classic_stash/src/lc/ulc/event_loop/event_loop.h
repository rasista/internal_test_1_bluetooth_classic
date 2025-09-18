#ifndef _EVENT_LOOP_H__
#define _EVENT_LOOP_H__

#include "stddef.h"
#include "stdint.h"
#include "string.h"
#include "debug.h"
#include "btc_plt_deps.h"

#define BIT(x)  (1 << x)

enum btc_event_priorities {
  BTC_EVENT_HCI_FROM_HOST_PENDING = 0,
  BTC_EVENT_HCI_EVENT_TO_HOST_PENDING,
  BTC_EVENT_HCI_EVENT_TO_HOST_SENT,
  BTC_EVENT_SCHEDULER_ACTIVITY_PENDING,
  BTC_EVENT_LPW_NOTIFICATION_PENDING_HANDLER,
  BTC_EVENT_LPW_RX_DONE,
#ifdef SCHED_SIM
  BTC_EVENT_VIRTUAL_TX_DONE,
#endif
  BTC_EVENT_MAX_NUM_EVENTS,
};

typedef struct event_loop_event_s{
  void (*event_handler)(void *ctx);
} event_loop_event_t;

typedef struct event_loop_s{
  uint32_t event_map;
  uint32_t mask_map;
  uint32_t mask_map_poll;
  event_loop_event_t event_list_ptr[BTC_EVENT_MAX_NUM_EVENTS];
  btc_lock_t event_lock;//! lock for event loop while setting and clearing events from different contexts
}event_loop_t;

/**
 * @fn		      void btc_event_loop_init(struct event_loop_s *event_loop)
 * @brief		    Scheduler init
 * @param[in]	  event_loop - Scheduler control block
 * @param[out]	none
 * @return		  none
 * @description This function initializes the event_loop
 */
void btc_event_loop_init(struct event_loop_s *event_loop);
/**
 * @fn		      void btc_register_event(uint32_t event_id, void (*event_handler_ptr)(void *), struct event_loop_s *event_loop)
 * @brief		    registers event_loop event
 * @param[in]	  event_id: Event no
 * @param[in]	  event_handler_ptr  - Event handler
 * @param[out]	event_loop - event_loop control block
 * @return		  none
 * @description This function registers the event_loop event
 */
void btc_event_loop_register_event(uint32_t event_id, void (*event_handler_ptr)(void *), struct event_loop_s *event_loop);
/**
 * @fn		      void btc_event_loop(void *ctx)
 * @brief		    event_loop - takes care of event_loopuling all events
 * @param[in]	  ctx - event_loop control block
 * @param[out]	none
 * @return		  none
 * @description This function event_loopules the events in round robin fashion
 */
void btc_event_loop(void *ctx);
/**
 * @fn		      void btc_event_loop_set_event(uint32 *map, uint32 event_num)
 * @brief		    set event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function sets the event_loop event
 */
void btc_event_loop_set_event(struct event_loop_s *event_loop, uint32_t event_num);
/**
 * @fn		      void btc_event_loop_clear_event(uint32 *map, uint32 event_num)
 * @brief		    clear event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function clears the event_loop event
 */
void btc_event_loop_clear_event(struct event_loop_s *event_loop, uint32_t event_num);
/**
 * @fn		      void btc_event_loop_mask_event(uint32 *map, uint32 event_num)
 * @brief		    mask the event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function masks the event_loop event
 */
void btc_event_loop_mask_event(struct event_loop_s *event_loop, uint32_t event_num);
/**
 * @fn		      void btc_event_loop_unmask_event(uint32 *map, uint32 event_num)
 * @brief		    unmasks the event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function unmasks the event_loop event
 */
void btc_event_loop_unmask_event(struct event_loop_s *event_loop, uint32_t event_num);
#endif
