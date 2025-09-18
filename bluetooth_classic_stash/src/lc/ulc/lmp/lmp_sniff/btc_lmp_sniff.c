#include "lmp.h"
#include "btc_lmp_power_control.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_hci_events.h"
#include "btc_lmp_sniff.h"
#include "btc_dm_sniff.h"
#include "hci_cmd_get_defines_autogen.h"

void lmp_sniff_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SNIFF_REQ_CMD);
  SET_LMP_SNIFF_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SNIFF_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SNIFF_REQ_PARAMETER_TIMING_CONTROL_FLAGS_1(lmp_tx_pkt->data, 0);
  SET_LMP_SNIFF_REQ_PARAMETER_D_SNIFF(lmp_tx_pkt->data, sniff_params->d_sniff);
  SET_LMP_SNIFF_REQ_PARAMETER_T_SNIFF(lmp_tx_pkt->data, sniff_params->t_sniff);
  SET_LMP_SNIFF_REQ_PARAMETER_TIMING_CONTROL_FLAGS_2(lmp_tx_pkt->data, sniff_params->sniff_attempt);
  SET_LMP_SNIFF_REQ_PARAMETER_SNIFF_TIMEOUT(lmp_tx_pkt->data, sniff_params->sniff_tout);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_SNIFF_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void extract_sniff_parameters_from_packet(uint8_t *bt_pkt_start, sniff_params_t *sniff_params)
{
    sniff_params->init_flag     = GET_LMP_SNIFF_REQ_PARAMETER_TIMING_CONTROL_FLAGS_1(bt_pkt_start);
    sniff_params->d_sniff       = *GET_LMP_SNIFF_REQ_PARAMETER_D_SNIFF(bt_pkt_start);
    sniff_params->t_sniff       = *GET_LMP_SNIFF_REQ_PARAMETER_T_SNIFF(bt_pkt_start);
    sniff_params->sniff_attempt = *GET_LMP_SNIFF_REQ_PARAMETER_TIMING_CONTROL_FLAGS_2(bt_pkt_start);
    sniff_params->sniff_tout    = *GET_LMP_SNIFF_REQ_PARAMETER_SNIFF_TIMEOUT(bt_pkt_start);
}

void lmp_sniff_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_SNIFF_REQ_TID(bt_pkt_start);
  uint8_t opcode = GET_LMP_SNIFF_REQ_OPCODE(bt_pkt_start);
  lmp_input_params[0] = opcode;
  if (btc_dev_info->btc_dev_mgmt_info.features_bitmap[0] & LMP_SNIFF) {  
    if (acl_dev_mgmt_info->link_policy_settings & ENABLE_SNIFF_MODE) {
      extract_sniff_parameters_from_packet(bt_pkt_start, sniff_params);
      uint16_t d_sniff_temp = sniff_params->d_sniff;
      uint16_t t_sniff_temp = sniff_params->t_sniff;
      // Check LMP_Supervision_Timeout < 3 * Sniff_interval, and if so, reject sniff
      REJECT_SNIFF_IF_SUPERVISION_TIMEOUT_INVALID(ulc, peer_id, tid, acl_link_mgmt_info, sniff_params, lmp_input_params);
      // Align multiple anchor points if other ACL connection with sniff exists and are overlapping
      uint16_t priority_roles = get_priority_role_running(ulc, peer_id);
      HANDLE_SNIFF_ANCHOR_ALIGNMENT(ulc, priority_roles, peer_id, tid, sniff_params, lmp_input_params, d_sniff_temp);
      // If anchor points are overlapping/sniff interval is changed, then request for sniff with new parameters
      if (SNIFF_PARAMS_INVALID(sniff_params, t_sniff_temp, d_sniff_temp)) {
        sniff_params->d_sniff = d_sniff_temp;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SNIFF_REQ, 0, peer_id, tid), NULL);
      } else {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
        // Request timing accuracy for slave sniff mode RX window calculation
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_TIMING_ACCURACY_REQ, 0, peer_id, tid), NULL);
        if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
          acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_TRANSITION_MODE;
          acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
        } else {
          acl_link_mgmt_info->device_status_flags &= ~(PAUSE_ACLD_TRAFFIC);
          acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_MODE;
          hci_mode_change_event(ulc, acl_dev_mgmt_info->conn_handle, BT_SUCCESS_STATUS, acl_link_mgmt_info->connected_sub_state, sniff_params->t_sniff);
        }
        // Handle sniff subrating if parameters are set and feature is supported
          if (IS_SNIFF_SUBRATE_PARAMS_SET(sniff_params)) {
          sniff_params->max_remote_sniff_subrate = dm_calc_remote_max_sniff_subrate(ulc, peer_id);
          calc_sniff_subrating_instant(ulc, peer_id);
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_SNIFF_SUBRATING_REQ, peer_id, tid), NULL);
        }
      }
    } else {
      // Sniff mode not enabled in link policy settings
      lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
    }
  } else {
    // LMP sniff feature not supported
    lmp_input_params[1] = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  }
}

