#include "lmp.h"
#include "btc_lmp_power_control.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_hci_events.h"

const uint8_t pwr_adj_resp_lookup[] = {
  CHANGED_ONE_STEP_FOR_ALL_MODULATIONS,
  MIN_POWER_FOR_ALL_MODULATIONS,
  MAX_POWER_FOR_ALL_MODULATIONS
};

/**
 * @brief Sends a vendor-specific HCI event for power control notifications
 * 
 * This function prepares and sends a vendor-specific HCI event to notify about
 * power control status changes. It creates a structured event with subevent type,
 * notification code, and optional notification data (power index when applicable).
 * The function is only active in simulation mode (SCHED_SIM), otherwise it's
 * a no-op for production builds.
 *
 * @param ulc Pointer to the ULC (Upper Layer Controller) structure
 * @param peer_id The peer device identifier
 * @param notification The notification type to send (e.g., VENDOR_POWER_CONTROL_POWER_INDEX_UPDATED)
 */
void send_power_control_vendor_event(ulc_t *ulc, uint8_t peer_id, uint8_t notification)
{
#ifdef SCHED_SIM
  printf (" \n parameters received: peer_id=%d, notification=%d\n", peer_id, notification);
  /* Prepare vendor event parameters */
  struct {
    uint8_t subevent;
    uint8_t notification;
    uint8_t notification_data;
    uint8_t reserved[7];
  } __attribute__((packed)) event_params = {
    .subevent = HCI_VENDOR_SPECIFIC_SUBEVENT_UT_POWER_CONTROL_EVENT,
    .notification = notification
  };

  if (notification == VENDOR_POWER_CONTROL_POWER_INDEX_UPDATED) {
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    conn_pwr_ctrl_info_t *conn_pwr_ctrl_info =  &acl_peer_info->conn_pwr_ctrl_info;
    event_params.notification_data = conn_pwr_ctrl_info->tx_power_index;
  }

  /* Send vendor specific event with power control parameters */
  hci_vendor_event((uint8_t *)&event_params);
#else
  (void)(ulc);
  (void)(peer_id);
  (void)(notification);
#endif
}
/**
 * @brief this function will take the average RSSI value and check if GOLDEN threshold is hit or not.
 *        Then it Increases or decreases or ignores the Average RSSI value in RANGE
 *
 * @param ulc
 * @param peer_id
 */
void lmp_initiate_power_control_adjustment(ulc_t *ulc, uint8_t peer_id)
{
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info_p = &acl_peer_info->acl_dev_mgmt_info;
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  struct btc_dev_mgmt_info_s *btc_dev_mgmt_info_p = &ulc->btc_dev_info.btc_dev_mgmt_info;
  uint32_t lmp_control_word = 0;
  uint8_t tid;
  uint8_t lmp_input_params;

  tid = GET_TID_OF_MODE(acl_dev_mgmt_info_p->mode);

  if (IS_LMP_ENHANCED_PWR_SUPPORTED(btc_dev_mgmt_info_p) && IS_LMP_ENHANCED_PWR_SUPPORTED(acl_dev_mgmt_info_p)) {
    if (conn_pwr_ctrl_info->rssi_average < GOLDEN_RECEIVE_POWER_RANGE_LOWER_LIMIT) {
      if (!conn_pwr_ctrl_info->max_power_reached) {
        if ((GOLDEN_RECEIVE_POWER_RANGE_LOWER_LIMIT - conn_pwr_ctrl_info->rssi_average) > 6) {
          lmp_input_params = INCR_TO_MAX_PWR;
        } else {
          lmp_input_params = INCR_PWR_ONE_STEP;
        }
        conn_pwr_ctrl_info->min_power_reached  = LMP_FALSE;
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_POWER_CONTROL_REQ, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
      }
    } else if (conn_pwr_ctrl_info->rssi_average > GOLDEN_RECEIVE_POWER_RANGE_UPPER_LIMIT) {
      if (!conn_pwr_ctrl_info->min_power_reached) {
        lmp_input_params = DECR_PWR_ONE_STEP;
        conn_pwr_ctrl_info->max_power_reached  = LMP_FALSE;
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_POWER_CONTROL_REQ, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
      }
    }
  } else if (IS_LMP_PWR_CONTROL_SUPPORTED(btc_dev_mgmt_info_p) && IS_LMP_PWR_CONTROL_SUPPORTED(acl_dev_mgmt_info_p)) {
    if (conn_pwr_ctrl_info->rssi_average < GOLDEN_RECEIVE_POWER_RANGE_LOWER_LIMIT) {
      if (!conn_pwr_ctrl_info->max_power_reached) {
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_INCR_POWER_REQ, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, NULL);
        conn_pwr_ctrl_info->min_power_reached  = LMP_FALSE;
        conn_pwr_ctrl_info->rssi_avg_pkt_count   = 0;
        conn_pwr_ctrl_info->rssi_average_sum     = 0;
      }
    } else if (conn_pwr_ctrl_info->rssi_average > GOLDEN_RECEIVE_POWER_RANGE_UPPER_LIMIT) {
      if (!conn_pwr_ctrl_info->min_power_reached) {
        lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_DECR_POWER_REQ, 0, peer_id, tid);
        call_lmp_tx_handler(ulc, lmp_control_word, NULL);
        conn_pwr_ctrl_info->max_power_reached  = LMP_FALSE;
        conn_pwr_ctrl_info->rssi_avg_pkt_count   = 0;
        conn_pwr_ctrl_info->rssi_average_sum     = 0;
      }
    }
  } else {
    // No enhanced power control feature is supported, so ignore the request
    // Preferably need counter  for stats
  }
}

