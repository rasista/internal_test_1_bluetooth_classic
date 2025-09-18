#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_key_mgmt_algos.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include "btc_dm_simple_secure_pairing.h"
#include "btc_lmp_legacy_authentication.h"
#include "btc_lmp_simple_secure_pairing.h"
#ifdef SCHED_SIM
#include "sw_ecdh.h"
#endif

void lmp_process_ssp_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code)
{
  (void)reason_code;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info;
  acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t confirmation_val[RAND_NUM_SIZE];
  uint8_t lmp_input_params[PUBLIC_KEY_LEN_256];
  uint32_t lmp_control_word;
  uint8_t io_cap[3];
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  
  switch (acl_link_mgmt_info->lmp_connection_state) {
    case HCI_IO_CAP_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == IO_CAPABILITIES_REPLY) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~IO_CAP_REQ_TO_HCI_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_IO_CAP_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_IO_CAP_REQ_SENT_STATE;
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_IO_CAPABILITIES_REQ, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, NULL);
      } else if (lmp_recvd_pkt_flag == IO_CAP_NEG_REPLY) {
        if (acl_link_mgmt_info->check_pending_msg_flag & IO_CAP_REQ_TO_HCI_SENT) {
          acl_link_mgmt_info->check_pending_msg_flag &= ~IO_CAP_REQ_TO_HCI_SENT;
          hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
            hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
            acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
          }
        }
      }
      break;
    }
    case LMP_IO_CAP_REQ_SENT_STATE:
    {
      acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_IO_CAP_REQ_SENT;
      if (lmp_recvd_pkt_flag == IO_CAP_RES_RCVD) {
        hci_io_cap_response_event(ulc, peer_id);
        acl_enc_info->auth_protocol = GET_AUTH_PROTOCOL(acl_enc_info, btc_dev_mgmt_info, acl_dev_mgmt_info);
        if (acl_enc_info->auth_protocol != OUT_OF_BAND) {
          memset(btc_dev_mgmt_info->r, 0, SIZE_OF_R);
        }
        memset(acl_enc_info->r, 0, SIZE_OF_R);
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_ENCAP_HEADER_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_ENCAP_HEADER_SENT_STATE;
        GET_ENCAPSULATED_HEADER(lmp_input_params, btc_dev_mgmt_info);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ENCAPSULATED_HEADER, 0, peer_id, tid), lmp_input_params);
      } else if (lmp_recvd_pkt_flag == IO_CAP_REQ_REJECTED) {
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
    case LMP_ENCAP_HEADER_SENT_STATE:
    {
      acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_ENCAP_HEADER_SENT;
      if (lmp_recvd_pkt_flag == ENCAP_HEADER_ACCEPTED) {
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_ENCAP_PAYLOAD_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_ENCAP_PAYLOAD_SENT_STATE;
        acl_enc_info->encap_payload_cnt = 0;
        if (((btc_dev_mgmt_info->dev_status_flags & PUBLIC_KEY_CALC_PENDING) == 0)) {
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ENCAPSULATED_PAYLOAD, 0, peer_id, tid), GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info));
        } else {
          //TODO
        }
      } else {
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
    case LMP_ENCAP_PAYLOAD_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == PUBLIC_KEY_PAYLOAD_ACCEPTED) {
        acl_link_mgmt_info->lmp_connection_state = PUBLIC_KEY_SENT_STATE;
      } else {
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
    case PUBLIC_KEY_SENT_STATE:
    {
      if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
        if (lmp_recvd_pkt_flag == COMMITMENT_VAL_RCVD) {
          acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_ENCAP_PAYLOAD_SENT;
          acl_link_mgmt_info->check_pending_msg_flag |= LMP_SIMPLE_PAIRING_NUM_SENT;
          acl_link_mgmt_info->lmp_connection_state = LMP_SIMPLE_PAIRING_NUM_SENT_STATE;
          generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);
        }
      } else if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
        if (lmp_recvd_pkt_flag == PUBLIC_KEY_RECEIVED) {
          acl_link_mgmt_info->check_pending_msg_flag |= HCI_PASSKEY_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = HCI_PASSKEY_REQ_SENT_STATE;
          if (IS_LOCAL_DISPLAYS_REMOTE_KEYBOARDS_IO_CAPABILITY(btc_dev_mgmt_info, acl_dev_mgmt_info)) {
            generate_rand_num(4, (uint8_t *)&acl_enc_info->passkey_display, PSEUDO_RAND_NUM);
            acl_enc_info->passkey_display = (acl_enc_info->passkey_display % 1000000);
            memcpy(btc_dev_mgmt_info->r, (uint8_t *)&acl_enc_info->passkey_display, 4);
            memcpy(acl_enc_info->r, (uint8_t *)&acl_enc_info->passkey_display, 4);
            hci_user_passkey_notification(ulc, peer_id);
          } else if (IS_KEYBOARD_ONLY_CAPABILITY(btc_dev_mgmt_info)) {
            acl_enc_info->passkey_entry_round = 0;
            acl_link_mgmt_info->device_status_flags &= ~USER_PASSKEY_RCVD;
            hci_user_passkey_request(ulc, peer_id);
          }
        }
      } else if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
        if (lmp_recvd_pkt_flag == PUBLIC_KEY_RECEIVED) {
          if (btc_dev_mgmt_info->oob_authentication_data == OOB_AUTH_DATA_AVAILABLE) {
            acl_link_mgmt_info->check_pending_msg_flag |= REMOTE_OOB_DATA_REQ_SENT;
            hci_remote_oob_data_request(ulc, peer_id);
          } else {
            acl_link_mgmt_info->check_pending_msg_flag |= LMP_SIMPLE_PAIRING_NUM_SENT;
            acl_link_mgmt_info->lmp_connection_state = LMP_SIMPLE_PAIRING_NUM_SENT_STATE;
            generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
            call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);
          }
        }
      }
      break;
    }
    case HCI_PASSKEY_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == PASSKEY_REPLY_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_SIMPLE_PAIRING_CONFIRM_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_SIMPLE_PAIRING_CONFIRM_SENT_STATE;
        /* Send commitment value calculated on each bit of passkey */
        acl_enc_info->passkey_entry_round = 0;
        generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
        BTC_PRINT(" LMP_SIMPLE_PAIRING_CONFIRM CALCULATION \n");
        BTC_HEX_DUMP("DEVICE PUBLIC KEY :", GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info));
        BTC_HEX_DUMP("REMOTE PUBLIC KEY :", acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info));
        BTC_HEX_DUMP("acl_enc_info->ssp_rand_own:", acl_enc_info->ssp_rand_own, RAND_NUM_SIZE);
        f1_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key,  acl_enc_info->ssp_rand_own,
                (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_input_params);
        BTC_HEX_DUMP("CALCUATED CONFIRMATION_NUM :", lmp_input_params, SIZE_OF_C_VAL);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_CONFIRM, 0, peer_id, tid), lmp_input_params);
      } else {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PASSKEY_FAILED, peer_id, tid), NULL);
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
    case LMP_SIMPLE_PAIRING_CONFIRM_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == COMMITMENT_VAL_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_SIMPLE_PAIRING_NUM_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_SIMPLE_PAIRING_NUM_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);
      } else {
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
    case LMP_SIMPLE_PAIRING_NUM_SENT_STATE:
    {
      acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_SIMPLE_PAIRING_NUM_SENT;
      if (acl_link_mgmt_info->device_status_flags & SENT_SIMPLE_PAIRING_NUM_ACCEPTED) {
        if (lmp_recvd_pkt_flag == LMP_SIMPLE_PAIRING_NUM_RCVD) {
          if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
            if (acl_enc_info->passkey_entry_round < 19) {
              acl_enc_info->passkey_entry_round++;
              acl_link_mgmt_info->check_pending_msg_flag |= LMP_SIMPLE_PAIRING_CONFIRM_SENT;
              acl_link_mgmt_info->lmp_connection_state = LMP_SIMPLE_PAIRING_CONFIRM_SENT_STATE;
              f1_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key, acl_enc_info->ssp_rand_own,
                      (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_input_params);
              call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_CONFIRM, 0, peer_id, tid), lmp_input_params);
            } else {
              if ((peer_id == (btc_dev_mgmt_info->shared_key_calc_pending_slave)) && ((btc_dev_mgmt_info->dev_status_flags & SHARED_KEY_CALC_PENDING) == 0)) {
                io_cap[0] = btc_dev_mgmt_info->io_capabilities;
                io_cap[1] = btc_dev_mgmt_info->oob_authentication_data;
                io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
                BTC_HEX_DUMP("acl_enc_info->dh_key", acl_enc_info->dh_key, GET_DHKEY_LEN(btc_dev_mgmt_info));
                BTC_HEX_DUMP("ssp_rand_peer", acl_enc_info->ssp_rand_peer, RAND_NUM_SIZE);
                BTC_HEX_DUMP("ssp_rand_own", acl_enc_info->ssp_rand_own, RAND_NUM_SIZE);
                BTC_HEX_DUMP("r", btc_dev_mgmt_info->r, RAND_NUM_SIZE);
                BTC_HEX_DUMP("io_cap", io_cap, 3);
                BTC_HEX_DUMP("remote mac addr", acl_dev_mgmt_info->bd_addr, MAC_ADDR_LEN);
                BTC_HEX_DUMP("bt_mac_addr", btc_dev_mgmt_info->bt_mac_addr, MAC_ADDR_LEN);
                f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r, &io_cap[0],
                        btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_input_params);
                
                BTC_HEX_DUMP("INITIATOR CALCULATED DHKEY ", lmp_input_params, RAND_NUM_SIZE);
                acl_link_mgmt_info->check_pending_msg_flag |= LMP_DHKEY_CHECK_SENT;
                acl_link_mgmt_info->lmp_connection_state = LMP_DHKEY_CHECK_SENT_STATE;
                call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
              } else {
                // TODO:wait for DHKEY calculation to finish
                acl_link_mgmt_info->check_pending_msg_flag |= SSP_NUM_RCVD;
              }
            }
          } else if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
            g_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, confirmation_val);
            *(uint32_t *)&confirmation_val[0] %= 1000000;
            acl_link_mgmt_info->check_pending_msg_flag |= HCI_USER_CONFIRMATION_REQ_SENT;
            acl_link_mgmt_info->lmp_connection_state = HCI_USER_CONFIRMATION_REQ_SENT_STATE;
            hci_user_confirmation_req_event(ulc, peer_id, *(uint32_t *)&confirmation_val[0]);
          } else if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
            if ((peer_id == (btc_dev_mgmt_info->shared_key_calc_pending_slave)) && ((btc_dev_mgmt_info->dev_status_flags & SHARED_KEY_CALC_PENDING) == 0)) {
              io_cap[0] = btc_dev_mgmt_info->io_capabilities;
              io_cap[1] = btc_dev_mgmt_info->oob_authentication_data;
              io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
              f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r,
                      &io_cap[0], btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_input_params);
              acl_link_mgmt_info->check_pending_msg_flag |= LMP_DHKEY_CHECK_SENT;
              acl_link_mgmt_info->lmp_connection_state = LMP_DHKEY_CHECK_SENT_STATE;
              call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
            } else {
              // TODO:wait for DHKEY calculation to finish
              acl_link_mgmt_info->check_pending_msg_flag |= SSP_NUM_RCVD;
            }
          }
        }
      } else {
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
    case HCI_USER_CONFIRMATION_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == USER_CONFIRM_REQ_NEG_REPLY_RECVD) {
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NUMERIC_COMPARISON_FAILED, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, NULL);
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      } else {
        *(uint16_t *)&io_cap[0] = (btc_dev_mgmt_info->oob_authentication_data << 8) | btc_dev_mgmt_info->io_capabilities;
        io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
        if ((peer_id == (btc_dev_mgmt_info->shared_key_calc_pending_slave)) && ((btc_dev_mgmt_info->dev_status_flags & SHARED_KEY_CALC_PENDING) == 0)) {
          f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r, io_cap,
                  btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_input_params);
          acl_link_mgmt_info->check_pending_msg_flag |= LMP_DHKEY_CHECK_SENT;
          acl_link_mgmt_info->lmp_connection_state = LMP_DHKEY_CHECK_SENT_STATE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
        } else {
          /* Wait for DHKEY calculation to finish */
          acl_link_mgmt_info->check_pending_msg_flag |= USER_CONFIRMATION_RCVD;
        }
      }
      break;
    }
    case LMP_DHKEY_CHECK_SENT_STATE:
    {
      acl_link_mgmt_info->check_pending_msg_flag &= ~LMP_DHKEY_CHECK_SENT;
      if (lmp_recvd_pkt_flag == LMP_DHKEY_CHECK_PASS) {
        hci_simple_pairing_complete(ulc, peer_id, BT_SUCCESS_STATUS);
        /* Link Key calculation */
        if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
          f2_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_peer, acl_enc_info->ssp_rand_own, btc_dev_mgmt_info->key_id,
                  acl_dev_mgmt_info->bd_addr, btc_dev_mgmt_info->bt_mac_addr, acl_enc_info->link_key);
        } else {
          f2_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, btc_dev_mgmt_info->key_id,
                  btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, acl_enc_info->link_key);
        }
        acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
        acl_link_mgmt_info->check_pending_msg_flag |= LMP_AU_RAND_SENT;
        acl_link_mgmt_info->lmp_connection_state = LMP_AU_RAND_SENT_STATE;
        generate_rand_num(RAND_NUM_SIZE, lmp_input_params, PSEUDO_RAND_NUM);
        e1_algo(acl_enc_info->link_key, lmp_input_params, acl_dev_mgmt_info->bd_addr, acl_enc_info->auth_link_key);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_AU_RAND, 0, peer_id, tid), lmp_input_params);
      } else {
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
      break;
    }
  }
}

