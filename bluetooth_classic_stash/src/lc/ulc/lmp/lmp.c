#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"
#include "btc_lmp.h"
#include "btc_process_hci_cmd.h"
#include "btc_hci_events.h"
#include "btc_dev.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_key_mgmt_algos.h"
#include "lmp.h"
#include "hci.h"
#include "btc.h"
#include "lmp_cmd_ptl_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
extern lmp_tx_handler_t lmp_tx_lut[];
extern lmp_tx_handler_t lmp_tx_ext_lut[];

extern const lmp_rx_handler_t lmp_rx_lut[];
extern const lmp_rx_handler_t lmp_rx_ext_lut[];
extern const lmp_accepted_rx_handler_t lmp_accepted_lut[];
extern const lmp_not_accepted_rx_handler_t lmp_not_accepted_lut[];
extern const lmp_accepted_rx_handler_t lmp_accepted_ext_lut[];
extern const lmp_not_accepted_ext_rx_handler_t lmp_not_accepted_ext_lut[];
void bt_start_role_switch(ulc_t *ulc, uint8_t peer_id)
{
  (void)ulc;
  (void)peer_id;
  return;
}

lmp_tx_handler_t get_lmp_tx_handler(uint32_t lmp_control_word)
{
  uint8_t lmp_opcode = DECODE_LMP_OPCODE(lmp_control_word);
  uint8_t lmp_ext_opcode = DECODE_LMP_EXOPCODE(lmp_control_word);
  if (lmp_opcode != 127) {
    return lmp_tx_lut[lmp_opcode];
  } else if (lmp_opcode == 127) {
    return lmp_tx_ext_lut[lmp_ext_opcode];
  }
  return NULL;
}

void call_lmp_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  lmp_tx_handler_t lmp_tx_handler = get_lmp_tx_handler(lmp_control_word);

  if (lmp_tx_handler != NULL) {
    lmp_tx_handler(ulc, lmp_control_word, lmp_input_params);
  }
}

lmp_rx_handler_t get_lmp_rx_handler(pkb_t *pkb)
{
  uint8_t lmp_opcode = GET_OPCODE(pkb);
  uint8_t lmp_ext_opcode = GET_EXT_OPCODE(pkb);
  if (lmp_opcode != 127) {
    return lmp_rx_lut[lmp_opcode];
  } else if (lmp_opcode == 127) {
    return lmp_rx_ext_lut[lmp_ext_opcode];
  }
  return NULL;
}

void call_lmp_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  lmp_rx_handler_t lmp_rx_handler = get_lmp_rx_handler(pkb);
  if (lmp_rx_handler != NULL) {
    lmp_rx_handler(ulc, pkb);
  }
}

lmp_accepted_rx_handler_t get_lmp_accepted_rx_handler(pkb_t *pkb)
{
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t opcode = GET_LMP_ACCEPTED_PARAMETER_OPCODE(bt_pkt_start);
  if (opcode != 127) {
    return lmp_accepted_lut[opcode];
  }
  return NULL;
}

void call_lmp_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  lmp_accepted_rx_handler_t lmp_accepted_rx_handler = get_lmp_accepted_rx_handler(pkb);
  if (lmp_accepted_rx_handler != NULL) {
    lmp_accepted_rx_handler(ulc, pkb);
  }
}

/**
 * @brief  this function will identify the function handler based on opcode
 *
 * @param pkb is the pointer to the packet structure of the received LMP message
 */
lmp_not_accepted_rx_handler_t get_lmp_not_accepted_rx_handler(pkb_t *pkb)
{
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t opcode = GET_LMP_NOT_ACCEPTED_PARAMETER_OPCODE(bt_pkt_start); 
  if (opcode != 127) {
    return lmp_not_accepted_lut[opcode];
  }
  return NULL;
}

/**
 * @brief  this function will execute all lmp_not_accepted_rx_handlers
 *
 * @param  ulc
 * @param pkb is the pointer to the packet structure of the received LMP message
 */
