#include "ulc.h"
#include "lmp.h"
#include "btc_lmp.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_hci_events.h"
#include "btc_process_hci_cmd.h"
#include "lmp_get_defines_autogen.h"
#include "btc_lmp.h"
#include "btc_key_mgmt_algos.h"

void lmp_name_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t lmp_input_params[2];
  uint8_t tid;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t name_offset = GET_LMP_NAME_REQ_PARAMETER_NAME_OFFSET(bt_pkt_start);
  tid = GET_LMP_NAME_REQ_TID(bt_pkt_start);
  if (name_offset < btc_dev_mgmt_info->device_name_len) {
    lmp_input_params[0] = name_offset;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NAME_RES, 0, peer_id, tid), &lmp_input_params);
  } else {
    lmp_input_params[0] = INVALID_LMP_PARAMETERS;
    lmp_input_params[1] = LMP_NAME_RES;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  }
}

void lmp_name_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t lmp_input_params;
  uint8_t actual_frament_length;
  uint8_t tid;
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t dev_name_len = GET_LMP_NAME_RES_PARAMETER_NAME_LENGTH(bt_pkt_start);
  uint8_t dev_name_offset = GET_LMP_NAME_RES_PARAMETER_NAME_OFFSET(bt_pkt_start);
  tid = GET_LMP_NAME_RES_TID(bt_pkt_start);
  acl_dev_mgmt_info->device_name_len = dev_name_len;
  if (acl_dev_mgmt_info->device_name_len > MAX_BT_DEVICE_NAME_LEN) {
    BTC_ASSERT(MAX_BT_DEVICE_NAME_LEN);
  }

  actual_frament_length = dev_name_len - dev_name_offset;
  uint32_t len = (((actual_frament_length) > LMP_MAX_NAME_FRAGMENT_LENGTH) ? LMP_MAX_NAME_FRAGMENT_LENGTH
                  : (actual_frament_length));
  memcpy((acl_dev_mgmt_info->device_name + dev_name_offset), &GET_LMP_NAME_RES_PARAMETER_NAME_FRAGMENT(bt_pkt_start), len);

  if ((actual_frament_length) > LMP_MAX_NAME_FRAGMENT_LENGTH) {
    lmp_input_params = dev_name_offset + LMP_MAX_NAME_FRAGMENT_LENGTH;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NAME_RES, 0, peer_id, tid), &lmp_input_params);
  } else {
    if ((acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) && (acl_link_mgmt_info->lmp_connection_state  == NAME_REQ_SENT_STATE)) {
      lmp_process_central_conn_state(ulc, peer_id, NAME_RESP_RCVD, 0);
    }

    if ((acl_link_mgmt_info->check_pending_req_from_hci & HCI_NAME_REQ_SENT)
        && (!(acl_link_mgmt_info->conn_purpose == TEMPORARY_CONN_FOR_REMOTE_NAME_RES))) {
      acl_link_mgmt_info->check_pending_req_from_hci &= ~HCI_NAME_REQ_SENT;
      /* Indicate the reception of complete remote device_name to HCI */
      hci_remote_dev_name_recvd_indication(ulc, peer_id, BT_SUCCESS_STATUS);
    } else if (acl_link_mgmt_info->conn_purpose == TEMPORARY_CONN_FOR_REMOTE_NAME_RES) {
      lmp_input_params = REM_USER_TERMINATED_CONNECTION;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DETACH, 0, peer_id, tid), &lmp_input_params);
    }
  }
}

void lmp_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t rx_pkt_opcode = GET_LMP_ACCEPTED_PARAMETER_OPCODE(pkb->data);
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (acl_peer_info->acl_link_mgmt_info.lmp_tx_pending != 0) {
    if (rx_pkt_opcode == acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode) {
      acl_peer_info->acl_link_mgmt_info.lmp_tx_pending        = 0;
      acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = 0;
    }
    call_lmp_accepted_rx_handler(ulc, pkb);
  }
}

