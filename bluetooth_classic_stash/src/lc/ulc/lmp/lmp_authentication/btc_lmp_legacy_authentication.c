#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_key_mgmt_algos.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include <unistd.h> 
#include "btc_lmp_legacy_authentication.h"
#include "btc_dm_authentication.h"

void lmp_in_rand_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t *random_num = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_IN_RAND_CMD);
  SET_LMP_IN_RAND_TID(lmp_tx_pkt->data, tid);
  SET_LMP_IN_RAND_OPCODE(lmp_tx_pkt->data);
  SET_LMP_IN_RAND_PARAMETER_RANDOM_NUMBER(lmp_tx_pkt->data, random_num);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_in_rand_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_IN_RAND_TID(bt_pkt_start);
  uint8_t *rx_random_num = &GET_LMP_IN_RAND_PARAMETER_RANDOM_NUMBER(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_IN_RAND_OPCODE(bt_pkt_start);
  if (acl_link_mgmt_info->check_pending_msg_flag & LMP_IN_RAND_SENT) {
    if (IS_FIXED_PIN_ENABLED(btc_dev_mgmt_info)) {
      lmp_input_params[1] = PAIRING_NOT_ALLOWED;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      lmp_process_central_authentication_state(ulc, peer_id, IN_RAND_NOT_ACCEPTED, 0);
    } else {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      e22_algo(rx_random_num, acl_enc_info->pin, acl_enc_info->pin_length, btc_dev_mgmt_info->bt_mac_addr, acl_enc_info->link_key);
      lmp_process_central_authentication_state(ulc, peer_id, IN_RAND_ACCEPTED, 0);
    }
  } else {
    if (IS_FIXED_PIN_ENABLED(btc_dev_mgmt_info)) {
      generate_rand_num(RAND_NUM_SIZE, acl_enc_info->device_key, PSEUDO_RAND_NUM);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_IN_RAND, 0, peer_id, tid), acl_enc_info->device_key);
    } else {
      acl_enc_info->auth_mode = AUTH_INITIATOR;
      acl_link_mgmt_info->check_pending_msg_flag |= PIN_CODE_REQ_TO_HCI_SENT;
      hci_pin_code_req(ulc, peer_id);
      memcpy(acl_enc_info->device_key, rx_random_num, RAND_NUM_SIZE);
    }
  }
}

void lmp_in_rand_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  e22_algo(acl_enc_info->device_key, acl_enc_info->pin, acl_enc_info->pin_length, acl_dev_mgmt_info->bd_addr, acl_enc_info->link_key);
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    lmp_process_central_authentication_state(ulc, peer_id, IN_RAND_ACCEPTED, 0);
  }
}

void lmp_in_rand_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t reason_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  lmp_process_central_authentication_state(ulc, peer_id, IN_RAND_NOT_ACCEPTED, reason_code);
}

void lmp_comb_key_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t *random_num = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_COMB_KEY_CMD);
  SET_LMP_COMB_KEY_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_COMB_KEY_OPCODE(lmp_tx_pkt->data);
  SET_LMP_COMB_KEY_PARAMETER_RANDOM_NUMBER(lmp_tx_pkt->data, random_num);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_comb_key_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_COMB_KEY_TID(bt_pkt_start);
  uint8_t opcode = GET_LMP_COMB_KEY_OPCODE(bt_pkt_start);
  uint8_t *rx_random_num = &GET_LMP_COMB_KEY_PARAMETER_RANDOM_NUMBER(bt_pkt_start);

  if (acl_link_mgmt_info->device_status_flags & DEVICE_LINK_KEY_EXISTS) {
    acl_link_mgmt_info->host_event_notification_flags |= LINK_KEY_CHANGED;
  }
  acl_link_mgmt_info->remote_initiated_conn = 0;
  XOR_BYTE_ARRAYS(rx_random_num, acl_enc_info->link_key, LINK_KEY_UINT32_BLOCKS);
  if (!(memcmp(rx_random_num, acl_dev_mgmt_info->local_lk_rand, RAND_NUM_SIZE))) {
    lmp_input_params[0] = opcode;
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
    return;
  }

  if (acl_link_mgmt_info->check_pending_msg_flag & LMP_COMB_KEY_SENT) {
    e21_algo(rx_random_num, acl_dev_mgmt_info->bd_addr, acl_enc_info->link_key);
    XOR_BYTE_ARRAYS(acl_enc_info->link_key, acl_enc_info->device_key, LINK_KEY_UINT32_BLOCKS);
    acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
    lmp_process_central_authentication_state(ulc, peer_id, COMB_KEY_RCVD, 0);
  } else {
    generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
    e21_algo(lmp_input_params, btc_dev_mgmt_info->bt_mac_addr, acl_enc_info->device_key);
    XOR_BYTE_ARRAYS(lmp_input_params, acl_enc_info->link_key, LINK_KEY_UINT32_BLOCKS);
    uint32_t lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_COMB_KEY, 0, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
    e21_algo(rx_random_num, acl_dev_mgmt_info->bd_addr, acl_enc_info->link_key);
    XOR_BYTE_ARRAYS(acl_enc_info->link_key, acl_enc_info->device_key, LINK_KEY_UINT32_BLOCKS);
    acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
  }
}