void lmp_io_cap_req_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[2];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  if (acl_link_mgmt_info->check_pending_msg_flag & IO_CAP_REQ_TO_HCI_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, IO_CAPABILITIES_REPLY, 0);
    } else {
      tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
      if (btc_dev_mgmt_info->authentication_requirement == MITM_PROTCTN_NOT_REQD_NO_BONDING) {
        if (acl_dev_mgmt_info->authentication_requirement != MITM_PROTCTN_NOT_REQD_NO_BONDING) {
          lmp_input_params[0] = LMP_IO_CAPABILITIES_REQ;
          lmp_input_params[1] = PAIRING_NOT_ALLOWED;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NOT_ACCEPTED_EXT, peer_id, tid), lmp_input_params);
          return;
        }
      }
      acl_link_mgmt_info->check_pending_msg_flag &= ~IO_CAP_REQ_TO_HCI_SENT;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_IO_CAPABILITIES_RES, peer_id, tid), NULL);
      acl_enc_info->auth_protocol = GET_AUTH_PROTOCOL(acl_enc_info, btc_dev_mgmt_info, acl_dev_mgmt_info);
      if (acl_enc_info->auth_protocol != OUT_OF_BAND) {
        memset(btc_dev_mgmt_info->r, 0, SIZE_OF_R);
      }
      memset(acl_enc_info->r, 0, SIZE_OF_R);
    }
  } else {
    // TODO check if OOB has to be processed here otherwise send some error response to host here
  }
}