void lmp_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  call_lmp_not_accepted_rx_handler(ulc, pkb);
}

void lmp_clkoffset_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_CLKOFFSET_REQ_TID(bt_pkt_start);

  /* Respond with clock offset response */
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_CLKOFFSET_RES, 0, peer_id, tid), NULL);
}

void lmp_clkoffset_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);

  /* Store the clock offset value */
  acl_peer_info->acl_dev_mgmt_info.clk_offset = *GET_LMP_CLKOFFSET_RES_PARAMETER_CLOCK_OFFSET(bt_pkt_start);

  /* Check if there was a pending request from HCI */
  if (acl_link_mgmt_info->check_pending_req_from_hci & HCI_READ_CLK_OFFSET) {
    acl_link_mgmt_info->check_pending_req_from_hci &= ~HCI_READ_CLK_OFFSET;
    /* Indicate receive of clock offset to HCI */
    hci_read_clk_offset_complete(ulc, peer_id, BT_SUCCESS_STATUS);
  }
}

void lmp_detach_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t reason = GET_LMP_DETACH_PARAMETER_ERROR_CODE(bt_pkt_start);
  acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_NOT_DONE;
  dm_acl_disconn_ind(ulc, peer_id, reason);
}

void lmp_encryption_mode_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_encryption_key_size_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_start_encryption_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_stop_encryption_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_switch_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_hold_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_hold_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_auto_rate_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_link_mgmt_info->device_status_flags |= DEVICE_AUTO_RATE_ENABLED;
  if ((IS_PERIPHERAL_MODE(acl_dev_mgmt_info) && (acl_dev_mgmt_info->ptt == BT_EDR_MODE))) {
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_PREFERRED_RATE, 0, peer_id, CENTERAL_INITIATED_TID), NULL);
  }
}

void lmp_preferred_rate_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint16_t pkt_type = 0;
  /* Preferred_rate */
  acl_dev_mgmt_info->preferred_rate = GET_LMP_PREFERRED_RATE_PARAMETER_DATA_RATE(bt_pkt_start);
  acl_dev_mgmt_info->preferred_rate_received = 1;

  if (acl_dev_mgmt_info->ptt == BT_BR_MODE) {
    if (acl_dev_mgmt_info->preferred_rate & DONT_USE_FEC) {
      pkt_type = BR_NON_FEC_PKT_TYPE;
    } else {
      pkt_type = BR_FEC_PKT_TYPE;
    }

    switch (((acl_dev_mgmt_info->preferred_rate & BR_PKT_SLOT_CHECK) >> 1)) {
      case BR_ALL_SLOT_PREFER:
        pkt_type &= BR_ALL_SLOT_PKT_TYPE;
        break;
      case BR_ONE_SLOT_PKTS:
        pkt_type &= BR_ONE_SLOT_PKT_TYPE;
        break;
      case BR_THREE_SLOT_PKTS:
        pkt_type &= BR_THREE_SLOT_PKT_TYPE;
        break;
      case BR_FIVE_SLOT_PKTS:
        pkt_type &= BR_FIVE_SLOT_PKT_TYPE;
        break;
    }
  } else {
    switch ((acl_dev_mgmt_info->preferred_rate & EDR_RATE_CHECK) >> 3) {
      case USE_DM1_PKTS:
        pkt_type |= BIT(BT_DM1_PKT_TYPE);
        break;
      case USE_EDR_2MBPS_PKTS:
        pkt_type |= ONLY_TWO_MBPS_PKTS_VALID;
        break;
      case USE_EDR_3MBPS_PKTS:
        pkt_type |= ONLY_THREE_MBPS_PKTS_VALID;
        break;
    }

    switch ((acl_dev_mgmt_info->preferred_rate & EDR_PKT_SLOT_CHECK) >> 5) {
      case EDR_ALL_SLOT_PREFER:
        pkt_type &= EDR_PACKETS_ALLOWED;
        break;
      case EDR_ONE_SLOT_PKTS:
        pkt_type &= EDR_ONE_SLOT_PKT_TYPE;
        break;
      case EDR_THREE_SLOT_PKTS:
        pkt_type &= EDR_THREE_SLOT_PKT_TYPE;
        break;
      case EDR_FIVE_SLOT_PKTS:
        pkt_type &= EDR_FIVE_SLOT_PKT_TYPE;
        break;
    }
  }
  lmp_change_conn_pkt_type(ulc, peer_id, pkt_type);
}

