#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"
#include "btc_lmp.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include "btc_dev.h"
#include "hci_cmd_get_defines_autogen.h"
#include "btc_key_mgmt_algos.h"
#include "lmp.h"
#include "btc_lmp_qos.h"
#include "btc_lmp_sniff.h"
#include "btc_dm_sniff.h"
#include "btc_dm_simple_secure_pairing.h"


void lmp_send_detach_frame(ulc_t *ulc, uint8_t peer_id, uint8_t reason)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info;
  acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DETACH, 0, peer_id, tid), &reason);
}

void dm_acl_conn_indication_from_lm(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;

  /* Inform BRM about the new sco connection */
  // brm_new_acl_connection_ind(ulc, peer_id);
  /* Ask HCI to send an event to HOST */
  if (acl_link_mgmt_info->host_event_notification_flags & CONNECTION_COMPLETE) {
    acl_link_mgmt_info->host_event_notification_flags &= ~CONNECTION_COMPLETE;
    hci_conn_complete_indication(ulc, peer_id, status, BT_ACL);
    if (status == BT_SUCCESS_STATUS) {
      acl_link_mgmt_info->host_event_notification_flags |= DISCONNNECTION_COMPLETE;
    }
  }
  if (status == BT_STATUS_SUCCESS) {
    acl_peer_info->acl_dev_mgmt_info.lm_connection_status = LM_LEVEL_CONN_DONE;
  }
}

uint8_t bt_get_free_lt_addr(btc_dev_mgmt_info_t *btc_dev_mgmt_info)
{
  uint8_t ii = 1;

  for (ii = 1; ii <= 7; ii++) {
    if (!(btc_dev_mgmt_info->free_lt_addr_bitmap & BIT(ii))) {
      btc_dev_mgmt_info->free_lt_addr_bitmap |= BIT(ii);
      return ii;
    }
  }
  BTC_ASSERT(NO_LT_ADDR);
  return ii;
}

void btc_lmp_init(ulc_t *ulc)
{
  uint8_t gx[24], gy[24] = {};

  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;

  if ((btc_dev_mgmt_info->features_bitmap[0] & (LMP_SOFFSET | LMP_RSWITCH)) == (LMP_SOFFSET | LMP_RSWITCH)) {
    btc_dev_mgmt_info->link_policy_settings |= ENABLE_ROLE_SWITCH;
  }
  if ((btc_dev_mgmt_info->features_bitmap[0] & LMP_HOLD_FLAG) == LMP_HOLD_FLAG) {
    btc_dev_mgmt_info->link_policy_settings |= ENABLE_HOLD_MODE;
  }
  if ((btc_dev_mgmt_info->features_bitmap[0] & LMP_SNIFF) == LMP_SNIFF) {
    btc_dev_mgmt_info->link_policy_settings |= ENABLE_SNIFF_MODE;
  }
  if ((btc_dev_mgmt_info->features_bitmap[1] & LMP_PARK) == LMP_PARK) {
    btc_dev_mgmt_info->link_policy_settings |= ENABLE_PARK_STATE;
  }
  btc_dev_mgmt_info->max_page_num = MAX_NUM_SUPPORTED_FEATURE_PAGE;
  btc_dev_mgmt_info->max_supported_enc_key_size = ENC_KEY_KC_MAX_LEN;
  btc_dev_mgmt_info->nbc = DEFAULT_NBC;

  *(uint32_t *)&gx[0] = 0x82ff1012;
  *(uint32_t *)&gx[4] = 0xf4ff0afd;
  *(uint32_t *)&gx[8] = 0x43a18800;
  *(uint32_t *)&gx[12] = 0x7cbf20eb;
  *(uint32_t *)&gx[16] = 0xb03090f6;
  *(uint32_t *)&gx[20] = 0x188da80e;

  *(uint32_t *)&gy[0] = 0x1e794811;
  *(uint32_t *)&gy[4] = 0x73f977a1;
  *(uint32_t *)&gy[8] = 0x6b24cdd5;
  *(uint32_t *)&gy[12] = 0x631011ed;
  *(uint32_t *)&gy[16] = 0xffc8da78;
  *(uint32_t *)&gy[20] = 0x07192b95;

  /* Initialising the key id value to be used in SSP */
  *(uint32_t *)&btc_dev_mgmt_info->key_id[0] = 0x62746c6b;

  generate_rand_num(PRIVATE_KEY_LEN, btc_dev_mgmt_info->private_key, TRUE_RAND_NUM);
  generate_rand_num(PRIVATE_KEY_LEN_256, btc_dev_mgmt_info->private_key_256, TRUE_RAND_NUM);
  /* Private Key should not exceed r1/2, so for now just making the most significant nibble to all zeroes
  * which ensures that the number is smaller than r1/2. Ideally private_key modulo r1/2 has to be done */
  btc_dev_mgmt_info->private_key[PRIVATE_KEY_LEN - 1] &= 0x0F;
  btc_dev_mgmt_info->private_key_256[PRIVATE_KEY_LEN_256 - 1] &= 0x0F;

  /* Public key Calculation */
  btc_dev_mgmt_info->dev_status_flags |= PUBLIC_KEY_CALC_PENDING;
  dm_generate_public_key(ulc);
  btc_dev_mgmt_info->dev_status_flags &= ~PUBLIC_KEY_CALC_PENDING;


  /*SSP Debug Private Key*/
  *(uint32_t *)&btc_dev_mgmt_info->debug_private_key[0] = 0xf4a518ff;
  *(uint32_t *)&btc_dev_mgmt_info->debug_private_key[4] = 0x625ed2ef;
  *(uint32_t *)&btc_dev_mgmt_info->debug_private_key[8] = 0xcf0c142b;
  *(uint32_t *)&btc_dev_mgmt_info->debug_private_key[12] = 0x005df1d6;
  *(uint32_t *)&btc_dev_mgmt_info->debug_private_key[16] = 0x918ddc27;
  *(uint32_t *)&btc_dev_mgmt_info->debug_private_key[20] = 0x07915f86;

  /*SSP Debug Public Key*/
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[0] = 0x1125f8ed;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[4] = 0xd2809ea5;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[8] = 0xfe7e4329;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[12] = 0x586f9fc3;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[16] = 0x984421a6;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[20] = 0x15207009;

  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[24] = 0xb9f7ea25;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[28] = 0x7fca856f;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[32] = 0x9dbbaa85;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[36] = 0x9f79e4b5;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[40] = 0x1bc5bd00;
  *(uint32_t *)&btc_dev_mgmt_info->debug_public_key[44] = 0xb09d42b8;

  uint8_t peer_id;
  acl_peer_info_t *acl_peer_info = NULL;
  acl_enc_info_t *acl_enc_info = NULL;
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = NULL;

  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    acl_enc_info = &acl_peer_info->acl_enc_info;
    conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
    acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
    acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
    /* Assigning the max power by default */
    conn_pwr_ctrl_info->tx_power_index = DEFAULT_EDR_POWER_INDEX; /* SIG_REVIEW: Check with sateesh and finalize this */
    conn_pwr_ctrl_info->min_allowed_power_index = MIN_ALLOWED_POWER_INDEX;
    conn_pwr_ctrl_info->max_allowed_power_index = MAX_ALLOWED_POWER_INDEX;

    acl_dev_mgmt_info->ptt = BT_BR_MODE;
    acl_dev_mgmt_info->tx_max_slots = 1;
    acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_NOT_DONE;

    acl_link_mgmt_info->device_status_flags = 0;
    acl_link_mgmt_info->link_supervision_timeout = DEFAULT_LINK_SUPERVISION_TIMEOUT;
    acl_link_mgmt_info->link_supervision_enabled = LINK_SUPERVISION_ENABLED;
    acl_link_mgmt_info->poll_interval = DEFAULT_POLL_INTERVAL; // TODO

    acl_enc_info->encryption_flags = 0;
  }

  BT_DBG_STATS_ASSIGN_VAL(btc_dev_info->btc_lmp_debug_stats.lmp_pkts_recvd, 0);
  BT_DBG_STATS_ASSIGN_VAL(btc_dev_info->btc_lmp_debug_stats.lmp_pkts_sent, 0);
}

