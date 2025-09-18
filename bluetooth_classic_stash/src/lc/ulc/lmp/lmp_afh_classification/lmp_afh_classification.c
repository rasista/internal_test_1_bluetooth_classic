#include "lmp_afh_classification.h"
#include "lmp_get_defines_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_cmd_ptl_autogen.h"
#include "btc_lmp.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "lmp.h"
#include "btc_dev.h"
#include "btc_process_hci_cmd.h"
#include "string.h"
#include "stdint.h"

void lmp_set_afh_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint32_t master_clock = get_clock_from_lpw(); // TODO need to read from the HW block
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SET_AFH_CMD);

  //Rounding off the last bit so that afh_instant is even
  acl_peer_info->acl_link_mgmt_info.afh_instant = CALCULATE_AFH_INSTANT(master_clock);
  SET_LMP_SET_AFH_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_SET_AFH_OPCODE(lmp_tx_pkt->data);
  SET_LMP_SET_AFH_PARAMETER_AFH_INSTANT(lmp_tx_pkt->data, acl_peer_info->acl_link_mgmt_info.afh_instant);
  SET_LMP_SET_AFH_PARAMETER_AFH_MODE(lmp_tx_pkt->data, *(uint8_t *)lmp_input_params);
  SET_LMP_SET_AFH_PARAMETER_AFH_CHANNEL_MAP(lmp_tx_pkt->data, acl_peer_info->acl_dev_mgmt_info.afh_channel_map_new);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_set_afh_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t lmp_input_params[2];
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
    lmp_input_params[0] = GET_LMP_SET_AFH_OPCODE(bt_pkt_start);
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, GET_LMP_SET_AFH_TID(bt_pkt_start)), &lmp_input_params);
  } else {
    // Peripheral saves the AFH info
    if (IS_AFH_ENABLED_IN_RX_PKT(bt_pkt_start)) {
      acl_link_mgmt_info->device_status_flags |= AFH_ENABLED;
      acl_link_mgmt_info->updates_pending |= UPDATE_AFH_PENDING;
      memcpy(acl_peer_info->acl_dev_mgmt_info.afh_channel_map_new, &GET_LMP_SET_AFH_PARAMETER_AFH_CHANNEL_MAP(bt_pkt_start), CHANNEL_MAP_BYTES);
    } else {
      acl_link_mgmt_info->device_status_flags &= ~AFH_ENABLED;
      acl_link_mgmt_info->updates_pending |= UPDATE_AFH_PENDING;
    }
    acl_link_mgmt_info->afh_instant = *GET_LMP_SET_AFH_PARAMETER_AFH_INSTANT(bt_pkt_start);
  }
}

void lmp_channel_classification_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_CHANNEL_CLASSIFICATION_REQ_CMD);
  if ((acl_peer_info->acl_link_mgmt_info.device_status_flags & AFH_ENABLED)) {
    SET_LMP_CHANNEL_CLASSIFICATION_REQ_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
    SET_LMP_CHANNEL_CLASSIFICATION_REQ_OPCODE(lmp_tx_pkt->data);
    SET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_REPORTING_MODE(lmp_tx_pkt->data, acl_peer_info->acl_dev_mgmt_info.afh_reporting_mode);
    SET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_MIN_INTERVAL(lmp_tx_pkt->data, acl_peer_info->acl_dev_mgmt_info.afh_min_interval);
    SET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_MAX_INTERVAL(lmp_tx_pkt->data, acl_peer_info->acl_dev_mgmt_info.afh_max_interval);
    //Workaround: Some peers claim AFH support but don't use it,Start internal AFH engine - stops when peer sends classification.
    acl_peer_info->acl_dev_mgmt_info.afh_interval = acl_peer_info->acl_dev_mgmt_info.afh_min_interval;
    //Set classification instant for internal engine at current clock + afh_interval
    acl_peer_info->acl_link_mgmt_info.afh_classification_instant =  CALCULATE_AFH_CLASSIFICATION_INSTANT(acl_peer_info->acl_dev_mgmt_info.afh_interval);
    //Inform classification is pending
    acl_peer_info->acl_link_mgmt_info.updates_pending |= UPDATE_AFH_CLASSIFICATION_PENDING;
    add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  }
}