void lmp_version_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_VERSION_REQ_TID(bt_pkt_start);

  acl_dev_mgmt_info->vers_number = GET_LMP_VERSION_REQ_PARAMETER_VERSION(bt_pkt_start);
  acl_dev_mgmt_info->comp_id = *GET_LMP_VERSION_REQ_PARAMETER_COMPANY_IDENTIFIER(bt_pkt_start);
  acl_dev_mgmt_info->sub_vers_number = *GET_LMP_VERSION_REQ_PARAMETER_SUBVERSION(bt_pkt_start);
  acl_link_mgmt_info->device_status_flags |= DEVICE_VERSION_INFO_RCVD;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_VERSION_RES, 0, peer_id, tid), NULL);
}

void lmp_version_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);

  acl_dev_mgmt_info->vers_number = GET_LMP_VERSION_RES_PARAMETER_VERSION(bt_pkt_start);
  acl_dev_mgmt_info->comp_id = *GET_LMP_VERSION_RES_PARAMETER_COMPANY_IDENTIFIER(bt_pkt_start);
  acl_dev_mgmt_info->sub_vers_number = *GET_LMP_VERSION_RES_PARAMETER_SUBVERSION(bt_pkt_start);
  acl_link_mgmt_info->device_status_flags |= DEVICE_VERSION_INFO_RCVD;

  if (acl_link_mgmt_info->check_pending_req_from_hci & HCI_VERSION_REQ_SENT) {
    acl_link_mgmt_info->check_pending_req_from_hci &= ~HCI_VERSION_REQ_SENT;
    /* Indicate receive of remote device version info to HCI */
    hci_read_remote_dev_vers_info_complete(ulc, acl_dev_mgmt_info->conn_handle, peer_id, BT_SUCCESS_STATUS);
  }

  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    if (acl_link_mgmt_info->lmp_connection_state == VERSION_REQ_SENT_STATE) {
      lmp_process_central_conn_state(ulc, peer_id, VERSION_RESP_RCVD, 0);
    }
  } else if (acl_link_mgmt_info->lmp_slave_connection_state == SLAVE_VERSION_REQ_SENT_STATE) {
    lmp_process_peripheral_conn_state(ulc, peer_id, VERSION_RESP_RCVD, 0);
  }
}

void lmp_features_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_FEATURES_REQ_TID(bt_pkt_start);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  update_feature_map(acl_peer_info, &(GET_LMP_FEATURES_REQ_PARAMETER_FEATURES(bt_pkt_start)), ulc);
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_RES, 0, peer_id, tid), NULL);
}

void lmp_features_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  update_feature_map(acl_peer_info, &(GET_LMP_FEATURES_RES_PARAMETER_FEATURES(bt_pkt_start)), ulc);
  acl_peer_info->acl_link_mgmt_info.device_status_flags |= DEVICE_FEATURES_RCVD;

  if (acl_peer_info->acl_link_mgmt_info.check_pending_req_from_hci & FEATURES_REQ_SENT) {
    acl_peer_info->acl_link_mgmt_info.check_pending_req_from_hci &= ~FEATURES_REQ_SENT;
    /* Indicate receive of remote_device_features to HCI */
    hci_remote_dev_features_recvd(ulc, peer_id, BT_SUCCESS_STATUS);
  }
  if (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE) {
    if (acl_peer_info->acl_link_mgmt_info.lmp_connection_state == FEATURES_REQ_SENT_STATE) {
      lmp_process_central_conn_state(ulc, peer_id, FEATURES_RESP_RCVD, 0);
    }
  } else if (acl_peer_info->acl_link_mgmt_info.lmp_slave_connection_state == SLAVE_FEATURE_REQ_SENT_STATE) {
    lmp_process_peripheral_conn_state(ulc, peer_id, FEATURES_RESP_RCVD, 0);
  }
}