void update_new_acl_pkt_type(acl_link_mgmt_info_t *acl_link_mgmt_info)
{
  if (acl_link_mgmt_info->device_status_flags & UPDATE_PACKET_TYPE) {
    acl_link_mgmt_info->device_status_flags &= ~UPDATE_PACKET_TYPE;
    acl_link_mgmt_info->acl_pkt_type = acl_link_mgmt_info->new_pkt_type;
  }
  return;
}

void lmp_switch_to_central(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  lmp_input_params.tid = PERIPHERAL_INITIATED_TID;
  lmp_input_params.peer_id = peer_id;

  if (!(acl_dev_mgmt_info->link_policy_settings & ENABLE_ROLE_SWITCH)) {
    hci_role_change_event(ulc, peer_id, COMMAND_DISALLOWED);
    return;
  }

  acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
  if ((acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_PAUSE_ENC) && (acl_dev_mgmt_info->features_bitmap[5] & LMP_PAUSE_ENC)) {
    acl_link_mgmt_info->check_pending_req_from_hci = ROLE_SWITCH_REQ;
    acl_link_mgmt_info->device_status_flags |= DEVICE_ENC_PAUSED;
    acl_link_mgmt_info->host_event_notification_flags |= ENC_KEY_REFRESH;

    lmp_input_params.exopcode = LMP_PAUSE_ENCRYPTION_REQ;
    lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
    lmp_input_params.reason_code = 0;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    // lmp_form_tx_pkt(ulc, LMP_ESCAPE_OPCODE, PERIPHERAL_INITIATED_TID, peer_id, NULL, 0);
  } else {
    acl_link_mgmt_info->host_event_notification_flags |= ROLE_CHANGE;
    lmp_input_params.reason_code = 0;
    lmp_input_params.exopcode = LMP_SLOT_OFFSET;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    lmp_input_params.exopcode = LMP_SWITCH_REQ;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
  }
  return;
}
void lmp_switch_to_peripheral(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  if (!(acl_dev_mgmt_info->link_policy_settings & ENABLE_ROLE_SWITCH)) {
    hci_role_change_event(ulc, peer_id, COMMAND_DISALLOWED);
    return;
  }

  lmp_input_params.tid = CENTERAL_INITIATED_TID;
  lmp_input_params.peer_id = peer_id;
  lmp_input_params.reason_code = 0;
  acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
  if ((acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_PAUSE_ENC) && (acl_dev_mgmt_info->features_bitmap[5] & LMP_PAUSE_ENC)) {
    acl_link_mgmt_info->check_pending_req_from_hci = ROLE_SWITCH_REQ;
    acl_link_mgmt_info->device_status_flags |= DEVICE_ENC_PAUSED;
    acl_link_mgmt_info->host_event_notification_flags |= ENC_KEY_REFRESH;
    lmp_input_params.exopcode = LMP_PAUSE_ENCRYPTION_REQ;
    lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    // lmp_form_tx_pkt(ulc, LMP_ESCAPE_OPCODE, CENTERAL_INITIATED_TID, peer_id, NULL, 0);
  } else {
    acl_link_mgmt_info->host_event_notification_flags |= ROLE_CHANGE;
    lmp_input_params.exopcode = LMP_SWITCH_REQ;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    // lmp_form_tx_pkt(ulc, LMP_SWITCH_REQ, CENTERAL_INITIATED_TID, peer_id, NULL, 0);
  }
  return;
}

