#include "btc_device.h"
#include "debug.h"
#include "lpw_scheduler.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
#endif

/**
 * @brief check_aperiodic_role_overlapping_with_periodic_start function will check if non periodic roles are overlapping with periodic roles start
 *
 * @param btc_trx_configs
 * @return uint8_t
 */
uint8_t check_aperiodic_role_overlapping_with_periodic_start(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs)
{
  /*  Need to check the current Tx Rx activity if it will overlap with periodic role start */
  uint32_t activity_end_time_tsf;
  btc_trx_configs_t *head = btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue.head;
  if (head) {//if there are entries in queues present
    activity_end_time_tsf = btc_trx_configs->max_radio_usage_in_us + btc_trx_configs->btc_rail_trx_config.tsf; // Aperiodic end time
    if (head->btc_rail_trx_config.tsf <= (activity_end_time_tsf + LPW_SCHED_ACTIVITY_SWITCH_DELAY_IN_US)) { /* overlapping */
      if (btc_trx_configs->priority <= head->priority) {
        // Aperiodic activity is lower or equal in priority than Periodic, so move to Periodic activity
        // overlapping and periodic is having priority
        return TRUE;
      }
      // here, aperiodic is overlapping and aperiodic is having priority
      // even though overlapping, we will NOT consider Periodic, due to priority
    }
  }
  return FALSE;
}

/*
 * @brief : deletes the head of the queue and returns the head value
 * @params : btc_dev_p
 * @return : btc_trx_configs_t structure pointer
 * */
btc_trx_configs_t* dequeue_periodic_role_from_periodic_queue()
{
  /* Dequeue is performed only when A periodic role is being configured for next interval, and enqueues with new parametrs */
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  lpw_sch_periodic_queue_t *lpw_sch_periodic_queue =  &btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue;
  btc_trx_configs_t *btc_trx_configs_head = NULL;
/* return Head and update tail value as new head */
  if (lpw_sch_periodic_queue->queue_size ) {//if there are entries in queues present
    btc_trx_configs_head = lpw_sch_periodic_queue->head;
    SET_CONF_FLAG_DEQUEUED(btc_trx_configs_head->flags);
    lpw_sch_periodic_queue->head = lpw_sch_periodic_queue->head->next;
    lpw_sch_periodic_queue->queue_size--;
  } else {
    //ERROR case, should not dequeue empty queue
    BTC_ASSERT(PERIODIC_QUEUE_EMPTY);
  }
  return btc_trx_configs_head;
}

/*
 * @brief : Adds new entry for periodic role into the periodic queue
 * deletes the head of the queue and returns the head value
 * @params : btc_dev_p, btc_trx_configs
 * */
void add_periodic_role_to_periodic_queue(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs)
{
  /*Note:  Start TSF of queue head is smaller and tail is larger.  Need to place new entry accordingly */
  lpw_sch_periodic_queue_t *lpw_sch_periodic_queue =  &btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue;
  btc_trx_configs_t *curr_node = lpw_sch_periodic_queue->head;
  btc_trx_configs_t *prev_node = curr_node;
  btc_trx_configs->next = NULL; // Can remove this statement if already done
  SET_CONF_FLAG_QUEUED(btc_trx_configs->flags);
  lpw_sch_periodic_queue->queue_size++;
  if (lpw_sch_periodic_queue->head == NULL) {
    /* queue is empty, so add directly */
    lpw_sch_periodic_queue->head = btc_trx_configs;
    return;
  }

  //queue is not empty, so need to iterate along the queue and add accordingly
  while (curr_node != NULL) {
    if (curr_node->btc_rail_trx_config.tsf > btc_trx_configs->btc_rail_trx_config.tsf) { //current node  is farther than the new node, adding the node before current node
      // Insert before current node
      btc_trx_configs->next = curr_node;

      if (curr_node == lpw_sch_periodic_queue->head) {
        //curr is head
        lpw_sch_periodic_queue->head = btc_trx_configs;
      } else {
        // Insert between prev_node and curr_node
        btc_trx_configs->next = prev_node->next;
        prev_node->next = btc_trx_configs;
      }
      return;
    }

    // Move to the next node
    prev_node = curr_node;
    curr_node = curr_node->next;
  }

  // If we reach here, we need to append to the end of the queue
  if (prev_node != NULL) {
    prev_node->next = btc_trx_configs;     // Link the last node to the new one
  }
}

/*
 * @brief : Removes periodic role from the periodic queue
 * deletes the role from the queue
 * @params : btc_dev_p, btc_trx_configs
 * */