void lmp_max_slot_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_max_slot_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  uint32_t lmp_control_word;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_MAX_SLOT_REQ_TID(bt_pkt_start);
  uint8_t max_slots = GET_LMP_MAX_SLOT_REQ_PARAMETER_MAX_SLOTS(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_MAX_SLOT_REQ_OPCODE(bt_pkt_start);
  if ((max_slots == 1) || ((max_slots == 3) && (acl_dev_mgmt_info->features_bitmap[0] & LMP_3SLOT))
      || ((max_slots == 5) && (acl_dev_mgmt_info->features_bitmap[0] & LMP_5SLOT))) {
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid);
  } else {
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid);
    lmp_input_params[1] = UNSUP_REM_FEATURE_OR_UNSUP_LMP_FEATURE;
  }
  call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
}

void lmp_timimg_accuracy_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_TIMIMG_ACCURACY_REQ_TID(bt_pkt_start);
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_TIMING_ACCURACY_RES, 0, peer_id, tid), NULL);
}

void lmp_timing_accuracy_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  acl_dev_mgmt_info->drift  = GET_LMP_TIMING_ACCURACY_RES_PARAMETER_DRIFT(bt_pkt_start);
  acl_dev_mgmt_info->jitter = GET_LMP_TIMING_ACCURACY_RES_PARAMETER_JITTER(bt_pkt_start);
  if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
    lmp_process_peripheral_conn_state(ulc, peer_id, TIMING_ACCURACY_RES_RECVD, 0);
  }
}

void lmp_setup_complete_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_link_mgmt_info->device_status_flags |= SETUP_COMPLETE;
  if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    if ((acl_link_mgmt_info->check_slave_pending_msg_flag & SLAVE_SETUP_COMPLETE_SENT)
        && (acl_link_mgmt_info->lmp_slave_connection_state == SLAVE_SETUP_COMPLETE_SENT_STATE)) {
      lmp_process_peripheral_conn_state(ulc, peer_id, SETUP_COMPLETE_RCVD, 0);
    }
  } else {
    if ((acl_link_mgmt_info->check_pending_msg_flag & SETUP_COMPLETE_SENT)
        && (acl_link_mgmt_info->lmp_connection_state == SETUP_COMPLETE_SENT_STATE)) {
      lmp_process_central_conn_state(ulc, peer_id, SETUP_COMPLETE_RCVD, 0);
    }
  }
}