void lmp_read_local_oob_data(ulc_t *ulc, uint8_t *oob_data)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(btc_dev_info->btc_dev_mgmt_info);
  uint8_t public_key[PUBLIC_KEY_LEN];

  memcpy(public_key, btc_dev_mgmt_info->public_key, PUBLIC_KEY_LEN);
  generate_rand_num(RAND_NUM_SIZE, btc_dev_mgmt_info->r, PSEUDO_RAND_NUM);               // TODO
  f1_algo(btc_dev_mgmt_info->public_key, public_key, btc_dev_mgmt_info->r, 0, oob_data); // TODO

  memcpy(&oob_data[16], btc_dev_mgmt_info->r, SIZE_OF_R);

  return;
}

uint8_t lmp_refresh_enc_key_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  uint8_t tx_tid;

  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    tx_tid = CENTERAL_INITIATED_TID;
  } else {
    tx_tid = PERIPHERAL_INITIATED_TID;
  }
  if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    return INVALID_HCI_COMMAND_PARAMETERS;
  } else {
    acl_link_mgmt_info->check_pending_req_from_hci |= REFRESH_ENC_KEY;

    acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
    acl_link_mgmt_info->host_event_notification_flags |= ENC_KEY_REFRESH;

    lmp_input_params.exopcode = LMP_PAUSE_ENCRYPTION_REQ;
    lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
    lmp_input_params.tid = tx_tid;
    lmp_input_params.peer_id = peer_id;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    // lmp_form_tx_pkt(ulc, LMP_ESCAPE_OPCODE, tx_tid, peer_id, input_params, 0);
    return BT_SUCCESS_STATUS;
  }
  return BT_SUCCESS_STATUS;
}

uint8_t lmp_accept_connection_req_from_dm(ulc_t *ulc, uint8_t peer_id, uint8_t role)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint8_t lmp_input_params;
  uint8_t tid;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

  /* If the remote device is a MASTER, initiating device is a master, so TID = 0*/
  if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }

  if (acl_link_mgmt_info->received_req_flag & ACL_CONNECTION_REQ) {
    btc_dev_info->btc_dev_mgmt_info.dev_status_flags &= ~ROLE_CHANGED;
    if (((btc_dev_info->btc_dev_mgmt_info.features_bitmap[0] & (LMP_SOFFSET | LMP_RSWITCH)) == (LMP_SOFFSET | LMP_RSWITCH))
        && (btc_dev_info->btc_dev_mgmt_info.link_policy_settings & ENABLE_ROLE_SWITCH) && (!role)) {
      // #ifdef BT_MS_ENABLE
      //       /* Masking inquiry index priority_level because creating a connection is more priority then inquiry */
      //       if (bt_info->bt_context_info_p->bt_roles & BT_ROLE_INQUIRY) {
      //         /* Mask inquiry role */
      //         bt_info->bt_context_info_p->bt_context[BT_CONTEXT_INDEX_INQUIRY].priority_level = BT_PRIORITY_MASKED;
      //       }
      // #endif
      acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
      if ((acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_PAUSE_ENC) && (acl_dev_mgmt_info->features_bitmap[5] & LMP_PAUSE_ENC)) {
        acl_link_mgmt_info->device_status_flags |= DEVICE_ENC_PAUSED;
        acl_link_mgmt_info->host_event_notification_flags |= ENC_KEY_REFRESH;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PAUSE_ENCRYPTION_REQ, peer_id, tid), NULL);
      } else {
        acl_link_mgmt_info->host_event_notification_flags |= ROLE_CHANGE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SLOT_OFFSET, 0, peer_id, tid), NULL);
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SWITCH_REQ, 0, peer_id, tid), NULL);
      }
      return 1;
    } else {
      lmp_input_params = LMP_HOST_CONNECTION_REQ;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
      /*Set status flag as Host connection completed*/
      acl_link_mgmt_info->device_status_flags |= HOST_CONN_REQ_COMPLETED;
      acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_PROCESS_CONN_START;
      lmp_process_peripheral_conn_state(ulc, peer_id, 0, 0);
      acl_link_mgmt_info->received_req_flag &= ~ACL_CONNECTION_REQ;
    }
  }
  return BT_STATUS_SUCCESS;
}