void lmp_io_cap_neg_reply_rcvd_from_hci(ulc_t *ulc, uint8_t peer_id, uint8_t reason_code)
{
  uint8_t tid;
  uint8_t lmp_input_params[2];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  if (acl_link_mgmt_info->check_pending_msg_flag & IO_CAP_REQ_TO_HCI_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, IO_CAP_NEG_REPLY, reason_code);
    } else {
      tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
      acl_link_mgmt_info->check_pending_msg_flag &= ~IO_CAP_REQ_TO_HCI_SENT;
      lmp_input_params[0]= LMP_IO_CAPABILITIES_REQ;
      lmp_input_params[1]= reason_code;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NOT_ACCEPTED_EXT, peer_id, tid), lmp_input_params);
      hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    }
  } else {
    // TODO check if OOB has to be processed here otherwise send some error response to host here
  }
}

void lmp_user_confirm_req_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[MAX_SIZE_OF_INPUT_PARAMS];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  uint8_t io_cap[3];
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  } else {
    tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
  }
  if (acl_link_mgmt_info->check_pending_msg_flag & HCI_USER_CONFIRMATION_REQ_SENT) {
    acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_USER_CONFIRMATION_REQ_SENT;
    acl_enc_info->user_confirmation_reply = USER_CONFIRMATION_POS_REPLY;
    /* Send LMP_ACCEPTED for pending LMP_DHKEY_CHECK if not already sent */
    if (acl_link_mgmt_info->check_pending_msg_flag & DHKEY_CHECK_PKT_RCVD) {
      acl_link_mgmt_info->check_pending_msg_flag &= ~DHKEY_CHECK_PKT_RCVD;
      lmp_input_params[0] = LMP_DHKEY_CHECK;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      if (acl_link_mgmt_info->check_pending_msg_flag & LMP_DHKEY_CHECK_SENT) {
        lmp_process_ssp_state(ulc, peer_id, LMP_DHKEY_CHECK_PASS, 0);
      } else {
        if ((peer_id == (btc_dev_mgmt_info->shared_key_calc_pending_slave)) && ((btc_dev_mgmt_info->dev_status_flags & SHARED_KEY_CALC_PENDING) == 0)) {
          io_cap[0] = btc_dev_mgmt_info->io_capabilities;
          io_cap[1] = btc_dev_mgmt_info->oob_authentication_data;
          io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
          f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r,
                  &io_cap[0], btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_input_params);
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
        } else {
          // TODO:wait for DHKEY calculation to be done
          acl_link_mgmt_info->check_pending_msg_flag |= DHKEY_CHECK_PKT_RCVD;
        }
      }
    }
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, USER_CONFIRM_REQ_REPLY_RECVD, 0);
    }
  }

}