void call_lmp_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  lmp_not_accepted_rx_handler_t lmp_not_accepted_rx_handler = get_lmp_not_accepted_rx_handler(pkb);
  if (lmp_not_accepted_rx_handler != NULL) {
    lmp_not_accepted_rx_handler(ulc, pkb);
  }
}


/**
 * @brief  this function will identify the function handler based on ext_opcode
 *
 * @param pkb is the pointer to the packet structure of the received LMP message
 */
lmp_accepted_ext_rx_handler_t get_lmp_accepted_ext_rx_handler(pkb_t *pkb)
{
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t ext_opcode = GET_LMP_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(bt_pkt_start);
  return lmp_accepted_ext_lut[ext_opcode];
}

void call_lmp_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  lmp_accepted_ext_rx_handler_t lmp_accepted_ext_rx_handler = get_lmp_accepted_ext_rx_handler(pkb);
  if (lmp_accepted_ext_rx_handler != NULL) {
    lmp_accepted_ext_rx_handler(ulc, pkb);
  }
}

lmp_not_accepted_ext_rx_handler_t get_lmp_not_accepted_ext_rx_handler(pkb_t *pkb)
{
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t ext_opcode = GET_LMP_NOT_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(bt_pkt_start);
  return lmp_not_accepted_ext_lut[ext_opcode];
}

void call_lmp_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  lmp_not_accepted_ext_rx_handler_t lmp_not_accepted_ext_rx_handler = get_lmp_not_accepted_ext_rx_handler(pkb);
  if (lmp_not_accepted_ext_rx_handler != NULL) {
    lmp_not_accepted_ext_rx_handler(ulc, pkb);
  }
}

/**
 * @brief Adds a transmission packet to the TX queue.
 *
 * This function enqueues the given packet into the TX send queue and sets
 * the necessary events to indicate that a packet is pending for OTA transmission.
 *
 * @param tx_q Pointer to the transmission queue.
 * @param pkb Pointer to the packet buffer to be added.
 */
void add_tx_pkt_to_queue(queue_t *tx_q, pkb_t *pkb)
{
  enqueue(tx_q, &pkb->queue_node);
#ifdef SCHED_SIM
  btc_event_loop_set_event(&ulc.event_loop, BTC_EVENT_VIRTUAL_TX_DONE);
#endif
  btc_plt_remove_from_pend(&ulc.event_loop_pend);
}

void add_pkt_to_aclc_q(ulc_t *ulc, pkb_t *lmp_tx_pkt, uint8_t peer_id)
{
#ifdef SCHED_SIM
  pkb_t *temp_pkb =  pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  memcpy(temp_pkb->data + META_DATA_LENGTH, lmp_tx_pkt->data, lmp_tx_pkt->pkt_len);
  temp_pkb->data[2] = peer_id;
  lmp_tx_pkt->pkt_len = lmp_tx_pkt->pkt_len + META_DATA_LENGTH;
  memcpy(lmp_tx_pkt->data, temp_pkb->data, lmp_tx_pkt->pkt_len);
  pkb_free(temp_pkb, &btc.pkb_pool[pkb_chunk_type_medium]);
  add_tx_pkt_to_queue(&ulc->tx_send_q, lmp_tx_pkt);
#else
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  queue_t *tx_aclc_q = &acl_peer_info->tx_aclc_q;
  add_tx_pkt_to_queue(tx_aclc_q, lmp_tx_pkt);
#endif
  return;
}

pkb_t *prepare_lmp_command_tx_pkt(uint8_t len)
{
  pkb_t *pkb =
    pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  pkb->data[0] |= BT_DM1_PKT_TYPE << 3;
  pkb->data[2] = ACLC_LLID;
  pkb->data[2] |= 0x04;       // initially keeping the flow bit  1 FLOW-ON
  pkb->data[2] |= (len << 3); // adding length in Payload header format for Basic Rate single-slot ACL packets
  pkb->pkt_len = len + LMP_PKT_HDR_LEN;

  return pkb;
}

