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
#include "btc_dm.h"
#include "btc_hci_events.h"
#include "usched_pi_scan.h"
#include "btc_dm_sniff.h"
#include "btc_dm_simple_secure_pairing.h"



void hci_cmd_set_event_filter_handler(void *ctx, pkb_t *pkb)
{
  uint8_t filter_type;
  uint8_t filter_cond_type;
  uint8_t auto_accept_flag;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(ulc->btc_dev_info.btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, SET_EVENT_FILTER_EVENT_PARAM_LENGTH);

  filter_type = GET_SET_EVENT_FILTER_FILTER_TYPE(pkb->data);
  filter_cond_type = GET_SET_EVENT_FILTER_FILTER_CONDITION_TYPE(pkb->data);

  switch (filter_type) {
    case HCI_CLEAR_ALL_EVENT_FLT_TYPE:
    {
      // Implement this functionality in the future
      btc_dev_mgmt_info->event_filter_type = 0;
      btc_dev_mgmt_info->event_filter_cond_type = 0;
      btc_dev_mgmt_info->event_auto_accept_flag = 0;
      break;
    }
    case HCI_INQ_EVENT_FLT_TYPE:
    {
      switch (filter_cond_type) {
        case HCI_INQ_ALL_RESP_COND_TYPE:
        {
          break;
        }
        case HCI_INQ_SP_CLASS_OF_DEVICE:
        {
          memcpy(btc_dev_mgmt_info->class_of_dev, &(pkb->data[5]), CLASS_OF_DEV_LEN);
          memcpy(btc_dev_mgmt_info->class_of_dev_mask, &(pkb->data[5 + CLASS_OF_DEV_LEN]), CLASS_OF_DEV_LEN);
          break;
        }
        case HCI_INQ_SP_BD_ADDR:
        {
          memcpy(btc_dev_mgmt_info->bt_mac_addr, &(pkb->data[5]), MAC_ADDR_LEN);
          break;
        }
        default:
        {
          BTC_ASSERT(BT_UNEXPECTED_HCI_EVENT_FLT_TYPE_RCVD);
          break;
        }
      }
      break;
    }
    case HCI_CONN_EVENT_FLT_TYPE:
    {
      switch (filter_cond_type) {
        case HCI_CONN_ALLOW_ALL_CONN:
        {
          btc_dev_mgmt_info->event_auto_accept_flag = auto_accept_flag = GET_SET_EVENT_FILTER_CONDITION(pkb->data);
          if (auto_accept_flag == 0x01) {
            if (btc_dev_mgmt_info->dev_status_flags & ROLE_SWITCH_EN) {
              btc_dev_mgmt_info->features_bitmap[0] |= LMP_RSWITCH;
              btc_dev_mgmt_info->link_policy_settings |= ENABLE_ROLE_SWITCH;
              btc_dev_info->btc_hci_params.event_map_mask[0] |= CONN_REQ_EVENT;
            } else {
              btc_dev_info->btc_hci_params.event_map_mask[0] |= CONN_REQ_EVENT;
            }
          }
          if (auto_accept_flag == 0x02) {
            if (btc_dev_mgmt_info->dev_status_flags & ROLE_SWITCH_EN) {
              btc_dev_mgmt_info->features_bitmap[0] &= ~(LMP_RSWITCH);
              btc_dev_mgmt_info->link_policy_settings &= ~ENABLE_ROLE_SWITCH;
              btc_dev_info->btc_hci_params.event_map_mask[0] &= ~CONN_REQ_EVENT;
            } else {
              btc_dev_info->btc_hci_params.event_map_mask[0] &= ~CONN_REQ_EVENT;
            }
          } else if (auto_accept_flag == 0x03) {
            if (btc_dev_mgmt_info->dev_status_flags & ROLE_SWITCH_EN) {
              btc_dev_mgmt_info->features_bitmap[0] |= LMP_RSWITCH;
              btc_dev_mgmt_info->link_policy_settings |= ENABLE_ROLE_SWITCH;
              btc_dev_info->btc_hci_params.event_map_mask[0] &= ~CONN_REQ_EVENT;
            } else {
              btc_dev_info->btc_hci_params.event_map_mask[0] &= ~CONN_REQ_EVENT;
            }
          }
          break;
        }
        case HCI_CONN_SP_CLASS_OF_DEVICE:
        {
          memcpy(btc_dev_mgmt_info->class_of_dev, &(pkb->data[5]), CLASS_OF_DEV_LEN);
          memcpy(btc_dev_mgmt_info->class_of_dev_mask, &(pkb->data[5 + CLASS_OF_DEV_LEN]), CLASS_OF_DEV_LEN);
          btc_dev_mgmt_info->event_auto_accept_flag = auto_accept_flag = pkb->data[11];
          break;
        }
        case HCI_CONN_SP_BD_ADDR:
        {
          memcpy(btc_dev_info->btc_hci_params.bd_addr, &pkb->data[5], MAC_ADDR_LEN);
          btc_dev_mgmt_info->event_auto_accept_flag = auto_accept_flag = pkb->data[11];
          break;
        }
        default:
        {
          BTC_ASSERT(BT_UNEXPECTED_HCI_EVENT_FLT_TYPE_RCVD);
          break;
        }
      }
      break;
    }
    default:
    {
      BTC_ASSERT(BT_UNEXPECTED_HCI_EVENT_FLT_TYPE_RCVD);
      break;
    }
  }
  SET_CE_SET_EVENT_FILTER_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);

  return;
}

