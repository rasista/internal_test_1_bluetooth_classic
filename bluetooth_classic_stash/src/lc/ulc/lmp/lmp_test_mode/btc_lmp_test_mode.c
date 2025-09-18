#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_lmp_test_mode.h"
#include "btc_dm.h"

void lmp_test_activate_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_TEST_ACTIVATE_CMD);
  SET_LMP_TEST_ACTIVATE_TID(lmp_tx_pkt->data, tid);
  SET_LMP_TEST_ACTIVATE_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_test_control_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  test_mode_params_t *test_mode_params = (test_mode_params_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_TEST_CONTROL_CMD);
  SET_LMP_TEST_CONTROL_TID(lmp_tx_pkt->data, tid);
  SET_LMP_TEST_CONTROL_OPCODE(lmp_tx_pkt->data);
  SET_LMP_TEST_CONTROL_PARAMETER_TEST_SENARIO(lmp_tx_pkt->data, test_mode_params->test_scenario);
  SET_LMP_TEST_CONTROL_PARAMETER_HOPPING_MODE(lmp_tx_pkt->data, test_mode_params->hopping_mode);
  SET_LMP_TEST_CONTROL_PARAMETER_TX_FREQUENCY(lmp_tx_pkt->data, test_mode_params->tx_freq);
  SET_LMP_TEST_CONTROL_PARAMETER_RX_FREQUENCY(lmp_tx_pkt->data, test_mode_params->rx_freq);
  SET_LMP_TEST_CONTROL_PARAMETER_POWER_MODE(lmp_tx_pkt->data, test_mode_params->pwr_cntrl_mode);
  SET_LMP_TEST_CONTROL_PARAMETER_POLL_PERIOD(lmp_tx_pkt->data, test_mode_params->poll_period);
  SET_LMP_TEST_CONTROL_PARAMETER_PACKET_TYPE(lmp_tx_pkt->data, test_mode_params->pkt_type);
  SET_LMP_TEST_CONTROL_PARAMETER_TEST_DATA_LENGTH(lmp_tx_pkt->data, test_mode_params->len_of_seq);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

/**
 * @brief This function will handle test activate request PDU received from remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_test_activate_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &ulc->btc_dev_info.btc_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_TEST_ACTIVATE_TID(bt_pkt_start);
  lmp_input_params[0] = GET_LMP_TEST_ACTIVATE_OPCODE(bt_pkt_start);

  // Check if local device supports test mode
  if (IS_HOST_DEV_TEST_MODE_ENABLED(btc_dev_mgmt_info)) {
    SET_TEST_MODE_ENABLED(acl_link_mgmt_info);
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
    acl_link_mgmt_info->lmp_connection_state = LMP_STANDBY_STATE;
    conn_pwr_ctrl_info->enable_power_control = DISABLE_POWER_CNTRL;
  } else {
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  }
}

/**
 * @brief This function will handle test control request PDU received from remote device
 *
 * @param ulc
 * @param pkb - packet structure of LMP packet received
 */
void lmp_test_control_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t tid = GET_LMP_TEST_CONTROL_TID(bt_pkt_start);
  uint8_t lmp_input_params[2];
  lmp_input_params[0] = GET_LMP_TEST_CONTROL_OPCODE(bt_pkt_start);
  // Send NOT_ACCEPTED as per spec - test_control not processed in normal operation
  lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
}