void lmp_use_semi_permanent_key_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_host_connection_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t lmp_input_params[2];
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &bt_dev_info->btc_dev_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_HOST_CONNECTION_REQ_TID(bt_pkt_start);

  if ((acl_link_mgmt_info->received_req_flag & ACL_CONNECTION_REQ)
      || (acl_link_mgmt_info->device_status_flags & HOST_CONN_REQ_COMPLETED)) {
    acl_link_mgmt_info->received_req_flag &= ~ACL_CONNECTION_REQ;
    lmp_input_params[0] = LMP_PDU_NOT_ALLOWED;
    lmp_input_params[1] = GET_LMP_HOST_CONNECTION_REQ_OPCODE(bt_pkt_start);
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
    lmp_send_detach_frame(ulc, peer_id, LMP_PDU_NOT_ALLOWED);
  }

  acl_link_mgmt_info->remote_initiated_conn = 1; //Recording that Remote Initiated Connection
  acl_link_mgmt_info->host_event_notification_flags |= CONNECTION_COMPLETE;

  if ((btc_dev_mgmt_info->event_filter_type == HCI_CONN_EVENT_FLT_TYPE)
      && ((btc_dev_mgmt_info->event_filter_cond_type == HCI_CONN_SP_BD_ADDR)
          || (btc_dev_mgmt_info->event_filter_cond_type == HCI_CONN_SP_CLASS_OF_DEVICE))) {
    if ((btc_dev_mgmt_info->event_filter_cond_type == HCI_CONN_SP_BD_ADDR)
        && memcmp(acl_dev_mgmt_info->bd_addr, bt_dev_info->btc_hci_params.bd_addr, MAC_ADDR_LEN)) {
      lmp_input_params[0] = CONN_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR;
      lmp_input_params[1] = LMP_HOST_CONNECTION_REQ;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
      acl_link_mgmt_info->received_req_flag |= ACL_CONNECTION_REQ;
    }

    if ((btc_dev_mgmt_info->event_filter_cond_type == HCI_CONN_SP_CLASS_OF_DEVICE)
        && memcmp(acl_dev_mgmt_info->class_of_dev, bt_dev_info->btc_dev_mgmt_info.class_of_dev, CLASS_OF_DEV_LEN)) {
      lmp_input_params[0] = CONN_REJECTED_DUE_TO_SECURITY_REASONS;
      lmp_input_params[1] = LMP_HOST_CONNECTION_REQ;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
      acl_link_mgmt_info->received_req_flag |= ACL_CONNECTION_REQ;
    }

    if (btc_dev_mgmt_info->event_auto_accept_flag == 0x1) {
      hci_conn_request_indication(ulc, peer_id, BT_ACL);
      acl_link_mgmt_info->updates_pending |= ACCEPT_CONN_TIMEOUT_PENDING;
      //acl_link_mgmt_info->accept_timeout_instant =
      // bt_dev_info->bt_current_slot_params_p->current_slot_num + (bt_dev_info->btc_dev_mgmt_info.conn_accept_tout << 1); TODO
    } else if (btc_dev_mgmt_info->event_auto_accept_flag == 0x2) {
      acl_link_mgmt_info->received_req_flag |= ACL_CONNECTION_REQ;
      lmp_accept_connection_req_from_dm(ulc, peer_id, 1);
    } else if (btc_dev_mgmt_info->event_auto_accept_flag == 0x3) {
      acl_link_mgmt_info->received_req_flag |= ACL_CONNECTION_REQ;
      lmp_accept_connection_req_from_dm(ulc, peer_id, 0);
    } else {
      hci_conn_request_indication(ulc, peer_id, BT_ACL);
    }
  } else {
    hci_conn_request_indication(ulc, peer_id, BT_ACL);
  }
  acl_link_mgmt_info->received_req_flag |= ACL_CONNECTION_REQ;
}

void lmp_slot_offset_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  acl_link_mgmt_info->device_status_flags |= SLOT_OFFSET_RCVD;
  acl_link_mgmt_info->slot_offset = *GET_LMP_SLOT_OFFSET_PARAMETER_SLOT_OFFSET(bt_pkt_start);
  memcpy(acl_dev_mgmt_info->bd_addr, &GET_LMP_SLOT_OFFSET_PARAMETER_BD_ADDR(bt_pkt_start), MAC_ADDR_LEN);
}

void lmp_page_mode_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_page_scan_mode_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_encryption_key_size_mask_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_encryption_key_size_mask_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_pause_encryption_aes_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint8_t ext_opcode = GET_LMP_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(bt_pkt_start);
  if ((acl_link_mgmt_info->lmp_ext_tx_pending == 0) || (ext_opcode != acl_link_mgmt_info->lmp_tx_pending_ext_opcode )) {
    return;
  } else {
    acl_link_mgmt_info->lmp_ext_tx_pending         = 0;
    acl_link_mgmt_info->lmp_tx_pending_ext_opcode = 0;
  }
  call_lmp_accepted_ext_rx_handler(ulc, pkb);

}