void lmp_unsniff_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_UNSNIFF_REQ_CMD);
  SET_LMP_UNSNIFF_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_UNSNIFF_REQ_OPCODE(lmp_tx_pkt->data);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_UNSNIFF_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_unsniff_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t lmp_input_params;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_UNSNIFF_REQ_TID(bt_pkt_start);
  if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
    acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_TRANSITION_MODE;
    acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
  } else {
    exit_sniff_mode(ulc, peer_id);
    hci_mode_change_event(ulc, acl_dev_mgmt_info->conn_handle, BT_SUCCESS_STATUS, acl_link_mgmt_info->connected_sub_state, 0);
  }
  lmp_input_params = GET_LMP_UNSNIFF_REQ_OPCODE(bt_pkt_start);
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
}


void lmp_sniff_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  sniff_params_t *sniff_params = &(acl_peer_info->sniff_params);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_UNSNIFF_REQ_TID(bt_pkt_start);
  acl_link_mgmt_info->device_status_flags &= ~(PAUSE_ACLD_TRAFFIC);
  acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_MODE;
  if (IS_SNIFF_SUBRATE_PARAMS_SET(sniff_params)) {
    sniff_params->max_remote_sniff_subrate = dm_calc_remote_max_sniff_subrate(ulc, peer_id);
    calc_sniff_subrating_instant(ulc, peer_id);
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_SNIFF_SUBRATING_REQ, peer_id, tid), NULL);
  }
  // Request timing accuracy for slave sniff mode RX window calculation
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_TIMING_ACCURACY_REQ, 0, peer_id, tid), NULL);
  bt_mode_change_event_to_hci(ulc, peer_id, BT_SUCCESS_STATUS, sniff_params->t_sniff);
}

void lmp_unsniff_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  if (IS_IN_SNIFF_TRANSITION_MODE(acl_link_mgmt_info)) {
    acl_link_mgmt_info->device_status_flags &= (~(PAUSE_ACLD_TRAFFIC));
  }
  exit_sniff_mode(ulc, peer_id);
  bt_mode_change_event_to_hci(ulc, peer_id, BT_SUCCESS_STATUS, 0);
}

void lmp_sniff_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  acl_link_mgmt_info->device_status_flags &= ~(PAUSE_ACLD_TRAFFIC);
  uint8_t rejection_reason = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  exit_sniff_mode(ulc, peer_id);
  bt_mode_change_event_to_hci(ulc, peer_id, rejection_reason, 0);
}

void lmp_supervision_timeout_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SUPERVISION_TIMEOUT_CMD);
  SET_LMP_SUPERVISION_TIMEOUT_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SUPERVISION_TIMEOUT_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SUPERVISION_TIMEOUT_PARAMETER_SUPERVISION_TIMEOUT(lmp_tx_pkt->data, acl_link_mgmt_info->link_supervision_timeout);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_supervision_timeout_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_SUPERVISION_TIMEOUT_TID(bt_pkt_start);
  if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
    lmp_input_params[0] = GET_LMP_SUPERVISION_TIMEOUT_OPCODE(bt_pkt_start);
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  } else {
    acl_link_mgmt_info->link_supervision_timeout = *GET_LMP_SUPERVISION_TIMEOUT_PARAMETER_SUPERVISION_TIMEOUT(bt_pkt_start);
    if (acl_link_mgmt_info->link_supervision_timeout) {
      acl_link_mgmt_info->link_supervision_enabled = LINK_SUPERVISION_ENABLED;
      /* Indicate change of link supervision timeout to HCI */
      hci_link_supervision_timeout(ulc, peer_id);
    } else {
      acl_link_mgmt_info->link_supervision_enabled = LINK_SUPERVISION_DISABLED;
    }
  }
}