void lmp_name_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_NAME_REQ_CMD);
  SET_LMP_NAME_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_NAME_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_NAME_REQ_PARAMETER_NAME_OFFSET(lmp_tx_pkt->data, 0);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_name_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t name_frag_len;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t name_offset = *((uint8_t *)lmp_input_params);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_NAME_RES_CMD);
  SET_LMP_NAME_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_NAME_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_NAME_RES_PARAMETER_NAME_OFFSET(lmp_tx_pkt->data, name_offset);
  SET_LMP_NAME_RES_PARAMETER_NAME_LENGTH(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.device_name_len);
  if ((btc_dev_info->btc_dev_mgmt_info.device_name_len - name_offset) < LMP_MAX_NAME_FRAGMENT_LENGTH) {
    name_frag_len = btc_dev_info->btc_dev_mgmt_info.device_name_len - name_offset;
  } else {
    name_frag_len = LMP_MAX_NAME_FRAGMENT_LENGTH;
  }
  SET_LMP_NAME_RES_PARAMETER_NAME_FRAGMENT(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.device_name + name_offset, name_frag_len);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_features_req_ext_tx_handler(ulc_t *ulc, uint32_t  lmp_control_word, void *lmp_input_params)
{
  uint8_t feature_bit_map_type;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_FEATURES_REQ_EXT_CMD);
  SET_LMP_FEATURES_REQ_EXT_TID(lmp_tx_pkt->data, tid);
  SET_LMP_FEATURES_REQ_EXT_OPCODE(lmp_tx_pkt->data);
  feature_bit_map_type =  *((uint8_t*)lmp_input_params);
  SET_LMP_FEATURES_REQ_EXT_PARAMETER_FEATURES_PAGE(lmp_tx_pkt->data, feature_bit_map_type);
  SET_LMP_FEATURES_REQ_EXT_PARAMETER_MAX_SUPPORTED_PAGE(lmp_tx_pkt->data, MAX_NUM_SUPPORTED_FEATURE_PAGE);
  if (feature_bit_map_type == PAGE_MODE_1) {
    SET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.ext_features_bitmap);
  } else if (feature_bit_map_type == PAGE_MODE_2) {
    SET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.ext_features_bitmap_page2);
  } else if (feature_bit_map_type == PAGE_MODE_0) {
    SET_LMP_FEATURES_REQ_EXT_PARAMETER_EXTENDED_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.features_bitmap);
  }
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_features_res_ext_tx_handler(ulc_t *ulc, uint32_t  lmp_control_word, void *lmp_input_params)
{
  uint8_t feature_bit_map_type;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_FEATURES_RES_EXT_CMD);
  SET_LMP_FEATURES_RES_EXT_TID(lmp_tx_pkt->data, tid);
  SET_LMP_FEATURES_RES_EXT_OPCODE(lmp_tx_pkt->data);
  feature_bit_map_type =  *((uint8_t*)lmp_input_params);
  SET_LMP_FEATURES_RES_EXT_PARAMETER_FEATURES_PAGE(lmp_tx_pkt->data, feature_bit_map_type);
  SET_LMP_FEATURES_RES_EXT_PARAMETER_MAX_SUPPORTED_PAGE(lmp_tx_pkt->data, MAX_NUM_SUPPORTED_FEATURE_PAGE);
  if (feature_bit_map_type == PAGE_MODE_1) {
    SET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.ext_features_bitmap);
  } else if (feature_bit_map_type == PAGE_MODE_2) {
    SET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.ext_features_bitmap_page2);
  } else if (feature_bit_map_type == PAGE_MODE_0) {
    SET_LMP_FEATURES_RES_EXT_PARAMETER_EXTENDED_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.features_bitmap);
  }
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_accepted_tx_handler(ulc_t *ulc, uint32_t  lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_ACCEPTED_CMD);
  SET_LMP_ACCEPTED_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ACCEPTED_OPCODE(lmp_tx_pkt->data);
  SET_LMP_ACCEPTED_PARAMETER_OPCODE(lmp_tx_pkt->data, *((uint8_t *)lmp_input_params));
  if (*(uint8_t *)lmp_input_params == LMP_SWITCH_REQ) {
    bt_start_role_switch(ulc, peer_id);
  }
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);

  return;
}

