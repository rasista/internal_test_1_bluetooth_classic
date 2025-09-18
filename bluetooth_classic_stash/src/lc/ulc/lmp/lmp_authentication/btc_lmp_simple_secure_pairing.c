#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_key_mgmt_algos.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include "btc_lmp_simple_secure_pairing.h"
#include "btc_dm_simple_secure_pairing.h"
#include "btc_lmp_legacy_authentication.h"
#include "debug.h"


void lmp_io_capability_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT(" lmp_io_capability_req_tx_handler \n");
  (void)lmp_input_params;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_IO_CAPABILITY_REQ_CMD);
  SET_LMP_IO_CAPABILITY_REQ_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_IO_CAPABILITY_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_IO_CAPABILITY_REQ_PARAMETER_IO_CAPABILITIES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.io_capabilities);
  SET_LMP_IO_CAPABILITY_REQ_PARAMETER_OOB_AUTH_DATA(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.oob_authentication_data);
  SET_LMP_IO_CAPABILITY_REQ_PARAMETER_AUTHENTICATION_REQUIREMENTS(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.authentication_requirement);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_io_capability_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_io_capability_req_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_IO_CAPABILITY_REQ_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_IO_CAPABILITY_REQ_EXTENDED_OPCODE(bt_pkt_start);
  if ((acl_link_mgmt_info->check_pending_msg_flag & LMP_IO_CAP_REQ_SENT) && IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
    lmp_input_params[1] = LMP_ERROR_TRANSACTION_COLLISION;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NOT_ACCEPTED_EXT, peer_id, tid), lmp_input_params);
  } else {
    if ((btc_dev_mgmt_info->features_bitmap[6] & LMP_SIMPLE_PAIR) && (btc_dev_mgmt_info->ext_features_bitmap[0] & LMP_HOST_SSP)) {
      acl_dev_mgmt_info->io_capabilities = GET_LMP_IO_CAPABILITY_REQ_PARAMETER_IO_CAPABILITIES(bt_pkt_start);
      acl_dev_mgmt_info->oob_authentication_data = GET_LMP_IO_CAPABILITY_REQ_PARAMETER_OOB_AUTH_DATA(bt_pkt_start);
      acl_dev_mgmt_info->authentication_requirement = GET_LMP_IO_CAPABILITY_REQ_PARAMETER_AUTHENTICATION_REQUIREMENTS(bt_pkt_start);
      acl_enc_info->auth_mode = AUTH_INITIATOR;
      /* Send IO capabilities response and request events to Host */
      hci_io_cap_response_event(ulc, peer_id);
      acl_link_mgmt_info->check_pending_msg_flag |= IO_CAP_REQ_TO_HCI_SENT;
      acl_enc_info->pairing_procedure = SECURE_SIMPLE_PAIRING;
      hci_io_cap_req_event(ulc, peer_id);
    } else {
      lmp_input_params[1] = SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_NOT_ACCEPTED_EXT, peer_id, tid), lmp_input_params);
    }
  }
}

void lmp_io_capability_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_io_capability_res_tx_handler \n");
  (void)lmp_input_params;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_IO_CAPABILITY_RES_CMD);
  SET_LMP_IO_CAPABILITY_RES_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_IO_CAPABILITY_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_IO_CAPABILITY_RES_PARAMETER_IO_CAPABILITIES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.io_capabilities);
  SET_LMP_IO_CAPABILITY_RES_PARAMETER_OOB_AUTH_DATA(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.oob_authentication_data);
  SET_LMP_IO_CAPABILITY_RES_PARAMETER_AUTHENTICATION_REQUIREMENTS(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.authentication_requirement);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_io_capability_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_io_capability_res_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  acl_dev_mgmt_info->io_capabilities            = GET_LMP_IO_CAPABILITY_RES_PARAMETER_IO_CAPABILITIES(bt_pkt_start);
  acl_dev_mgmt_info->oob_authentication_data    = GET_LMP_IO_CAPABILITY_RES_PARAMETER_OOB_AUTH_DATA(bt_pkt_start);
  acl_dev_mgmt_info->authentication_requirement = GET_LMP_IO_CAPABILITY_RES_PARAMETER_AUTHENTICATION_REQUIREMENTS(bt_pkt_start);
  if (acl_link_mgmt_info->check_pending_msg_flag & LMP_IO_CAP_REQ_SENT) {
    lmp_process_ssp_state(ulc, peer_id, IO_CAP_RES_RCVD, 0);
  }
}

