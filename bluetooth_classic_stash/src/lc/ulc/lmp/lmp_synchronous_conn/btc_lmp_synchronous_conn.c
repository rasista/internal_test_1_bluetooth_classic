
#include "btc_lmp.h"
#include "lmp.h"
#include "btc_dm_synchronous_conn.h"
#include "btc_lmp_synchronous_conn.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"

void copy_esco_params(esco_link_params_t *dest_params, esco_link_params_t *src_params, uint8_t reset)
{
  if (reset == RESET_ESCO_PARAMS) {
    /* Reset esco link params */
    memset((uint8_t *)src_params, 0, BT_ESCO_LINK_PARAMS_SIZE);
  } else {
    /* Reset esco link params */
    memcpy((uint8_t *)dest_params, (uint8_t *)src_params, BT_ESCO_LINK_PARAMS_SIZE);
    /* esco_handle_ocupied and esco_conn_handle will be updated later.*/
    if (reset == COPY_N_RESET_ESCO_PARAMS) {
      /* Reset esco link params */
      memset((uint8_t *)src_params, 0, BT_ESCO_LINK_PARAMS_SIZE);
    }
  }
}


uint8_t lmp_esco_params_accept_check(ulc_t *ulc,
                                     uint8_t peer_id,
                                     esco_link_params_t *esco_link_update_params)
{
  uint8_t status = 0;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t update_tx_bandwidth, update_rx_bandwidth;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  /**********Negotiation and renegotiation rules.***************/
  /* RULE 1 : Negotiation state shall NOT be set to zero after the initial LMP_ESCO_LINK_REQ is sent.
   * RULE 2 : Air mode, TX and RX bandwidths are Non-negotiable.
   * RULE 3 : If Latency violation happens - ( Tesco + Wesco + reserved_sync_slots > allowed local latency ),
              propose new params with negotiation state = 3 OR send LMP_not_accepted_ext.
   * RULE 4 : If LMP_ESCO_LINK_REQ is rcvd with nego_state = 3, do not send params larger or equal to prev params.
   * RULE 5 : Negotiation state = 2, Reserved slot violation.
   * RULE 6 : If requested params are not supported, propose supported params with negotiation state  = 4 OR send LMP_NOT_ACCEPTED_EXT.
   * RULE 7 : If proposing new params for reasons other than above, set negotiation state = 1.
   * */

  /* RULE 3 : slot packets (EV4, EV5, 2EV5, 3EV5) support only transparent air mode according to spec.*/
  if ((esco_link_update_params->air_mode != TRANSPARENT_MODE) && ((esco_link_update_params->central_to_peripheral_pkt_type & 0x0C) == 0x0C)) {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }

  // RULE 2 :
  if (((esco_link_update_params->air_mode == TRANSPARENT_MODE) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[2] & LMP_SYNCH_DATA)) || (((esco_link_update_params->air_mode == MU_LAW_MODE) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_ULAW)) || ((esco_link_update_params->air_mode == A_LAW_MODE) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_ALAW)) || ((esco_link_update_params->air_mode == CVSD_MODE) && (btc_dev_info->btc_dev_mgmt_info.features_bitmap[2] & LMP_CVSD)))) {
    status |= ACCEPT_PARAMS_SET; /* PARAM ACCEPTED*/
  } else {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }
  if (esco_link_update_params->esco_offset != 0) {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }
  // RULE 2 :
  //! Calculating TX and RX BW based on the requested pkt length.
  if (acl_peer_info->acl_dev_mgmt_info.mode == BT_CENTRAL_MODE) {
    //! We are slave. So RX BW will be caluculated from master_to_slave_pkt_len.
    update_rx_bandwidth =
      1600 * ((esco_link_update_params->peripheral_to_central_pkt_len) / (esco_link_update_params->esco_interval));
    update_tx_bandwidth =
      1600 * ((esco_link_update_params->central_to_peripheral_pkt_len) / (esco_link_update_params->esco_interval));
  } else {
    //! We are master. So RX BW will be caluculated from slave_to_master_pkt_len.
    update_rx_bandwidth =
      1600 * ((esco_link_update_params->peripheral_to_central_pkt_len) / (esco_link_update_params->esco_interval));
    update_tx_bandwidth =
      1600 * ((esco_link_update_params->central_to_peripheral_pkt_len) / (esco_link_update_params->esco_interval));
  }

  /*Doubt: LC layer says 3EV5 can support upto 540 bytes. But LMP says just (10 * Tesco)/2 = 80 bytes */