void lmp_not_accepted_tx_handler(ulc_t *ulc, uint32_t  lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t *input_params = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_NOT_ACCEPTED_CMD);
  SET_LMP_NOT_ACCEPTED_TID(lmp_tx_pkt->data, tid);
  SET_LMP_NOT_ACCEPTED_OPCODE(lmp_tx_pkt->data);
  SET_LMP_NOT_ACCEPTED_PARAMETER_OPCODE(lmp_tx_pkt->data, input_params[0]);
  SET_LMP_NOT_ACCEPTED_PARAMETER_ERROR_OPCODE(lmp_tx_pkt->data, input_params[1]);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_clkoffset_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_CLKOFFSET_REQ_CMD);
  SET_LMP_CLKOFFSET_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_CLKOFFSET_REQ_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_clkoffset_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint16_t clk_offset = 0;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_CLKOFFSET_RES_CMD);
  SET_LMP_CLKOFFSET_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_CLKOFFSET_RES_OPCODE(lmp_tx_pkt->data);
  //clk_offset = (((read_hw_clock(LMAC_TIMER_1) & 0x1FFFC) - (read_hw_clock(LMAC_TIMER_2) & 0x1FFFC)) >> 2) & 0x7FFF;
  SET_LMP_CLKOFFSET_RES_PARAMETER_CLOCK_OFFSET(lmp_tx_pkt->data, clk_offset);
  acl_peer_info->acl_dev_mgmt_info.clk_offset = clk_offset;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_detach_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t reason_code = *((uint8_t *)lmp_input_params);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_DETACH_CMD);
  acl_peer_info->acl_link_mgmt_info.device_status_flags |= PAUSE_ACLD_TRAFFIC;
  SET_LMP_DETACH_TID(lmp_tx_pkt->data, tid);
  SET_LMP_DETACH_OPCODE(lmp_tx_pkt->data);
  SET_LMP_DETACH_PARAMETER_ERROR_CODE(lmp_tx_pkt->data, reason_code);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  acl_peer_info->acl_dev_mgmt_info.lm_connection_status = LM_LEVEL_CONN_NOT_DONE;
}

