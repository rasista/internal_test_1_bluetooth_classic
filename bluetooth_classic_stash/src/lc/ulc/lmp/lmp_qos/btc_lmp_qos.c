#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_hci_events.h"
#include "btc_lmp_qos.h"

void lmp_quality_of_service_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_QUALITY_OF_SERVICE_CMD);
  SET_LMP_QUALITY_OF_SERVICE_TID(lmp_tx_pkt->data, tid);
  SET_LMP_QUALITY_OF_SERVICE_OPCODE(lmp_tx_pkt->data);
  SET_LMP_QUALITY_OF_SERVICE_PARAMETER_POLL_INTERVAL(lmp_tx_pkt->data, acl_link_mgmt_info->poll_interval);
  SET_LMP_QUALITY_OF_SERVICE_PARAMETER_N_BC(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.nbc);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_quality_of_service_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_QUALITY_OF_SERVICE_TID(bt_pkt_start);
  uint8_t lmp_input_params[2];
  if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
    lmp_input_params[0] = GET_LMP_QUALITY_OF_SERVICE_OPCODE(bt_pkt_start);
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    uint32_t lmp_control_word = ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid);
    call_lmp_tx_handler(ulc, lmp_control_word, &lmp_input_params);
  } else {
    acl_link_mgmt_info->poll_interval = *GET_LMP_QUALITY_OF_SERVICE_PARAMETER_POLL_INTERVAL(bt_pkt_start);
    bt_dev_info->btc_dev_mgmt_info.nbc = GET_LMP_QUALITY_OF_SERVICE_PARAMETER_N_BC(bt_pkt_start);
    acl_dev_mgmt_info->latency        = (acl_link_mgmt_info->poll_interval) * BT_SLOT_TIME_US;
    /* Indicate completion of QoS setup to HCI */
    hci_qos_setup_complete_indication(ulc, peer_id, BT_SUCCESS_STATUS);
  }
}

void lmp_quality_of_service_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_QUALITY_OF_SERVICE_REQ_CMD);
  SET_LMP_QUALITY_OF_SERVICE_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_QUALITY_OF_SERVICE_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_QUALITY_OF_SERVICE_REQ_PARAMETER_POLL_INTERVAL(lmp_tx_pkt->data, acl_link_mgmt_info->poll_interval_pending);
  SET_LMP_QUALITY_OF_SERVICE_REQ_PARAMETER_N_BC(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.nbc);
  acl_link_mgmt_info->lmp_tx_pending = 1;
  acl_link_mgmt_info->lmp_tx_pending_opcode = LMP_QUALITY_OF_SERVICE_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_quality_of_service_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_QUALITY_OF_SERVICE_REQ_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_QUALITY_OF_SERVICE_REQ_OPCODE(bt_pkt_start);
  uint16_t requested_poll_interval = *GET_LMP_QUALITY_OF_SERVICE_REQ_PARAMETER_POLL_INTERVAL(bt_pkt_start);
  uint8_t requested_nbc = GET_LMP_QUALITY_OF_SERVICE_REQ_PARAMETER_N_BC(bt_pkt_start);
  if (process_lmp_quality_of_service_req(ulc, peer_id, requested_poll_interval, requested_nbc)){
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  } else {
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
    /* Indicate completion of QoS setup to HCI */
    hci_qos_setup_complete_indication(ulc, peer_id, BT_SUCCESS_STATUS);
  }
}

void lmp_quality_of_service_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_link_mgmt_info->poll_interval = acl_link_mgmt_info->poll_interval_pending;
  /* Indicate completion of QoS setup to HCI */
  hci_qos_setup_complete_indication(ulc, peer_id, BT_SUCCESS_STATUS);
}

void lmp_quality_of_service_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_link_mgmt_info->poll_interval_pending = 0;
  /* Indicate completion of QoS setup to HCI */
  hci_qos_setup_complete_indication(ulc, peer_id, UNSPECIFIED_ERROR);
}

uint8_t process_lmp_quality_of_service_req(ulc_t *ulc, uint8_t peer_id, uint16_t requested_poll_interval, uint8_t requested_nbc)
{
  uint8_t status;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  if (IS_POLL_INTERVAL_VALID(requested_poll_interval)) {
    acl_link_mgmt_info->poll_interval = requested_poll_interval;
    acl_dev_mgmt_info->latency = (acl_link_mgmt_info->poll_interval) * BT_SLOT_TIME_US;
    if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
      bt_dev_info->btc_dev_mgmt_info.nbc = requested_nbc;
    }
    status =  BT_STATUS_SUCCESS;
  } else {
    status =  BT_STATUS_FAILURE;
  }
  return status;
}

void lmp_qos_setup(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint16_t poll_interval;
  uint8_t tid = GET_TID_OF_MODE(acl_dev_mgmt_info->mode);
  poll_interval = CALCULATE_POLL_INTERVAL(acl_dev_mgmt_info->latency);
  acl_link_mgmt_info->poll_interval_pending = poll_interval;
  acl_dev_mgmt_info->latency = poll_interval * BT_SLOT_TIME_US;
  uint8_t is_peripheral_guaranteed;
  // Determine LMP opcode and handle poll interval update
  uint8_t lmp_opcode = LMP_QUALITY_OF_SERVICE_REQ;  // Default for all cases
  is_peripheral_guaranteed = IS_PERIPHERAL_MODE(acl_dev_mgmt_info) && IS_GUARANTEED_SERVICE(acl_dev_mgmt_info);
  if (is_peripheral_guaranteed) {
    lmp_opcode = LMP_QUALITY_OF_SERVICE;
    acl_link_mgmt_info->poll_interval = acl_link_mgmt_info->poll_interval_pending;
  }
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(lmp_opcode, 0, peer_id, tid), NULL);
  return;
}