void lmp_channel_classification_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_CHANNEL_CLASSIFICATION_CMD);
  SET_LMP_CHANNEL_CLASSIFICATION_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_CHANNEL_CLASSIFICATION_OPCODE(lmp_tx_pkt->data);
  SET_LMP_CHANNEL_CLASSIFICATION_PARAMETER_AFH_CHANNEL_CLASSIFICATION(lmp_tx_pkt->data, acl_peer_info->acl_dev_mgmt_info.afh_channel_classification);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_channel_classification_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[GET_PEER_ID(pkb)];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  // Mark that channel classification request was received
  acl_link_mgmt_info->device_status_flags |= CHANNEL_CLASSIFICATION_REQ_SENT;
  acl_dev_mgmt_info->afh_reporting_mode = GET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_REPORTING_MODE(bt_pkt_start);
  acl_dev_mgmt_info->afh_min_interval   = *GET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_MIN_INTERVAL(bt_pkt_start);
  acl_dev_mgmt_info->afh_max_interval   = *GET_LMP_CHANNEL_CLASSIFICATION_REQ_PARAMETER_AFH_MAX_INTERVAL(bt_pkt_start);
  lmp_start_channel_classification(ulc, pkb);
}

void lmp_channel_classification_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[GET_PEER_ID(pkb)];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);

  // Save the received channel_classification info
  memcpy(acl_dev_mgmt_info->afh_channel_classification, &GET_LMP_CHANNEL_CLASSIFICATION_PARAMETER_AFH_CHANNEL_CLASSIFICATION(bt_pkt_start), CHANNEL_MAP_BYTES);
  acl_link_mgmt_info->device_status_flags |= CHANNEL_CLASSIFICATION_RECVD;
  lmp_central_send_classification(ulc, pkb);
  acl_link_mgmt_info->updates_pending &= ~UPDATE_AFH_CLASSIFICATION_PENDING;
}

void lmp_start_channel_classification(ulc_t *ulc, pkb_t *pkb)
{
  uint32_t random_num = 0;
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;

  if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
    // If peer supports Channel classification, send channel classification request to peer if not already sent
    if ((btc_dev_mgmt_info->features_bitmap[5] & LMP_AFH_CLS_MASTER)
        && (acl_dev_mgmt_info->features_bitmap[4] & LMP_AFH_CLS_SLAVE)
        && !(acl_link_mgmt_info->device_status_flags & CHANNEL_CLASSIFICATION_REQ_SENT)) {
      // Reporting mode is enabled
      acl_dev_mgmt_info->afh_reporting_mode = AFH_REPORTING_ENABLED;
      if (btc_dev_mgmt_info->afh_min_interval_from_host != 0) {
        acl_dev_mgmt_info->afh_min_interval = btc_dev_mgmt_info->afh_min_interval_from_host;
        acl_dev_mgmt_info->afh_max_interval = btc_dev_mgmt_info->afh_max_interval_from_host;
      } else {
        acl_dev_mgmt_info->afh_min_interval = AFH_DEFAULT_MIN_INTERVAL;
        acl_dev_mgmt_info->afh_max_interval = AFH_DEFAULT_MAX_INTERVAL;
      }
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_CHANNEL_CLASSIFICATION_REQ, peer_id, GET_TID_OF_MODE(acl_dev_mgmt_info->mode)), NULL); // Use variable
      // Mark as Channel Classification request sent
      acl_link_mgmt_info->device_status_flags |= CHANNEL_CLASSIFICATION_REQ_SENT;
    }
    // Master supports AFH but no classification received, trigger internal AFH
    else if ((btc_dev_mgmt_info->features_bitmap[5] & LMP_AFH_CLS_MASTER)
             && !(acl_link_mgmt_info->device_status_flags & CHANNEL_CLASSIFICATION_RECVD)) {
      // Next classification to be performed at current TSF + afh_interval
      acl_link_mgmt_info->afh_classification_instant =  CALCULATE_AFH_CLASSIFICATION_INSTANT(acl_dev_mgmt_info->afh_interval);
      acl_link_mgmt_info->updates_pending |= UPDATE_AFH_CLASSIFICATION_PENDING;
    }
  } else {
    if (acl_dev_mgmt_info->afh_max_interval - acl_dev_mgmt_info->afh_min_interval) {
      GENERATE_AFH_RANDOM_NUM(&random_num);
      acl_dev_mgmt_info->afh_interval =
        acl_dev_mgmt_info->afh_min_interval
        + ((random_num & 0xFFFF) % ((acl_dev_mgmt_info->afh_max_interval - acl_dev_mgmt_info->afh_min_interval)));
    } else {
      acl_dev_mgmt_info->afh_interval = acl_dev_mgmt_info->afh_min_interval;
    }
    if (IS_AFH_REPORTING_ENABLED(acl_dev_mgmt_info)) {
      acl_link_mgmt_info->afh_classification_instant = ((get_clock_from_lpw() >> 1) + (acl_dev_mgmt_info->afh_interval));
      acl_link_mgmt_info->updates_pending |= UPDATE_AFH_CLASSIFICATION_PENDING;
    } else {
      acl_link_mgmt_info->updates_pending &= ~UPDATE_AFH_CLASSIFICATION_PENDING;
    }
  }
}