void lmp_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint8_t ext_opcode = GET_LMP_NOT_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(bt_pkt_start);
  if ((acl_link_mgmt_info->lmp_ext_tx_pending == 0) || (ext_opcode != acl_link_mgmt_info->lmp_tx_pending_ext_opcode )) {
    return;
  } else {
    acl_link_mgmt_info->lmp_ext_tx_pending         = 0;
    acl_link_mgmt_info->lmp_tx_pending_ext_opcode = 0;
  }
  call_lmp_not_accepted_ext_rx_handler(ulc, pkb);
}

void lmp_features_req_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t feature_bit_map_type = GET_LMP_FEATURES_REQ_EXT_PARAMETER_FEATURES_PAGE(bt_pkt_start);
  uint8_t tid = GET_LMP_FEATURES_REQ_EXT_TID(bt_pkt_start);

  if (feature_bit_map_type == PAGE_MODE_1) {
    memcpy(acl_peer_info->acl_dev_mgmt_info.ext_features_bitmap, &GET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(bt_pkt_start), EXT_FEATURE_MASK_LENGTH);
    acl_peer_info->acl_link_mgmt_info.device_status_flags |= DEVICE_EXT_FEATURES_RCVD;
  } else if (feature_bit_map_type == PAGE_MODE_2) {
    memcpy(acl_peer_info->acl_dev_mgmt_info.ext_features_bitmap_page2, &GET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(bt_pkt_start), EXT_FEATURE_MASK_LENGTH);
    acl_peer_info->acl_link_mgmt_info.device_status_flags |= DEVICE_EXT_FEATURES_RCVD;
  } else if (feature_bit_map_type == PAGE_MODE_0) {
    memcpy(acl_peer_info->acl_dev_mgmt_info.features_bitmap, &GET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(bt_pkt_start), FEATURE_MASK_LENGTH);
  }
  acl_peer_info->acl_dev_mgmt_info.max_page_num = GET_LMP_FEATURES_REQ_EXT_PARAMETER_MAX_SUPPORTED_PAGE(bt_pkt_start);
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_FEATURES_RES_EXT, peer_id, tid), &feature_bit_map_type);
}

