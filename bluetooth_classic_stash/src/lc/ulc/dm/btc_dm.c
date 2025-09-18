#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_dm.h"

void flush_queue(queue_t *q)
{
    pkb_t *pkb;
    while (q->size > 0) {
        pkb = (pkb_t *)dequeue(q);
        scatter_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]);
    }
}


void btc_deint(ulc_t *ulc)
{
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  flush_queue(&ulc->hci_q);
  flush_queue(&ulc->hci_event_pending_q);
  memset(&btc_dev_info->btc_dev_mgmt_info, 0, sizeof(btc_dev_mgmt_info_t));
  memset(&btc_dev_info->btc_hci_params, 0, sizeof(btc_hci_params_t));
  memset(&btc_dev_info->inquiry_params, 0, sizeof(inquiry_params_t));
  memset(&btc_dev_info->inquiry_scan_params, 0, sizeof(inquiry_scan_params_t));
  memset(&btc_dev_info->page_params, 0, sizeof(page_params_t));
  memset(&btc_dev_info->page_scan_params, 0, sizeof(page_scan_params_t));
}

void add_pkt_to_acld_q(ulc_t *ulc, pkb_t *lmp_tx_pkt, uint8_t peer_id)
{
#ifdef SCHED_SIM
  pkb_t *temp_pkb =  pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  memcpy(temp_pkb->data + META_DATA_LENGTH, lmp_tx_pkt->data, lmp_tx_pkt->pkt_len);
  temp_pkb->data[2] = peer_id;
  lmp_tx_pkt->pkt_len = lmp_tx_pkt->pkt_len + META_DATA_LENGTH;
  memcpy(lmp_tx_pkt->data, temp_pkb->data, lmp_tx_pkt->pkt_len);
  pkb_free(temp_pkb, &btc.pkb_pool[pkb_chunk_type_medium]);
  add_tx_pkt_to_queue(&ulc->tx_send_q, lmp_tx_pkt);
#else
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  queue_t *tx_acld_q = &acl_peer_info->tx_acld_q;
  add_tx_pkt_to_queue(tx_acld_q, lmp_tx_pkt);
#endif
}