#ifndef SIM_9118
  if ((update_rx_bandwidth <= esco_link_update_params->rx_bandwidth) && (update_tx_bandwidth <= esco_link_update_params->tx_bandwidth)) {
    status |= ACCEPT_PARAMS_SET; /* PARAM ACCEPTED*/
  } else {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }
#endif

  //! RULE 6 : Packet type supported or not check.
  if (esco_link_update_params->peripheral_to_central_pkt_type != esco_link_update_params->central_to_peripheral_pkt_type) {
    esco_link_update_params->peripheral_to_central_pkt_type = esco_link_update_params->central_to_peripheral_pkt_len;
    status |= SEND_ANOTHER_PARAMS_SET; /* DIFF PARAM REQUESTED*/
  }
  if ((((esco_link_update_params->central_to_peripheral_pkt_type & 0x30) == 0x30) && (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_EDR_ESCO_3M))) || (((esco_link_update_params->central_to_peripheral_pkt_type & 0x30) == 0x20) && (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_EDR_ESCO_2M)))) {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }

  /* Here we just need to check if EV4 and EV5 packet types are supported or not.
   * EV3 packet type is always supported, so if LMP supports 2M and 3M => 2EV3 and 3EV3 packet types are also supported. */
  if (((esco_link_update_params->central_to_peripheral_pkt_type == LMP_EV4_PKT_TYPE) && (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[4] & LMP_EV4))) //! EV4 check
      || ((esco_link_update_params->central_to_peripheral_pkt_type >= LMP_EV5_PKT_TYPE) && (!(esco_link_update_params->central_to_peripheral_pkt_type & BIT(1))) && (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[4] & LMP_EV5)))) {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }

  if (esco_link_update_params->central_to_peripheral_pkt_type & BIT(1)) {
    //! EV3, 2EV3, 3EV3 - single slot packets
    esco_link_update_params->esco_reserved_slot_num = 2;
  } else {
    //! EV4, EV5, 2EV5, 3EV5 - 3 slot packets
    esco_link_update_params->esco_reserved_slot_num = 6;
  }

  //! RULE 6 :
  if ((esco_link_update_params->esco_window == 0) && ((esco_link_update_params->retry_effort == 0) || (esco_link_update_params->retry_effort == 0xFF))) {
    //! If the remote device does not want any retransmission, lets make the retry window = 0.
    esco_link_update_params->esco_window = 0;
    status |= ACCEPT_PARAMS_SET; /* PARAM ACCEPTED*/
  } else if (((esco_link_update_params->esco_window != 0)) && (esco_link_update_params->retry_effort & 0x03)) {
    //! TODO : For now handling only one retry.
    if (esco_link_update_params->esco_window == esco_link_update_params->esco_reserved_slot_num) {
      status |= ACCEPT_PARAMS_SET; /* PARAM ACCEPTED*/
    } else {
      esco_link_update_params->esco_window = esco_link_update_params->esco_reserved_slot_num;
      status |= SEND_ANOTHER_PARAMS_SET; /* DIFF PARAM REQUESTED*/
    }
  } else {
    esco_link_update_params->esco_window = 0;
    status |= SEND_ANOTHER_PARAMS_SET; /* DIFF PARAM REQUESTED*/
  }

  esco_link_update_params->esco_reserved_slot_num += esco_link_update_params->esco_window;

  //! RULE 3 : Rejecting esco link req if latency violation is caused by the requested params.
  if (((esco_link_update_params->esco_interval + esco_link_update_params->esco_reserved_slot_num) * 625) > ((esco_link_update_params->max_latency) * 1000)) {
    status |= REJECT_PARAMS_SET; /* PARAM NOT SUPPORTED*/
    return status;
  }

  return status;
  // TODO : Add support for negotiation in case of latency violation.
  // TODO : Add support for different master and slave packet types.
  // TODO : Add reserved slot violation check.
}