void lmp_sniff_subrating_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SNIFF_SUBRATING_REQ_CMD);
  SET_LMP_SNIFF_SUBRATING_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SNIFF_SUBRATING_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_MAX_SNIFF_SUBRATE(lmp_tx_pkt->data, sniff_params->max_remote_sniff_subrate);
  SET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_MIN_SNIFF_MODE_TIEOUT(lmp_tx_pkt->data, sniff_params->min_remote_sniff_subrating_tout);
  SET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_SNIFF_SUBRATING_INSTANT(lmp_tx_pkt->data, sniff_params->sniff_subrating_instant);
  acl_peer_info->acl_link_mgmt_info.lmp_ext_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_ext_opcode = LMP_SNIFF_SUBRATING_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_sniff_subrating_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SNIFF_SUBRATING_RES_CMD);
  SET_LMP_SNIFF_SUBRATING_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SNIFF_SUBRATING_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SNIFF_SUBRATING_RES_PARAMETER_MAX_SNIFF_SUBRATE(lmp_tx_pkt->data, sniff_params->max_remote_sniff_subrate);
  SET_LMP_SNIFF_SUBRATING_RES_PARAMETER_MIN_SNIFF_MODE_TIMEOUT(lmp_tx_pkt->data, sniff_params->min_remote_sniff_subrating_tout);
  SET_LMP_SNIFF_SUBRATING_RES_PARAMETER_SNIFF_SUBRATING_INSTANT(lmp_tx_pkt->data, sniff_params->sniff_subrating_instant);
  acl_peer_info->acl_link_mgmt_info.lmp_ext_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_ext_opcode = LMP_SNIFF_SUBRATING_RES;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_sniff_subrating_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params   = &acl_peer_info->sniff_params;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_SET_AFH_TID(bt_pkt_start);

  if (IS_IN_SNIFF_MODE(acl_link_mgmt_info)) {
    sniff_params->max_local_sniff_subrate = GET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_MAX_SNIFF_SUBRATE(bt_pkt_start);
    sniff_params->subrating_tout_rcvd = *GET_LMP_SNIFF_SUBRATING_REQ_PARAMETER_MIN_SNIFF_MODE_TIEOUT(bt_pkt_start);
    if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
      sniff_params->sniff_subrating_instant = (*GET_LMP_SNIFF_SUBRATING_RES_PARAMETER_SNIFF_SUBRATING_INSTANT(bt_pkt_start) & 0x7FFFFFF);
    } else {
      calc_sniff_subrating_instant(ulc, peer_id);
    }
    sniff_params->max_remote_sniff_subrate = dm_calc_remote_max_sniff_subrate(ulc, peer_id);
    lmp_input_params[0] = LMP_SNIFF_SUBRATING_RES;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_SNIFF_SUBRATING_RES, peer_id, tid), &lmp_input_params);
    hci_sniff_subrating_event(ulc, peer_id, BT_STATUS_SUCCESS);
  } else {
    lmp_input_params[0] = LMP_SNIFF_SUBRATING_REQ;
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NOT_ACCEPTED_EXT, peer_id, tid), &lmp_input_params);
  }
}

void lmp_sniff_subrating_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params   = &acl_peer_info->sniff_params;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  sniff_params->max_local_sniff_subrate = GET_LMP_SNIFF_SUBRATING_RES_PARAMETER_MAX_SNIFF_SUBRATE(bt_pkt_start);
  sniff_params->subrating_tout_rcvd     = *GET_LMP_SNIFF_SUBRATING_RES_PARAMETER_MIN_SNIFF_MODE_TIMEOUT(bt_pkt_start);
  if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
    sniff_params->sniff_subrating_instant = (*GET_LMP_SNIFF_SUBRATING_RES_PARAMETER_SNIFF_SUBRATING_INSTANT(bt_pkt_start) & 0x7FFFFFF);
  }
  if (btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_SNIFF_SUBR) {
    sniff_params->sniff_subrating_enable = SNIFF_SUBR_ENABLE;
  }
  hci_sniff_subrating_event(ulc, peer_id, BT_STATUS_SUCCESS);
}

void lmp_sniff_subrating_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;
  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_SNIFF_SUBRATING_REQ with peer id - %d, error code - %d \n", peer_id, error_code);
}

void lmp_enable_or_disable_sniff_mode(ulc_t *ulc, uint8_t peer_id, uint8_t enable)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  if (enable == SNIFF_MODE_ENABLE) {
    if (acl_dev_mgmt_info->link_policy_settings & ENABLE_SNIFF_MODE) {
      if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
        acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_TRANSITION_MODE;
        acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
      }
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SNIFF_REQ, 0, peer_id, tid), NULL);
    } else {
      bt_mode_change_event_to_hci(ulc, peer_id, COMMAND_DISALLOWED, 0);
    }
  } else {
    if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
      acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_TRANSITION_MODE;
      acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
    }
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_UNSNIFF_REQ, 0, peer_id, tid), NULL);
  }
}

uint8_t lmp_change_sniff_subrating_params_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb)
{
  uint8_t status;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  sniff_params_t *sniff_params = &(acl_peer_info->sniff_params);
  uint8_t tid;
  tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  if (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_SNIFF_SUBR)) {
    status = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
  } else {
    sniff_params->remote_max_sniff_latency = GET_SNIFF_SUBRATING_MAX_LATENCY(pkb->data);
    sniff_params->min_remote_sniff_subrating_tout = GET_SNIFF_SUBRATING_MIN_REMOTE_TIMEOUT(pkb->data);
    sniff_params->min_local_sniff_subrating_tout = GET_SNIFF_SUBRATING_MIN_LOCAL_TIMEOUT(pkb->data);
    sniff_params->sniff_subrate_params_set = SNIFF_SUBRATE_PARAMS_SET;
    if (acl_link_mgmt_info->connected_sub_state == CONNECTED_SNIFF_MODE) {
      sniff_params->max_remote_sniff_subrate = dm_calc_remote_max_sniff_subrate(ulc, peer_id);
      calc_sniff_subrating_instant(ulc, peer_id);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_SNIFF_SUBRATING_REQ, peer_id, tid), NULL);
    }
    status = BT_SUCCESS_STATUS;
  }
  return status;
}