void lmp_encapsulated_header_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_encapsulated_header_tx_handler \n");
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t *encap_header = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_ENCAPSULATED_HEADER_CMD);
  SET_LMP_ENCAPSULATED_HEADER_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_ENCAPSULATED_HEADER_OPCODE(lmp_tx_pkt->data);
  SET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_MAJOR_TYPE(lmp_tx_pkt->data, encap_header[0]);
  SET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_MINOR_TYPE(lmp_tx_pkt->data, encap_header[1]);
  SET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_PAYLOAD_LENGTH(lmp_tx_pkt->data, encap_header[2]);
  acl_peer_info->acl_enc_info.encap_payload_len = encap_header[2];
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_ENCAPSULATED_HEADER;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_encapsulated_header_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_encapsulated_header_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_ENCAPSULATED_HEADER_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_ENCAPSULATED_HEADER_OPCODE(bt_pkt_start);
  uint8_t publik_key_major_type = GET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_MAJOR_TYPE(bt_pkt_start);
  uint8_t publik_key_minor_type = GET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_MINOR_TYPE(bt_pkt_start);
  uint8_t payload_length = GET_LMP_ENCAPSULATED_HEADER_PARAMETER_ENCAP_PAYLOAD_LENGTH(bt_pkt_start);
  if (IS_SUPPORTED_PUBLIC_KEY_TYPE(publik_key_major_type, publik_key_minor_type)) {
    if (((btc_dev_mgmt_info->features_bitmap[6]) & (LMP_SIMPLE_PAIR | LMP_ENCAP_PDU)) == (LMP_SIMPLE_PAIR | LMP_ENCAP_PDU)) {
      acl_enc_info->encap_payload_len = payload_length;
      acl_enc_info->encap_payload_cnt = 0;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
    } else {
      lmp_input_params[1] = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
    }
  } else {
    lmp_input_params[1] = UNSUPPORTED_LMP_PARAMETER_VALUE;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
  }
}

void lmp_encapsulated_header_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_encapsulated_header_not_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
    uint8_t reason_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
    lmp_process_ssp_state(ulc, peer_id, ENCAP_HEADER_REJECTED, reason_code);
  }
}

void lmp_encapsulated_header_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_encapsulated_header_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_ACCEPTED_TID(bt_pkt_start);
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    lmp_process_ssp_state(ulc, peer_id, ENCAP_HEADER_ACCEPTED, 0);
  } else {
    if (((btc_dev_mgmt_info->dev_status_flags & PUBLIC_KEY_CALC_PENDING) == 0)) {
      acl_enc_info->encap_payload_cnt = 0;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ENCAPSULATED_PAYLOAD, 0, peer_id, tid), GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info));
    } else {
      //TODO
    }
  }
}

