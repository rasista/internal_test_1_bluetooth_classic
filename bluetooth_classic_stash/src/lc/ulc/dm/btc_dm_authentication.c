#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_key_mgmt_algos.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include "btc_dm_authentication.h"
#include "btc_lmp_legacy_authentication.h"


void lmp_process_central_authentication_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code) /* Ref:V2-P:F-3.1 */
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  uint32_t rr = 0;
  uint32_t lmp_control_word;
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  uint8_t rand_num[RAND_NUM_SIZE];
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  switch (acl_link_mgmt_info->lmp_connection_state) {
    case HCI_LINK_KEY_REQUEST_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == LINK_KEY_NOT_PRESENT) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_LINK_KEY_REQ_SENT;
        if ((btc_dev_mgmt_info->dev_status_flags & SIMPLE_PAIRING_EN) && (btc_dev_mgmt_info->ext_features_bitmap[0] & LMP_HOST_SSP) && (acl_dev_mgmt_info->features_bitmap[6] & LMP_SIMPLE_PAIR) && (acl_dev_mgmt_info->ext_features_bitmap[0] & LMP_HOST_SSP)) {
          acl_enc_info->pairing_procedure = SECURE_SIMPLE_PAIRING;
          acl_link_mgmt_info->check_pending_msg_flag |= IO_CAP_REQ_TO_HCI_SENT;
          acl_link_mgmt_info->lmp_connection_state = HCI_IO_CAP_REQ_SENT_STATE;
          hci_io_cap_req_event(ulc, peer_id);
        } else {
          acl_enc_info->pairing_procedure = LEGACY_PAIRING;
          acl_link_mgmt_info->check_pending_msg_flag |= PIN_CODE_REQ_TO_HCI_SENT;
          acl_link_mgmt_info->lmp_connection_state = HCI_PIN_CODE_REQUEST_SENT_STATE;
          hci_pin_code_req(ulc, peer_id);
        }
      } else if (lmp_recvd_pkt_flag == LINK_KEY_PRESENT) {
        if (acl_link_mgmt_info->remote_initiated_conn == 0) {
          /* Donot Initiate AU_RAND, when remote initiated connection */
          acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_LINK_KEY_REQ_SENT;
          acl_link_mgmt_info->check_pending_msg_flag |= LMP_AU_RAND_SENT;
          acl_link_mgmt_info->lmp_connection_state = LMP_AU_RAND_SENT_STATE;
          generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
          e1_algo(acl_enc_info->link_key, lmp_input_params, acl_dev_mgmt_info->bd_addr, acl_enc_info->auth_link_key);
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_AU_RAND, 0, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        }
      }
      break;
    }
    case HCI_PIN_CODE_REQUEST_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == PIN_CODE_REPLY) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~PIN_CODE_REQ_TO_HCI_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_IN_RAND_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_IN_RAND_SENT_STATE;
        generate_rand_num(RAND_NUM_SIZE, acl_enc_info->device_key, PSEUDO_RAND_NUM);
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_IN_RAND, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, acl_enc_info->device_key);
      } else if (lmp_recvd_pkt_flag == PIN_CODE_NEG_REPLY) {
        if (acl_link_mgmt_info->check_pending_msg_flag & PIN_CODE_REQ_TO_HCI_SENT) {
          acl_link_mgmt_info->check_pending_msg_flag &= ~PIN_CODE_REQ_TO_HCI_SENT;
          hci_auth_complete(ulc, peer_id, PIN_OR_KEY_MISSING);
        }
      }
      break;
    }
    case LMP_IN_RAND_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == IN_RAND_ACCEPTED) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_IN_RAND_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_COMB_KEY_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_COMB_KEY_SENT_STATE;
        generate_rand_num(RAND_NUM_SIZE, rand_num, PSEUDO_RAND_NUM);
        memcpy(acl_dev_mgmt_info->local_lk_rand, rand_num, RAND_NUM_SIZE);
        e21_algo(rand_num, btc_dev_mgmt_info->bt_mac_addr, acl_enc_info->device_key);
        *(uint32_t *)&lmp_input_params[0] = *(uint32_t *)&rand_num[0] ^ (*(uint32_t *)&acl_enc_info->link_key[0]);
        *(uint32_t *)&lmp_input_params[4] = *(uint32_t *)&rand_num[4] ^ (*(uint32_t *)&acl_enc_info->link_key[4]);
        *(uint32_t *)&lmp_input_params[8] = *(uint32_t *)&rand_num[8] ^ (*(uint32_t *)&acl_enc_info->link_key[8]);
        *(uint32_t *)&lmp_input_params[12] = *(uint32_t *)&rand_num[12] ^ (*(uint32_t *)&acl_enc_info->link_key[12]);
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_COMB_KEY, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
      } else if (lmp_recvd_pkt_flag == IN_RAND_NOT_ACCEPTED) {
        if (acl_link_mgmt_info->check_pending_msg_flag & LMP_IN_RAND_SENT) {
          acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_IN_RAND_SENT;
          lmp_input_params[0] = CONN_REJECTED_DUE_TO_SECURITY_REASONS;
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_DETACH, 0, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
          hci_auth_complete(ulc, peer_id, CONN_REJECTED_DUE_TO_SECURITY_REASONS);
        }
      }
      break;
    }
    case LMP_COMB_KEY_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == COMB_KEY_RCVD) {
        /*Added for SIG Certification. Test cases passes only with a delay in sending LMP_AU_RAND*/
        for (rr = 0; rr < 100000; rr++) { // This Value needs to be adjusted based on the system clock speed TODO
          acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_COMB_KEY_SENT;
        }
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_AU_RAND_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_AU_RAND_SENT_STATE;
        generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
        e1_algo(acl_enc_info->link_key, lmp_input_params, acl_dev_mgmt_info->bd_addr, acl_enc_info->auth_link_key);
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_AU_RAND, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
      }
      break;
    }
    case LMP_AU_RAND_SENT_STATE:
    {
      // FIXME: Now, waiting for AU_RAND from remote device even after receving SRES
      if (lmp_recvd_pkt_flag == EXPECTED_SRES_RECVD) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_AU_RAND_SENT;
        /* Sending SET_AFH before sending setup complete */
        if ((btc_dev_mgmt_info->features_bitmap[5] & LMP_AFH_CAP_MASTER) && (acl_dev_mgmt_info->features_bitmap[4] & LMP_AFH_CAP_SLAVE)) {
          lmp_input_params[0] = AFH_ENABLE;
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_SET_AFH, 0, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        }
      } else if (lmp_recvd_pkt_flag == WRONG_SRES_RECVD) {
        if (acl_link_mgmt_info->check_pending_msg_flag & LMP_AU_RAND_SENT) {
          acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_AU_RAND_SENT;
          lmp_input_params[0] = AUTHENTICATION_FAILURE;
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_DETACH, 0, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        } else {
          // TODO send unexpected frame kind of error here
        }
      } else if (lmp_recvd_pkt_flag == AU_RAND_REJECTED) {
        if ((btc_dev_mgmt_info->dev_status_flags & SIMPLE_PAIRING_EN) && (btc_dev_mgmt_info->ext_features_bitmap[0] & LMP_HOST_SSP) && (acl_dev_mgmt_info->features_bitmap[6] & LMP_SIMPLE_PAIR) && (acl_dev_mgmt_info->ext_features_bitmap[0] & LMP_HOST_SSP)) {
          acl_enc_info->pairing_procedure = SECURE_SIMPLE_PAIRING;
          acl_link_mgmt_info->check_pending_msg_flag |= IO_CAP_REQ_TO_HCI_SENT;
          acl_link_mgmt_info->lmp_connection_state = HCI_IO_CAP_REQ_SENT_STATE;
          hci_io_cap_req_event(ulc, peer_id);
        } else {
          acl_enc_info->pairing_procedure = LEGACY_PAIRING;
          acl_link_mgmt_info->check_pending_msg_flag |= PIN_CODE_REQ_TO_HCI_SENT;
          acl_link_mgmt_info->lmp_connection_state = HCI_PIN_CODE_REQUEST_SENT_STATE;
          hci_pin_code_req(ulc, peer_id);
        }
      }
      break;
    }
    case ENC_MODE_REQ_SENT_STATE:
    {
      if ((lmp_recvd_pkt_flag == ENCRYPTION_MODE_ACCEPTED) && (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE)) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~ENCRYPTION_MODE_REQ_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= ENC_KEY_SIZE_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = ENC_KEY_SIZE_REQ_SENT_STATE;
        acl_enc_info->enc_key_size = btc_dev_mgmt_info->max_supported_enc_key_size;
        lmp_input_params[0] = btc_dev_mgmt_info->max_supported_enc_key_size;
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ENCRYPTION_KEY_SIZE_REQ, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
      } else if (lmp_recvd_pkt_flag == ENCRYPTION_MODE_NOT_ACCEPTED) {
        hci_enc_changed_event(ulc, acl_dev_mgmt_info->conn_handle, reason_code, ENC_DISABLED);
      }
      break;
    }
    case ENC_KEY_SIZE_REQ_SENT_STATE:
    {
      if ((lmp_recvd_pkt_flag == ENC_KEY_SIZE_ACCEPTED) && (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE)) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~ENC_KEY_SIZE_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = START_ENCRYPTION_SENT_STATE;
        generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_START_ENCRYPTION_REQ, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        e3_algo(acl_enc_info->link_key, lmp_input_params, &acl_enc_info->auth_link_key[4], acl_enc_info->enc_key_kc);
        calculate_kc_prime_from_kc((uint16_t *)acl_enc_info->enc_key_kc, acl_enc_info->enc_key_size, (uint16_t *)acl_enc_info->enc_key_kc);
      }
      /* Request EDR mode (2/3 Mbps) for audio transfers if not already set by remote device */
      if (lmp_recvd_pkt_flag == ENCRYPTION_ACCEPTED) {
        if ((!(acl_link_mgmt_info->device_status_flags & DEVICE_PTT_RCVD)) && (acl_dev_mgmt_info->ptt == BT_BR_MODE)) {
          acl_link_mgmt_info->check_pending_msg_flag |= PTT_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = PTT_REQ_SENT_STATE;
          acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
          lmp_input_params[0] = BT_EDR_MODE;
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PACKET_TYPE_TABLE_REQ, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        }
      }
      break;
    }
    case START_ENCRYPTION_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == ENCRYPTION_ACCEPTED) {
        if (!(acl_link_mgmt_info->device_status_flags & DEVICE_PTT_RCVD)) {
          acl_link_mgmt_info->check_pending_msg_flag |= PTT_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = PTT_REQ_SENT_STATE;
          acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
          lmp_input_params[0] = BT_EDR_MODE;
          lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PACKET_TYPE_TABLE_REQ, peer_id, tid);
          call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
        } else {
          /* Sending SET_AFH before sending setup complete */
          if ((btc_dev_mgmt_info->features_bitmap[5] & LMP_AFH_CAP_MASTER) && (acl_dev_mgmt_info->features_bitmap[4] & LMP_AFH_CAP_SLAVE)) {
            lmp_input_params[0] = AFH_ENABLE;
            lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_SET_AFH, 0, peer_id, tid);
            call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

void lmp_auth_req_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  acl_enc_info->auth_mode = AUTH_RESPONDER;
  acl_link_mgmt_info->remote_initiated_conn = 0; // Clearing as Authentication initiated by stack

  acl_link_mgmt_info->check_pending_req_from_hci |= AUTH_REQ_FROM_HCI;
  if (!(acl_link_mgmt_info->device_status_flags & DEVICE_LINK_KEY_EXISTS)) {
    link_key_info_t *link_key_info = bt_find_link_key(ulc, acl_dev_mgmt_info->bd_addr);
    if (link_key_info) {
      memcpy(acl_enc_info->link_key, link_key_info->link_key, LINK_KEY_LEN);
      acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
    } 
  } 

  if (!(acl_link_mgmt_info->device_status_flags & DEVICE_LINK_KEY_EXISTS)) {
    acl_link_mgmt_info->check_pending_msg_flag |= HCI_LINK_KEY_REQ_SENT;
    acl_link_mgmt_info->lmp_connection_state = HCI_LINK_KEY_REQUEST_SENT_STATE;
    hci_link_key_req(ulc, peer_id);
  } else {
    acl_enc_info->pairing_requirement = PAIRING_NOT_REQUIRED;
    generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
    e1_algo(acl_enc_info->link_key, lmp_input_params, acl_dev_mgmt_info->bd_addr, acl_enc_info->auth_link_key);
    uint32_t lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_AU_RAND, 0, peer_id, tid); 
    call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
  }
}

