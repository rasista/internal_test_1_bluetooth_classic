#include "event_loop.h"

/**
 * @fn		      int16_t find_event(uint32_t active_event_map)
 * @brief		    Finds next event set
 * @param[in]	  active_event_map - event map
 * @param[out]	none
 * @return		  none
 * @description This function finds the next bit (event) set starting from the LSB
 */
uint32_t find_event(uint32_t active_event_map)
{
  uint8_t i;
  for (i = 0; i < BTC_EVENT_MAX_NUM_EVENTS; i++) {
    if (active_event_map & BIT(i)) {
      break;
    }
  }
  return i;
}

/**
 * @fn		      void btc_event_loop(void *ctx)
 * @brief		    event_loop - takes care of event_loopuling all events
 * @param[in]	  ctx - event_loop control block
 * @param[out]	none
 * @return		  none
 * @description This function event_loopules the events in round robin fashion
 */
void btc_event_loop(void *ctx)
{
  struct event_loop_s *event_loop  = (struct event_loop_s *)ctx;
  uint32_t active_event_map;
  uint8_t  event_no;
  event_loop_event_t *temp_event;
  event_loop_event_t*event_list_ptr;
  event_list_ptr = event_loop->event_list_ptr;

  /* Unmask the events to be polled */
  event_loop->mask_map |= event_loop->mask_map_poll;
  event_loop->mask_map_poll = 0;

  /* Check for active events and serve them sequentially */

  while ((active_event_map = (event_loop->event_map & event_loop->mask_map))) {
    event_no = find_event(active_event_map);
    temp_event = &event_list_ptr[event_no];
    if (temp_event->event_handler) {
      temp_event->event_handler(ctx);
    } else {
      BTC_ASSERT(EVENT_LOOP_UNREGISTERED_HANDLER);
    }
  }
}

/**
 * @fn		      void btc_event_loop_init(struct event_loop_s *event_loop)
 * @brief		    Scheduler init
 * @param[in]	  event_loop - Scheduler control block
 * @param[out]	none
 * @return		  none
 * @description This function initializes the event_loop
 */
void btc_event_loop_init(struct event_loop_s *event_loop)
{
  if (event_loop == NULL) {
    BTC_ASSERT(NULL_POINTER);
  } else {
    event_loop->event_map = 0x0;
    event_loop->mask_map = 0xFFFFFFFF;
    event_loop->mask_map_poll = 0x0;
    memset(event_loop->event_list_ptr, 0, (sizeof(event_loop_event_t) * BTC_EVENT_MAX_NUM_EVENTS)); //TODO Check all library functions
    btc_plt_create_lock(&event_loop->event_lock);
  }
}

/**
 * @fn		      void btc_register_event(uint32_t event_id, void (*event_handler_ptr)(void *), struct event_loop_s *event_loop)
 * @brief		    registers event_loop event
 * @param[in]	  event_id: Event no
 * @param[in]	  event_handler_ptr  - Event handler
 * @param[out]	event_loop - event_loop control block
 * @return		  none
 * @description This function registers the event_loop event
 */
void btc_event_loop_register_event(uint32_t event_id, void (*event_handler_ptr)(void *), struct event_loop_s *event_loop)
{
  if (event_id >= BTC_EVENT_MAX_NUM_EVENTS) {
    /* To add a new event one should change
     * the max num events define corresponding to
     * that protocol . It is present in event_loop.h*/
    BTC_ASSERT(EVENT_LOOP_MAX_NUM_EVENTS_CROSSED_IN_REGISTER_EVENTS);
  }

  if (event_loop->event_list_ptr[event_id].event_handler) {
    BTC_ASSERT(EVENT_LOOP_ERR_OVERWRITING_REGISTERED_EVENT);
  } else {
    event_loop->event_list_ptr[event_id].event_handler = event_handler_ptr;
  }
  return;
}

/**
 * @fn		      void btc_event_loop_set_event(uint32 *map, uint32 event_num)
 * @brief		    set event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function sets the event_loop event
 */
void btc_event_loop_set_event(struct event_loop_s *event_loop, uint32_t event_num)
{
  btc_plt_acquire_lock(&event_loop->event_lock);
  event_loop->event_map |= BIT(event_num);
  btc_plt_release_lock(&event_loop->event_lock);
}

/**
 * @fn		      void btc_event_loop_clear_event(uint32 *map, uint32 event_num)
 * @brief		    clear event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function clears the event_loop event
 */
void btc_event_loop_clear_event(struct event_loop_s *event_loop, uint32_t event_num)
{
  btc_plt_acquire_lock(&event_loop->event_lock);
  event_loop->event_map &= ~BIT(event_num);
  btc_plt_release_lock(&event_loop->event_lock);
}

/**
 * @fn		      void btc_event_loop_mask_event(uint32 *map, uint32 event_num)
 * @brief		    mask the event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function masks the event_loop event
 */
void btc_event_loop_mask_event(struct event_loop_s *event_loop, uint32_t event_num)
{
  btc_plt_acquire_lock(&event_loop->event_lock);
  event_loop->mask_map &= ~BIT(event_num);
  btc_plt_release_lock(&event_loop->event_lock);
}

/**
 * @fn		      void btc_event_loop_unmask_event(uint32 *map, uint32 event_num)
 * @brief		    unmasks the event
 * @param[out]	map - active event map
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function unmasks the event_loop event
 */
void btc_event_loop_unmask_event(struct event_loop_s *event_loop, uint32_t event_num)
{
  btc_plt_acquire_lock(&event_loop->event_lock);
  event_loop->mask_map |= BIT(event_num);
  btc_plt_release_lock(&event_loop->event_lock);
}

/**
 * @fn		      void btc_event_loop_set_event_mask_poll_map(struct event_loop_s *event_loop, uint32 event_num)
 * @brief		    sets the polling event
 * @param[in]	  bsm - event_loop control block
 * @param[in]	  event_num - event number
 * @return		  none
 * @description This function sets the event as polling event
 */
void btc_event_loop_set_event_mask_poll_map(struct event_loop_s *event_loop, uint32_t event_num)
{
  btc_plt_acquire_lock(&event_loop->event_lock);
  event_loop->mask_map &= ~BIT(event_num);
  event_loop->mask_map_poll |= BIT(event_num);
  btc_plt_release_lock(&event_loop->event_lock);
}