void lmp_encapsulated_payload_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_encapsulated_payload_tx_handler \n");
  BTC_HEX_DUMP("lmp_input_params", (uint8_t *)lmp_input_params, MAX_SIZE_OF_INPUT_PARAMS);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t *encap_payload = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_ENCAPSULATED_PAYLOAD_CMD);
  SET_LMP_ENCAPSULATED_PAYLOAD_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_ENCAPSULATED_PAYLOAD_OPCODE(lmp_tx_pkt->data);
  SET_LMP_ENCAPSULATED_PAYLOAD_PARAMETER_ENCAP_DATA(lmp_tx_pkt->data, encap_payload);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_ENCAPSULATED_PAYLOAD;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_encapsulated_payload_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_encapsulated_payload_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[3];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_ENCAPSULATED_PAYLOAD_TID(bt_pkt_start);
  uint8_t *encap_data = &GET_LMP_ENCAPSULATED_PAYLOAD_PARAMETER_ENCAP_DATA(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_ENCAPSULATED_PAYLOAD_OPCODE(bt_pkt_start);

  if (acl_link_mgmt_info->device_status_flags & PUBLIC_KEY_RCVD) {
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
  } else {
    memcpy(&acl_enc_info->public_key[MAX_SIZE_OF_INPUT_PARAMS * acl_enc_info->encap_payload_cnt], encap_data, MAX_SIZE_OF_INPUT_PARAMS);
    acl_enc_info->encap_payload_cnt++;
    if (memcmp(acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info))) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
    } else {
      /*CVE-2020-26558: Vulnerability fix, When same public key is received from Remote Device,
      *  send PAIRING_NOT_ALLOWED error to remote device and update to host*/
      acl_enc_info->encap_payload_cnt = 0;
      lmp_input_params[1] = PAIRING_NOT_ALLOWED;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      hci_simple_pairing_complete(ulc, peer_id, PAIRING_NOT_ALLOWED);
      return;
    }
    if ((MAX_SIZE_OF_INPUT_PARAMS * acl_enc_info->encap_payload_cnt) >= acl_enc_info->encap_payload_len) {
      acl_link_mgmt_info->device_status_flags |= PUBLIC_KEY_RCVD;
      if (btc_dev_mgmt_info->ssp_debug_mode != SSP_DEBUG_MODE_ENABLED) {
        if (!(memcmp(acl_enc_info->public_key, GET_DEVICE_DEBUG_PUBLIC_KEY(btc_dev_mgmt_info), GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info)))) {
          btc_dev_mgmt_info->ssp_link_key_type = SSP_DEBUG_LINK_KEY;
        } else {
          btc_dev_mgmt_info->ssp_link_key_type = SSP_COMB_KEY;
        }
      }
      if ((btc_dev_mgmt_info->dev_status_flags & SHARED_KEY_CALC_PENDING) == 0) {
        /* Calculate DHKey */
        btc_dev_mgmt_info->dev_status_flags |= SHARED_KEY_CALC_PENDING;
        //FIXME:what if SSP is going on simultaneously for 2 slaves??Is it possible?
        btc_dev_mgmt_info->shared_key_calc_pending_slave = peer_id;
        
        /* FIXED: Use peer's public key instead of local public key for ECDH calculation
         * Device A: dh_key = privA × pubB (local_private × peer_public)  
         * Device B: dh_key = privB × pubA (local_private × peer_public)
         * This ensures both devices calculate the same shared secret */
        dm_derive_dhkey(ulc, peer_id);
        btc_dev_mgmt_info->dev_status_flags &= ~SHARED_KEY_CALC_PENDING;
        lmp_dhkey_calculation_done(ulc, peer_id);
      } else {
        /* We cannot compute the shared key now, * will have to wait until the started calculation is done */
        btc_dev_mgmt_info->shared_key_trig_pending_slave |= BIT(peer_id);
      }
      
      if ((IS_AUTH_RESPONDER(acl_enc_info) && (acl_link_mgmt_info->device_status_flags & PUBLIC_KEY_SENT))) {
        if (!IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
          lmp_process_ssp_state(ulc, peer_id, PUBLIC_KEY_RECEIVED, 0);
        }
      } else {
        GET_ENCAPSULATED_HEADER(lmp_input_params, btc_dev_mgmt_info);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ENCAPSULATED_HEADER, 0, peer_id, tid), lmp_input_params);
      }
    }
  }
}

void lmp_encapsulated_payload_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_encapsulated_payload_not_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
    uint8_t reason_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
    lmp_process_ssp_state(ulc, peer_id, ENCAP_PAYLOAD_REJECTED, reason_code);
  }
}