void channel_classification_timeout_handler(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &btc_dev_info->btc_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  uint8_t lmp_input_params[1];
  uint8_t valid_channel_cnt = 0;

  if (IS_CHANNEL_ASSESSMENT_ENABLED(btc_dev_mgmt_info)) {
    uint8_t temp_map[CHANNEL_MAP_BYTES];
    uint8_t temp_map1[CHANNEL_MAP_BYTES];
    bt_calculate_classification_channel_map(temp_map);

    if (IS_PERIPHERAL_MODE(acl_dev_mgmt_info)) {
      if ((btc_dev_mgmt_info->features_bitmap[5] & LMP_AFH_CLS_MASTER)
          && (acl_dev_mgmt_info->features_bitmap[4] & LMP_AFH_CLS_SLAVE)) {
        // Peer didn't respond to classification request, assume all
        if ((acl_link_mgmt_info->device_status_flags & CHANNEL_CLASSIFICATION_REQ_SENT)
            && !(acl_link_mgmt_info->device_status_flags & CHANNEL_CLASSIFICATION_RECVD)) {
          // Set default AFH channel map
          CHANNEL_MAP_SET_DEFAULT(acl_dev_mgmt_info->afh_channel_map_new);
        }

        // Temporary map to check we have minimum number of good channels
        CHANNEL_MAP_AND_COMBINE(temp_map1, acl_dev_mgmt_info->afh_channel_map_new, temp_map);
        valid_channel_cnt = get_no_of_ones((uint8_t *)temp_map1, NUM_OF_MAX_CHANNELS);
        if ((valid_channel_cnt == 0)) {
          // Set default AFH channel map
          CHANNEL_MAP_SET_DEFAULT(acl_dev_mgmt_info->afh_channel_map_new);
        }
        if (valid_channel_cnt > MIN_GOOD_CHANNEL_CNT) {
          CHANNEL_MAP_AND(acl_dev_mgmt_info->afh_channel_map_new, temp_map);
        }
        if ((valid_channel_cnt > 0) && (valid_channel_cnt <= MIN_GOOD_CHANNEL_CNT)) {
          CHANNEL_MAP_OR(acl_dev_mgmt_info->afh_channel_map_new, temp_map);
        }
        if (btc_dev_mgmt_info->host_ch_classification_cmd == 1) {
          CHANNEL_MAP_AND(acl_dev_mgmt_info->afh_channel_map_new, btc_dev_mgmt_info->host_classification_channel_map);
        }
        // Send SET_AFH PDU with combined local, peer, and host channel maps
      } else if (btc_dev_mgmt_info->features_bitmap[5] & LMP_AFH_CLS_MASTER) {
        CHANNEL_MAP_COPY(acl_dev_mgmt_info->afh_channel_map_new, temp_map);
        if (btc_dev_mgmt_info->host_ch_classification_cmd == 1) {
          CHANNEL_MAP_AND(acl_dev_mgmt_info->afh_channel_map_new, btc_dev_mgmt_info->host_classification_channel_map);
        }
      }
    } else {
      memset(acl_dev_mgmt_info->afh_channel_classification, 0, CHANNEL_MAP_BYTES);
      if (btc_dev_mgmt_info->host_ch_classification_cmd == 1) {
        CHANNEL_MAP_AND_COMBINE(temp_map1, btc_dev_mgmt_info->host_classification_channel_map, temp_map);
        valid_channel_cnt = get_no_of_ones((uint8_t *)temp_map1, NUM_OF_MAX_CHANNELS);
        if (valid_channel_cnt < MIN_GOOD_CHANNEL_CNT) {
          CHANNEL_MAP_OR(temp_map, btc_dev_mgmt_info->host_classification_channel_map);
        } else {
          CHANNEL_MAP_COPY(temp_map, temp_map1);
        }
      }
      lmp_update_afh_channel_map(acl_dev_mgmt_info, temp_map, 1); // 1 = update classification
    }
  } else {
    // Keep all channel pairs as unknown
    if (btc_dev_mgmt_info->host_ch_classification_cmd == 1) {
      for (uint8_t i = 0; i <= NUM_OF_MAX_CHANNELS; i += 2) {
        uint8_t byte_pos = i >> 3;
        uint8_t bit_pos = i & BIT_POSITION_MASK;
        uint8_t channel_pair = btc_dev_mgmt_info->host_classification_channel_map[byte_pos];
        EXTRACT_CHANNEL_PAIR(channel_pair, bit_pos);
        if (IS_GOOD_CHANNEL_PAIR(channel_pair)) { //good
          acl_dev_mgmt_info->afh_channel_classification[byte_pos] |= (UNKNOWN_CHANNEL << bit_pos);
        } else { //bad
          acl_dev_mgmt_info->afh_channel_classification[byte_pos] |= (BAD_CHANNEL << bit_pos);
        }
      }
    } else {
      memcpy(acl_dev_mgmt_info->afh_channel_classification, acl_dev_mgmt_info->afh_channel_map_new, CHANNEL_MAP_BYTES);
    }
  }
  if (IS_CENTRAL_MODE(acl_dev_mgmt_info)) {
    lmp_input_params[0] = LMP_CHANNEL_CLASSIFICATION;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_CHANNEL_CLASSIFICATION, peer_id, GET_TID_OF_MODE(acl_dev_mgmt_info->mode)), &lmp_input_params); // Use variable
  }
}