void lmp_encryption_mode_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_ENCRYPTION_MODE_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_ENCRYPTION_MODE_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ENCRYPTION_MODE_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_ENCRYPTION_MODE_REQ_PARAMETER_ENCRYPTION_MODE(lmp_tx_pkt->data, lmp_input_params->input_params[0]);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_ENCRYPTION_MODE_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_encryption_key_size_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_ENCRYPTION_KEY_SIZE_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_ENCRYPTION_KEY_SIZE_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ENCRYPTION_KEY_SIZE_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_ENCRYPTION_KEY_SIZE_REQ_PARAMETER_KEY_SIZE(lmp_tx_pkt->data, lmp_input_params->input_params[0]);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_ENCRYPTION_MODE_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_start_encryption_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_START_ENCRYPTION_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_START_ENCRYPTION_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_START_ENCRYPTION_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_START_ENCRYPTION_REQ_PARAMETER_RANDOM_NUMBER(lmp_tx_pkt->data, lmp_input_params->input_params);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending        = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_START_ENCRYPTION_REQ;
  acl_peer_info->acl_enc_info.encryption_flags |= DECRYPT_EN;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_stop_encryption_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_STOP_ENCRYPTION_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_STOP_ENCRYPTION_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_STOP_ENCRYPTION_REQ_OPCODE(lmp_tx_pkt->data);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending        = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_STOP_ENCRYPTION_REQ;
  acl_peer_info->acl_enc_info.encryption_flags |= DECRYPT_EN;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_switch_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_SWITCH_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_SWITCH_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SWITCH_REQ_OPCODE(lmp_tx_pkt->data);
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending        = 1;
  // acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = opcode;
  //*(uint32 *)&pkt[1] = get_master_clock(bt_common_info) + (((slave_info->poll_interval << 1) > 0x0f)?(slave_info->poll_interval << 1):0x0f); //TODO should be greater of 32 or 2*Tpoll
  //*(uint32 *)&pkt[1] = get_master_clock(bt_common_info) + 0x1000;//(((slave_info->poll_interval << 1) > 0x0f)?(slave_info->poll_interval << 1):0x0f); //TODO should be greater of 32 or 2*Tpoll
  if (acl_peer_info->acl_dev_mgmt_info.mode == BT_CENTRAL_MODE) {
    /*Since if the switch instant offset is more there will be clock drift in the slotoffset so we are using less switch
     * instant offset in this case or else we can switch to the smart fix*/
    // *(uint32 *)&pkt[1] =
    //   (((read_hw_clock(slave_conn_info->timer_in_use) >> 1) & ~3)
    //    + 0x1000); //(((slave_info->poll_interval << 1) > 0x0f)?(slave_info->poll_interval << 1):0x0f); //TODO should be greater of 32 or 2*Tpoll
    SET_LMP_SWITCH_REQ_PARAMETER_SWITCH_INSTANT(lmp_tx_pkt->data, 1);      //TODO
  } else if (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE) {
// #ifdef BT_SIG_ENABLE
//           *(uint32 *)&pkt[1] =
//             ((read_hw_clock(slave_conn_info->timer_in_use) >> 1)
//              + 0x0500); //(((slave_info->poll_interval << 1) > 0x0f)?(slave_info->poll_interval << 1):0x0f); //TODO should be greater of 32 or 2*Tpoll
// #else
//           *(uint32 *)&pkt[1] =
//             ((read_hw_clock(slave_conn_info->timer_in_use) >> 1)
//              + 0x0100); //(((slave_info->poll_interval << 1) > 0x0f)?(slave_info->poll_interval << 1):0x0f); //TODO should be greater of 32 or 2*Tpoll
// #endif
  }
  tx_pkt_len += 4;
  acl_peer_info->acl_link_mgmt_info.role_switch_instant = 1; //TODO

  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_hold_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  (void) tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_hold_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  (void) tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_auto_rate_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_AUTO_RATE_CMD);
  SET_LMP_AUTO_RATE_TID(lmp_tx_pkt->data, tid);
  SET_LMP_AUTO_RATE_OPCODE(lmp_tx_pkt->data);
  btc_dev_info->btc_dev_mgmt_info.auto_rate = AUTO_RATE_ENABLED;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_preferred_rate_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_PREFERRED_RATE_CMD);
  SET_LMP_PREFERRED_RATE_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PREFERRED_RATE_OPCODE(lmp_tx_pkt->data);
  if (!(acl_peer_info->acl_link_mgmt_info.device_status_flags & DEVICE_AUTO_RATE_ENABLED)) {
    BTC_ASSERT(AUTO_RATE_NOT_ENABLED);
  }
  uint8_t datarate = 0;

  /* Here, Setting the datarate Bits in the Order.So,Don't Change it */
  /* Filling BIT(1) & BIT(2) */
  if (acl_peer_info->acl_dev_mgmt_info.ptt == BT_BR_MODE) {
    if (acl_peer_info->acl_dev_mgmt_info.tx_max_slots == ONE_SLOT_PKT) {
      datarate |= 0x02;
    } else if (acl_peer_info->acl_dev_mgmt_info.tx_max_slots == THREE_SLOT_PKT) {
      datarate |= 0x04;
    } else if (acl_peer_info->acl_dev_mgmt_info.tx_max_slots == FIVE_SLOT_PKT) {
      datarate |= 0x06;
    } else {
      BTC_ASSERT(UNKOWN_SLOT_TYPE);
    }
  }

  /* Filling BIT(5) & BIT(6) */
  if (acl_peer_info->acl_dev_mgmt_info.ptt == BT_EDR_MODE) {
    if (acl_peer_info->acl_dev_mgmt_info.tx_max_slots == ONE_SLOT_PKT) {
      datarate |= 0x20;
    } else if (acl_peer_info->acl_dev_mgmt_info.tx_max_slots == THREE_SLOT_PKT) {
      datarate |= 0x40;
    } else if (acl_peer_info->acl_dev_mgmt_info.tx_max_slots == FIVE_SLOT_PKT) {
      datarate |= 0x60;
    } else {
      BTC_ASSERT(UNKOWN_SLOT_TYPE);
    }
  }
  /* Preferred_rate */
  SET_LMP_PREFERRED_RATE_PARAMETER_DATA_RATE(lmp_tx_pkt->data, datarate);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_version_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_VERSION_REQ_CMD);
  SET_LMP_VERSION_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_VERSION_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_VERSION_REQ_PARAMETER_VERSION(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.vers_number);
  SET_LMP_VERSION_REQ_PARAMETER_COMPANY_IDENTIFIER(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.comp_id);
  SET_LMP_VERSION_REQ_PARAMETER_SUBVERSION(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.sub_vers_number);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_version_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_VERSION_RES_CMD);
  SET_LMP_VERSION_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_VERSION_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_VERSION_RES_PARAMETER_VERSION(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.vers_number);
  SET_LMP_VERSION_RES_PARAMETER_COMPANY_IDENTIFIER(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.comp_id);
  SET_LMP_VERSION_RES_PARAMETER_SUBVERSION(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.sub_vers_number);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_features_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_FEATURES_REQ_CMD);
  SET_LMP_FEATURES_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_FEATURES_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_FEATURES_REQ_PARAMETER_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.features_bitmap);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_features_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_FEATURES_RES_CMD);
  SET_LMP_FEATURES_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_FEATURES_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_FEATURES_RES_PARAMETER_FEATURES(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.features_bitmap);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_max_slot_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t max_slots = *(uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_MAX_SLOT_CMD);
  SET_LMP_MAX_SLOT_TID(lmp_tx_pkt->data, tid);
  SET_LMP_MAX_SLOT_OPCODE(lmp_tx_pkt->data);
  SET_LMP_MAX_SLOT_PARAMETER_MAX_SLOTS(lmp_tx_pkt->data, max_slots);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_max_slot_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t max_slots = *(uint8_t *)lmp_input_params;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_MAX_SLOT_REQ_CMD);
  SET_LMP_MAX_SLOT_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_MAX_SLOT_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_MAX_SLOT_REQ_PARAMETER_MAX_SLOTS(lmp_tx_pkt->data, max_slots);
  btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.lmp_tx_pending = 1;
  btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_MAX_SLOT_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_timimg_accuracy_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_TIMIMG_ACCURACY_REQ_CMD);
  SET_LMP_TIMIMG_ACCURACY_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_TIMIMG_ACCURACY_REQ_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_timing_accuracy_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_TIMING_ACCURACY_RES_CMD);
  SET_LMP_TIMING_ACCURACY_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_TIMING_ACCURACY_RES_OPCODE(lmp_tx_pkt->data);
  SET_LMP_TIMING_ACCURACY_RES_PARAMETER_DRIFT(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.drift);
  SET_LMP_TIMING_ACCURACY_RES_PARAMETER_JITTER(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.jitter);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_setup_complete_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SETUP_COMPLETE_CMD);
  SET_LMP_SETUP_COMPLETE_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SETUP_COMPLETE_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_use_semi_permanent_key_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  (void) tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_host_connection_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_HOST_CONNECTION_REQ_CMD);
  SET_LMP_HOST_CONNECTION_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_HOST_CONNECTION_REQ_OPCODE(lmp_tx_pkt->data);
  acl_peer_info->acl_link_mgmt_info.remote_initiated_conn  = 0;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending         = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_HOST_CONNECTION_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_slot_offset_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  (void)lmp_input_params;
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SLOT_OFFSET_CMD);
  SET_LMP_SLOT_OFFSET_TID(lmp_tx_pkt->data, DECODE_LMP_TID(lmp_control_word));
  SET_LMP_SLOT_OFFSET_OPCODE(lmp_tx_pkt->data);
  btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.slot_offset = get_slot_offset(ulc, peer_id);
  SET_LMP_SLOT_OFFSET_PARAMETER_SLOT_OFFSET(lmp_tx_pkt->data, btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.slot_offset);
  SET_LMP_SLOT_OFFSET_PARAMETER_BD_ADDR(lmp_tx_pkt->data, btc_dev_info->btc_dev_mgmt_info.bt_mac_addr);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}

