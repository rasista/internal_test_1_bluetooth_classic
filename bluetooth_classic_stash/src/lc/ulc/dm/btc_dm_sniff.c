#include "lmp.h"
#include "btc_lmp_power_control.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "lmp_cmd_ptl_autogen.h"
#include "lmp_set_defines_autogen.h"
#include "lmp_get_defines_autogen.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "btc_hci_events.h"
#include "btc_lmp_sniff.h"
#include "btc_dm_sniff.h"
#include "hci_cmd_get_defines_autogen.h"
#include "btc_process_hci_cmd.h"
#include "btc_lmp.h"

uint32_t get_current_tsf(ulc_t *ulc)
{
  (void)ulc; // Unused parameter
  return 0;
}

uint32_t get_next_anchor_pt_tsf(ulc_t *ulc, uint8_t peer_id)
{
  (void)ulc; // Unused parameter
  (void)peer_id; // Unused parameter
  return 0;
}

uint16_t get_priority_role_running(ulc_t *ulc, uint8_t peer_id)
{
  (void)peer_id; // Unused parameter
  (void)ulc; // Unused parameter
  return 0;
}

uint32_t get_priority_slot_start_tsf(ulc_t *ulc, uint8_t peer_id)
{
  (void)ulc; // Unused parameter
  (void)peer_id; // Unused parameter
  return 0;
}

uint32_t get_next_sniff_anchor_point(ulc_t *ulc, uint8_t peer_id)
{
  (void)ulc; // Unused parameter
  (void)peer_id; // Unused parameter
  return 0;
}

void sniff_role_change_from_lpw(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
  uint8_t sniff_req = *bt_pkt_start;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  sniff_params_t *sniff_params = &(acl_peer_info->sniff_params);
  acl_link_mgmt_info->device_status_flags &= ~(PAUSE_ACLD_TRAFFIC);
  if (sniff_req == LMP_SNIFF_REQ) {
    acl_link_mgmt_info->connected_sub_state = CONNECTED_SNIFF_MODE;
    bt_mode_change_event_to_hci(ulc, peer_id, BT_STATUS_SUCCESS, sniff_params->t_sniff);
    if (IS_SNIFF_SUBRATE_PARAMS_SET(sniff_params)) {
      sniff_params->max_remote_sniff_subrate = dm_calc_remote_max_sniff_subrate(ulc, peer_id);
      calc_sniff_subrating_instant(ulc, peer_id);
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ESCAPE_OPCODE, LMP_SNIFF_SUBRATING_REQ, PERIPHERAL_INITIATED_TID, peer_id), NULL);
    }
  } else if (sniff_req == LMP_UNSNIFF_REQ) {
    exit_sniff_mode(ulc, peer_id);
    bt_mode_change_event_to_hci(ulc, peer_id, BT_STATUS_SUCCESS, 0);
  }}

void sniff_subrating_params_update_from_lpw(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t peer_id = GET_PEER_ID(pkb);
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  if (btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_SNIFF_SUBR) {
    sniff_params->sniff_subrating_enable     = SNIFF_SUBR_ENABLE;
  }
}

uint8_t dm_enable_sniff_mode_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb)
{
  uint8_t status = BT_SUCCESS_STATUS;
  btc_dev_info_t *bt_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  uint16_t max_interval = GET_SNIFF_MODE_SNIFF_MAX_INTERVAL(pkb->data);
  uint16_t min_interval = GET_SNIFF_MODE_SNIFF_MIN_INTERVAL(pkb->data);
  sniff_params->sniff_attempt = GET_SNIFF_MODE_SNIFF_ATTEMPT(pkb->data);
  sniff_params->sniff_tout = GET_SNIFF_MODE_SNIFF_TIMEOUT(pkb->data);
  if (IS_SNIFF_PARAMS_INVALID(sniff_params, acl_link_mgmt_info, max_interval, min_interval)) {
    status = UNSUPPORTED_LMP_PARAMETER_VALUE;
  } else {
    if (max_interval != min_interval) {
      uint16_t rand_num;
      generate_rand_num(2, (uint8_t *)&rand_num, PSEUDO_RAND_NUM);
      // Ensure t_sniff is even by masking the LSB
      uint16_t range = max_interval - min_interval;
      sniff_params->t_sniff = CALCULATE_SNIFF_RANDOM_EVEN_INTERVAL(min_interval, rand_num, range);
    } else {
      sniff_params->t_sniff = min_interval;
    }
    if (IS_SNIFF_SUPERVISION_TIMEOUT_INVALID(acl_link_mgmt_info->link_supervision_timeout, sniff_params->t_sniff)) {
      status = UNSUPPORTED_LMP_PARAMETER_VALUE;
    }
    sniff_params->d_sniff = 0;
    uint16_t priority_role_running = get_priority_role_running(ulc, peer_id);
    if (HAS_OTHER_ACTIVE_PEERS(priority_role_running, peer_id)) {
      sniff_params->d_sniff = align_sniff_anchor_point(ulc, peer_id);
    }
  }
  return status;
}

 void bt_mode_change_event_to_hci(ulc_t *ulc, uint8_t peer_id, uint8_t status, uint16_t interval)
{
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
#ifdef BT_MS_ENABLE
  //bt_update_priority_role(bt_common_info, slave_id, slave_conn_info->connected_sub_state);
#endif
  hci_mode_change_event(ulc, acl_dev_mgmt_info->conn_handle, status, acl_link_mgmt_info->connected_sub_state, interval);
}