void lmp_central_send_classification(ulc_t *ulc, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  uint8_t lmp_input_params[1];
  lmp_update_afh_channel_map(acl_dev_mgmt_info, NULL, 0); // 0 = Not update classification, NULL = no temp_map
  channel_classification_timeout_handler(ulc, pkb);
  lmp_input_params[0] = AFH_ENABLE;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SET_AFH, 0, peer_id, GET_TID_OF_MODE(acl_dev_mgmt_info->mode)), &lmp_input_params); // Use variable
}

void lmp_update_afh_channel_map(acl_dev_mgmt_info_t *acl_dev_mgmt_info, uint8_t *temp_map, uint8_t update_classification)
{
  // Clear the target arrays if updating classification
  if (update_classification) {
    memset(acl_dev_mgmt_info->afh_channel_classification, 0, CHANNEL_MAP_BYTES);
  }

  for (uint8_t i = 0; i <= NUM_OF_MAX_CHANNELS; i += 2) {
    uint8_t byte_pos = i >> 3;
    uint8_t bit_pos = i & BIT_POSITION_MASK;
    uint8_t channel_pair;

    if (update_classification && temp_map) {
      // Update classification from temp_map
      channel_pair = temp_map[byte_pos];
      channel_pair = GET_CHANNEL_PAIR_BITS(channel_pair, bit_pos);

      if (IS_GOOD_CHANNEL_PAIR(channel_pair)) {
        acl_dev_mgmt_info->afh_channel_classification[byte_pos] |= (GOOD_CHANNEL << bit_pos);
      } else if ((channel_pair == BAD_GOOD_CHANNEL_PAIR) || (channel_pair == GOOD_BAD_CHANNEL_PAIR)) {
        acl_dev_mgmt_info->afh_channel_classification[byte_pos] |= (UNKNOWN_CHANNEL << bit_pos);
      } else {
        acl_dev_mgmt_info->afh_channel_classification[byte_pos] |= (BAD_CHANNEL << bit_pos);
      }
    }
    // Update AFH channel map from classification
    channel_pair = GET_CHANNEL_PAIR_CLASSIFICATION(acl_dev_mgmt_info->afh_channel_classification, byte_pos, bit_pos);

    if (IS_GOOD_CHANNEL_PAIR(channel_pair) || IS_UNKNOWN_CHANNEL(channel_pair)) {
      SET_CHANNEL_PAIR_GOOD(acl_dev_mgmt_info->afh_channel_map_new, byte_pos, bit_pos);
    } else if (IS_BAD_CHANNEL(channel_pair)) {
      SET_CHANNEL_PAIR_BAD(acl_dev_mgmt_info->afh_channel_map_new, byte_pos, bit_pos);
    }
  }
}