void lmp_encapsulated_payload_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_encapsulated_payload_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_ENCAPSULATED_PAYLOAD_TID(bt_pkt_start);
  if (acl_link_mgmt_info->device_status_flags & PUBLIC_KEY_SENT) {
    if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, PUBLIC_KEY_PAYLOAD_ACCEPTED, 0);
    } else {
      if ((acl_link_mgmt_info->device_status_flags & PUBLIC_KEY_RCVD)) {
        if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
          generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
          f1_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key, acl_enc_info->ssp_rand_own, 0, lmp_input_params);
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_CONFIRM, 0, peer_id, tid), lmp_input_params);
        } else if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
          acl_link_mgmt_info->check_pending_msg_flag |= HCI_PASSKEY_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = HCI_PASSKEY_REQ_SENT_STATE;
          if (((btc_dev_mgmt_info->io_capabilities == DISPLAY_YES_NO) || (btc_dev_mgmt_info->io_capabilities == DISPLAY_ONLY))
              && (acl_dev_mgmt_info->io_capabilities == KEYBOARD_ONLY)) {
            generate_rand_num(4, (uint8_t *)&acl_enc_info->passkey_display, PSEUDO_RAND_NUM);
            acl_enc_info->passkey_display = (acl_enc_info->passkey_display % 1000000);
            memcpy(btc_dev_mgmt_info->r, (uint8_t *)&acl_enc_info->passkey_display, 4);
            memcpy(acl_enc_info->r, (uint8_t *)&acl_enc_info->passkey_display, 4);
            hci_user_passkey_notification(ulc, peer_id);
          } else if (btc_dev_mgmt_info->io_capabilities == KEYBOARD_ONLY) {
            acl_enc_info->passkey_entry_round = 0;
            acl_link_mgmt_info->device_status_flags &= ~USER_PASSKEY_RCVD;
            hci_user_passkey_request(ulc, peer_id);
          }
        } else if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
          if (btc_dev_mgmt_info->oob_authentication_data == OOB_AUTH_DATA_AVAILABLE) {
            acl_link_mgmt_info->check_pending_msg_flag |= REMOTE_OOB_DATA_REQ_SENT;
            hci_remote_oob_data_request(ulc, peer_id);
          }
        }
      }
    }
  } else {
    uint32_t offset = MAX_SIZE_OF_INPUT_PARAMS * (++acl_enc_info->encap_payload_cnt);
    uint32_t public_key_length = GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info);
    uint8_t *device_public_key = GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info);
    if (offset < public_key_length) {
      if ((public_key_length - offset) <= MAX_SIZE_OF_INPUT_PARAMS) {
        acl_link_mgmt_info->device_status_flags |= PUBLIC_KEY_SENT;
      }
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ENCAPSULATED_PAYLOAD, 0, peer_id, tid), &device_public_key[MAX_SIZE_OF_INPUT_PARAMS * acl_enc_info->encap_payload_cnt]);
    }
  }
}

void lmp_keypress_notification_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_keypress_notification_tx_handler \n");
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_KEYPRESS_NOTIFICATION_CMD);
  SET_LMP_KEYPRESS_NOTIFICATION_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_KEYPRESS_NOTIFICATION_OPCODE(lmp_tx_pkt->data);
  SET_LMP_KEYPRESS_NOTIFICATION_PARAMETER_NOTIFICATION_TYPE(lmp_tx_pkt->data, *(uint8_t*)lmp_input_params);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_keypress_notification_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_keypress_notification_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t notification_type = GET_LMP_KEYPRESS_NOTIFICATION_PARAMETER_NOTIFICATION_TYPE(bt_pkt_start);
  if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
    /*Keypress Notification Event*/
    hci_keypress_notification_event(ulc, peer_id, notification_type);
    if (notification_type == PASSKEY_ENTRY_COMPLETED) {
      lmp_process_ssp_state(ulc, peer_id, PASSKEY_REPLY_RCVD, 0);
    }
  }
}

