#include "btc_dev.h"
#include "btc_hci.h"
#include "hci.h"
#include "hci_cmd_get_defines_autogen.h"
#include "hci_handlers_autogen.h"
#include "hci_cmd_set_defines_status_event_autogen.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "ulc.h"
#include "btc_process_hci_cmd.h"
#include "btc_lmp.h"

void hci_cmd_read_link_quality_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF, link_quality = 0;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_READ_LINK_QUALITY_HANDLE(pkb->data);

  peer_id = bt_conn_handle_to_peer_id(ulc, peer_id);
  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    link_quality = btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.link_quality;
  }

  /* TODO : Link quality information should be calculated from BER
   * For now, we are providing this info.
   */
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LINK_QUALITY_EVENT_PARAM_LENGTH);
  SET_CE_READ_LINK_QUALITY_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_LINK_QUALITY_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_READ_LINK_QUALITY_LINK_QUALITY(cmd_complete_event->data, link_quality);

  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_rssi_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF, rssi;
  ulc_t *ulc = (ulc_t *)ctx;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_READ_RSSI_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, peer_id);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_RSSI_EVENT_PARAM_LENGTH);
  SET_CE_READ_RSSI_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_RSSI_HANDLE(cmd_complete_event->data, conn_handle);

  if (status == BT_STATUS_SUCCESS) {
    rssi = (uint8_t)dm_read_rssi_for_conn(ulc, peer_id);
    SET_CE_READ_RSSI_RSSI(cmd_complete_event->data, rssi);
  }

  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_afh_channel_map_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF, afh_mode = 0;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_READ_AFH_CHANNEL_MAP_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, peer_id);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_RSSI_EVENT_PARAM_LENGTH);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    acl_dev_mgmt_info_t acl_dev_mgmt_info = btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info;
    acl_link_mgmt_info_t acl_link_mgmt_info = btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info;
    if (acl_link_mgmt_info.device_status_flags & AFH_ENABLED) {
      afh_mode = 1;
    }
    SET_CE_READ_AFH_CHANNEL_MAP_AFH_CHANNEL_MAP(cmd_complete_event->data, acl_dev_mgmt_info.afh_channel_map_in_use);
  }

  SET_CE_READ_AFH_CHANNEL_MAP_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_AFH_CHANNEL_MAP_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_READ_AFH_CHANNEL_MAP_AFH_MODE(cmd_complete_event->data, afh_mode);

  /* Calling command complete event */
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_clock_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF, clock = 0;
  ulc_t *ulc = (ulc_t *)ctx;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_READ_CLOCK_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, peer_id);
  //if (peer_id <= 0xFF) {
  if (GET_READ_CLOCK_WHICH_CLOCK(pkb->data) == 0) {
    // ipl_reg = conditional_intr_disable();
    // clock   = read_hw_clock(LMAC_TIMER_1); //TODO
    // conditional_intr_enable(ipl_reg);
  } else if (GET_READ_CLOCK_WHICH_CLOCK(pkb->data) == 1) {
    // ipl_reg = conditional_intr_disable();
    // clock   = read_hw_clock(bt_info->bt_current_slot_params_p->lmac_timer_in_use_for_intrpts);
    // conditional_intr_enable(ipl_reg);
  } else {
    status = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
  }
  // } else {
  //   status = UNKNOWN_CONNECTION_IDENTIFIER;
  // }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_CLOCK_EVENT_PARAM_LENGTH);
  SET_CE_READ_CLOCK_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_CLOCK_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_READ_CLOCK_CLOCK(cmd_complete_event->data, clock);
  SET_CE_READ_CLOCK_ACCURACY(cmd_complete_event->data, 0xFFFF);

  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_encryption_key_size_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle, key_size = 0;
  uint8_t peer_id = 0xFF;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_READ_ENCRYPTION_KEY_SIZE_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, peer_id);

  if (peer_id < 0xFF) {
    acl_link_mgmt_info_t acl_link_mgmt_info = btc_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info;
    acl_enc_info_t acl_enc_info = btc_dev_info->acl_peer_info[peer_id]->acl_enc_info;
    if (acl_link_mgmt_info.device_status_flags & DEVICE_ENC_ENABLED) {
      key_size = acl_enc_info.enc_key_size;
    } else {
      status = INSUFFICIENT_SECURITY;
    }
  } else {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_ENCRYPTION_KEY_SIZE_EVENT_PARAM_LENGTH);
  SET_CE_READ_ENCRYPTION_KEY_SIZE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_ENCRYPTION_KEY_SIZE_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_READ_ENCRYPTION_KEY_SIZE_KEY_SIZE(cmd_complete_event->data, key_size);

  send_hci_response_packet(cmd_complete_event);
  return;
}