void bt_calculate_classification_channel_map(uint8_t *temp_map)
{
  // For now, just set all channels as good
  for (int i = 0; i < CHANNEL_MAP_BYTES; i++) {
    temp_map[i] = 0xFF;
  }
}

uint8_t get_no_of_ones(uint8_t *map, uint8_t max_channels)
{
  uint8_t count = 0;
  for (uint8_t i = 0; i < max_channels; i++) {
    if (IS_BIT_SET_IN_MAP(map, i)) {
      count++;
    }
  }
  return count;
}

// Get current clock value from LPW- needs to be replaced with actual extraction of clock
uint32_t get_clock_from_lpw()
{
  // TODO: Replace with actual hardware clock read from LPW timer
  uint32_t clock = 0;
  clock += 0x100; // Simulate clock progression,return a dummy value
  return clock;
}

// Generic function to perform channel map operations
void perform_channel_map_operation(uint8_t *dst_map, const uint8_t *src1_map, const uint8_t *src2_map, channel_map_operation_t operation)
{
  // Handle SET_DEFAULT
  if (operation == CHANNEL_MAP_OP_SET_DEFAULT) {
    *(uint32_t *)&dst_map[0] = AFH_CHANNEL_MAP_DEFAULT_LOW;
    *(uint32_t *)&dst_map[4] = AFH_CHANNEL_MAP_DEFAULT_HIGH;
    *(uint16_t *)&dst_map[8] = AFH_CHANNEL_MAP_DEFAULT_UPPER;
    return;
  }

  // Handle other operations with loop
  for (uint8_t i = 0; i < CHANNEL_MAP_BYTES; i++) {
    switch (operation) {
      case CHANNEL_MAP_OP_AND:
        dst_map[i] &= src1_map[i];
        break;
      case CHANNEL_MAP_OP_OR:
        dst_map[i] |= src1_map[i];
        break;
      case CHANNEL_MAP_OP_COPY:
        dst_map[i] = src1_map[i];
        break;
      case CHANNEL_MAP_OP_AND_COMBINE:
        dst_map[i] = (src1_map[i] & src2_map[i]);
        break;
      default:
        break;
    }
  }
}