void lmp_simple_pairing_confirm_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_simple_pairing_confirm_tx_handler \n");
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SIMPLE_PAIRING_CONFIRM_CMD);
  SET_LMP_SIMPLE_PAIRING_CONFIRM_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_SIMPLE_PAIRING_CONFIRM_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SIMPLE_PAIRING_CONFIRM_PARAMETER_COMMITMENT_VALUE(lmp_tx_pkt->data, (uint8_t *)lmp_input_params);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_simple_pairing_confirm_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_simple_pairing_confirm_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[RAND_NUM_SIZE];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_SIMPLE_PAIRING_CONFIRM_TID(bt_pkt_start);
  memcpy(acl_enc_info->ssp_commitment, &GET_LMP_SIMPLE_PAIRING_CONFIRM_PARAMETER_COMMITMENT_VALUE(bt_pkt_start), SIZE_OF_C_VAL);
  BTC_HEX_DUMP("ssp_commitment", acl_enc_info->ssp_commitment, SIZE_OF_C_VAL);
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
      lmp_process_ssp_state(ulc, peer_id, COMMITMENT_VAL_RCVD, 0);
  } else {
    if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
      if (btc_dev_mgmt_info->io_capabilities == KEYBOARD_ONLY) {
        if (acl_link_mgmt_info->device_status_flags & USER_PASSKEY_RCVD) {
        generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
        f1_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key, acl_enc_info->ssp_rand_own,
          (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_input_params);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_CONFIRM, 0, peer_id, tid), lmp_input_params);
        } else {
          if (acl_link_mgmt_info->check_pending_msg_flag & HCI_PASSKEY_REQ_SENT) {
            /* Wait for PASSKEY_REQ_REPLY from host */
            acl_link_mgmt_info->device_status_flags |= PENDING_SIMPLE_PAIRING_CONFIRM;
            acl_enc_info->passkey_entry_round = 0;
          } else {
            lmp_input_params[0] = GET_LMP_SIMPLE_PAIRING_CONFIRM_OPCODE(bt_pkt_start);
            lmp_input_params[1] = PIN_OR_KEY_MISSING;
            call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
          }
        }
      } else {
        generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
        f1_algo(GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->public_key, acl_enc_info->ssp_rand_own,
          (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_input_params);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_CONFIRM, 0, peer_id, tid), lmp_input_params);
      }
    }
  }
}

void lmp_simple_pairing_confirm_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_simple_pairing_confirm_not_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t reason_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    lmp_process_ssp_state(ulc, peer_id, LMP_SIMPLE_PAIRING_NUM_REJECTED, reason_code);
  } else {
    hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
  }
}