void hci_cmd_read_pin_type_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t fixed_pin = 0;
  fixed_pin = bt_dev_info->btc_dev_mgmt_info.fixed_pin;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_PIN_TYPE_EVENT_PARAM_LENGTH);
  SET_CE_READ_PIN_TYPE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_PIN_TYPE_PIN_TYPE(cmd_complete_event->data, fixed_pin);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_pin_type_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  bt_dev_info->btc_dev_mgmt_info.fixed_pin = GET_WRITE_PIN_TYPE_PIN_TYPE(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_PIN_TYPE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_PIN_TYPE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_stored_link_key_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t num_of_keys_read = 0;
  uint8_t bd_addrs[MAX_NUM_OF_STORED_LINK_KEY * MAC_ADDR_LEN];
  uint8_t read_all_flag, ii;
  uint8_t no_of_link_key_stored = bt_dev_info->btc_dev_mgmt_info.no_of_link_key_stored;

  read_all_flag = GET_READ_STORED_LINK_KEY_READ_ALL_FLAG(pkb->data);
  switch (read_all_flag) {
    case HCI_READ_STORED_LINK_KEY_SP_BD_ADDR:
    {
      if (bt_find_link_key(ulc, GET_READ_STORED_LINK_KEY_BD_ADDR(pkb->data))) {
        hci_return_link_keys_event(ulc, 1, GET_READ_STORED_LINK_KEY_BD_ADDR(pkb->data));
        num_of_keys_read++;
      }
      break;
    }
    case HCI_READ_STORED_LINK_KEY_FOR_ALL_BD_ADDR:
    {
      if (no_of_link_key_stored) {
        for (ii = 0; ii < no_of_link_key_stored; ii++) {
          memcpy(&bd_addrs[ii * MAC_ADDR_LEN], &bt_dev_info->btc_dev_mgmt_info.link_key_info[ii].bd_addr, MAC_ADDR_LEN);
        }
        hci_return_link_keys_event(ulc, no_of_link_key_stored, bd_addrs);
        num_of_keys_read += no_of_link_key_stored;
      }

      break;
    }
    default:
    {
      status = INVALID_HCI_COMMAND_PARAMETERS;
      break;
    }
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_STORED_LINK_KEY_EVENT_PARAM_LENGTH);
  SET_CE_READ_STORED_LINK_KEY_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_STORED_LINK_KEY_MAX_NUM_KEYS(cmd_complete_event->data, MAX_NUM_OF_STORED_LINK_KEY);
  SET_CE_READ_STORED_LINK_KEY_NUM_KEYS_READ(cmd_complete_event->data, no_of_link_key_stored);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_write_stored_link_key_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(btc_dev_info->btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t link_key[LINK_KEY_LEN];
  uint8_t no_of_keys, ii, no_of_keys_written = 0;

  no_of_keys = GET_WRITE_STORED_LINK_KEY_NUM_KEYS_TO_WRITE(pkb->data);
  if (btc_dev_mgmt_info->no_of_link_key_stored < MAX_NUM_OF_STORED_LINK_KEY) {
    for (ii = 0; ii < no_of_keys; ii++) {
      memcpy(bd_addr, (GET_WRITE_STORED_LINK_KEY_BD_ADDR(pkb->data) + (ii * (MAC_ADDR_LEN + LINK_KEY_LEN))), MAC_ADDR_LEN);
      memcpy(link_key, (GET_WRITE_STORED_LINK_KEY_LINK_KEY(pkb->data) + (ii * (MAC_ADDR_LEN + LINK_KEY_LEN))), LINK_KEY_LEN);
      bt_store_link_key(ulc, bd_addr, link_key);
      no_of_keys_written++;
      if (btc_dev_mgmt_info->no_of_link_key_stored >= MAX_NUM_OF_STORED_LINK_KEY) {
        break;
      }
    }
  } else {
    status = UNKNOWN_HCI_COMMAND;
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_STORED_LINK_KEY_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_STORED_LINK_KEY_STATUS(cmd_complete_event->data, status);
  SET_CE_WRITE_STORED_LINK_KEY_NUM_KEYS_WRITTEN(cmd_complete_event->data, no_of_keys_written);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_delete_stored_link_key_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t delete_all_flag;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t no_of_stored_keys;
  uint8_t no_of_keys_deleted = 0;

  memcpy(bd_addr, GET_DELETE_STORED_LINK_KEY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  no_of_stored_keys = btc_dev_mgmt_info->no_of_link_key_stored;
  delete_all_flag = GET_DELETE_STORED_LINK_KEY_DELETE_ALL_FLAG(pkb->data);
  switch (delete_all_flag) {
    case HCI_DEL_STORED_LINK_KEY_SP_BD_ADDR:
    {
      bt_delete_link_key(ulc, bd_addr);
      bt_clear_security_flags(ulc, bd_addr, delete_all_flag);
      break;
    }
    case HCI_DEL_STORED_LINK_KEY_FOR_ALL_BD_ADDR:
    {
      memset((uint8_t *)&btc_dev_mgmt_info->link_key_info, '\0', (sizeof(link_key_info_t) * MAX_NUM_OF_STORED_LINK_KEY));
      btc_dev_mgmt_info->no_of_link_key_stored = 0;
      bt_clear_security_flags(ulc, bd_addr, delete_all_flag);
      break;
    }
    default:
    {
      status = INVALID_HCI_COMMAND_PARAMETERS;
      break;
    }
  }

  if (no_of_stored_keys >= btc_dev_mgmt_info->no_of_link_key_stored) {
    no_of_keys_deleted = (no_of_stored_keys - btc_dev_mgmt_info->no_of_link_key_stored);
  } else {
    BTC_PRINT("Warning: Inconsistent link key count detected!\n");
    BTC_ASSERT(0);
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, DELETE_STORED_LINK_KEY_EVENT_PARAM_LENGTH);
  SET_CE_DELETE_STORED_LINK_KEY_STATUS(cmd_complete_event->data, status);
  SET_CE_DELETE_STORED_LINK_KEY_NUM_KEYS_DELETED(cmd_complete_event->data, no_of_keys_deleted);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_local_name_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t byte_pos;
  memset((uint8_t *)btc_dev_mgmt_info->device_name, '\0', MAX_BT_DEVICE_NAME_LEN);
  for (byte_pos = 0; byte_pos < MAX_BT_DEVICE_NAME_LEN; byte_pos++) {
    if (pkb->data[4 + byte_pos] == '\0') {
      break;
    }
    btc_dev_mgmt_info->device_name[byte_pos] = pkb->data[4 + byte_pos];
  }
  btc_dev_mgmt_info->device_name_len = byte_pos;
  BTC_PRINT("local name is : %s \n", btc_dev_mgmt_info->device_name);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_LOCAL_NAME_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_LOCAL_NAME_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_local_name_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LOCAL_NAME_EVENT_PARAM_LENGTH);
  SET_CE_READ_LOCAL_NAME_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_LOCAL_NAME_LOCAL_NAME(cmd_complete_event->data, btc_dev_mgmt_info->device_name);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_connection_accept_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t btc_dev_mgmt_info = bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_accept_tout = 0;

  conn_accept_tout = btc_dev_mgmt_info.conn_accept_tout;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_CONNECTION_ACCEPT_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_READ_CONNECTION_ACCEPT_TIMEOUT_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_CONNECTION_ACCEPT_TIMEOUT_CONNECTION_ACCEPT_TIMEOUT(cmd_complete_event->data, conn_accept_tout);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_write_connection_accept_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  btc_dev_mgmt_info->conn_accept_tout = GET_WRITE_CONNECTION_ACCEPT_TIMEOUT_CONN_ACCEPT_TIMEOUT(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_CONNECTION_ACCEPT_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_CONNECTION_ACCEPT_TIMEOUT_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_page_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  page_params_t page_params = bt_dev_info->page_params;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t page_tout = 0;

  page_tout = page_params.page_tout;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_PAGE_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_READ_PAGE_TIMEOUT_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_PAGE_TIMEOUT_PAGE_TIMEOUT(cmd_complete_event->data, page_tout);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_page_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  page_params_t *page_params = &bt_dev_info->page_params;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  page_params->page_tout = GET_WRITE_PAGE_TIMEOUT_PAGE_TIMEOUT(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_PAGE_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_PAGE_TIMEOUT_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_scan_enable_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t btc_dev_mgmt_info = bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t scan_enable = 0;

  scan_enable = btc_dev_mgmt_info.scan_enable;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_SCAN_ENABLE_EVENT_PARAM_LENGTH);
  SET_CE_READ_SCAN_ENABLE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_SCAN_ENABLE_SCAN_ENABLE(cmd_complete_event->data, scan_enable);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_scan_enable_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t is_window;
  uint16_t is_interval;
  uint16_t ps_window;
  uint16_t ps_interval;
  // Parse the scan enable values
  uint8_t old_scan_enable = btc_dev_mgmt_info->scan_enable;
  uint8_t new_scan_enable = GET_WRITE_SCAN_ENABLE_SCAN_ENABLE(pkb->data);

  is_window = bt_dev_info->inquiry_scan_params.inquiry_scan_window;
  is_interval = bt_dev_info->inquiry_scan_params.inquiry_scan_interval;
  ps_window = bt_dev_info->page_scan_params.page_scan_window;
  ps_interval = bt_dev_info->page_scan_params.page_scan_interval;

  /*Ensures that Inquiry Scan is newly enabled by checking it is enabled in the new configuration but not in the old configuration.*/
  if ((new_scan_enable & INQUIRY_SCAN_EN) && (!(old_scan_enable & INQUIRY_SCAN_EN))) {
    usched_pi_scan_parameters_init(&(ulc->scheduler_info), is_window, is_interval, INQUIRY_SCAN_ROLE);
  }

  /*Ensures that Page Scan is newly enabled by checking it is enabled in the new configuration but not in the old configuration.*/
  if ((new_scan_enable & PAGE_SCAN_EN) && (!(old_scan_enable & PAGE_SCAN_EN))) {
    usched_pi_scan_parameters_init(&(ulc->scheduler_info), ps_window, ps_interval, PAGE_SCAN_ROLE);
  }

  /*Ensures that Inquiry Scan is newly disabled by checking it is disabled in the new configuration but not in the old configuration.*/
  if ((old_scan_enable & INQUIRY_SCAN_EN) && (!(new_scan_enable & INQUIRY_SCAN_EN))) {
    usched_pi_scan_parameters_deinit(&(ulc->scheduler_info), INQUIRY_SCAN_ROLE);
  }

  /*Ensures that Page Scan is newly disabled by checking it is disabled in the new configuration but not in the old configuration.*/
  if ((old_scan_enable & PAGE_SCAN_EN) && (!(new_scan_enable & PAGE_SCAN_EN))) {
    usched_pi_scan_parameters_deinit(&(ulc->scheduler_info), PAGE_SCAN_ROLE);
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_SCAN_ENABLE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_SCAN_ENABLE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_page_scan_activity_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  page_scan_params_t page_scan_params = bt_dev_info->page_scan_params;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t page_scan_window = 0;
  uint16_t page_scan_interval = 0;

  page_scan_window = page_scan_params.page_scan_window;
  page_scan_interval = page_scan_params.page_scan_interval;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_PAGE_SCAN_ACTIVITY_EVENT_PARAM_LENGTH);
  SET_CE_READ_PAGE_SCAN_ACTIVITY_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_PAGE_SCAN_ACTIVITY_PAGE_SCAN_INTERVAL(cmd_complete_event->data, page_scan_interval);
  SET_CE_READ_PAGE_SCAN_ACTIVITY_PAGE_SCAN_WINDOW(cmd_complete_event->data, page_scan_window);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_page_scan_activity_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  page_scan_params_t *page_scan_params = &(bt_dev_info->page_scan_params);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  page_scan_params->page_scan_window = GET_WRITE_PAGE_SCAN_ACTIVITY_PAGE_SCAN_WINDOW(pkb->data);
  page_scan_params->page_scan_interval = GET_WRITE_PAGE_SCAN_ACTIVITY_PAGE_SCAN_INTERVAL(pkb->data);

  if (page_scan_params->page_scan_interval <= 0x800 /* 1.28 sec */) {
    if (page_scan_params->page_scan_window == page_scan_params->page_scan_interval) {
      page_scan_params->page_scan_rep_mode = PAGE_SCAN_MODE_R0;
    } else {
      page_scan_params->page_scan_rep_mode = PAGE_SCAN_MODE_R1;
    }
  } else {
    page_scan_params->page_scan_rep_mode = PAGE_SCAN_MODE_R2;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_PAGE_SCAN_ACTIVITY_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_PAGE_SCAN_ACTIVITY_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_inquiry_scan_activity_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  inquiry_scan_params_t inquiry_scan_params = bt_dev_info->inquiry_scan_params;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t inquiry_scan_window = 0;
  uint16_t inquiry_scan_interval = 0;

  inquiry_scan_window = inquiry_scan_params.inquiry_scan_window;
  inquiry_scan_interval = inquiry_scan_params.inquiry_scan_interval;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_INQUIRY_SCAN_ACTIVITY_EVENT_PARAM_LENGTH);
  SET_CE_READ_INQUIRY_SCAN_ACTIVITY_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_INQUIRY_SCAN_ACTIVITY_INQUIRY_SCAN_INTERVAL(cmd_complete_event->data, inquiry_scan_interval);
  SET_CE_READ_INQUIRY_SCAN_ACTIVITY_INQUIRY_SCAN_WINDOW(cmd_complete_event->data, inquiry_scan_window);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_write_inquiry_scan_activity_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  inquiry_scan_params_t *inquiry_scan_params = &bt_dev_info->inquiry_scan_params;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  inquiry_scan_params->inquiry_scan_window = GET_WRITE_INQUIRY_SCAN_ACTIVITY_INQUIRY_SCAN_WINDOW(pkb->data);
  inquiry_scan_params->inquiry_scan_interval = GET_WRITE_INQUIRY_SCAN_ACTIVITY_INQUIRY_SCAN_INTERVAL(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_INQUIRY_SCAN_ACTIVITY_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_INQUIRY_SCAN_ACTIVITY_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_authentication_enable_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t btc_dev_mgmt_info = bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t auth_enable = 0;

  auth_enable = btc_dev_mgmt_info.auth_enable;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_AUTHENTICATION_ENABLE_EVENT_PARAM_LENGTH);
  SET_CE_READ_AUTHENTICATION_ENABLE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_AUTHENTICATION_ENABLE_AUTHENTICATION_ENABLE(cmd_complete_event->data, auth_enable);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_write_authentication_enable_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  btc_dev_mgmt_info->auth_enable = GET_WRITE_AUTHENTICATION_ENABLE_AUTHENTICATION_ENABLE(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_AUTHENTICATION_ENABLE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_AUTHENTICATION_ENABLE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_class_of_device_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t btc_dev_mgmt_info = bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_CLASS_OF_DEVICE_EVENT_PARAM_LENGTH);
  SET_CE_READ_CLASS_OF_DEVICE_STATUS(cmd_complete_event->data, status);
  memcpy(&cmd_complete_event->data[7], btc_dev_mgmt_info.class_of_dev, CLASS_OF_DEV_LEN);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_write_class_of_device_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(bt_dev_info->btc_dev_mgmt_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  memcpy(btc_dev_mgmt_info->class_of_dev, GET_WRITE_CLASS_OF_DEVICE_CLASS_OF_DEVICE(pkb->data), CLASS_OF_DEV_LEN);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_CLASS_OF_DEVICE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_CLASS_OF_DEVICE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_voice_setting_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t btc_dev_mgmt_info = bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t voice_setting = 0;

  voice_setting = btc_dev_mgmt_info.voice_setting;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_VOICE_SETTING_EVENT_PARAM_LENGTH);
  SET_CE_READ_VOICE_SETTING_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_VOICE_SETTING_VOICE_SETTING(cmd_complete_event->data, voice_setting);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_write_voice_setting_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &bt_dev_info->btc_dev_mgmt_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  btc_dev_mgmt_info->voice_setting = GET_WRITE_VOICE_SETTING_VOICE_SETTING(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_VOICE_SETTING_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_VOICE_SETTING_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_automatic_flush_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle, auto_flush_timeout = 0;
  uint8_t peer_id = 0xFF;
  conn_handle = GET_READ_AUTOMATIC_FLUSH_TIMEOUT_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    auto_flush_timeout = bt_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.auto_flush_timeout;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_AUTOMATIC_FLUSH_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_READ_AUTOMATIC_FLUSH_TIMEOUT_FLUSH_TIMEOUT(cmd_complete_event->data, auto_flush_timeout);
  SET_CE_READ_AUTOMATIC_FLUSH_TIMEOUT_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_READ_AUTOMATIC_FLUSH_TIMEOUT_STATUS(cmd_complete_event->data, status);
  return;
}
void hci_cmd_write_automatic_flush_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF;

  conn_handle = GET_WRITE_AUTOMATIC_FLUSH_TIMEOUT_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    bt_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.auto_flush_timeout = GET_WRITE_AUTOMATIC_FLUSH_TIMEOUT_FLUSH_TIMEOUT(pkb->data);
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_AUTOMATIC_FLUSH_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_AUTOMATIC_FLUSH_TIMEOUT_STATUS(cmd_complete_event->data, status);
  SET_CE_WRITE_AUTOMATIC_FLUSH_TIMEOUT_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  send_hci_response_packet(cmd_complete_event);

  return;
}

void hci_cmd_read_transmit_power_level_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  conn_pwr_ctrl_info_t conn_pwr_ctrl_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF;

  conn_handle = GET_READ_TRANSMIT_POWER_LEVEL_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_TRANSMIT_POWER_LEVEL_EVENT_PARAM_LENGTH);
  SET_CE_READ_TRANSMIT_POWER_LEVEL_STATUS(cmd_complete_event->data, status);
  if (status == BT_SUCCESS_STATUS) {
    conn_pwr_ctrl_info = bt_dev_info->acl_peer_info[peer_id]->conn_pwr_ctrl_info;
    if (GET_READ_TRANSMIT_POWER_LEVEL_TYPE(pkb->data) == READ_CURRENT_TX_PWR_LEVEL) {
      SET_CE_READ_TRANSMIT_POWER_LEVEL_TX_POWER_LEVEL(cmd_complete_event->data, conn_pwr_ctrl_info.tx_power_index);
    } else {
      SET_CE_READ_TRANSMIT_POWER_LEVEL_TX_POWER_LEVEL(cmd_complete_event->data, conn_pwr_ctrl_info.max_allowed_power_index);
    }
  }
  SET_CE_READ_TRANSMIT_POWER_LEVEL_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  send_hci_response_packet(cmd_complete_event);

  return;
}

void hci_cmd_read_link_supervision_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle, link_supervision_timeout = 0;
  uint8_t peer_id = INVALID_PEER_ID;
  conn_handle = GET_READ_LINK_SUPERVISION_TIMEOUT_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    link_supervision_timeout = bt_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.link_supervision_timeout;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LINK_SUPERVISION_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_READ_LINK_SUPERVISION_TIMEOUT_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_LINK_SUPERVISION_TIMEOUT_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_READ_LINK_SUPERVISION_TIMEOUT_LINK_SUPERVISION_TIMEOUT(cmd_complete_event->data, link_supervision_timeout);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_link_supervision_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_WRITE_LINK_SUPERVISION_TIMEOUT_CONNECTION_HANDLE(pkb->data);
  uint16_t link_supervision_timeout = GET_WRITE_LINK_SUPERVISION_TIMEOUT_LINK_SUPERVISION_TIMEOUT(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    bt_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.link_supervision_timeout = link_supervision_timeout;
    lmp_change_link_supervision_tout_from_hci(ulc, peer_id);
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), WRITE_LINK_SUPERVISION_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_LINK_SUPERVISION_TIMEOUT_STATUS(cmd_complete_event->data, status);
  SET_CE_WRITE_LINK_SUPERVISION_TIMEOUT_HANDLE(cmd_complete_event->data, conn_handle);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_number_of_supported_iac_handler(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_NUMBER_OF_SUPPORTED_IAC_EVENT_PARAM_LENGTH);
  SET_CE_READ_NUMBER_OF_SUPPORTED_IAC_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_NUMBER_OF_SUPPORTED_IAC_NUM_SUPPORTED_IAC(cmd_complete_event->data, BT_NUM_OF_SUPP_IAC);
  send_hci_response_packet(cmd_complete_event);

  return;
}
void hci_cmd_read_current_iac_lap_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_CURRENT_IAC_LAP_EVENT_PARAM_LENGTH);
  SET_CE_READ_CURRENT_IAC_LAP_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_CURRENT_IAC_LAP_NUM_CURRENT_IAC(cmd_complete_event->data, BT_NUM_OF_SUPP_IAC);
  memcpy(&cmd_complete_event->data[8], bt_dev_info->inquiry_params.inquiry_address_generic, BT_LAP_LEN);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_current_iac_lap_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  memcpy(bt_dev_info->inquiry_params.inquiry_address_generic, GET_WRITE_CURRENT_IAC_LAP_LAP(pkb->data), BT_LAP_LEN);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_CURRENT_IAC_LAP_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_CURRENT_IAC_LAP_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_set_afh_host_channel_classification_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t good_channels_cnt = 0;

  /* updating the afh_host_channel_classification */
  memcpy(bt_dev_info->btc_dev_mgmt_info.host_classification_channel_map, GET_SET_AFH_HOST_CHANNEL_CLASSIFICATION_CHANNEL_MAP(pkb->data), CHANNEL_MAP_BYTES);
  good_channels_cnt = get_no_of_good_channels(bt_dev_info->btc_dev_mgmt_info.host_classification_channel_map);

  if ((good_channels_cnt >= MIN_GOOD_CHANNEL_CNT) && (!IS_AFH_CHANNEL_MAP_RESERVED_BIT_SET(pkb->data))) {
      bt_dev_info->btc_dev_mgmt_info.host_ch_classification_cmd = 1;
  } else {
      status = INVALID_HCI_COMMAND_PARAMETERS;
  }
  
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), SET_AFH_HOST_CHANNEL_CLASSIFICATION_EVENT_PARAM_LENGTH);
  SET_CE_SET_AFH_HOST_CHANNEL_CLASSIFICATION_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_inquiry_scan_type_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t inquiry_scan_type = 0;

  inquiry_scan_type = bt_dev_info->inquiry_scan_params.inquiry_scan_type;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_INQUIRY_SCAN_TYPE_EVENT_PARAM_LENGTH);
  SET_CE_READ_INQUIRY_SCAN_TYPE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_INQUIRY_SCAN_TYPE_INQUIRY_SCAN_TYPE(cmd_complete_event->data, inquiry_scan_type);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_inquiry_scan_type_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->inquiry_scan_params.inquiry_scan_type = GET_WRITE_INQUIRY_SCAN_TYPE_INQUIRY_SCAN_TYPE(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_INQUIRY_SCAN_TYPE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_INQUIRY_SCAN_TYPE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_inquiry_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t inquiry_mode = 0;

  inquiry_mode = bt_dev_info->inquiry_params.inquiry_mode;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_INQUIRY_MODE_EVENT_PARAM_LENGTH);
  SET_CE_READ_INQUIRY_MODE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_INQUIRY_MODE_INQUIRY_MODE(cmd_complete_event->data, inquiry_mode);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_inquiry_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->inquiry_params.inquiry_mode = GET_WRITE_INQUIRY_MODE_INQUIRY_MODE(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_INQUIRY_MODE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_INQUIRY_MODE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_page_scan_type_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t page_scan_type = 0;

  page_scan_type = bt_dev_info->page_scan_params.page_scan_type;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_PAGE_SCAN_TYPE_EVENT_PARAM_LENGTH);
  SET_CE_READ_PAGE_SCAN_TYPE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_PAGE_SCAN_TYPE_PAGE_SCAN_TYPE(cmd_complete_event->data, page_scan_type);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_page_scan_type_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->page_scan_params.page_scan_type = GET_WRITE_PAGE_SCAN_TYPE_PAGE_SCAN_TYPE(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_PAGE_SCAN_TYPE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_PAGE_SCAN_TYPE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_afh_channel_assessment_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), READ_AFH_CHANNEL_ASSESSMENT_MODE_EVENT_PARAM_LENGTH);
  SET_CE_READ_AFH_CHANNEL_ASSESSMENT_MODE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_AFH_CHANNEL_ASSESSMENT_MODE_AFH_CHANNEL_ASSESSMENT_MODE(cmd_complete_event->data, bt_dev_info->btc_dev_mgmt_info.channel_assessment_mode);
  send_hci_response_packet(cmd_complete_event);
}
void hci_cmd_write_afh_channel_assessment_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->btc_dev_mgmt_info.channel_assessment_mode = GET_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_AFH_CHANNEL_ASSESSMENT_MODE(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), WRITE_AFH_CHANNEL_ASSESSMENT_MODE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_extended_inquiry_response_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t extended_inquiry_response[SIZE_OF_EIR];
  uint8_t fec_requirement_for_eir;

  fec_requirement_for_eir = bt_dev_info->inquiry_params.fec_requirement_for_eir;
  memcpy(extended_inquiry_response, bt_dev_info->btc_dev_mgmt_info.extended_inquiry_response, SIZE_OF_EIR);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_EXTENDED_INQUIRY_RESPONSE_EVENT_PARAM_LENGTH);
  SET_CE_READ_EXTENDED_INQUIRY_RESPONSE_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_EXTENDED_INQUIRY_RESPONSE_EXTENDED_INQUIRY_RESPONSE(cmd_complete_event->data, extended_inquiry_response);
  SET_CE_READ_EXTENDED_INQUIRY_RESPONSE_FEC_REQUIRED(cmd_complete_event->data, fec_requirement_for_eir);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_extended_inquiry_response_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint32_t ptr_count = 0;
  uint8_t *data_ptr;

  bt_dev_info->inquiry_params.fec_requirement_for_eir = GET_WRITE_EXTENDED_INQUIRY_RESPONSE_FEC_REQUIRED(pkb->data);
  memset(bt_dev_info->btc_dev_mgmt_info.extended_inquiry_response, 0, SIZE_OF_EIR);
  memcpy(bt_dev_info->btc_dev_mgmt_info.extended_inquiry_response, GET_WRITE_EXTENDED_INQUIRY_RESPONSE_EXTENDED_INQUIRY_RESPONSE(pkb->data), SIZE_OF_EIR);
  data_ptr = bt_dev_info->btc_dev_mgmt_info.extended_inquiry_response + (SIZE_OF_EIR - 1);
  while (*data_ptr == 0) {
    ptr_count++;
    data_ptr--;
  }
  bt_dev_info->inquiry_params.eir_length = SIZE_OF_EIR - ptr_count;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_EXTENDED_INQUIRY_RESPONSE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_EXTENDED_INQUIRY_RESPONSE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_refresh_encryption_key_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF;
  ulc_t *ulc = (ulc_t *)ctx;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_REFRESH_ENCRYPTION_KEY_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    status = lmp_refresh_enc_key_from_hci(ulc, peer_id);
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, REFRESH_ENCRYPTION_KEY_EVENT_PARAM_LENGTH);
  SET_CE_REFRESH_ENCRYPTION_KEY_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_local_oob_data_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t oob_data[32];

  lmp_read_local_oob_data(ulc, oob_data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_LOCAL_OOB_DATA_EVENT_PARAM_LENGTH);
  SET_CE_READ_LOCAL_OOB_DATA_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_LOCAL_OOB_DATA_C(cmd_complete_event->data, oob_data[0]); /// made changes need to look into it
  SET_CE_READ_LOCAL_OOB_DATA_R(cmd_complete_event->data, (oob_data + 16)[0]); /// made changes for warning removal
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_inquiry_response_transmit_power_level_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t tx_power_index;

  tx_power_index = bt_dev_info->btc_dev_mgmt_info.tx_non_connected_power_index;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_EVENT_PARAM_LENGTH);
  SET_CE_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_TX_POWER(cmd_complete_event->data, tx_power_index);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_inquiry_transmit_power_level_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->btc_dev_mgmt_info.tx_non_connected_power_index = GET_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_TX_POWER_LEVEL(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_send_keypress_notification_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t notification_type = GET_SEND_KEYPRESS_NOTIFICATION_NOTIFICATION_TYPE(pkb->data);
  memcpy(bd_addr, GET_SEND_KEYPRESS_NOTIFICATION_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);
  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    lmp_send_key_press_notification(ulc, peer_id, notification_type);
    status = BT_SUCCESS_STATUS;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), SEND_KEYPRESS_NOTIFICATION_EVENT_PARAM_LENGTH);
  SET_CE_SEND_KEYPRESS_NOTIFICATION_STATUS(cmd_complete_event->data, status);
  SET_CE_SEND_KEYPRESS_NOTIFICATION_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_enhanced_flush_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  conn_handle = GET_ENHANCED_FLUSH_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    acl_peer_info->acl_link_mgmt_info.enhanced_flush_pkt_type = GET_ENHANCED_FLUSH_PACKET_TYPE(pkb->data);
    if (acl_peer_info->acl_link_mgmt_info.enhanced_flush_pkt_type != AUTOMATICALLY_FLUSHABLE) {
      status = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
    } else {
      acl_peer_info->acl_link_mgmt_info.enhanced_flush_pkt_type = 1;
    }
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(hciOpcode, ENHANCED_FLUSH_EVENT_PARAM_LENGTH);
  SET_SE_ENHANCED_FLUSH_STATUS(cmd_status_event->data, status);
  send_hci_response_packet(cmd_status_event);
  return;
}

