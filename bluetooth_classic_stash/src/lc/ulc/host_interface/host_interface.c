#include "host_interface.h"
#include "btc.h"
/*
   function: host_q_pending_handler
   description: This function is used to handle the host q pending event.
   parameters: ctx - context
   return: None
 */
void hci_pending_handler(void *ctx)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  queue_t *host_q = &ulc_ptr->hci_q;

  BTC_PRINT("host_q_pending_handler\n");
  while (host_q->size > 0) {
    pkb_t *pkb = (pkb_t *)dequeue(host_q);
    call_hci_handler(ctx, pkb);
  }
  btc_event_loop_clear_event(&ulc_ptr->event_loop, BTC_EVENT_HCI_FROM_HOST_PENDING);
}

//! function to process ulc.hci_event_pending
void hci_event_pending_handler(void *ctx)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  queue_t *hci_event_pending_q = &ulc_ptr->hci_event_pending_q;

  //! ideally placing it before free is not required, yet doing it for the sake of same flow as hci_event_sent_handler
  btc_event_loop_clear_event(&ulc_ptr->event_loop, BTC_EVENT_HCI_EVENT_TO_HOST_PENDING);
  BTC_PRINT("hci_event_pending_handler\n");
  //! somethin gis still present in the hci_event_pending_q, trigger the transport to send the packets
  if (hci_event_pending_q->size > 0) {
    schedule_hci_events_to_host();
  }
}

//! function to process ulc.hci_event_pending_q
void hci_event_sent_handler(void *ctx)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  queue_t *hci_event_sent_q = &ulc_ptr->hci_event_sent_q;

  //! clear the event before start freeing up the packets, this will ensure that if there was a race while setting this event,
  //! The packet free is still executed for that race condition.
  //! The expectation from the transport is to add packets to the queue first and then set the event.
  btc_event_loop_clear_event(&ulc_ptr->event_loop, BTC_EVENT_HCI_EVENT_TO_HOST_SENT);
  BTC_PRINT("hci_event_sent_handler\n");
  //! call scatter_free for all the packets in the hci_event_sent_q
  while (hci_event_sent_q->size > 0) {
    pkb_t *pkb = (pkb_t *)dequeue(hci_event_sent_q);
    scatter_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]);
  }
}