void lmp_simple_pairing_number_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_simple_pairing_number_tx_handler \n");
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SIMPLE_PAIRING_NUMBER_CMD);
  SET_LMP_SIMPLE_PAIRING_NUMBER_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_SIMPLE_PAIRING_NUMBER_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SIMPLE_PAIRING_NUMBER_PARAMETER_NONCVALUE(lmp_tx_pkt->data, (uint8_t *)lmp_input_params);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_SIMPLE_PAIRING_NUMBER;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_simple_pairing_number_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_simple_pairing_number_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[2];
  uint8_t lmp_auth_data[RAND_NUM_SIZE];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_SIMPLE_PAIRING_NUMBER_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_SIMPLE_PAIRING_NUMBER_OPCODE(bt_pkt_start);

  memcpy(acl_enc_info->ssp_rand_peer, &GET_LMP_SIMPLE_PAIRING_NUMBER_PARAMETER_NONCVALUE(bt_pkt_start), RAND_NUM_SIZE);
  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
      f1_algo(acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->ssp_rand_peer, 0, lmp_auth_data);
    } else if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
      f1_algo(acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->ssp_rand_peer,
              (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_auth_data);
    }

    if (!IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
      BTC_HEX_DUMP("lmp_auth_data", lmp_auth_data, SIZE_OF_C_VAL);
      BTC_HEX_DUMP("ssp_commitment", acl_enc_info->ssp_commitment, SIZE_OF_C_VAL);
      if (!memcmp(lmp_auth_data, acl_enc_info->ssp_commitment, SIZE_OF_C_VAL)) {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        lmp_process_ssp_state(ulc, peer_id, LMP_SIMPLE_PAIRING_NUM_RCVD, 0);
      } else {
        lmp_input_params[1] = AUTHENTICATION_FAILURE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
    }
    if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
      if (acl_enc_info->oob_commitment_check == OOB_COMMITMENT_CHECK_PASS) {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        lmp_process_ssp_state(ulc, peer_id, LMP_SIMPLE_PAIRING_NUM_RCVD, 0);
      } else {
        lmp_input_params[1] = AUTHENTICATION_FAILURE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
        if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
          hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
        }
      }
    }
  } else {
    if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);
    } else if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
      BTC_HEX_DUMP("remote public key", acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info));
      BTC_HEX_DUMP("local public key", GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info));
      BTC_HEX_DUMP("ssp_rand_peer", acl_enc_info->ssp_rand_peer, RAND_NUM_SIZE);
      f1_algo(acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->ssp_rand_peer,
              (((acl_enc_info->passkey_display >> acl_enc_info->passkey_entry_round) & 0x1) | BIT(7)), lmp_auth_data);
      BTC_HEX_DUMP("lmp_auth_data", lmp_auth_data, SIZE_OF_C_VAL);
      BTC_HEX_DUMP("ssp_commitment", acl_enc_info->ssp_commitment, SIZE_OF_C_VAL);
      if (!memcmp(lmp_auth_data, acl_enc_info->ssp_commitment, SIZE_OF_C_VAL)) {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);
      } else {
        lmp_input_params[1] = AUTHENTICATION_FAILURE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
      }
    } else if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
      if (acl_enc_info->oob_commitment_check == OOB_COMMITMENT_CHECK_PASS) {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        generate_rand_num(RAND_NUM_SIZE, acl_enc_info->ssp_rand_own, PSEUDO_RAND_NUM);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SIMPLE_PAIRING_NUMBER, 0, peer_id, tid), acl_enc_info->ssp_rand_own);

      } else {
        lmp_input_params[1] = AUTHENTICATION_FAILURE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
      }
    }
  }
}

void lmp_simple_pairing_number_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_simple_pairing_number_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t confirmation_val[RAND_NUM_SIZE];

  if (IS_AUTH_RESPONDER(acl_enc_info)) {
    acl_link_mgmt_info->device_status_flags |= SENT_SIMPLE_PAIRING_NUM_ACCEPTED;
  } else {
    if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
      g_algo(acl_enc_info->public_key, GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info), acl_enc_info->ssp_rand_peer, acl_enc_info->ssp_rand_own, confirmation_val);
      *(uint32_t *)&confirmation_val[0] %= 1000000;
      acl_link_mgmt_info->check_pending_msg_flag |= HCI_USER_CONFIRMATION_REQ_SENT;
      hci_user_confirmation_req_event(ulc, peer_id, *(uint32_t *)&confirmation_val[0]);
    } else if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
      acl_enc_info->passkey_entry_round++;
    } else if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
      return;
    }
  }
}

void lmp_simple_pairing_number_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_simple_pairing_number_not_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
  if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
    hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
  }  
}