void hci_cmd_read_enhanced_transmit_power_level_handler(void *ctx, pkb_t *pkb)
{
  uint16_t conn_handle;
  uint8_t peer_id = 0xFF;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  conn_pwr_ctrl_info_t conn_pwr_ctrl_info;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t power_index = 0;

  conn_handle = GET_READ_ENHANCED_TRANSMIT_POWER_LEVEL_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_ENHANCED_TRANSMIT_POWER_LEVEL_EVENT_PARAM_LENGTH);
  SET_CE_READ_ENHANCED_TRANSMIT_POWER_LEVEL_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_ENHANCED_TRANSMIT_POWER_LEVEL_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);

  if (status == BT_SUCCESS_STATUS) {
    conn_pwr_ctrl_info = btc_dev_info->acl_peer_info[peer_id]->conn_pwr_ctrl_info;
    if (GET_READ_ENHANCED_TRANSMIT_POWER_LEVEL_TYPE(pkb->data) == READ_CURRENT_TX_PWR_LEVEL) {
      power_index = conn_pwr_ctrl_info.tx_power_index;
    } else {
      power_index = conn_pwr_ctrl_info.max_allowed_power_index;
    }
  }
  SET_CE_READ_ENHANCED_TRANSMIT_POWER_LEVEL_TX_POWER_LEVEL_GFSK(cmd_complete_event->data, power_index);
  SET_CE_READ_ENHANCED_TRANSMIT_POWER_LEVEL_TX_POWER_LEVEL_DQPSK(cmd_complete_event->data, power_index);
  SET_CE_READ_ENHANCED_TRANSMIT_POWER_LEVEL_TX_POWER_LEVEL_8DPSK(cmd_complete_event->data, power_index);

  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_set_event_mask_page_2_handler(void *ctx, pkb_t *pkb)
{
  // TODO
  (void)ctx;
  (void)pkb;
  return;
}