void lmp_au_rand_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t *random_num = ((uint8_t *)lmp_input_params);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_AU_RAND_CMD);
  if ((acl_peer_info->acl_link_mgmt_info.sec_sm_flags_1 & AURAND_NOT_ACCEPTED)) {
    SET_FLAG(acl_peer_info->acl_link_mgmt_info.sec_sm_flags_1, SENT_AU_RAND);
  }
  SET_LMP_AU_RAND_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_AU_RAND_OPCODE(lmp_tx_pkt->data);
  SET_LMP_AU_RAND_PARAMETER_RANDOM_NUMBER(lmp_tx_pkt->data, random_num);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_AU_RAND;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_au_rand_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_AU_RAND_TID(bt_pkt_start);
  uint8_t *random_number = &GET_LMP_AU_RAND_PARAMETER_RANDOM_NUMBER(bt_pkt_start);
  uint32_t lmp_control_word;
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  uint8_t opcode = GET_LMP_AU_RAND_OPCODE(bt_pkt_start);

  /* Security Fix: Reject unsolicited AU_RAND packets before host connection is established */
  if (!(acl_link_mgmt_info->device_status_flags & HOST_CONN_REQ_COMPLETED)) {
    lmp_input_params[0] = opcode;
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
    return;
  }

  if (!(acl_link_mgmt_info->device_status_flags & DEVICE_LINK_KEY_EXISTS)) {
    link_key_info_t *link_key_info;
    link_key_info = bt_find_link_key(ulc, acl_dev_mgmt_info->bd_addr);
    if (link_key_info) {
      memcpy(acl_enc_info->link_key, link_key_info->link_key, LINK_KEY_LEN);
      acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
    }
  }
  
  /* when link key exists*/
  if (acl_link_mgmt_info->device_status_flags & DEVICE_LINK_KEY_EXISTS) {
    e1_algo(acl_enc_info->link_key, &GET_LMP_AU_RAND_PARAMETER_RANDOM_NUMBER(bt_pkt_start), btc_dev_mgmt_info->bt_mac_addr, acl_enc_info->auth_link_key);
    *(uint32_t *)lmp_input_params = *(uint32_t *)acl_enc_info->auth_link_key;
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_SRES, 0, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
    acl_link_mgmt_info->device_status_flags |= SRES_SENT;

    if (acl_link_mgmt_info->device_status_flags & SRES_RCVD) {
      acl_link_mgmt_info->device_status_flags &= ~(SRES_SENT | SRES_RCVD);
      if (IS_AUTH_REQ_WITH_PAIRING(acl_link_mgmt_info, acl_enc_info)) {
        uint8_t link_key_type = GET_LINK_KEY_TYPE(acl_enc_info->pairing_procedure, btc_dev_mgmt_info->ssp_link_key_type);
        hci_link_key_notification(ulc, peer_id, link_key_type);
        hci_auth_complete(ulc, peer_id, BT_SUCCESS_STATUS);
        acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        if (IS_AUTH_RESPONDER(acl_enc_info)) {
          lmp_process_central_authentication_state(ulc, peer_id, EXPECTED_SRES_RECVD, 0);
        }
        return;
      } else if (acl_link_mgmt_info->check_pending_req_from_hci & CHANGE_CONN_LINK_KEY) {
        uint8_t link_key_type = GET_LINK_KEY_TYPE_FROM_PAIRING_PROCEDURE(acl_enc_info->pairing_procedure);
        hci_link_key_notification(ulc, peer_id, link_key_type);
        if (acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED) {
          if (IS_BOTH_DEVICES_SUPPORT_PAUSE_ENC(btc_dev_mgmt_info->features_bitmap, acl_dev_mgmt_info->features_bitmap)) {
            acl_link_mgmt_info->host_event_notification_flags |= ENC_KEY_REFRESH;
            acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
            lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PAUSE_ENCRYPTION_REQ, peer_id, tid);
            call_lmp_tx_handler(ulc, lmp_control_word, NULL);
          } else {
            acl_enc_info->change_lk_for_legacy_device = 1;
          }
        } else {
          acl_link_mgmt_info->check_pending_req_from_hci &= ~CHANGE_CONN_LINK_KEY;
          hci_change_conn_link_key_complete(ulc, peer_id, BT_SUCCESS_STATUS);
        }
        return;
      }
    } else if ((!(acl_link_mgmt_info->device_status_flags & AUTHENTICATION_COMPLETE))
        || (acl_link_mgmt_info->host_event_notification_flags & LINK_KEY_CHANGED)) {
        if (acl_link_mgmt_info->remote_initiated_conn == 0) {
          /* Donot Initiate AU_RAND, when Remote Initiated connection */
          generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
          e1_algo(acl_enc_info->link_key, lmp_input_params, acl_dev_mgmt_info->bd_addr, acl_enc_info->auth_link_key);
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_AU_RAND, 0, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        }
    }
  } else {
    memcpy(acl_enc_info->device_key, random_number, LINK_KEY_LEN);
    acl_link_mgmt_info->check_pending_msg_flag |= HCI_LINK_KEY_REQ_SENT;
    acl_enc_info->auth_mode = AUTH_INITIATOR;
    hci_link_key_req(ulc, peer_id);
  }       
}