void lmp_page_mode_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_PAGE_MODE_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_PAGE_MODE_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PAGE_MODE_REQ_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_PAGE_MODE_REQ_PARAMETER_PAGE_SCHEME
  // SET_LMP_PAGE_MODE_REQ_PARAMETER_PAGE_SCHEME_SETTING
  BTC_ASSERT(THIS_SHOULD_NEVER_HAPPEN_SINCE_THIS_PACKET_SHOULD_NOT_BE_FORMED);

  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_page_scan_mode_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_PAGE_SCAN_MODE_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_PAGE_SCAN_MODE_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PAGE_SCAN_MODE_REQ_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_PAGE_SCAN_MODE_REQ_PARAMETER_PAGE_SCHEME
  // SET_LMP_PAGE_SCAN_MODE_REQ_PARAMETER_PAGE_SCHEME_SETTING  //TODO
  BTC_ASSERT(THIS_SHOULD_NEVER_HAPPEN_SINCE_THIS_PACKET_SHOULD_NOT_BE_FORMED);

  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_encryption_key_size_mask_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_ENCRYPTION_KEY_SIZE_MASK_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_ENCRYPTION_KEY_SIZE_MASK_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ENCRYPTION_KEY_SIZE_MASK_REQ_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_encryption_key_size_mask_res_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_ENCRYPTION_KEY_SIZE_MASK_RES_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_ENCRYPTION_KEY_SIZE_MASK_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ENCRYPTION_KEY_SIZE_MASK_RES_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_pause_encryption_aes_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_PAUSE_ENCRYPTION_AES_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_PAUSE_ENCRYPTION_AES_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PAUSE_ENCRYPTION_AES_REQ_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_PAUSE_ENCRYPTION_AES_REQ_PARAMETER_RANDOM_NUMBER  //TODO
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_accepted_ext_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t *lmp_input_data = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_ACCEPTED_EXIT_CMD);
  SET_LMP_ACCEPTED_EXT_TID(lmp_tx_pkt->data, tid);
  SET_LMP_ACCEPTED_EXT_OPCODE(lmp_tx_pkt->data);
  SET_LMP_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(lmp_tx_pkt->data, lmp_input_data[0]);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_not_accepted_ext_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t *lmp_input_data = (uint8_t *)lmp_input_params;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_NOT_ACCEPTED_EXIT_CMD);
  SET_LMP_NOT_ACCEPTED_EXT_TID(lmp_tx_pkt->data, tid);
  SET_LMP_NOT_ACCEPTED_EXT_OPCODE(lmp_tx_pkt->data);
  SET_LMP_NOT_ACCEPTED_EXT_PARAMETER_EXTENDED_OPCODE(lmp_tx_pkt->data, lmp_input_data[0]);
  SET_LMP_NOT_ACCEPTED_EXT_PARAMETER_ERROR_CODE(lmp_tx_pkt->data, lmp_input_data[1]);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_clk_adj_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  (void) tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  // SET_LMP_CLK_ADJ_OPCODE
  // SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_ID
  // SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_INSTANT
  // SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_OFFSET
  // SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_SLOTS
  // SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_MODE
  // SET_LMP_CLK_ADJ_PARAMETER_CLK_ADJ_CLK
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_clk_adj_ack_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_CLK_ADJ_ACK_TID(lmp_tx_pkt->data, tid);
  SET_LMP_CLK_ADJ_ACK_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_CLK_ADJ_ACK_PARAMETER_CLK_ADJ_ID()
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_clk_adj_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_CLK_ADJ_REQ_TID(lmp_tx_pkt->data, tid);
  // SET_LMP_CLK_ADJ_REQ_OPCODE
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_packet_type_table_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params)
{
  uint8_t tid = DECODE_LMP_TID(lmp_control_word);
  uint8_t peer_id = DECODE_LMP_PEER_ID(lmp_control_word);
  uint8_t bt_bdr_mode = *(uint8_t *)lmp_input_params;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_PACKET_TYPE_TABLE_REQ_CMD);
  SET_LMP_PACKET_TYPE_TABLE_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PACKET_TYPE_TABLE_REQ_OPCODE(lmp_tx_pkt->data);
  SET_LMP_PACKET_TYPE_TABLE_REQ_PARAMETER_PACKET_TYPE_TABLE(lmp_tx_pkt->data, bt_bdr_mode);
  acl_peer_info->acl_link_mgmt_info.lmp_ext_tx_pending         = 1;
  acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_ext_opcode = LMP_PACKET_TYPE_TABLE_REQ;
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_pause_encryption_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_PAUSE_ENCRYPTION_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_PAUSE_ENCRYPTION_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PAUSE_ENCRYPTION_REQ_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_resume_encryption_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  uint8_t tx_pkt_len = SET_PTL_OF_LMP_RESUME_ENCRYPTION_REQ_CMD;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_RESUME_ENCRYPTION_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_RESUME_ENCRYPTION_REQ_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}

