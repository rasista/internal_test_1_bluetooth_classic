#include "lmp.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_process_hci_cmd.h"
#include "btc_lmp_power_control.h"
#include "btc_lmp_test_mode.h"

/**
 * @brief Main test mode LMP packet dispatcher
 *        This function receives all test mode related LMP packets and processes them
 *
 * @param ulc - ULC context
 * @param pkb - received LMP packet
 */
void lmp_test_mode_rx_packet_handler(ulc_t *ulc, pkb_t *rx_pkb)
{
  uint8_t peer_id = GET_PEER_ID(rx_pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(rx_pkb);
  uint8_t lmp_opcode = GET_OPCODE(rx_pkb);
  uint8_t lmp_ext_opcode = GET_EXT_OPCODE(rx_pkb);
  uint8_t tid = GET_TID(rx_pkb);
  uint8_t lmp_input_params[MAX_SIZE_OF_INPUT_PARAMS];
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  conn_pwr_ctrl_info_t *conn_pwr_ctrl_info = &acl_peer_info->conn_pwr_ctrl_info;

  switch (lmp_opcode) {
    case LMP_DETACH: {
      acl_dev_mgmt_info->lm_connection_status = LM_LEVEL_CONN_NOT_DONE;
      dm_acl_disconn_ind(ulc, peer_id, GET_LMP_DETACH_PARAMETER_ERROR_CODE(bt_pkt_start));
      break;
    }
    case LMP_INCR_POWER_REQ: {
      if (conn_pwr_ctrl_info->tx_power_index <= (conn_pwr_ctrl_info->max_allowed_power_index)) {
        conn_pwr_ctrl_info->tx_power_index ++;
      } else {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_MAX_POWER, 0, peer_id, tid), NULL);
      }
      break;
    }
    case LMP_DECR_POWER_REQ: {
      if (conn_pwr_ctrl_info->tx_power_index >= (conn_pwr_ctrl_info->min_allowed_power_index)) {
        conn_pwr_ctrl_info->tx_power_index --;
      } else {
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_MIN_POWER, 0, peer_id, tid), NULL);
      }
      break;
    }
    case LMP_FEATURES_REQ: {
      update_feature_map(acl_peer_info, &(GET_LMP_FEATURES_REQ_PARAMETER_FEATURES(bt_pkt_start)), ulc);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_FEATURES_RES, 0, peer_id, tid), NULL);
      break;
    }
    case LMP_TEST_ACTIVATE: {
      lmp_input_params[0] = lmp_opcode;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      break;
    }
    case LMP_TEST_CONTROL: {
      test_mode_params_t *test_mode_params = &acl_peer_info->test_mode_params;
      test_mode_params->test_scenario = (GET_LMP_TEST_CONTROL_PARAMETER_TEST_SENARIO(bt_pkt_start) ^ 0x55);
      if (test_mode_params->test_scenario != PAUSE_TEST_MODE) {
        test_mode_params->hopping_mode = (GET_LMP_TEST_CONTROL_PARAMETER_HOPPING_MODE(bt_pkt_start) ^ 0x55);
        test_mode_params->tx_freq = (GET_LMP_TEST_CONTROL_PARAMETER_TX_FREQUENCY(bt_pkt_start) ^ 0x55);
        test_mode_params->rx_freq = (GET_LMP_TEST_CONTROL_PARAMETER_RX_FREQUENCY(bt_pkt_start) ^ 0x55);
        test_mode_params->pwr_cntrl_mode = (GET_LMP_TEST_CONTROL_PARAMETER_POWER_MODE(bt_pkt_start) ^ 0x55);
        test_mode_params->poll_period = (GET_LMP_TEST_CONTROL_PARAMETER_POLL_PERIOD(bt_pkt_start) ^ 0x55);
        test_mode_params->pkt_type = ((GET_LMP_TEST_CONTROL_PARAMETER_PACKET_TYPE(bt_pkt_start) ^ 0x55) & 0xf);
        test_mode_params->test_lt_type = ((GET_LMP_TEST_CONTROL_PARAMETER_PACKET_TYPE(bt_pkt_start) ^ 0x55) >> 4);
        test_mode_params->link_type = (((GET_LMP_TEST_CONTROL_PARAMETER_PACKET_TYPE(bt_pkt_start) ^ 0x55) >> 4) & 0x02);
        test_mode_params->len_of_seq = *GET_LMP_TEST_CONTROL_PARAMETER_TEST_DATA_LENGTH(bt_pkt_start);
        test_mode_params->len_of_seq ^= 0x5555;
        SET_TEST_LT_TYPE(test_mode_params);
        test_mode_params->test_mode_type = test_mode_params->test_lt_type;
      }
      btc_dev_info->btc_dev_mgmt_info.testmode_block_host_pkts = 1;
      lmp_input_params[0] = lmp_opcode;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      SET_TEST_MODE_TYPE(&test_mode_params->test_mode_type, test_mode_params->test_lt_type);    
      lmp_process_test_config(ulc, peer_id);
      break;
    }
    case LMP_SET_AFH: {
      if (GET_LMP_SET_AFH_PARAMETER_AFH_MODE(bt_pkt_start) == AFH_ENABLE) {
        acl_link_mgmt_info->device_status_flags |= AFH_ENABLED;
        acl_link_mgmt_info->updates_pending |= UPDATE_AFH_PENDING;
        memcpy(acl_dev_mgmt_info->afh_channel_map_new, &GET_LMP_SET_AFH_PARAMETER_AFH_CHANNEL_MAP(bt_pkt_start), CHANNEL_MAP_BYTES);
      } else {
        acl_link_mgmt_info->device_status_flags &= ~AFH_ENABLED;
        acl_link_mgmt_info->updates_pending |= UPDATE_AFH_PENDING;
      }
      acl_link_mgmt_info->afh_instant = *GET_LMP_SET_AFH_PARAMETER_AFH_INSTANT(bt_pkt_start);
      break;
    }
    case LMP_ESCAPE_OPCODE: {
      switch (lmp_ext_opcode) {
        case LMP_POWER_CONTROL_REQ: {
          if (GET_LMP_POWER_CONTROL_REQ_PARAMETER_POWER_ADJ_REQ(bt_pkt_start) == DECR_PWR_ONE_STEP) {
            if (conn_pwr_ctrl_info->tx_power_index >= (conn_pwr_ctrl_info->min_allowed_power_index)) {
            conn_pwr_ctrl_info->tx_power_index --;
            lmp_input_params[0] = 0;
            } else {
            lmp_input_params[0] = MIN_POWER;
            }
          } else if (GET_LMP_POWER_CONTROL_REQ_PARAMETER_POWER_ADJ_REQ(bt_pkt_start) == INCR_PWR_ONE_STEP) {
            if (conn_pwr_ctrl_info->tx_power_index <= (conn_pwr_ctrl_info->max_allowed_power_index)) {
            conn_pwr_ctrl_info->tx_power_index ++;
            lmp_input_params[0] = 0;
            } else {
            lmp_input_params[0] = MAX_POWER;
            }
          } else if (GET_LMP_POWER_CONTROL_REQ_PARAMETER_POWER_ADJ_REQ(bt_pkt_start) == INCR_TO_MAX_PWR) {
            conn_pwr_ctrl_info->tx_power_index = conn_pwr_ctrl_info->max_allowed_power_index;
          }

          if (conn_pwr_ctrl_info->tx_power_index < (conn_pwr_ctrl_info->min_allowed_power_index)) {
            lmp_input_params[0] = MIN_POWER;
          } else if (conn_pwr_ctrl_info->tx_power_index > (conn_pwr_ctrl_info->max_allowed_power_index)) {
            lmp_input_params[0] = MAX_POWER;
          }
          call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_POWER_CONTROL_RES, peer_id, tid), lmp_input_params); 
          break;
        }
        case LMP_POWER_CONTROL_RES: {
          if (GET_LMP_POWER_CONTROL_RES_PARAMETER_POWER_ADJ_RSP(bt_pkt_start) == MIN_POWER_FOR_ALL_MODULATIONS) {
              conn_pwr_ctrl_info->min_power_reached = 1;
          } else if (GET_LMP_POWER_CONTROL_RES_PARAMETER_POWER_ADJ_RSP(bt_pkt_start) == MAX_POWER_FOR_ALL_MODULATIONS) {
              conn_pwr_ctrl_info->max_power_reached = 1;
          }
          conn_pwr_ctrl_info->enable_power_control = ENABLE_POWER_CNTRL;
          conn_pwr_ctrl_info->rssi_avg_pkt_count = 0;
          conn_pwr_ctrl_info->rssi_average = 0;
          conn_pwr_ctrl_info->rssi_average_sum = 0;
          break;
        }
      }
      break;
    }
    case LMP_NOT_ACCEPTED: {
      break;
    }
    default: {
      lmp_input_params[0] = lmp_opcode;
      lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), lmp_input_params);    
      break;
    }
  }
}

/**
 * @brief Process all pending test mode RX packets for a peer/device.
 *
 * @param bt_common_info - pointer to bt_common_info_t
 * @param slave_id - peer/device index
 */
void btc_process_test_mode_packets(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t llid_val = GET_LLID(pkb);
  uint8_t rx_pkt_type  =  GET_PKT_TYPE(pkb);
  if (rx_pkt_type == BT_DM1_PKT_TYPE && llid_val == ACLC_LLID) {
    lmp_test_mode_rx_packet_handler(ulc, pkb);
  }
}