void lmp_au_rand_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t rejection_reason = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  SET_FLAG(acl_link_mgmt_info->sec_sm_flags_1, AURAND_NOT_ACCEPTED);
  lmp_process_central_authentication_state(ulc, peer_id, AU_RAND_REJECTED, rejection_reason);
}

void lmp_sres_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint32_t auth_link_key = *((uint32_t *)lmp_input_params);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SRES_CMD);
  SET_LMP_SRES_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_SRES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SRES_PARAMETER_AUTHENTICATION_RSP(lmp_tx_pkt->data, auth_link_key);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_sres_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint32_t auth_link_key = *GET_LMP_SRES_PARAMETER_AUTHENTICATION_RSP(bt_pkt_start);
  if (auth_link_key == *(uint32_t *)acl_enc_info->auth_link_key) {
    acl_link_mgmt_info->device_status_flags |= SRES_RCVD;
    acl_link_mgmt_info->device_status_flags |= AUTHENTICATION_COMPLETE;
    if (acl_link_mgmt_info->check_pending_req_from_hci & CHANGE_CONN_LINK_KEY) {
      if (acl_link_mgmt_info->device_status_flags & SRES_SENT) {
        acl_link_mgmt_info->device_status_flags &= ~(SRES_SENT | SRES_RCVD);
        uint8_t link_key_type = GET_LINK_KEY_TYPE_FROM_PAIRING_PROCEDURE(acl_enc_info->pairing_procedure);
        hci_link_key_notification(ulc, peer_id, link_key_type);
        if (acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED) {
          if (IS_BOTH_DEVICES_SUPPORT_PAUSE_ENC(btc_dev_mgmt_info->features_bitmap, acl_dev_mgmt_info->features_bitmap)) {
            acl_link_mgmt_info->host_event_notification_flags |= ENC_KEY_REFRESH;
            acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
            call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PAUSE_ENCRYPTION_REQ, peer_id, CENTERAL_INITIATED_TID), NULL);
          } else {
            acl_enc_info->change_lk_for_legacy_device = 1;
          }
        } else {
          acl_link_mgmt_info->check_pending_req_from_hci &= ~CHANGE_CONN_LINK_KEY;
          hci_change_conn_link_key_complete(ulc, peer_id, BT_SUCCESS_STATUS);
        }
      }
      return;
    } else if (IS_AUTH_REQ_WITHOUT_PAIRING(acl_link_mgmt_info, acl_enc_info)) {
      if (!(acl_link_mgmt_info->sec_sm_flags_1 & AURAND_NOT_ACCEPTED)) {
        acl_link_mgmt_info->sec_sm_flags_1 &= ~SENT_AU_RAND;
      } else {
        if (acl_enc_info->pairing_procedure == LEGACY_PAIRING) {
          hci_link_key_notification(ulc, peer_id, COMB_KEY);
        } else {
          hci_link_key_notification(ulc, peer_id, AUTHENTICATED_COMB_KEY);
        }
      }
      hci_auth_complete(ulc, peer_id, BT_SUCCESS_STATUS);
      acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
      return;
    } else {
      if (acl_link_mgmt_info->device_status_flags & SRES_SENT) {
        acl_link_mgmt_info->device_status_flags &= ~(SRES_SENT | SRES_RCVD);
        if (acl_link_mgmt_info->host_event_notification_flags & LINK_KEY_CHANGED) {
          acl_link_mgmt_info->host_event_notification_flags &= ~LINK_KEY_CHANGED;
          hci_link_key_notification(ulc, peer_id, COMB_KEY);
          return;
        } else {
          if ((acl_enc_info->pairing_procedure == SECURE_SIMPLE_PAIRING)
              && (btc_dev_mgmt_info->ssp_link_key_type == SSP_DEBUG_LINK_KEY)) {
            hci_link_key_notification(ulc, peer_id, DEBUG_COMB_KEY);
          } else {
            if (acl_enc_info->pairing_procedure == LEGACY_PAIRING) {
              hci_link_key_notification(ulc, peer_id, COMB_KEY);
            } else {
              hci_link_key_notification(ulc, peer_id, AUTHENTICATED_COMB_KEY);
            }
          }
        }
      } else {
        return;
      }
    }
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_central_authentication_state(ulc, peer_id, EXPECTED_SRES_RECVD, 0);
    }
  } else {
    if (acl_link_mgmt_info->check_pending_req_from_hci & CHANGE_CONN_LINK_KEY) {
      hci_change_conn_link_key_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    } else if (IS_AUTH_RESPONDER(acl_enc_info)) {
      hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    } else {
      hci_conn_complete_indication(ulc, peer_id, AUTHENTICATION_FAILURE, BT_ACL);
    }
    lmp_process_central_authentication_state(ulc, peer_id, WRONG_SRES_RECVD, 0);
  }
}