uint8_t lmp_negotiate_esco_params(ulc_t *ulc, uint8_t peer_id, uint8_t tid, uint8_t esco_nego_status)
{
#ifndef RF_TESTING
  uint8_t status;
  esco_link_params_t *esco_link_params, *esco_link_temp_params;
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  esco_link_params = &acl_peer_info->esco_link_params[acl_peer_info->esco_info.esco_handle];
  esco_link_temp_params = &acl_peer_info->esco_link_params[0];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;

  status = lmp_esco_params_accept_check(ulc, peer_id, esco_link_temp_params);
  lmp_input_params.tid = tid;
  lmp_input_params.peer_id = peer_id;

  //! When the ESCO_LINK_REQ by peer is rejected.
  if (status & REJECT_PARAMS_SET) {
    //! Sending LMP_NOT_ACCEPTED_EXT.
    lmp_input_params.exopcode = LMP_NOT_ACCEPTED_EXT;
    lmp_input_params.input_params[0] = LMP_ESCAPE_OPCODE;
    lmp_input_params.input_params[1] = LMP_ESCO_LINK_REQ;
    lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
    lmp_input_params.reason_code = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    // lmp_form_tx_pkt(ulc,LMP_ESCAPE_OPCODE,tid,peer_id,input_params,UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
    copy_esco_params(esco_link_params, esco_link_temp_params, RESET_ESCO_PARAMS);
    if (esco_nego_status != RENEGOTIATE_REQ) {
      dm_esco_link_formation_indication_from_lm(ulc, peer_id, UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
      if (acl_link_mgmt_info->check_pending_msg_flag & SYNC_LINK_REQ_SENT) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~SYNC_LINK_REQ_SENT;
      }
      if (esco_link_params->esco_handle_occupied != ESCO_HANDLE_OCCUPIED) {
        memset((uint8_t *)&acl_peer_info->esco_info, 0, sizeof(esco_info_t));
      }
    }
  }
  //! When we send ESCO_LINK_REQ in response to the ESCO_LINK_REQ by peer to request for another set of parameters.
  else if (status & SEND_ANOTHER_PARAMS_SET) {
    esco_link_temp_params->negotiation_state = ESCO_UNDEFINED_STATE;
    lmp_input_params.exopcode = LMP_ESCO_LINK_REQ;
    lmp_input_params.input_params[0] = acl_peer_info->esco_info.esco_handle;
    lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
    lmp_input_params.reason_code = 0;
    // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
    // lmp_form_tx_pkt(ulc, LMP_ESCAPE_OPCODE, tid, peer_id, input_params, 0);
    if (esco_nego_status == RENEGOTIATE_REQ) {
      /* Setting ESCO link request sent flag. This flag is used
       * to decode for what case we have received the LMP_ACCEPTED_EXT resp from the other device. */
      acl_link_mgmt_info->check_pending_msg_flag |= ESCO_SCO_CHANGE_PKT_REQ_SENT;
    } else if (esco_nego_status == HOST_ACCEPT_ESCO_LINK) {
      acl_link_mgmt_info->check_pending_msg_flag |= SYNC_LINK_REQ_SENT;
    }
  }
  //! When we are ready to accept the ESCO_LINK_REQ by peer.
  else if (status & ACCEPT_PARAMS_SET) {
    if ((esco_nego_status == HOST_ACCEPT_ESCO_LINK) && (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE)) {
      /* If local device is master, LMP_ACCEPTED_EXT cannot be sent directly so send link request again */
      lmp_input_params.exopcode = LMP_ESCO_LINK_REQ;
      lmp_input_params.input_params[0] = acl_peer_info->esco_info.esco_handle;
      esco_link_temp_params->esco_lt_addr = bt_get_free_lt_addr(&btc_dev_info->btc_dev_mgmt_info);
      lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
      lmp_input_params.reason_code = 0;
      // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
      // lmp_form_tx_pkt(ulc, LMP_ESCAPE_OPCODE, tid, peer_id, input_params, 0);
      acl_link_mgmt_info->check_pending_msg_flag |= SYNC_LINK_REQ_SENT;
    } else {
      /* Send LMP_ACCEPTED_EXT. */
      lmp_input_params.exopcode = LMP_ACCEPTED_EXT;
      lmp_input_params.input_params[0] = LMP_ESCAPE_OPCODE;
      lmp_input_params.input_params[1] = LMP_ESCO_LINK_REQ;
      lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
      lmp_input_params.reason_code = 0;
      // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
      // lmp_form_tx_pkt(ulc, LMP_ESCAPE_OPCODE, tid, peer_id, input_params, 0);
      copy_esco_params(esco_link_params, esco_link_temp_params, COPY_N_RESET_ESCO_PARAMS);
      acl_peer_info->esco_info.esco_lt_addr = esco_link_params->esco_lt_addr;
      dm_esco_link_formation_indication_from_lm(ulc, peer_id, BT_STATUS_SUCCESS);
      if (acl_link_mgmt_info->check_pending_msg_flag & SYNC_LINK_REQ_SENT) {
        acl_link_mgmt_info->check_pending_msg_flag &= ~SYNC_LINK_REQ_SENT;
      }
      return SEND_SYNC_CONN_CHANGED;
    }
  }
#endif
  return 0;
}