/**
 * @brief this function will send updated power index params to LPW from ULC
 *
 * @param ulc
 * @param peer_id
 */
void update_lpw_with_new_power_index(ulc_t *ulc, uint8_t peer_id)
{
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info =  &acl_peer_info->conn_pwr_ctrl_info;
  hss_cmd_set_device_parameters_acl_t set_device_parameters_acl;

  set_device_parameters_acl.connection_device_index = peer_id;
  set_device_parameters_acl.peer_role = acl_peer_info->acl_dev_mgmt_info.mode;
  set_device_parameters_acl.tx_pwr_index = conn_pwr_ctrl_info->tx_power_index;
  set_device_parameters_acl.br_edr_mode = acl_peer_info->acl_dev_mgmt_info.ptt;
  return;
  // TODO: will be handled during ULC-LLC integration
  set_device_parameters_acl_cmd(&set_device_parameters_acl, (shared_mem_t *)&ulc->tx_mbox_queue);     // pkb_alloc, fill the macros and enqueue
}

/**
 * @brief This function will execute from the LPW to ULC message handler,
 *        from received RSSI averaging, will call lmp_initiate_power_control_adjustment  function call trigger
 *
 * @param ulc
 * @param peer_id
 * @param rssi - rssi of peer_id
 */
void lmp_power_control_manager(ulc_t *ulc, uint8_t peer_id, int8_t rssi)
{
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;

  if (conn_pwr_ctrl_info->rssi_avg_pkt_count < RSSI_AVG_MAX_PACKET_COUNT) {
    conn_pwr_ctrl_info->rssi_avg_pkt_count++;
    conn_pwr_ctrl_info->rssi_average_sum += rssi;
  } else {
    conn_pwr_ctrl_info->rssi_average_sum += (rssi - conn_pwr_ctrl_info->rssi_average);
  }

  if (conn_pwr_ctrl_info->rssi_avg_pkt_count == RSSI_AVG_MAX_PACKET_COUNT) {
    conn_pwr_ctrl_info->rssi_average =
      (conn_pwr_ctrl_info->rssi_average_sum / conn_pwr_ctrl_info->rssi_avg_pkt_count);
    conn_pwr_ctrl_info->rssi_avg_pkt_count = 0;
    conn_pwr_ctrl_info->rssi_average_sum = 0;
    lmp_initiate_power_control_adjustment(ulc, peer_id);
  }
}

/**
 * @brief this function will form enhanced power control request LMP packet
 *
 * @param ulc
 * @param lmp_input_params - to form LMP packet
 */
void lmp_power_control_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_POWER_CONTROL_REQ_CMD);
  SET_LMP_POWER_CONTROL_REQ_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_POWER_CONTROL_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_POWER_CONTROL_REQ_PARAMETER_POWER_ADJ_REQ(lmp_tx_pkt->data, *((uint8_t *)lmp_input_params));
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

/**
 * @brief this function will form enhanced power control response LMP packet
 *
 * @param ulc
 * @param lmp_input_params - to form LMP packet
 */
void lmp_power_control_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t pwr_adj_res = *((uint8_t *)lmp_input_params);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_POWER_CONTROL_RES_CMD);
  SET_LMP_POWER_CONTROL_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_POWER_CONTROL_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_POWER_CONTROL_RES_PARAMETER_POWER_ADJ_RSP(lmp_tx_pkt->data, pwr_adj_resp_lookup[pwr_adj_res]);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