void lmp_user_confirm_req_neg_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[2];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  } else {
    tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
  }
  if (acl_link_mgmt_info->check_pending_msg_flag & HCI_USER_CONFIRMATION_REQ_SENT) {
    acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_USER_CONFIRMATION_REQ_SENT;
    acl_enc_info->user_confirmation_reply = USER_CONFIRMATION_NEG_REPLY;
    /* Sending an LMP_NOT_ACCEPTED frame for pending LMP_DHKEY_CHECK */
    if (acl_link_mgmt_info->check_pending_msg_flag & DHKEY_CHECK_PKT_RCVD) {
      acl_link_mgmt_info->check_pending_msg_flag &= ~DHKEY_CHECK_PKT_RCVD;
      lmp_input_params[0] = LMP_DHKEY_CHECK;
      lmp_input_params[1] = AUTHENTICATION_FAILURE;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    }

    if (acl_enc_info->auth_mode == AUTH_RESPONDER) {
      lmp_process_ssp_state(ulc, peer_id, USER_CONFIRM_REQ_NEG_REPLY_RECVD, 0);
    } else {
      // TODO
    }
  }
}

void lmp_send_key_press_notification(ulc_t *ulc, uint8_t peer_id, uint8_t notification_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  uint8_t lmp_input_params = notification_type;
  uint8_t tid;

  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  } else {
    tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
  }
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_KEYPRESS_NOTIFICATION, peer_id, tid), &lmp_input_params);
}

