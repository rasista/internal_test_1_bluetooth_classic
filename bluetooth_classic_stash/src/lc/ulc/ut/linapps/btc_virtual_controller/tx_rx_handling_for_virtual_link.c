#include "tx_rx_handling_for_virtual_link.h"
#include "btc.h"
#include "pkb_mgmt.h"
#include "string.h"
#include "ulc.h"
#include "usched_hci_cmd_event_handlers.h"
/**
 * @brief Handles data received over the air from the peer device.
 *
 * This function processes the received data by allocating a packet buffer,
 * adding metadata to indicate that the packet is an On-Air packet, and
 * enqueuing it into the LPW RX Done queue. It also triggers the event loop
 * to handle the LPW RX Done event.
 *
 * @param data Pointer to the received data buffer.
 * @param length Length of the received data.
 *
 * @note Metadata is added to the beginning of the packet to differentiate
 *       On-Air packets from internal packets.
 * @note The function sets the BTC_EVENT_LPW_RX_DONE event in the event loop
 *       and removes the event from the pending list.
 */
void receive_done_callback(uint8_t *data, uint8_t length)
{
  BTC_PRINT("\n RECEIVED DONE HANDLER \n");
  pkb_t *recvd_data_pkt = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  memcpy(recvd_data_pkt->data, data, length);
  recvd_data_pkt->data[0] = ON_THE_AIR_PKT_TYPE;
  recvd_data_pkt->pkt_len = length;
  enqueue(&ulc.lpw_rx_done_q, &recvd_data_pkt->queue_node);
  btc_event_loop_set_event(&ulc.event_loop, BTC_EVENT_LPW_RX_DONE);
  btc_plt_remove_from_pend(&ulc.event_loop_pend);
}

void virtual_tx_done_handler(void *ctx)
{
  ulc_t *ulc_ptr         = (ulc_t *)ctx;
  queue_t *acl_pending_q = &ulc_ptr->tx_send_q;
  while (acl_pending_q->size > 0) {
    pkb_t *pkb = (pkb_t *)dequeue(acl_pending_q);
    if (pkb != NULL) {
      usched_tx_pkt_handler(pkb->data, pkb->pkt_len);
    }
    scatter_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]);
  }
  btc_event_loop_clear_event(&ulc.event_loop, BTC_EVENT_VIRTUAL_TX_DONE);
}
