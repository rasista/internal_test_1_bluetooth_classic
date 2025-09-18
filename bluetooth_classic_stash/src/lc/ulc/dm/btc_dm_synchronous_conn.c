#include "btc_dm_synchronous_conn.h"
#include "btc_hci_events.h"


void dm_sco_link_formation_indication_from_lm(ulc_t *ulc, uint8_t peer_id, uint8_t accept_cmd_type, uint8_t status)
{
  (void)accept_cmd_type;
  (void)status;
  (void)peer_id;
  (void)ulc;
  return;
}


void brm_new_esco_connection_ind(ulc_t *ulc, uint8_t peer_id, acl_peer_info_t *acl_peer_info)
{
  // TODO
  (void)ulc;
  (void)peer_id;
  (void)acl_peer_info;
  return;
}


void dm_esco_link_formation_indication_from_lm(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t acl_link_mgmt_info = acl_peer_info->acl_link_mgmt_info;

  if (status == BT_STATUS_SUCCESS) {
    /* Inform BRM about the new sco connection */
    brm_new_esco_connection_ind(ulc, peer_id, acl_peer_info);
  }
  /* In case of eSCO link req accepted, status = BT_STATUS_SUCCESS.
   * In case of failure, status = failure reason. */
  /* Ask HCI to send an event to HOST */
  /* When HCI sends accept/reject synchronous connection command,
   * synchronous connection complete event must be sent. */
  if (acl_link_mgmt_info.check_pending_req_from_hci & SYNC_CONN_REQ) {
    /*Resetting flag*/
    acl_link_mgmt_info.check_pending_req_from_hci &= ~SYNC_CONN_REQ;
    if (acl_link_mgmt_info.check_pending_msg_flag & ESCO_SCO_CHANGE_PKT_REQ_SENT) {
      /*Resetting flag*/
      acl_link_mgmt_info.check_pending_msg_flag &= ~ESCO_SCO_CHANGE_PKT_REQ_SENT;
      hci_sync_conn_changed(ulc, peer_id, BT_ESCO, status);
    } else {
      hci_sync_conn_complete(ulc, peer_id, BT_ESCO, status);
    }
  }
  return;
}