/**
 * @brief this function will form legacy power control increase Tx power LMP packet
 *
 * @param ulc
 * @param lmp_input_params - to form LMP packet
 */
void lmp_incr_power_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_INCR_POWER_REQ_CMD);
  SET_LMP_INCR_POWER_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_INCR_POWER_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_INCR_POWER_REQ_PARAMETER_RESERVED(lmp_tx_pkt->data, 0);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

/**
 * @brief this function will form legacy power control decrease Tx power LMP packet
 *
 * @param ulc
 * @param lmp_input_params - to form LMP packet
 */
void lmp_decr_power_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_DECR_POWER_REQ_CMD);
  SET_LMP_DECR_POWER_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_DECR_POWER_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_DECR_POWER_REQ_PARAMETER_RESERVED(lmp_tx_pkt->data, 0);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

/**
 * @brief this function will form legacy power control MAX Tx power reached LMP packet
 *
 * @param ulc
 * @param lmp_input_params - to form LMP packet
 */
void lmp_max_power_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_MAX_POWER_CMD);
  SET_LMP_MAX_POWER_TID(lmp_tx_pkt->data, tid);
  SET_LMP_MAX_POWER_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

/**
 * @brief this function will form legacy power control MIN Tx power reached LMP packet
 *
 * @param ulc
 * @param lmp_input_params - to form LMP packet
 */
void lmp_min_power_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  (void)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_MIN_POWER_CMD);
  SET_LMP_MIN_POWER_TID(lmp_tx_pkt->data, tid);
  SET_LMP_MIN_POWER_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

/**
 * @brief this function will handle enhanced power control response PDU,
 *        that is received after sending enhanced power control request
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_power_control_res_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t power_adj_res = GET_LMP_POWER_CONTROL_RES_PARAMETER_POWER_ADJ_RSP(bt_pkt_start);
  uint8_t vendor_power_control_notification = VENDOR_POWER_CONTROL_RES_RX;

  if (power_adj_res == MIN_POWER_FOR_ALL_MODULATIONS) {
    conn_pwr_ctrl_info->min_power_reached = LMP_TRUE;
    vendor_power_control_notification = VENDOR_POWER_CONTROL_RES_RX_MIN_POWER_REACHED;
  } else if (power_adj_res == MAX_POWER_FOR_ALL_MODULATIONS) {
    conn_pwr_ctrl_info->max_power_reached = LMP_TRUE;
    vendor_power_control_notification = VENDOR_POWER_CONTROL_RES_RX_MAX_POWER_REACHED;
  }
  /*INFO: CHANGED_ONE_STEP_FOR_ALL_MODULATIONS and ENHANCED POWER CONTROL received for each data rate are IGNORED */
  conn_pwr_ctrl_info->rssi_avg_pkt_count   = 0;
  conn_pwr_ctrl_info->rssi_average_sum     = 0;
  send_power_control_vendor_event(ulc, peer_id, vendor_power_control_notification);
}