void lmp_change_conn_pkt_type(ulc_t *ulc, uint8_t peer_id, uint16_t pkt_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;
  uint8_t lmp_input_params;
  uint32_t lmp_control_word;

  tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  acl_link_mgmt_info->new_pkt_type = (pkt_type) | BIT(BT_DM1_PKT_TYPE);
  if (acl_dev_mgmt_info->ptt == BT_EDR_MODE) {
    if ((pkt_type & EDR_PACKETS_NOT_ALLOWED) == EDR_PACKETS_NOT_ALLOWED) {
      acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
      acl_dev_mgmt_info->requested_ptt = BT_BR_MODE;
      lmp_input_params = BT_BR_MODE;
      lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PACKET_TYPE_TABLE_REQ, peer_id, tid);
      call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
    } else {
      update_new_acl_pkt_type(acl_link_mgmt_info);
      hci_conn_pkt_type_changed_event(ulc, peer_id, BT_ACL, BT_SUCCESS_STATUS);
    }
  } else if (acl_dev_mgmt_info->ptt == BT_BR_MODE) {
    if ((pkt_type & EDR_PACKETS_NOT_ALLOWED) != EDR_PACKETS_NOT_ALLOWED) {
      acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
      acl_dev_mgmt_info->requested_ptt = BT_EDR_MODE;
      lmp_input_params = BT_EDR_MODE;
      lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PACKET_TYPE_TABLE_REQ, peer_id, tid);
      call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
    } else {
      if (((pkt_type & PTYPE_DH5_MAY_BE_USED) || (pkt_type & PTYPE_DM5_MAY_BE_USED)) && (acl_dev_mgmt_info->tx_max_slots < 5)) {
        acl_dev_mgmt_info->requested_max_slot = 5;
        lmp_input_params = 5;
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_MAX_SLOT_REQ, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
      } else if (((pkt_type & PTYPE_DH3_MAY_BE_USED) || (pkt_type & PTYPE_DM3_MAY_BE_USED)) && (acl_dev_mgmt_info->tx_max_slots < 3)) {
        acl_dev_mgmt_info->requested_max_slot = 3;
        lmp_input_params = 3;
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_MAX_SLOT_REQ, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
      } else {
        update_new_acl_pkt_type(acl_link_mgmt_info);
        hci_conn_pkt_type_changed_event(ulc, peer_id, BT_ACL, BT_SUCCESS_STATUS);
      }
    }
  }
}