void remove_periodic_role_from_periodic_queue(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs)
{
  lpw_sch_periodic_queue_t *lpw_sch_periodic_queue =  &btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue;
  btc_trx_configs_t *curr_node = lpw_sch_periodic_queue->head;
  btc_trx_configs_t *prev_node = NULL;
  while (curr_node != NULL) {
    if (curr_node == btc_trx_configs) {
      if (lpw_sch_periodic_queue->head == btc_trx_configs) {
        //Head of the queue is to be removed
        lpw_sch_periodic_queue->head = lpw_sch_periodic_queue->head->next;
        break;
      }
      prev_node->next = curr_node->next;
      break;
    }
    prev_node = curr_node;
    curr_node = curr_node->next;
  }

  if (curr_node == NULL) {
    /* role not found, assert */
    BTC_ASSERT(PERIODIC_QUEUE_EMPTY);
  }

  lpw_sch_periodic_queue->queue_size--;
  SET_CONF_FLAG_DEQUEUED(btc_trx_configs->flags);
}

/*
 *@brief : Aperiodic role when triggered from Host SS, will be updated with necessary details WRT aperiodic role
 * @params : btc_dev_p, state_machine
 * */
void update_aperiodic_role(btc_dev_t *btc_dev_p, sm_t *state_machine)
{
  if (btc_dev_p->lpw_scheduler_info.l1_scheduled_sm) {
    /* aperiodic role cannot be occupied already, assertion is needed over here */
    /* TODO: uncomment below AFTER writing APT testcases */
    //BTC_ASSERT("INVALID");
  }
  btc_dev_p->lpw_scheduler_info.l1_scheduled_sm = state_machine;
  btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured = APERIODIC_NOT_PUNCTURED;
}

/*
 * @Brief: common stop functionality related to aperiodic role closure from host
 * @params : btc_dev_p
 * */
void stop_aperiodic_role(btc_dev_t *btc_dev_p)
{
  // final closure of a role or temperary switch
  // 1. If role is aborted , abort signal to aperiodic role will be called after periodic activity is done
  // 2. Once Aperiodic activity is removed, periodic handler should know and not call aperiodic abort
  // 3. Set status of state machine to IDLE state
  lpw_scheduler_info_t *lpw_scheduler_info = &btc_dev_p->lpw_scheduler_info;

  lpw_scheduler_info->is_aperiodic_role_punctured = APERIODIC_NOT_PUNCTURED;

  if (btc_dev_p->l2_scheduled_sm == lpw_scheduler_info->l1_scheduled_sm) {
    btc_dev_p->l2_scheduled_sm = NULL;
    /*TODO: RAIL needs to be intimated as it was configured */
    l2_sched_configure_next_activity_on_role_removal(btc_dev_p);
  }
  SM_STATE(btc_dev_p->lpw_scheduler_info.l1_scheduled_sm, SM_STATE_ROLE_IDLE);
  lpw_scheduler_info->l1_scheduled_sm = NULL;
}

/*
 * @Brief: common stop functionality related to periodic role closure from host
 * @params : btc_dev_p, btc_trx_configs
 * */
void stop_periodic_role(btc_dev_t *btc_dev_p, btc_trx_configs_t *btc_trx_configs)
{
  // final closure of a periodic role
  // 1. dequeue from periodic queue if role not l2_scheduled_sm
  // 2. Set status of state machine to IDLE state
  if (btc_dev_p->l2_scheduled_sm == btc_trx_configs->sm) {
    /*TODO: RAIL needs to be intimated as it was configured
     * do we need to reset structure ? */
    l2_sched_configure_next_activity_on_role_removal(btc_dev_p);
  } else {
    remove_periodic_role_from_periodic_queue(btc_dev_p, btc_trx_configs);
  }
  SM_STATE(btc_trx_configs->sm, SM_STATE_ROLE_IDLE);
  btc_dev_p->l2_scheduled_sm = NULL;
}

/*
 * @Brief: start next activity when role is ended from Host SS
 * @params : btc_dev_p, btc_trx_configs
 * */
void l2_sched_configure_next_activity_on_role_removal(btc_dev_t *btc_dev_p)
{
  /* TODO: */
  /* Check for punctured aperiodic role and next periodic activity in queue */
  btc_trx_configs_t *btc_trx_configs;
  uint32_t status;
  if (btc_dev_p->lpw_scheduler_info.is_aperiodic_role_punctured != APERIODIC_NOT_PUNCTURED) {
    /* Punctured  aperiodic role available */
    restore_aperiodic_role_punctured();
  } else if (btc_dev_p->lpw_scheduler_info.lpw_sch_periodic_queue.head) {
    /* periodic role available in periodic waiting queue */
    btc_trx_configs = dequeue_periodic_role_from_periodic_queue();
    status =   schedule_tx_rx_activity(btc_trx_configs);
    if (status != BTC_RAIL_PASS_STATUS) {
      handle_overriding_role_abort_status(btc_trx_configs);
    }
  }
}