void lmp_user_passkey_req_reply_rcvd(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  if (acl_link_mgmt_info->check_pending_msg_flag & HCI_PASSKEY_REQ_SENT) {
    acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_PASSKEY_REQ_SENT;
    acl_link_mgmt_info->device_status_flags |= USER_PASSKEY_RCVD;
    memcpy(btc_dev_mgmt_info->r, (uint8_t *)&acl_enc_info->passkey_display, 4);
    memcpy(acl_enc_info->r, (uint8_t *)&acl_enc_info->passkey_display, 4);
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, PASSKEY_REPLY_RCVD, 0);
    } else {
      if (acl_link_mgmt_info->device_status_flags & PENDING_SIMPLE_PAIRING_CONFIRM) {
        tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
        generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
        f1_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key, acl_enc_info->ssp_rand_own,
                (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_input_params);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_CONFIRM, 0, peer_id, tid), lmp_input_params);
        acl_link_mgmt_info->device_status_flags &= ~PENDING_SIMPLE_PAIRING_CONFIRM;
      }
    }
  }
}

void lmp_user_passkey_req_neg_reply_rcvd(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t lmp_input_params[2];
  uint8_t tid;
  if (acl_link_mgmt_info->check_pending_msg_flag & HCI_PASSKEY_REQ_SENT) {
    acl_link_mgmt_info->device_status_flags &= ~USER_PASSKEY_RCVD;
    acl_link_mgmt_info->check_pending_msg_flag &= ~HCI_PASSKEY_REQ_SENT;
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, PASSKEY_NEG_REPLY_RCVD, 0);
    } else {
      if (acl_link_mgmt_info->device_status_flags & PENDING_SIMPLE_PAIRING_CONFIRM) {
        tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
        lmp_input_params[0] = LMP_SIMPLE_PAIRING_CONFIRM;
        lmp_input_params[1] = PIN_OR_KEY_MISSING;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        acl_link_mgmt_info->device_status_flags &= ~PENDING_SIMPLE_PAIRING_CONFIRM;
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
      }
    }
  }
}