void lmp_start_enc_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = 0;

  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }
  lmp_input_params.tid = tid;
  lmp_input_params.peer_id = peer_id;
  if (acl_link_mgmt_info->device_status_flags & AUTHENTICATION_COMPLETE) {
    if (!(acl_link_mgmt_info->host_event_notification_flags & ENC_MODE_CHANGE)) {
      if (acl_link_mgmt_info->sec_sm_flags_1 & SENT_AU_RAND) {
        SET_FLAG(acl_link_mgmt_info->sec_sm_flags_1, START_ENC_REQ_RCVD);
        return;
      }

      acl_link_mgmt_info->sec_sm_flags_1 &= ~(START_ENC_REQ_RCVD | AURAND_NOT_ACCEPTED | SENT_AU_RAND);
      acl_link_mgmt_info->check_pending_req_from_hci |= START_ENCRYPTION;
      acl_link_mgmt_info->host_event_notification_flags |= ENC_MODE_CHANGE;
      acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
      acl_link_mgmt_info->lmp_connection_state = ENC_MODE_REQ_SENT_STATE;

      lmp_input_params.input_params[0] = BT_ENCRYPTION_ENABLE;

      lmp_input_params.opcode = LMP_ENCRYPTION_MODE_REQ;
      lmp_input_params.reason_code = 0;
      // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO

      // lmp_form_tx_pkt(ulc, LMP_ENCRYPTION_MODE_REQ, tid, peer_id, input_params, 0);
    }
  } else {
    hci_enc_changed_event(ulc, acl_dev_mgmt_info->conn_handle, AUTHENTICATION_FAILURE, ENC_DISABLED);
  }
  return;
}
void lmp_stop_enc_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = 0;
  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }
  lmp_input_params.tid = tid;
  lmp_input_params.peer_id = peer_id;

  acl_link_mgmt_info->check_pending_req_from_hci |= STOP_ENCRYPTION;
  acl_link_mgmt_info->host_event_notification_flags |= ENC_MODE_CHANGE;
  acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;

  lmp_input_params.input_params[0] = BT_ENCRYPTION_DISABLE;
  lmp_input_params.opcode = LMP_ENCRYPTION_MODE_REQ;
  lmp_input_params.reason_code = 0;
  // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
  // lmp_form_tx_pkt(ulc, LMP_ENCRYPTION_MODE_REQ, tid, peer_id, input_params, 0);
  return;
}
uint8_t lmp_change_conn_link_key_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_enc_info_t *acl_enc_info = &(acl_peer_info->acl_enc_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = 0;

  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }
  lmp_input_params.tid = tid;
  lmp_input_params.peer_id = peer_id;
  acl_link_mgmt_info->check_pending_req_from_hci |= CHANGE_CONN_LINK_KEY;
  acl_link_mgmt_info->check_pending_msg_flag |= LMP_COMB_KEY_SENT;
  acl_link_mgmt_info->lmp_connection_state = LMP_COMB_KEY_SENT_STATE;

  generate_rand_num(RAND_NUM_SIZE, lmp_input_params.input_params, PSEUDO_RAND_NUM);
  e21_algo(lmp_input_params.input_params, btc_dev_info->btc_dev_mgmt_info.bt_mac_addr, acl_enc_info->device_key);

  *(uint32_t *)&lmp_input_params.input_params[0] ^= (*(uint32_t *)&acl_enc_info->link_key[0]);
  *(uint32_t *)&lmp_input_params.input_params[4] ^= (*(uint32_t *)&acl_enc_info->link_key[4]);
  *(uint32_t *)&lmp_input_params.input_params[8] ^= (*(uint32_t *)&acl_enc_info->link_key[8]);
  *(uint32_t *)&lmp_input_params.input_params[12] ^= (*(uint32_t *)&acl_enc_info->link_key[12]);

  lmp_input_params.opcode = LMP_COMB_KEY;
  lmp_input_params.reason_code = 0;
  // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
  // lmp_form_tx_pkt(ulc, LMP_COMB_KEY, tid, peer_id, input_params, 0);
  return BT_SUCCESS_STATUS;
}
void lmp_remote_dev_features_req_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = 0;
  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }

  /* Form LMP tx pkt for features_req */
  acl_link_mgmt_info->check_pending_req_from_hci |= FEATURES_REQ_SENT;
  if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    if (acl_link_mgmt_info->lmp_slave_connection_state != SLAVE_FEATURE_REQ_SENT_STATE) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_REQ, 0, peer_id, tid), NULL);
    }
  } else {
    if (acl_link_mgmt_info->lmp_connection_state != FEATURES_REQ_SENT_STATE) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_REQ, 0, peer_id, tid), NULL);
    }
  }
  return;
}
void lmp_remote_ext_features_req(ulc_t *ulc, uint8_t peer_id, uint8_t requested_feature_page)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid = 0;

  tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  /* Form LMP tx pkt for ext_features_req */
  acl_link_mgmt_info->check_pending_req_from_hci |= FEATURES_REQ_EXT_SENT;
  if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    if (acl_link_mgmt_info->lmp_slave_connection_state != SLAVE_EXT_FEATURES_REQ_SENT_STATE) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE , LMP_FEATURES_REQ_EXT, peer_id, tid), &requested_feature_page);
    }
  } else {
    if (acl_link_mgmt_info->lmp_connection_state != EXT_FEATURES_REQ_SENT_STATE) {
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_FEATURES_REQ_EXT, peer_id, tid), &requested_feature_page);
    }
  }
  return;
}
void lmp_read_remote_version_req_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;

  /* If the remote device is a PERIPHERAL, initiating device is a central, so TID = 1*/
  tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  /* Form LMP tx pkt for version_req */
  acl_link_mgmt_info->check_pending_req_from_hci |= HCI_VERSION_REQ_SENT;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_VERSION_REQ, 0, peer_id, tid), NULL);
  return;
}
void lmp_read_clk_offset_req_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;

  /* Determine TID based on device mode */
  tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  /* Set flag indicating clock offset request is pending from HCI */
  acl_link_mgmt_info->check_pending_req_from_hci |= HCI_READ_CLK_OFFSET;
  /* Form LMP tx pkt for clock_offset_req */
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_CLKOFFSET_REQ, 0, peer_id, tid), NULL);
  return;
}

void lmp_remote_dev_name_req_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  uint8_t tid;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NAME_REQ, 0, peer_id, tid), NULL);
}

void lmp_start_connection(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  btc_dev_info->btc_dev_mgmt_info.dev_status_flags &= ~ROLE_CHANGED;
  acl_link_mgmt_info->lmp_connection_state = START_CONNECTION;
  acl_link_mgmt_info->check_pending_msg_flag = 0;
  lmp_process_central_conn_state(ulc, peer_id, 0, 0);
  return;
}

uint8_t lmp_process_central_conn_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code) /* Ref:V2-P:F-3.1 */
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t lmp_input_params;
  uint8_t tid;

  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info_s = &acl_peer_info->conn_pwr_ctrl_info;
  tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);

  switch (acl_link_mgmt_info->lmp_connection_state) {
    case START_CONNECTION:
    {
      acl_link_mgmt_info->check_pending_msg_flag |= FEATURES_REQ_SENT;
      acl_link_mgmt_info->lmp_connection_state = FEATURES_REQ_SENT_STATE;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_REQ, 0, peer_id, tid), NULL);
      break;
    }
    case FEATURES_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == FEATURES_RESP_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~FEATURES_REQ_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= EXT_FEATURES_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = EXT_FEATURES_REQ_SENT_STATE;
        if (acl_dev_mgmt_info->features_bitmap[7] & LMP_EXTFEATURES) {
          lmp_input_params = 1;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_FEATURES_REQ_EXT, peer_id, tid), &lmp_input_params);
        } else {
          acl_link_mgmt_info->check_pending_msg_flag |= NAME_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = NAME_REQ_SENT_STATE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NAME_REQ, 0, peer_id, tid), NULL);
        }
      }
      break;
    }
    case EXT_FEATURES_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == EXT_FEATURES_RESP_RCVD) {
#ifdef BT_EXTENDED_FEATURES_BITMAP2
        if (acl_dev_mgmt_info->max_page_num == 2) {
          acl_link_mgmt_info->lmp_connection_state = EXT_FEATURES_REQ_PAGE_2_SENT_STATE;
          lmp_input_params = MAX_NUM_SUPPORTED_FEATURE_PAGE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_FEATURES_REQ_EXT, peer_id, tid), &lmp_input_params);
        } else