void exit_sniff_mode(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  acl_peer_info->acl_link_mgmt_info.connected_sub_state = CONNECTED_ACTIVE_MODE;
  memset((uint8_t *)sniff_params, 0, sizeof(sniff_params_t));
  acl_peer_info->acl_link_mgmt_info.sniff_poll_transition = SAFE_SNIFF_TRANSITION_SLOTS;
}

void lmp_change_link_supervision_tout_from_hci(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_link_mgmt_info_t *acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
  if (acl_link_mgmt_info->link_supervision_timeout) {
    acl_link_mgmt_info->link_supervision_enabled = LINK_SUPERVISION_ENABLED;
  } else {
    acl_link_mgmt_info->link_supervision_enabled = LINK_SUPERVISION_DISABLED;
  }
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_SUPERVISION_TIMEOUT, 0, peer_id, CENTERAL_INITIATED_TID), NULL);
}

uint16_t align_sniff_anchor_point(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  uint32_t curr_tsf = 0;
  uint16_t d_sniff  = sniff_params->d_sniff;
  uint8_t tmp_peer_id = 0;
  uint32_t d_sniff_temp_tsf = 0;
  uint16_t priority_role_running = get_priority_role_running(ulc, peer_id); // TODO
  uint32_t tmp_priority_slot_start_tsf;
  uint32_t next_anchor_pt_tsf;

  /* Check for other active peers in sniff mode */
  for (tmp_peer_id = 0; (tmp_peer_id < MAX_NUM_ACL_CONNS); tmp_peer_id++) {
    if (tmp_peer_id != peer_id && (priority_role_running & (BIT(tmp_peer_id)))) {
      acl_peer_info_t *temp_acl_peer_info = btc_dev_info->acl_peer_info[tmp_peer_id];
      curr_tsf         = get_current_tsf(ulc); // TODO
      d_sniff_temp_tsf = curr_tsf + (sniff_params->d_sniff * BT_SLOT_TIME_US);
      /* Check if anchor points conflict:
      * - If no conflict: keep original d_sniff
      * - If conflict: adjust d_sniff to avoid overlap
      * TODO: 
      * 1. Consider sniff subrating
      * 2. HCI timing alignment issues with queued packets
      * 3. Extended role duration impact on multiple connections
      */
      tmp_priority_slot_start_tsf = get_priority_slot_start_tsf(ulc, tmp_peer_id); //TODO
      if (IS_NO_SNIFF_CONFLICT(tmp_priority_slot_start_tsf, d_sniff_temp_tsf, sniff_params->sniff_attempt * BT_SLOT_TIME_US)) {
        d_sniff = sniff_params->d_sniff;
      } else {
        sniff_params_t *temp_sniff_params = &temp_acl_peer_info->sniff_params;
        /* TODO: Enhance for multiple connections
        * 1. Synchronize sniff intervals for multiple peers
        * 2. Adjust sniff timeout for multi-peer scenarios
        */
        next_anchor_pt_tsf = get_next_anchor_pt_tsf(ulc, tmp_peer_id); // TODO
        d_sniff = CALCULATE_D_SNIFF(next_anchor_pt_tsf, curr_tsf, temp_sniff_params->sniff_attempt, temp_sniff_params->sniff_tout);
      }
      break; // TODO: Remove break to support more than 2 peers
    }
  }
  return d_sniff;
}

uint16_t dm_calc_remote_max_sniff_subrate(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params   = &acl_peer_info->sniff_params;
  uint16_t max_sniff_subrate;
  max_sniff_subrate = CALCULATE_MAX_SNIFF_SUBRATE(sniff_params->remote_max_sniff_latency, sniff_params->t_sniff);
  if (!max_sniff_subrate) {
    max_sniff_subrate = 1;
  }
  else if (max_sniff_subrate > MAX_SNIFF_SUB_RATE) {
    /* Prevent overflow: sniff_subrate is 1 byte, but latency/t_sniff may exceed 255 */
    BTC_ASSERT(INVALID_PARAM);
  }
  return max_sniff_subrate;
}

/**
 * @brief This function chooses the sniff subrating instant
 * @params ulc structure
 * @params peer_id
 */
void calc_sniff_subrating_instant(ulc_t *ulc, uint8_t peer_id)
{
  uint32_t next_sniff_anchor_point;
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
  if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
    next_sniff_anchor_point = get_next_sniff_anchor_point(ulc, peer_id);
    sniff_params->sniff_subrating_instant = CALCULATE_SNIFF_SUBRATING_INSTANT(next_sniff_anchor_point, sniff_params->t_sniff);
  } else {
    sniff_params->sniff_subrating_instant = 0;
  }
}