void lmp_remote_oob_data_req_reply_rcvd(ulc_t *ulc, uint8_t peer_id, uint8_t *hash_value)
{
  uint8_t public_key[PUBLIC_KEY_LEN_256];
  uint8_t temp_buff[RAND_NUM_SIZE] = { 0 };
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  } else {
    tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
  }
  memcpy(public_key, acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info));
  if (acl_link_mgmt_info->check_pending_msg_flag & REMOTE_OOB_DATA_REQ_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      acl_link_mgmt_info->check_pending_msg_flag |= LMP_SIMPLE_PAIRING_NUM_SENT;
      acl_link_mgmt_info->lmp_connection_state = LMP_SIMPLE_PAIRING_NUM_SENT_STATE;
      generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);
    }
    f1_algo(acl_enc_info->public_key, public_key, acl_enc_info->r, 0, temp_buff);
    if (COMPARE_16_BYTE_ARRAYS(temp_buff, hash_value)) {
      acl_enc_info->oob_commitment_check = OOB_COMMITMENT_CHECK_PASS;
    } else {
      acl_enc_info->oob_commitment_check = OOB_COMMITMENT_CHECK_FAIL;
    }
  }
}

void lmp_remote_oob_data_req_neg_reply_rcvd(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t lmp_input_params = 0;
  uint8_t tid;
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  } else {
    tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
  }
  if (acl_link_mgmt_info->check_pending_msg_flag & REMOTE_OOB_DATA_REQ_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_OOB_FAILED, 0, peer_id, tid), &lmp_input_params);
    }
  }
}

// This is just for error resolving, for actual function need to include bt key related files.
uint8_t compare_public_key(uint32_t *key, uint32_t key_length)
{
  (void)key;
  (void)key_length;
  return 0;
}