void lmp_dhkey_check_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_dhkey_check_tx_handler \n");
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_DHKEY_CHECK_CMD);
  SET_LMP_DHKEY_CHECK_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_DHKEY_CHECK_OPCODE(lmp_tx_pkt->data);
  SET_LMP_DHKEY_CHECK_PARAMETER_CONFIRMATION_VALUE(lmp_tx_pkt->data, (uint8_t *)lmp_input_params);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_DHKEY_CHECK;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_dhkey_check_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_dhkey_check_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;
  uint8_t lmp_input_params[2];
  uint8_t lmp_auth_data[RAND_NUM_SIZE];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_DHKEY_CHECK_TID(bt_pkt_start);
  lmp_input_params[0]  = GET_LMP_DHKEY_CHECK_OPCODE(bt_pkt_start);
  uint8_t io_cap[3];
  io_cap[0] = acl_dev_mgmt_info->io_capabilities;
  io_cap[1] = acl_dev_mgmt_info->oob_authentication_data;
  io_cap[2] = acl_dev_mgmt_info->authentication_requirement;


  BTC_HEX_DUMP("acl_enc_info->dh_key", acl_enc_info->dh_key, GET_DHKEY_LEN(btc_dev_mgmt_info));
  BTC_HEX_DUMP("ssp_rand_peer", acl_enc_info->ssp_rand_peer, RAND_NUM_SIZE);
  BTC_HEX_DUMP("ssp_rand_own", acl_enc_info->ssp_rand_own, RAND_NUM_SIZE);
  BTC_HEX_DUMP("r", btc_dev_mgmt_info->r, RAND_NUM_SIZE);
  BTC_HEX_DUMP("io_cap", io_cap, 3);
  BTC_HEX_DUMP("bd_addr", acl_dev_mgmt_info->bd_addr, MAC_ADDR_LEN);
  BTC_HEX_DUMP("bt_mac_addr", btc_dev_mgmt_info->bt_mac_addr, MAC_ADDR_LEN);

  f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_peer, acl_enc_info->ssp_rand_own, btc_dev_mgmt_info->r,
          &io_cap[0], acl_dev_mgmt_info->bd_addr, btc_dev_mgmt_info->bt_mac_addr, lmp_auth_data);
 
  BTC_HEX_DUMP("lmp_auth_data", lmp_auth_data, RAND_NUM_SIZE);
  BTC_HEX_DUMP("received confirmation value", &GET_LMP_DHKEY_CHECK_PARAMETER_CONFIRMATION_VALUE(bt_pkt_start), RAND_NUM_SIZE);
  if (!memcmp(&GET_LMP_DHKEY_CHECK_PARAMETER_CONFIRMATION_VALUE(bt_pkt_start), lmp_auth_data, RAND_NUM_SIZE)) {
    if ((compare_public_key((uint32_t *)acl_enc_info->public_key, (GET_DEVICE_PRIVATE_KEY_LEN(btc_dev_mgmt_info) / 4))) != BT_STATUS_SUCCESS) {
      lmp_input_params[1] = AUTHENTICATION_FAILURE;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    }
    if (IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)) {
      /* Send a response only in case we receive a user confirmation response from the host.*/
      if (!(acl_link_mgmt_info->check_pending_msg_flag & HCI_USER_CONFIRMATION_REQ_SENT)) {
        if (acl_enc_info->user_confirmation_reply == USER_CONFIRMATION_POS_REPLY) {
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
        } else {
          lmp_input_params[1] = AUTHENTICATION_FAILURE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
          hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
          return;
        }
      } else {
        /* Wait for user confirmation before responding to DHKEY_REQ */
        acl_link_mgmt_info->check_pending_msg_flag |= DHKEY_CHECK_PKT_RCVD;
        return;
      }
    } else if (IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
    } else if (IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
    }

    if (acl_link_mgmt_info->check_pending_msg_flag & LMP_DHKEY_CHECK_SENT) {
      lmp_process_ssp_state(ulc, peer_id, LMP_DHKEY_CHECK_PASS, 0);
    } else {
      io_cap[0] = btc_dev_mgmt_info->io_capabilities;
      io_cap[1] = btc_dev_mgmt_info->oob_authentication_data;
      io_cap[2] = btc_dev_mgmt_info->authentication_requirement;
      f3_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, acl_enc_info->r,
              &io_cap[0], btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, lmp_auth_data);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DHKEY_CHECK, 0, peer_id, tid), lmp_auth_data);
    }
  } else {
    lmp_input_params[1] = AUTHENTICATION_FAILURE;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
    hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
    if (acl_link_mgmt_info->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) {
      hci_auth_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
      acl_link_mgmt_info->check_pending_req_from_hci &= ~AUTH_REQ_FROM_HCI;
    }
  }
}