void lmp_features_res_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t feature_bit_map_type = GET_LMP_FEATURES_RES_EXT_PARAMETER_FEATURES_PAGE(bt_pkt_start);

  if (feature_bit_map_type == PAGE_MODE_1) {
    memcpy(acl_peer_info->acl_dev_mgmt_info.ext_features_bitmap, &GET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(bt_pkt_start), EXT_FEATURE_MASK_LENGTH);
    acl_peer_info->acl_link_mgmt_info.device_status_flags |= DEVICE_EXT_FEATURES_RCVD;
  } else if (feature_bit_map_type == PAGE_MODE_2) {
    memcpy(acl_peer_info->acl_dev_mgmt_info.ext_features_bitmap_page2, &GET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(bt_pkt_start), EXT_FEATURE_MASK_LENGTH);
    acl_peer_info->acl_link_mgmt_info.device_status_flags |= DEVICE_EXT_FEATURES_RCVD;
  } else if (feature_bit_map_type == PAGE_MODE_0) {
    memcpy(acl_peer_info->acl_dev_mgmt_info.features_bitmap, &GET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(bt_pkt_start), FEATURE_MASK_LENGTH);
  }

  acl_peer_info->acl_dev_mgmt_info.max_page_num = GET_LMP_FEATURES_RES_EXT_PARAMETER_MAX_SUPPORTED_PAGE(bt_pkt_start);

  if (acl_peer_info->acl_link_mgmt_info.check_pending_req_from_hci & FEATURES_REQ_EXT_SENT) {
    acl_peer_info->acl_link_mgmt_info.check_pending_req_from_hci &= ~FEATURES_REQ_EXT_SENT;
    /* Indicate Read remote supported extended features complete to HCI */
    hci_read_remote_ext_features_complete(ulc, BT_SUCCESS_STATUS, peer_id, feature_bit_map_type);
  } else if (acl_peer_info->acl_link_mgmt_info.conn_purpose == TEMPORARY_CONN_FOR_REMOTE_NAME_RES) {
    /*This case comes when Feature req response comes as part of Remote name request*/
    /* Indicating extended features notification to host */
    hci_remote_host_supp_features_notification_event(ulc, peer_id);
  }
  if (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE) {
    if ((acl_peer_info->acl_link_mgmt_info.lmp_connection_state == EXT_FEATURES_REQ_SENT_STATE)
        || (acl_peer_info->acl_link_mgmt_info.lmp_connection_state == EXT_FEATURES_REQ_PAGE_2_SENT_STATE)) {
      lmp_process_central_conn_state(ulc, peer_id, EXT_FEATURES_RESP_RCVD, 0);
    }
  } else if ((acl_peer_info->acl_link_mgmt_info.lmp_slave_connection_state == SLAVE_EXT_FEATURES_REQ_SENT_STATE)
             || (acl_peer_info->acl_link_mgmt_info.lmp_slave_connection_state == SLAVE_EXT_FEATURES_REQ_PAGE2_SENT_STATE)) {
    lmp_process_peripheral_conn_state(ulc, peer_id, EXT_FEATURES_RESP_RCVD, 0);
  }
}

void lmp_clk_adj_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_clk_adj_ack_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_clk_adj_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_packet_type_table_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t lmp_input_params[2];
  uint32_t lmp_control_word;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_PACKET_TYPE_TABLE_REQ_TID(bt_pkt_start);

  if (acl_link_mgmt_info->check_pending_msg_flag & PTT_REQ_SENT) {
    lmp_input_params[0] = GET_LMP_PACKET_TYPE_TABLE_REQ_EXTENDED_OPCODE(bt_pkt_start);
    lmp_input_params[1] = LMP_ERROR_TRANSACTION_COLLISION;
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NOT_ACCEPTED_EXT, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
  } else {
      if (GET_LMP_PACKET_TYPE_TABLE_REQ_PARAMETER_PACKET_TYPE_TABLE(bt_pkt_start) == BT_EDR_MODE) {
        acl_dev_mgmt_info->ptt = BT_EDR_MODE;
      } else {
        acl_dev_mgmt_info->ptt = BT_BR_MODE;
      }
      acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
      acl_link_mgmt_info->device_status_flags |= DEVICE_PTT_RCVD;
      lmp_input_params[0] = GET_LMP_PACKET_TYPE_TABLE_REQ_EXTENDED_OPCODE(bt_pkt_start);
      lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_ACCEPTED_EXT, peer_id, tid);
      call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
  }
}

void lmp_pause_encryption_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_resume_encryption_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_ping_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_ping_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_sam_set_type0_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_sam_define_map_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_sam_switch_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_max_slot_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  acl_dev_mgmt_info->tx_max_slots = acl_dev_mgmt_info->requested_max_slot;
  /* Indicate change of max slots to HCI */
  hci_max_slots_changed_indication(ulc, peer_id);
  acl_link_mgmt_info->max_acl_pkt_len = bt_dev_info->btc_dev_mgmt_info.max_acl_pkt_len;
  if (acl_link_mgmt_info->check_pending_req_from_hci & CHANGE_PKT_TYPE) {
    update_new_acl_pkt_type(acl_link_mgmt_info);
    hci_conn_pkt_type_changed_event(ulc, peer_id, BT_ACL, BT_SUCCESS_STATUS);
  } else {
    if (acl_link_mgmt_info->lmp_connection_state == MAX_SLOT_REQ_SENT_STATE) {
      lmp_process_central_conn_state(ulc, peer_id, 0, 0);
    }
  }
}