void hci_cmd_read_extended_page_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t ext_page_tout;

  ext_page_tout = bt_dev_info->page_params.ext_page_tout;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_EXTENDED_PAGE_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_READ_EXTENDED_PAGE_TIMEOUT_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_EXTENDED_PAGE_TIMEOUT_EXTENDED_PAGE_TIMEOUT(cmd_complete_event->data, ext_page_tout);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_extended_page_timeout_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->page_params.ext_page_tout = GET_WRITE_EXTENDED_PAGE_TIMEOUT_EXTENDED_PAGE_TIMEOUT(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_EXTENDED_PAGE_TIMEOUT_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_EXTENDED_PAGE_TIMEOUT_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_read_extended_inquiry_length_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t ext_inq_len;

  ext_inq_len = bt_dev_info->inquiry_params.ext_inq_len;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_EXTENDED_INQUIRY_LENGTH_EVENT_PARAM_LENGTH);
  SET_CE_READ_EXTENDED_INQUIRY_LENGTH_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_EXTENDED_INQUIRY_LENGTH_EXTENDED_INQUIRY_LENGTH(cmd_complete_event->data, ext_inq_len);
  send_hci_response_packet(cmd_complete_event);
  return;
}
void hci_cmd_write_extended_inquiry_length_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  bt_dev_info->inquiry_params.ext_inq_len = GET_WRITE_EXTENDED_INQUIRY_LENGTH_EXTENDED_INQUIRY_LENGTH(pkb->data);

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_EXTENDED_INQUIRY_LENGTH_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_EXTENDED_INQUIRY_LENGTH_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_simple_pairing_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t simple_secure_pairing_mode = (bt_dev_info->btc_dev_mgmt_info.dev_status_flags & SIMPLE_PAIRING_EN) ? 1 : 0;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_SIMPLE_PAIRING_MODE_EVENT_PARAM_LENGTH);
  SET_CE_READ_SIMPLE_PAIRING_MODE_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_SIMPLE_PAIRING_MODE_SIMPLE_PAIRING_MODE(cmd_complete_event->data, simple_secure_pairing_mode);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_simple_pairing_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t simple_secure_pairing_mode = GET_WRITE_SIMPLE_PAIRING_MODE_SIMPLE_PAIRING_MODE(pkb->data);
  if (simple_secure_pairing_mode) {
    bt_dev_info->btc_dev_mgmt_info.dev_status_flags |= SIMPLE_PAIRING_EN;
  } else {
    bt_dev_info->btc_dev_mgmt_info.dev_status_flags &= ~SIMPLE_PAIRING_EN;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_SIMPLE_PAIRING_MODE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_SIMPLE_PAIRING_MODE_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_set_event_mask_handler(void *ctx, pkb_t *pkb)
{
  // TODO
  (void)ctx;
  (void)pkb;
  return;
}

void hci_cmd_reset_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t peer_id;
  btc_deint(ulc);
  for (peer_id = 0; peer_id < MAX_NUM_ACL_CONNS; peer_id++) {
    bt_reset_peer_info(ulc, peer_id);
  }

  // DEVICE RESET TODO

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, RESET_EVENT_PARAM_LENGTH);
  SET_CE_RESET_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_read_secure_connections_host_support_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t secure_connections_host_support = (bt_dev_info->btc_dev_mgmt_info.dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? 1 : 0;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, READ_SECURE_CONNECTIONS_HOST_SUPPORT_EVENT_PARAM_LENGTH);
  SET_CE_READ_SECURE_CONNECTIONS_HOST_SUPPORT_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_SECURE_CONNECTIONS_HOST_SUPPORT_SECURE_CONNECTIONS_HOST_SUPPORT(cmd_complete_event->data, secure_connections_host_support);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_secure_connections_host_support_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t secure_connections_host_support = GET_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_SECURE_CONNECTIONS_HOST_SUPPORT(pkb->data);

  if (secure_connections_host_support) {
    bt_dev_info->btc_dev_mgmt_info.dev_status_flags |= SECURE_CONNECTIONS_HOST_SUPPORT;
  } else {
    bt_dev_info->btc_dev_mgmt_info.dev_status_flags &= ~SECURE_CONNECTIONS_HOST_SUPPORT;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}