void lmp_dhkey_calculation_done(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  uint8_t io_cap[3];
  uint8_t lmp_input_params[RAND_NUM_SIZE];

  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  } else {
    tid = GET_INITIATOR_TID_OF_MODE(acl_dev_mgmt_info->mode);
  }

  /* Resume LMP packets that were waiting for shared key calculation to complete */
  if (acl_link_mgmt_info->check_pending_msg_flag & DHKEY_CHECK_PKT_RCVD) {
    acl_link_mgmt_info->check_pending_msg_flag &= ~DHKEY_CHECK_PKT_RCVD;
    io_cap[0] = btc_dev_mgmt_info->io_capabilities;
    io_cap[1] = btc_dev_mgmt_info->oob_authentication_data;
    io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
    f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r,
            &io_cap[0],  btc_dev_mgmt_info->bt_mac_addr,  acl_dev_mgmt_info->bd_addr, lmp_input_params);
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
  } else if (acl_link_mgmt_info->check_pending_msg_flag & USER_CONFIRMATION_RCVD) {
    acl_link_mgmt_info->check_pending_msg_flag &= ~USER_CONFIRMATION_RCVD;
    f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r,
            io_cap, btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_input_params);
    acl_link_mgmt_info->check_pending_msg_flag |= LMP_DHKEY_CHECK_SENT;
    acl_link_mgmt_info->lmp_connection_state = LMP_DHKEY_CHECK_SENT_STATE;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
  } else if (acl_link_mgmt_info->check_pending_msg_flag & SSP_NUM_RCVD) {
    acl_link_mgmt_info->check_pending_msg_flag &= ~SSP_NUM_RCVD;
    io_cap[0] = btc_dev_mgmt_info->io_capabilities;
    io_cap[1] = btc_dev_mgmt_info->oob_authentication_data;
    io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
    f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r,
            &io_cap[0], btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_input_params);
    acl_link_mgmt_info->check_pending_msg_flag |= LMP_DHKEY_CHECK_SENT;
    acl_link_mgmt_info->lmp_connection_state = LMP_DHKEY_CHECK_SENT_STATE;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_input_params);
  }
}

void dm_generate_public_key(ulc_t *ulc)
{
#ifdef SCHED_SIM
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  ecdh_generate_public_key(0, &btc_dev_mgmt_info->private_key[0], &btc_dev_mgmt_info->public_key[0], &btc_dev_mgmt_info->public_key[24]);
  ecdh_generate_public_key(1, &btc_dev_mgmt_info->private_key_256[0], &btc_dev_mgmt_info->public_key_256[0], &btc_dev_mgmt_info->public_key_256[32]);
  BTC_HEX_DUMP("Public Key 192:", &btc_dev_mgmt_info->public_key[0], 48);
  BTC_HEX_DUMP("Public Key 256:", &btc_dev_mgmt_info->public_key_256[0], 64);
#else
  (void)ulc; //TODO
#endif
}

void dm_derive_dhkey(ulc_t *ulc, uint8_t peer_id)
{
#ifdef SCHED_SIM
  BTC_PRINT("Derive DHKey \n");
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t *device_private_key = GET_DEVICE_PRIVATE_KEY(btc_dev_mgmt_info);
  uint8_t *peripheral_public_key_mid = GET_PERIPHERAL_PUBLIC_KEY_MID(btc_dev_mgmt_info, acl_enc_info);
  BTC_HEX_DUMP("Private key:", GET_DEVICE_PRIVATE_KEY(btc_dev_mgmt_info), GET_DEVICE_PRIVATE_KEY_LEN(btc_dev_mgmt_info));
  BTC_HEX_DUMP("Peer Public Key:", acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info));

  ecdh_derive_dhkey(GET_DEVICE_CURVE_TYPE(btc_dev_mgmt_info), device_private_key, acl_enc_info->public_key, peripheral_public_key_mid,  acl_enc_info->dh_key);
  BTC_HEX_DUMP("DHKey:", acl_enc_info->dh_key, GET_DHKEY_LEN(btc_dev_mgmt_info));
#else
  (void)ulc;   //TODO
  (void)peer_id; // TODO
#endif
}