void lmp_host_connection_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  /*Set status flag as Host connection completed*/
  acl_link_mgmt_info->device_status_flags |= HOST_CONN_REQ_COMPLETED;
  if ((acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) && (bt_dev_info->btc_dev_mgmt_info.dev_status_flags & ROLE_CHANGED)) {
    acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_PROCESS_CONN_START;
    lmp_process_peripheral_conn_state(ulc, peer_id, 0, 0);
  } else {
    if (acl_link_mgmt_info->lmp_connection_state == HOST_CONNECTION_REQ_SENT_STATE) {
      lmp_process_central_conn_state(ulc, peer_id, HOST_CONNECTION_REQ_ACCEPTED, 0);
    }
  }
}

void lmp_encryption_mode_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_encryption_key_size_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_start_encryption_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_stop_encryption_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_switch_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  bt_start_role_switch(ulc, peer_id);
}

void lmp_max_slot_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}
void lmp_host_connection_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_encryption_mode_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}
void lmp_encryption_key_size_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}
void lmp_start_encryption_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}
void lmp_stop_encryption_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}
void lmp_switch_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_packet_type_table_req_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  if (acl_link_mgmt_info->check_pending_msg_flag & PTT_REQ_SENT) {
    if (acl_link_mgmt_info->lmp_connection_state == PTT_REQ_SENT_STATE) {
      lmp_process_central_conn_state(ulc, peer_id, PTT_ACCEPTED, 0);
    }
  }
  if ((acl_link_mgmt_info->check_slave_pending_msg_flag & SLAVE_PTT_REQ_SENT)
      && (acl_link_mgmt_info->lmp_slave_connection_state == SLAVE_PTT_REQ_SENT_STATE)) {
    lmp_process_peripheral_conn_state(ulc, peer_id, PTT_ACCEPTED, 0);
  }
  if (acl_link_mgmt_info->check_pending_req_from_hci & CHANGE_PKT_TYPE) {
    acl_dev_mgmt_info->ptt = acl_dev_mgmt_info->requested_ptt;
    acl_link_mgmt_info->device_status_flags &= ~PAUSE_ACLD_TRAFFIC;
    update_new_acl_pkt_type(acl_link_mgmt_info);
    hci_conn_pkt_type_changed_event(ulc, peer_id, BT_ACL, BT_SUCCESS_STATUS);
  }
}

void lmp_eSCO_link_req_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_remove_eSCO_link_req_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_packet_type_table_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  uint8_t  rejection_reason = GET_LMP_NOT_ACCEPTED_EXT_PARAMETER_ERROR_CODE(bt_pkt_start);
  if (acl_link_mgmt_info->check_pending_msg_flag & PTT_REQ_SENT) {
    if (acl_link_mgmt_info->lmp_connection_state == PTT_REQ_SENT_STATE) {
      lmp_process_central_conn_state(ulc, peer_id, PTT_REJECTED, rejection_reason);
    }
  }
  if ((acl_link_mgmt_info->check_slave_pending_msg_flag & SLAVE_PTT_REQ_SENT)
      && (acl_link_mgmt_info->lmp_connection_state == SLAVE_PTT_REQ_SENT_STATE)) {
    lmp_process_peripheral_conn_state(ulc, peer_id, PTT_REJECTED, rejection_reason);
  }
  if (acl_link_mgmt_info->check_pending_req_from_hci & CHANGE_PKT_TYPE) {
    hci_conn_pkt_type_changed_event(ulc, peer_id, BT_ACL, UNSPECIFIED_ERROR);
    acl_link_mgmt_info->device_status_flags &= ~PAUSE_ACLD_TRAFFIC;
  }
}

void lmp_eSCO_link_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_remove_eSCO_link_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_pause_encryption_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_io_capability_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