void lmp_link_key_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[4];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
  if (acl_link_mgmt_info->check_pending_msg_flag & HCI_LINK_KEY_REQ_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
        acl_enc_info->pairing_requirement = PAIRING_NOT_REQUIRED;
      }
      acl_link_mgmt_info->lmp_connection_state = HCI_LINK_KEY_REQUEST_SENT_STATE;
      lmp_process_central_authentication_state(ulc, peer_id, LINK_KEY_PRESENT, 0);
    } else {
      e1_algo(acl_enc_info->link_key, acl_enc_info->device_key, btc_dev_info->btc_dev_mgmt_info.bt_mac_addr, acl_enc_info->auth_link_key);
      *(uint32_t *)&lmp_input_params = *(uint32_t *)acl_enc_info->auth_link_key;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SRES, 0, peer_id, tid), lmp_input_params);
      acl_link_mgmt_info->device_status_flags |= SRES_SENT;
      acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_LINK_KEY_REQ_SENT;
      // TODO: If authentication_enable is set send LMP_AU_RAND
    }
  }
}

void lmp_link_key_neg_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[2];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid =  GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  /* If initiated in LMP */
  if (acl_link_mgmt_info->check_pending_msg_flag & HCI_LINK_KEY_REQ_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
        acl_enc_info->pairing_requirement = PAIRING_REQUIRED;
      }
      acl_link_mgmt_info->lmp_connection_state = HCI_LINK_KEY_REQUEST_SENT_STATE;
      lmp_process_central_authentication_state(ulc, peer_id, LINK_KEY_NOT_PRESENT, 0);
    } else {
      lmp_input_params[0] = PIN_OR_KEY_MISSING;
      lmp_input_params[1] = LMP_AU_RAND;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_LINK_KEY_REQ_SENT;
    }
  }
}

void lmp_pin_code_req_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  /* If initiated in LMP */
  if (acl_link_mgmt_info->check_pending_msg_flag & PIN_CODE_REQ_TO_HCI_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_central_authentication_state(ulc, peer_id, PIN_CODE_REPLY, 0);
    } else {
      acl_link_mgmt_info->check_pending_msg_flag &= ~PIN_CODE_REQ_TO_HCI_SENT;
      e22_algo(acl_enc_info->device_key, acl_enc_info->pin, acl_enc_info->pin_length, btc_dev_info->btc_dev_mgmt_info.bt_mac_addr, acl_enc_info->link_key);
      lmp_input_params = LMP_IN_RAND;
      uint32_t lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid);
      call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
    }
  }
}

void lmp_pin_code_req_neg_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[2];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  /* If initiated in LMP */
  if (acl_link_mgmt_info->check_pending_msg_flag & PIN_CODE_REQ_TO_HCI_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_central_authentication_state(ulc, peer_id, PIN_CODE_NEG_REPLY, 0);
    } else {
      lmp_input_params[0] = LMP_IN_RAND;
      lmp_input_params[1] = PIN_OR_KEY_MISSING;
      uint32_t lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid);
      call_lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
    }
  }
}