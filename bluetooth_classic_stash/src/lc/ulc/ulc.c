/*
   File Header:
   File Name: ulc.c
   File Description: This file contains the starting point for ULC implementation.
 */

#include "ulc.h"
#include "host_to_lpw_interface.h"
#include "btc_hci_rx_pkt_handlers.h"

#ifdef SCHED_SIM
  #include "tx_rx_handling_for_virtual_link.h"
#endif

ulc_t ulc;

// Function Name: ulc_init
// Description: This function is used to initialize the ULC module.
// Parameters: None
// Return: None
void *ulc_init(void *ctx)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  btc_hci_init(ctx);
  btc_lmp_init(ulc_ptr);
  // Initialize the event_loop
  btc_event_loop_init(&ulc_ptr->event_loop);
  // register the event handlers
  btc_event_loop_register_event(BTC_EVENT_HCI_FROM_HOST_PENDING, hci_pending_handler, &ulc_ptr->event_loop);
  btc_event_loop_register_event(BTC_EVENT_HCI_EVENT_TO_HOST_PENDING, hci_event_pending_handler, &ulc_ptr->event_loop);
  btc_event_loop_register_event(BTC_EVENT_HCI_EVENT_TO_HOST_SENT, hci_event_sent_handler, &ulc_ptr->event_loop);
  btc_event_loop_register_event(BTC_EVENT_LPW_NOTIFICATION_PENDING_HANDLER, (void *)lpw_command_processed_handler, &ulc_ptr->event_loop);
  btc_event_loop_register_event(BTC_EVENT_LPW_RX_DONE, lpw_rx_done_handler, &ulc_ptr->event_loop);
#ifdef SCHED_SIM
  btc_event_loop_register_event(BTC_EVENT_VIRTUAL_TX_DONE, virtual_tx_done_handler, &ulc_ptr->event_loop);
#endif
  //btc_event_loop_register_event(BTC_EVENT_LLC_Q_PENDING, llc_q_pending_handler, &ulc->event_loop);
  //btc_event_loop_register_event(BTC_EVENT_SCHEDULER_ACTIVITY_PENDING, scheduler_activity_pending_handler, &ulc->event_loop);
  if (btc_plt_create_pend(&ulc_ptr->event_loop_pend) != 0)
  {
    BTC_PRINT(" event loop pending mechanism init Failed");
    return NULL;
  }
  return NULL;
}

void *ulc_loop(void *ctx)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  while (1) {
    btc_plt_add_to_pend(&ulc_ptr->event_loop_pend);
    BTC_PRINT("Calling event loop\n");
    btc_event_loop(ctx);
  }
}