uint8_t lmp_accept_sync_conn_req_from_dm(ulc_t *ulc, uint8_t peer_id, uint8_t accept_cmd_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  lmp_input_params_t lmp_input_params;
(void) lmp_input_params;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t tid;

  /* If the remote device is a MASTER, initiating device is a master, so TID = 0*/
  if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    tid = CENTERAL_INITIATED_TID;
  } else {
    tid = PERIPHERAL_INITIATED_TID;
  }
  lmp_input_params.tid = tid;
  lmp_input_params.peer_id = peer_id;

  if (acl_link_mgmt_info->received_req_flag & SCO_CONNECTION_REQ) {
    acl_link_mgmt_info->received_req_flag &= ~SCO_CONNECTION_REQ;
    /* If local device is master, send link request again */
    if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
      /* Sending LMP accepted frame */
      lmp_input_params.input_params[0] = LMP_SCO_LINK_REQ;
      lmp_input_params.opcode = LMP_ACCEPTED;
      lmp_input_params.reason_code = 0;
      // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
      // lmp_form_tx_pkt(ulc, LMP_ACCEPTED, tid, peer_id, input_params, 0);
      // TODO:Wait for ACK and indicate conn_complete from LC?
      /* Indicate sco link formation to DM */
      dm_sco_link_formation_indication_from_lm(ulc, peer_id, accept_cmd_type, BT_STATUS_SUCCESS);
    } else {
      lmp_input_params.input_params[0] = acl_dev_mgmt_info->sco_handle;
      lmp_input_params.opcode = LMP_SCO_LINK_REQ;
      lmp_input_params.reason_code = 0;
      // call_lmp_tx_handler(ulc, &lmp_input_params);   TODO
      // lmp_form_tx_pkt(ulc, LMP_SCO_LINK_REQ, tid, peer_id, input_params, 0);
      /* Setting SCO link request sent flag */
      acl_link_mgmt_info->check_pending_msg_flag |= SYNC_LINK_REQ_SENT;
    }
  } else if (acl_link_mgmt_info->received_req_flag & ESCO_CONNECTION_REQ) {
    acl_link_mgmt_info->received_req_flag &= ~ESCO_CONNECTION_REQ;
    lmp_negotiate_esco_params(ulc, peer_id, tid, HOST_ACCEPT_ESCO_LINK);
  }

  return BT_STATUS_SUCCESS;
}