#endif
        {
          acl_link_mgmt_info->check_pending_msg_flag &= ~EXT_FEATURES_REQ_SENT;
          acl_link_mgmt_info->check_pending_msg_flag |= NAME_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = NAME_REQ_SENT_STATE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NAME_REQ, 0, peer_id, tid), NULL);
        }
      }
      break;
    }
    case EXT_FEATURES_REQ_PAGE_2_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == EXT_FEATURES_RESP_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~EXT_FEATURES_REQ_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= NAME_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = NAME_REQ_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NAME_REQ, 0, peer_id, tid), NULL);
      }
      break;
    }
    case NAME_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == NAME_RESP_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~NAME_REQ_SENT;
        if (acl_link_mgmt_info->conn_purpose == CONN_FOR_DATA_PKTS_EXCHANGE) {
          acl_link_mgmt_info->check_pending_msg_flag |= VERSION_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = VERSION_REQ_SENT_STATE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_VERSION_REQ, 0, peer_id, tid), NULL);
        }
      }
      break;
    }
    case VERSION_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == VERSION_RESP_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~VERSION_REQ_SENT;
        acl_link_mgmt_info->check_pending_msg_flag |= HOST_CONNECTION_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = HOST_CONNECTION_REQ_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_HOST_CONNECTION_REQ, 0, peer_id, tid), NULL);
      }
      break;
    }
    case HOST_CONNECTION_REQ_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == HOST_CONNECTION_REQ_REJECTED) {
        if (acl_link_mgmt_info->check_pending_msg_flag & HOST_CONNECTION_REQ_SENT) {
          dm_acl_conn_indication_from_lm(ulc, peer_id, reason_code);
          lmp_input_params = reason_code;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_DETACH, 0, peer_id, tid), &lmp_input_params);
        }
      } else if (lmp_recvd_pkt_flag == HOST_CONNECTION_REQ_ACCEPTED) {
        acl_link_mgmt_info->device_status_flags &= ~PAUSE_ACLD_TRAFFIC;

        /* Sending setup complete */
        acl_link_mgmt_info->check_pending_msg_flag |= SETUP_COMPLETE_SENT;
        acl_link_mgmt_info->lmp_connection_state = SETUP_COMPLETE_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SETUP_COMPLETE, 0, peer_id, tid), NULL);
        if (acl_link_mgmt_info->device_status_flags & SETUP_COMPLETE) {
          acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_DONE;
          conn_pwr_ctrl_info_s->enable_power_control = ENABLE_POWER_CNTRL;
          acl_link_mgmt_info->check_pending_msg_flag |= MAX_SLOT_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = MAX_SLOT_REQ_SENT_STATE;
          acl_dev_mgmt_info->requested_max_slot = 5;
          lmp_input_params = 5;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_MAX_SLOT_REQ, 0, peer_id, tid), &lmp_input_params);
        }
        /* Sending SET_AFH after sending setup complete */
        if ((btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_AFH_CAP_MASTER)
            && (acl_dev_mgmt_info->features_bitmap[4] & LMP_AFH_CAP_SLAVE)) {
          lmp_input_params = AFH_ENABLE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SET_AFH, 0, peer_id, tid), &lmp_input_params);
        }
      }
      acl_link_mgmt_info->check_pending_msg_flag &= ~HOST_CONNECTION_REQ_SENT;
      break;
    }
    case SETUP_COMPLETE_SENT_STATE:
    {
      if (lmp_recvd_pkt_flag == SETUP_COMPLETE_RCVD) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~SETUP_COMPLETE_SENT;
        acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_DONE;
        // conn_pwr_ctrl_info_s->enable_power_control = DISABLE_POWER_CNTRL; //To disable Power Control as master
        conn_pwr_ctrl_info_s->enable_power_control = ENABLE_POWER_CNTRL;
        dm_acl_conn_indication_from_lm(ulc, peer_id, BT_STATUS_SUCCESS);
        /* It is optional to give max slot req. Can be commented during testing */
        acl_link_mgmt_info->check_pending_msg_flag |= MAX_SLOT_REQ_SENT;
        acl_link_mgmt_info->lmp_connection_state = MAX_SLOT_REQ_SENT_STATE;
        acl_dev_mgmt_info->requested_max_slot = 5;
        lmp_input_params = 5;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_MAX_SLOT_REQ, 0, peer_id, tid), &lmp_input_params);
        if ((btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_CQDDR) && (acl_dev_mgmt_info->features_bitmap[1] & LMP_CQDDR)) {
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_AUTO_RATE, 0, peer_id, tid), NULL);
        }
      }
      break;
    }
    case TIMING_ACCURACY_REQ_SENT_STATE:
    {
      // TODO
      break;
    }
    case MAX_SLOT_REQ_SENT_STATE:
    {
        if (!(acl_link_mgmt_info->device_status_flags & DEVICE_PTT_RCVD)) {
          acl_link_mgmt_info->check_pending_msg_flag |= PTT_REQ_SENT;
          acl_link_mgmt_info->lmp_connection_state = PTT_REQ_SENT_STATE;
          acl_link_mgmt_info->device_status_flags |= PAUSE_ACLD_TRAFFIC;
          if ((btc_dev_info->btc_dev_mgmt_info.features_bitmap[3] & (LMP_EDR_ACL_2M | LMP_EDR_ACL_3M))
          && (acl_dev_mgmt_info->features_bitmap[3] & (LMP_EDR_ACL_2M | LMP_EDR_ACL_3M))) {
            lmp_input_params = BT_EDR_MODE;
          } else {
            lmp_input_params = BT_BR_MODE;
          }
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_PACKET_TYPE_TABLE_REQ, peer_id, tid), &lmp_input_params);
        }
      break;
    }
    case PTT_REQ_SENT_STATE:
    {
      acl_link_mgmt_info->check_pending_msg_flag &= ~PTT_REQ_SENT;
      acl_link_mgmt_info->device_status_flags |= DEVICE_PTT_RCVD;
      if (lmp_recvd_pkt_flag == PTT_ACCEPTED) {
        if ((btc_dev_info->btc_dev_mgmt_info.features_bitmap[3] & (LMP_EDR_ACL_2M | LMP_EDR_ACL_3M))
          && (acl_dev_mgmt_info->features_bitmap[3] & (LMP_EDR_ACL_2M | LMP_EDR_ACL_3M))) {
            acl_dev_mgmt_info->ptt = BT_EDR_MODE;
          } else {
            acl_dev_mgmt_info->ptt = BT_BR_MODE;
          }
      } else if (lmp_recvd_pkt_flag == PTT_REJECTED) {
        acl_dev_mgmt_info->ptt = BT_BR_MODE;
      } else {
        BTC_ASSERT(BT_INVALID_PTT);
      }
      acl_link_mgmt_info->device_status_flags &= ~PAUSE_ACLD_TRAFFIC;
      break;
    }
    default:
    {
      break;
    }
  }
  return BT_STATUS_SUCCESS;
}