void lmp_dhkey_check_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_dhkey_check_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_enc_info_t *acl_enc_info = &acl_peer_info->acl_enc_info;

  if (IS_AUTH_INITIATOR(acl_enc_info)) {
    hci_simple_pairing_complete(ulc, peer_id, BT_SUCCESS_STATUS);
    acl_link_mgmt_info->remote_initiated_conn = 0;
    /* Calculating Link key */
    if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
      f2_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_peer, acl_enc_info->ssp_rand_own, btc_dev_mgmt_info->key_id,
              acl_dev_mgmt_info->bd_addr, btc_dev_mgmt_info->bt_mac_addr, acl_enc_info->link_key);
    } else {
      f2_algo(acl_enc_info->dh_key, acl_enc_info->ssp_rand_own, acl_enc_info->ssp_rand_peer, btc_dev_mgmt_info->key_id,
              btc_dev_mgmt_info->bt_mac_addr, acl_dev_mgmt_info->bd_addr, acl_enc_info->link_key);
    }
    acl_link_mgmt_info->device_status_flags |= DEVICE_LINK_KEY_EXISTS;
  }
}

void lmp_dhkey_check_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_dhkey_check_not_accepted_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  if (acl_link_mgmt_info->check_pending_msg_flag & LMP_DHKEY_CHECK_SENT) {
    uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
    uint8_t reason_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
    lmp_process_ssp_state(ulc, peer_id, LMP_DHKEY_CHECK_FAILED, reason_code);
  }
}

void lmp_numeric_comparison_failed_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_numeric_comparison_failed_tx_handler \n");
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_NUMERIC_COMPARISON_FAILED_CMD);
  SET_LMP_NUMERIC_COMPARISON_FAILED_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_NUMERIC_COMPARISON_FAILED_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_numeric_comparison_failed_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_numeric_comparison_failed_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
}

void lmp_passkey_failed_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_passkey_failed_tx_handler \n");
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_PASSKEY_FAILED_CMD);
  SET_LMP_PASSKEY_FAILED_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_PASSKEY_FAILED_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_passkey_failed_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_passkey_failed_rx_handler  \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  hci_simple_pairing_complete(ulc, peer_id, AUTHENTICATION_FAILURE);
}

void lmp_oob_failed_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  BTC_PRINT("lmp_oob_failed_tx_handler  \n");
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_OOB_FAILED_CMD);
  SET_LMP_OOB_FAILED_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_OOB_FAILED_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, DECODE_LMP_PEER_ID(lmp_control_word));
}

void lmp_oob_failed_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  BTC_PRINT("lmp_oob_failed_rx_handler \n");
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t lmp_input_params[2];
  uint8_t tid = GET_LMP_OOB_FAILED_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_OOB_FAILED_OPCODE(bt_pkt_start);
  lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
}

void lmp_unit_key_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)ulc;
  (void)lmp_control_word;
  (void)lmp_input_params;
}

void lmp_temp_rand_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)ulc;
  (void)lmp_control_word;
  (void)lmp_input_params;
}

void lmp_temp_key_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)ulc;
  (void)lmp_control_word;
  (void)lmp_input_params;
}

void lmp_unit_key_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t lmp_input_params[2];
  uint8_t tid = GET_LMP_UNIT_KEY_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_UNIT_KEY_OPCODE(bt_pkt_start);
  lmp_input_params[1] = PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
}

void lmp_temp_rand_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t lmp_input_params[2];
  uint8_t tid = GET_LMP_TEMP_RAND_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_TEMP_RAND_OPCODE(bt_pkt_start);
  lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
}

void lmp_temp_key_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t lmp_input_params[2];
  uint8_t tid = GET_LMP_TEMP_KEY_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_TEMP_KEY_OPCODE(bt_pkt_start);
  lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);
}