void lmp_ping_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_PING_REQ_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PING_REQ_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_ping_res_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_PING_RES_TID(lmp_tx_pkt->data, tid);
  SET_LMP_PING_RES_OPCODE(lmp_tx_pkt->data);
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_sam_set_type0_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_SAM_SET_TYPE0_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SAM_SET_TYPE0_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_SAM_SET_TYPE0_PARAMETER_UPDATE_MODE
  // SET_LMP_SAM_SET_TYPE0_PARAMETER_SAM_TYPE0_SUBMAP
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_sam_define_map_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_SAM_DEFINE_MAP_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SAM_DEFINE_MAP_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_SAM_DEFINE_MAP_PARAMETER_SAM_INDEX
  // SET_LMP_SAM_DEFINE_MAP_PARAMETER_T_SAM_SM
  // SET_LMP_SAM_DEFINE_MAP_PARAMETER_N_SAM_SM
  // SET_LMP_SAM_DEFINE_MAP_PARAMETER_SAM_SUBMAPS
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
void lmp_sam_switch_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
  uint8_t tid = lmp_input_params->tid;
  uint8_t peer_id = lmp_input_params->peer_id;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  (void) btc_dev_info;
  uint8_t tx_pkt_len = 1;
  pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(tx_pkt_len);
  SET_LMP_SAM_SWITCH_TID(lmp_tx_pkt->data, tid);
  SET_LMP_SAM_SWITCH_OPCODE(lmp_tx_pkt->data);
  // SET_LMP_SAM_SWITCH_PARAMETER_SAM_INDEX
  // SET_LMP_SAM_SWITCH_PARAMETER_TIMING_CONTROL_FLAGS
  // SET_LMP_SAM_SWITCH_PARAMETER_D_SAM
  // SET_LMP_SAM_SWITCH_PARAMETER_SAM_INSTANT
  add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
  return;
}