void update_feature_map(acl_peer_info_t *acl_peer_info, uint8_t *rx_pkt_buff, ulc_t *ulc_ptr)
{
  memcpy(acl_peer_info->acl_dev_mgmt_info.features_bitmap, rx_pkt_buff, FEATURE_MASK_LENGTH);
  if ((!(acl_peer_info->acl_dev_mgmt_info.features_bitmap[1] & LMP_PCONTROL_REQ))
      && (ulc_ptr->btc_dev_info.btc_dev_mgmt_info.features_bitmap[2] & LMP_PCONTROL)) {
    acl_peer_info->conn_pwr_ctrl_info.tx_power_index = 4;
    acl_peer_info->conn_pwr_ctrl_info.max_allowed_power_index = 4;
  }
}

uint8_t lmp_process_peripheral_conn_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code)
{
  uint8_t lmp_input_params[2];
  uint8_t tid;
  (void)reason_code;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  conn_pwr_ctrl_info_t *peripheral_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  tid = PERIPHERAL_INITIATED_TID;

  switch (acl_link_mgmt_info->lmp_slave_connection_state) {
    case SLAVE_PROCESS_CONN_START: {
      acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_FEATURE_REQ_SENT_STATE;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_REQ, 0, peer_id, tid), NULL);
      break;
    }
    case SLAVE_PTT_REQ_SENT_STATE: {
      if (lmp_recvd_pkt_flag == PTT_ACCEPTED) {
        // if (bt_ram_info->bt_bdr_mode & IS_BDR_MODE_ONLY)
        //   slave_dev_info->ptt = BT_BR_MODE;
        // else
        acl_dev_mgmt_info->ptt = BT_EDR_MODE;
      }

      if (acl_link_mgmt_info->check_slave_pending_msg_flag & SLAVE_PTT_REQ_SENT) {
        acl_link_mgmt_info->check_slave_pending_msg_flag &= ~SLAVE_PTT_REQ_SENT;
      }
      acl_link_mgmt_info->device_status_flags &= ~PAUSE_ACLD_TRAFFIC;
      acl_link_mgmt_info->device_status_flags |= DEVICE_PTT_RCVD;
      acl_link_mgmt_info->check_slave_pending_msg_flag |= SLAVE_SETUP_COMPLETE_SENT;
      acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_SETUP_COMPLETE_SENT_STATE;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SETUP_COMPLETE, 0, peer_id, tid), NULL);
      if (acl_link_mgmt_info->device_status_flags & SETUP_COMPLETE) {
        acl_link_mgmt_info->lmp_connection_state = LM_LEVEL_CONN_DONE;
      }
      break;
    }
    case SLAVE_SETUP_COMPLETE_SENT_STATE: {
      if (lmp_recvd_pkt_flag == SETUP_COMPLETE_RCVD) {
        if (acl_link_mgmt_info->check_slave_pending_msg_flag & SLAVE_SETUP_COMPLETE_SENT) {
          acl_link_mgmt_info->check_slave_pending_msg_flag &= ~SLAVE_SETUP_COMPLETE_SENT;
          acl_dev_mgmt_info->latency = 10000; //! 10ms
          lmp_qos_setup(ulc, peer_id);
        }
        acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_DONE;
        dm_acl_conn_indication_from_lm(ulc, peer_id, BT_STATUS_SUCCESS);
        peripheral_pwr_ctrl_info->enable_power_control = ENABLE_POWER_CNTRL;
        //master_info->enable_power_control = DISABLE_POWER_CNTRL; //To Disable Power Control as slave
        if (acl_dev_mgmt_info->requested_max_slot < 5) {
          acl_link_mgmt_info->check_slave_pending_msg_flag |= SLAVE_MAX_SLOT_REQ_SENT;
          acl_dev_mgmt_info->requested_max_slot = 5;
          lmp_input_params[0]      = 5;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_MAX_SLOT_REQ, 0, peer_id, tid), &lmp_input_params);
        }
      }
      break;
    }
    case SLAVE_TIMING_ACCURACY_REQ_SENT_STATE: {
      if (lmp_recvd_pkt_flag == TIMING_ACCURACY_RES_RECVD) {
        acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_FEATURE_REQ_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_REQ, 0, peer_id, tid), NULL);
      }
      break;
    }
    case SLAVE_FEATURE_REQ_SENT_STATE: {
      if (lmp_recvd_pkt_flag == FEATURES_RESP_RCVD) {
        if (acl_dev_mgmt_info->features_bitmap[7] & LMP_EXTFEATURES) {
          acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_EXT_FEATURES_REQ_SENT_STATE;
          // Only send LMP_FEATURES_REQ_EXT if there's no HCI request pending
          if (!(acl_link_mgmt_info->check_pending_req_from_hci & FEATURES_REQ_EXT_SENT)) {
            lmp_input_params[0] = 1;
            call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_FEATURES_REQ_EXT, peer_id, tid), &lmp_input_params);
          } 
        } else {
          acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_VERSION_REQ_SENT_STATE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_VERSION_REQ, 0, peer_id, tid), NULL);
        }
      }
      break;
    }
    case SLAVE_EXT_FEATURES_REQ_SENT_STATE: {
      if (lmp_recvd_pkt_flag == EXT_FEATURES_RESP_RCVD) {
#ifdef BT_EXTENDED_FEATURES_BITMAP2
        if (acl_dev_mgmt_info->max_page_num == 2) {
          acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_EXT_FEATURES_REQ_PAGE2_SENT_STATE;
          lmp_input_params[0]               = MAX_NUM_SUPPORTED_FEATURE_PAGE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_FEATURES_REQ_EXT, peer_id, tid), &lmp_input_params);
        } else
