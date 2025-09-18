#include "btc_dev.h"
#include "btc_hci.h"
#include "hci.h"
#include "hci_cmd_get_defines_autogen.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "hci_handlers_autogen.h"
#include "ulc.h"

void hci_cmd_read_local_version_information_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info_p = &(ulc->btc_dev_info.btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LOCAL_VERSION_INFORMATION_EVENT_PARAM_LENGTH);
  SET_CE_READ_LOCAL_VERSION_INFORMATION_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_LOCAL_VERSION_INFORMATION_HCI_VERSION(cmd_complete_event->data, btc_dev_mgmt_info_p->hci_vers_num);
  SET_CE_READ_LOCAL_VERSION_INFORMATION_HCI_SUBVERSION(cmd_complete_event->data, btc_dev_mgmt_info_p->hci_revision);
  SET_CE_READ_LOCAL_VERSION_INFORMATION_LMP_VERSION(cmd_complete_event->data, btc_dev_mgmt_info_p->lmp_vers_num);
  SET_CE_READ_LOCAL_VERSION_INFORMATION_COMPANY_IDENTIFIER(cmd_complete_event->data, btc_dev_mgmt_info_p->comp_id);
  SET_CE_READ_LOCAL_VERSION_INFORMATION_LMP_SUBVERSION(cmd_complete_event->data, btc_dev_mgmt_info_p->lmp_sub_vers_num);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_local_supported_commands_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_hci_params_t *btc_hci_params = &(ulc->btc_dev_info.btc_hci_params);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LOCAL_SUPPORTED_COMMANDS_EVENT_PARAM_LENGTH);
  SET_CE_READ_LOCAL_SUPPORTED_COMMANDS_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_LOCAL_SUPPORTED_COMMANDS_SUPPORTED_COMMANDS(cmd_complete_event->data, btc_hci_params->hci_cmd_bitmap);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_local_extended_features_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(ulc->btc_dev_info.btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LOCAL_EXTENDED_FEATURES_EVENT_PARAM_LENGTH);
  uint8_t page_num = GET_READ_LOCAL_EXTENDED_FEATURES_PAGE_NUMBER(pkb->data);
  SET_CE_READ_LOCAL_EXTENDED_FEATURES_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_LOCAL_EXTENDED_FEATURES_PAGE_NUMBER(cmd_complete_event->data, page_num);
  SET_CE_READ_LOCAL_EXTENDED_FEATURES_MAX_PAGE_NUMBER(cmd_complete_event->data, btc_dev_mgmt_info->max_page_num);
  if (page_num == 0) {
    SET_CE_READ_LOCAL_EXTENDED_FEATURES_EXTENDED_LMP_FEATURES(cmd_complete_event->data, btc_dev_mgmt_info->features_bitmap);
  } else if (page_num == 1) {
    SET_CE_READ_LOCAL_EXTENDED_FEATURES_EXTENDED_LMP_FEATURES(cmd_complete_event->data, btc_dev_mgmt_info->ext_features_bitmap);
  } else {
    memset(&(cmd_complete_event->data[8]), 0, EXT_FEATURE_MASK_LENGTH);
  }
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_local_supported_features_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(ulc->btc_dev_info.btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LOCAL_SUPPORTED_FEATURES_EVENT_PARAM_LENGTH);
  SET_CE_READ_LOCAL_SUPPORTED_FEATURES_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_LOCAL_SUPPORTED_FEATURES_LMP_FEATURES(cmd_complete_event->data, btc_dev_mgmt_info->features_bitmap);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_buffer_size_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(ulc->btc_dev_info.btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_BUFFER_SIZE_EVENT_PARAM_LENGTH);
  SET_CE_READ_BUFFER_SIZE_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_BUFFER_SIZE_ACL_DATA_PACKET_LENGTH(cmd_complete_event->data, btc_dev_mgmt_info->max_acl_pkt_len);
  SET_CE_READ_BUFFER_SIZE_SYNCHRONOUS_DATA_PACKET_LENGTH(cmd_complete_event->data, btc_dev_mgmt_info->max_sco_pkt_len);
  SET_CE_READ_BUFFER_SIZE_TOTAL_NUM_ACL_DATA_PACKETS(cmd_complete_event->data, btc_dev_mgmt_info->max_acl_pkt_len);
  SET_CE_READ_BUFFER_SIZE_HOST_TOTAL_NUM_SYNCHRONOUS_DATA_PACKETS(cmd_complete_event->data, btc_dev_mgmt_info->max_sco_pkt_len);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_bd_addr_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(ulc->btc_dev_info.btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_BD_ADDR_EVENT_PARAM_LENGTH);
  SET_CE_READ_BD_ADDR_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_BD_ADDR_BD_ADDR(cmd_complete_event->data, btc_dev_mgmt_info->bt_mac_addr);
  send_hci_response_packet(cmd_complete_event);
  return;
}