void lmp_SCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_SCO_LINK_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_SCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
  sco_link_params_t *sco_link_params;
  sco_update_params_t *sco_update_params;
  sco_link_params   = &acl_peer_info->sco_link_params[lmp_input_params->input_params[0]];
  sco_update_params = &acl_peer_info->sco_update_params[lmp_input_params->input_params[0]];

  if ((acl_peer_info->acl_dev_mgmt_info.mode == BT_CENTRAL_MODE)
      && (acl_peer_info->acl_link_mgmt_info.check_pending_req_from_hci & CHANGE_PKT_TYPE)) {
    /* SCO handle */
    SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(lmp_tx_pkt->data, sco_link_params->master_given_sco_handle);
  } else {
    /* SCO handle */
    SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(lmp_tx_pkt->data, sco_link_params->master_given_sco_handle);
  }

  /* Timing control flags */
  /* TODO: Add support for CLK_INIT_2_MODE, similar to ESCO */
  SET_LMP_SCO_LINK_REQ_PARAMETER_TIMING_CONTROL_FLAGS(lmp_tx_pkt->data, CLK_INIT_1_MODE);

  //pkt[2] = sco_link_params->initialisation_flag;

  /* DSCO */
  SET_LMP_SCO_LINK_REQ_PARAMETER_D_SCO(lmp_tx_pkt->data, sco_link_params->sco_offset);

  if (acl_peer_info->acl_link_mgmt_info.check_pending_req_from_hci & CHANGE_PKT_TYPE) {
    /* TSCO */
    SET_LMP_SCO_LINK_REQ_PARAMETER_T_SCO(lmp_tx_pkt->data, sco_update_params->sco_interval);
    /* SCO packet type */
    if (sco_update_params->sco_pkt_type == BT_HV1_PKT_TYPE) {
      SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV1_PKT_TYPE);
    } else if (sco_update_params->sco_pkt_type == BT_HV2_PKT_TYPE) {
      SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV2_PKT_TYPE);
    } else if (sco_update_params->sco_pkt_type == BT_HV3_PKT_TYPE) {
      SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV3_PKT_TYPE);
    }
  } else {
    /* TSCO */
    SET_LMP_SCO_LINK_REQ_PARAMETER_T_SCO(lmp_tx_pkt->data, sco_update_params->sco_interval);

    /* SCO packet type */
    if (sco_link_params->sco_pkt_type == BT_HV1_PKT_TYPE) {
      SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV1_PKT_TYPE);
    } else if (sco_link_params->sco_pkt_type == BT_HV2_PKT_TYPE) {
      SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV2_PKT_TYPE);
    } else if (sco_link_params->sco_pkt_type == BT_HV3_PKT_TYPE) {
      SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV3_PKT_TYPE);
    }
  }
  //pkt[5] = LMP_HV1_PKT_TYPE;/* Making the hard coding thing for the packet type supported */

  /* air mode */
  SET_LMP_SCO_LINK_REQ_PARAMETER_AIR_MODE(lmp_tx_pkt->data, sco_link_params->air_mode);

  if (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE) {
    acl_peer_info->acl_link_mgmt_info.lmp_tx_pending        = 1;
    acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_SCO_LINK_REQ;
  }

  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_remove_SCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_REMOVE_SCO_LINK_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_REMOVE_SCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_REMOVE_SCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_REMOVE_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(lmp_tx_pkt->data, lmp_input_params->input_params[0]);
  SET_LMP_REMOVE_SCO_LINK_REQ_PARAMETER_ERROR_CODE(lmp_tx_pkt->data, lmp_input_params->reason_code);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending        = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_REMOVE_SCO_LINK_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_eSCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  esco_link_params_t *esco_link_temp_params;
  esco_link_temp_params = &acl_peer_info->esco_link_params[0];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_ESCO_LINK_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_ESCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ESCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
  if ((acl_peer_info->acl_dev_mgmt_info.mode == BT_CENTRAL_MODE) && (acl_peer_info->acl_link_mgmt_info.received_req_flag |= ESCO_CONNECTION_REQ)) {
    /* eSCO handle */
    SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(lmp_tx_pkt->data, esco_link_temp_params->master_given_esco_handle);
  } else {
    /* eSCO handle */
    SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(lmp_tx_pkt->data, lmp_input_params->input_params[0]);
  }

  SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_LT_ADDR(lmp_tx_pkt->data, esco_link_temp_params->esco_lt_addr);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_TIMING_CONTROL_FLAGS(lmp_tx_pkt->data, esco_link_temp_params->initialisation_flag);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_D_ESCO(lmp_tx_pkt->data, esco_link_temp_params->esco_offset);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_T_ESCO(lmp_tx_pkt->data, esco_link_temp_params->esco_interval);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_W_ESCO(lmp_tx_pkt->data, esco_link_temp_params->esco_window);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_PACKET_TYPE_C_P(lmp_tx_pkt->data, esco_link_temp_params->central_to_peripheral_pkt_type);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_PACKET_TYPE_P_C(lmp_tx_pkt->data, esco_link_temp_params->peripheral_to_central_pkt_type);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_PACKET_LENGTH_C_P(lmp_tx_pkt->data, esco_link_temp_params->central_to_peripheral_pkt_len);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_PACKET_LENGTH_P_C(lmp_tx_pkt->data, esco_link_temp_params->peripheral_to_central_pkt_len);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_AIR_MODE(lmp_tx_pkt->data, esco_link_temp_params->air_mode);
  SET_LMP_ESCO_LINK_REQ_PARAMETER_NEGOTIATION_STATE(lmp_tx_pkt->data, esco_link_temp_params->negotiation_state);
  acl_peer_info->acl_link_mgmt_info.lmp_ext_tx_pending         = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_ext_opcode = LMP_ESCO_LINK_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_remove_eSCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_REMOVE_ESCO_LINK_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_REMOVE_ESCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_REMOVE_ESCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_REMOVE_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(lmp_tx_pkt->data, lmp_input_params->input_params[1]);
  SET_LMP_REMOVE_ESCO_LINK_REQ_PARAMETER_ERROR_CODE(lmp_tx_pkt->data, lmp_input_params->reason_code);
  acl_peer_info->acl_link_mgmt_info.lmp_ext_tx_pending         = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_ext_opcode = LMP_REMOVE_ESCO_LINK_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}


void lmp_SCO_link_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_remove_SCO_link_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}


void lmp_eSCO_link_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

void lmp_remove_eSCO_link_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  (void) ulc;
  (void) pkb;
  /* RFU */
}