/**
 * @brief this function will handle enhanced power control request PDU,
 *        and respond with enhanced power control response PDU
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_power_control_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint32_t lmp_control_word = 0;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_POWER_CONTROL_REQ_TID(bt_pkt_start);
  uint8_t update_power_index = LMP_FALSE;
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  uint8_t power_adj_req_rcvd = GET_LMP_POWER_CONTROL_REQ_PARAMETER_POWER_ADJ_REQ(bt_pkt_start);
  uint8_t power_adj_res = STEP_POWER_CHANGE;

  if ((power_adj_req_rcvd == DECR_PWR_ONE_STEP)) {
    if (conn_pwr_ctrl_info->tx_power_index >= (conn_pwr_ctrl_info->min_allowed_power_index + PWR_CHANGE_STEP_SIZE)) {
      conn_pwr_ctrl_info->tx_power_index = conn_pwr_ctrl_info->tx_power_index - PWR_CHANGE_STEP_SIZE;
      update_power_index = LMP_TRUE;
    } else {
      power_adj_res = MIN_POWER;
    }
  } else if ((power_adj_req_rcvd == INCR_PWR_ONE_STEP)) {
    if (conn_pwr_ctrl_info->tx_power_index <= (conn_pwr_ctrl_info->max_allowed_power_index - PWR_CHANGE_STEP_SIZE)) {
      conn_pwr_ctrl_info->tx_power_index = conn_pwr_ctrl_info->tx_power_index + PWR_CHANGE_STEP_SIZE;
      update_power_index = LMP_TRUE;
    } else {
      power_adj_res = MAX_POWER;
    }
  } else if (power_adj_req_rcvd == INCR_TO_MAX_PWR) {
    conn_pwr_ctrl_info->tx_power_index = conn_pwr_ctrl_info->max_allowed_power_index;
    update_power_index = LMP_TRUE;
  }

  if (conn_pwr_ctrl_info->tx_power_index < (conn_pwr_ctrl_info->min_allowed_power_index + PWR_CHANGE_STEP_SIZE)) {
    power_adj_res = MIN_POWER;
  } else if (conn_pwr_ctrl_info->tx_power_index
             > (conn_pwr_ctrl_info->max_allowed_power_index - PWR_CHANGE_STEP_SIZE)) {
    power_adj_res = MAX_POWER;
  }

  lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_POWER_CONTROL_RES, peer_id, tid);
  call_lmp_tx_handler(ulc, lmp_control_word, &power_adj_res);
  if (update_power_index == LMP_TRUE) {
    update_lpw_with_new_power_index(ulc, peer_id);
  }
}

/**
 * @brief this function will handle legacy power control power increament request PDU
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_incr_power_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_INCR_POWER_REQ_TID(bt_pkt_start);
  uint32_t lmp_control_word = 0;
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;

  if (conn_pwr_ctrl_info->tx_power_index <= conn_pwr_ctrl_info->max_allowed_power_index  - PWR_CHANGE_STEP_SIZE) {
    // after increasing power, index is within limits
    conn_pwr_ctrl_info->tx_power_index = conn_pwr_ctrl_info->tx_power_index + PWR_CHANGE_STEP_SIZE;
    update_lpw_with_new_power_index(ulc, peer_id);
  } else {
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_MAX_POWER, 0, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, NULL);
  }
}

/**
 * @brief this function will handle legacy power control power decreament request PDU
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_decr_power_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_DECR_POWER_REQ_TID(bt_pkt_start);
  uint32_t lmp_control_word = 0;
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;

  if (conn_pwr_ctrl_info->tx_power_index >= conn_pwr_ctrl_info->min_allowed_power_index + PWR_CHANGE_STEP_SIZE) {
    // after decreasing power, index is within limits
    conn_pwr_ctrl_info->tx_power_index = conn_pwr_ctrl_info->tx_power_index - PWR_CHANGE_STEP_SIZE;
    update_lpw_with_new_power_index(ulc, peer_id);
  } else {
    lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_MIN_POWER, 0, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, NULL);
  }
}

/**
 * @brief this function will handle legacy power control MAX power reached indication by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_max_power_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  send_power_control_vendor_event(ulc, peer_id, VENDOR_POWER_CONTROL_MAX_POWER_RX);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  conn_pwr_ctrl_info->max_power_reached = LMP_TRUE;
}

/**
 * @brief this function will handle legacy power control MIN power reached indication by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_min_power_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  send_power_control_vendor_event(ulc, peer_id, VENDOR_POWER_CONTROL_MIN_POWER_RX);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  conn_pwr_ctrl_info->min_power_reached = LMP_TRUE;
}

/**
 * @brief this function will handle legacy power control increament power req not accepted by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_incr_power_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;

  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_INCR_POWER_REQ with peer id - %d, error code - %d \n", peer_id, error_code);
}

/**
 * @brief this function will handle legacy power control decreament power req not accepted by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_decr_power_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;

  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_DECR_POWER_REQ with peer id - %d, error code - %d \n", peer_id, error_code);
}

/**
 * @brief this function will handle legacy power control max power reached indication not accepted by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_max_power_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;

  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_MAX_POWER with peer id - %d, error code - %d \n", peer_id, error_code);
}

/**
 * @brief this function will handle legacy power control min power reached indication not accepted by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_min_power_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;

  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_MIN_POWER with peer id - %d, error code - %d \n", peer_id, error_code);
}

/**
 * @brief this function will handle enhanced power control request not accepted by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_power_control_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_EXT_PARAMETER_ERROR_CODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;

  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_POWER_CONTROL_REQ with peer id - %d, error code - %d \n", peer_id, error_code);
}

/**
 * @brief this function will handle enhanced power control response not accepted by remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_power_control_res_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t error_code = GET_LMP_NOT_ACCEPTED_EXT_PARAMETER_ERROR_CODE(bt_pkt_start);
  (void)peer_id;
  (void)error_code;
  (void)ulc;

  BTC_PRINT("ERROR : NOT_ACCEPTED received for LMP_POWER_CONTROL_RES with peer id - %d, error code - %d \n", peer_id, error_code);
}