#endif
        {
          acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_VERSION_REQ_SENT_STATE;
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_VERSION_REQ, 0, peer_id, tid), NULL);
        }
      }
      break;
    }
    case SLAVE_EXT_FEATURES_REQ_PAGE2_SENT_STATE: {
      if (lmp_recvd_pkt_flag == EXT_FEATURES_RESP_RCVD) {
        acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_VERSION_REQ_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_VERSION_REQ, 0, peer_id, tid), NULL);
      }
      break;
    }
    case SLAVE_VERSION_REQ_SENT_STATE: {
      if (lmp_recvd_pkt_flag == VERSION_RESP_RCVD) {
        acl_link_mgmt_info->check_slave_pending_msg_flag |= SLAVE_SETUP_COMPLETE_SENT;
        acl_link_mgmt_info->lmp_slave_connection_state = SLAVE_SETUP_COMPLETE_SENT_STATE;
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SETUP_COMPLETE, 0, peer_id, tid), NULL);
        if (acl_link_mgmt_info->device_status_flags & SETUP_COMPLETE) {
          acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_DONE;
          peripheral_pwr_ctrl_info->enable_power_control = ENABLE_POWER_CNTRL;
          if (acl_dev_mgmt_info->requested_max_slot < 5) {
            acl_link_mgmt_info->check_slave_pending_msg_flag |= SLAVE_MAX_SLOT_REQ_SENT;
            acl_dev_mgmt_info->requested_max_slot = 5;
            lmp_input_params[0]      = 5;
            call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_MAX_SLOT_REQ, 0, peer_id, tid), &lmp_input_params);
          }
        }
      }
      break;
    }
    case SLAVE_MAX_SLOT_REQ_SENT_STATE: {
      //TODO
      break;
    }
    default: {
      break;
    }
  }
  return BT_STATUS_SUCCESS;
}

uint8_t get_no_of_good_channels(uint8_t *addr)
 {
  uint8_t good_channel_count = 0;
  uint8_t bit_pos;
  for (bit_pos = 0; bit_pos < NUM_OF_MAX_CHANNELS; bit_pos++) {
    if (addr[(bit_pos >> BYTE_POSITION_SHIFT)] & BIT((bit_pos & BIT_POSITION_MASK))) {
      good_channel_count++;
    }
  }
  return good_channel_count;
 }

uint16_t get_slot_offset(ulc_t *ulc, uint8_t peer_id)
{
  (void)ulc;
  (void)peer_id;
  return 0;
}
