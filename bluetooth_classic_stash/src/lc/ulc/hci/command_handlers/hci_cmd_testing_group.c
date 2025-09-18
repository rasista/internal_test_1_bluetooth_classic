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
#include "btc_dm_simple_secure_pairing.h"


void hci_cmd_enable_device_under_test_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;

  btc_dev_info->btc_dev_mgmt_info.host_dev_test_mode_enable = DEVICE_TEST_MODE_ENABLED;
  inquiry_scan_params_t *inquiry_scan_params = &(btc_dev_info->inquiry_scan_params);
  inquiry_scan_params->inquiry_scan_interval = DEFAULT_ISCAN_INTERVAL;
  inquiry_scan_params->inquiry_scan_window = DEFAULT_ISCAN_WINDOW;

  page_scan_params_t *page_scan_params = &(btc_dev_info->page_scan_params);
  page_scan_params->page_scan_interval = DEFAULT_PSCAN_INTERVAL;
  page_scan_params->page_scan_window = DEFAULT_PSCAN_WINDOW;

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, ENABLE_DEVICE_UNDER_TEST_MODE_EVENT_PARAM_LENGTH);
  SET_CE_ENABLE_DEVICE_UNDER_TEST_MODE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_simple_pairing_debug_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t debug_mode = GET_WRITE_SIMPLE_PAIRING_MODE_SIMPLE_PAIRING_MODE(pkb->data);
  btc_dev_mgmt_info_t *btc_dev_mgmt_info = &(btc_dev_info->btc_dev_mgmt_info);

  if (btc_dev_mgmt_info->ssp_debug_mode == debug_mode)
  {
    if (debug_mode == SSP_DEBUG_MODE_ENABLED)
    {
      btc_dev_mgmt_info->ssp_debug_mode = SSP_DEBUG_MODE_ENABLED;
      btc_dev_mgmt_info->ssp_link_key_type = SSP_DEBUG_LINK_KEY;
      memcpy(btc_dev_mgmt_info->private_key, btc_dev_mgmt_info->debug_private_key, PRIVATE_KEY_LEN);
      memcpy(btc_dev_mgmt_info->public_key, btc_dev_mgmt_info->debug_public_key, PUBLIC_KEY_LEN);
    }
    else if (debug_mode == SSP_DEBUG_MODE_DISABLED)
    {
      btc_dev_mgmt_info->ssp_debug_mode = SSP_DEBUG_MODE_DISABLED;
      btc_dev_mgmt_info->ssp_link_key_type = SSP_COMB_KEY;
      generate_rand_num(PRIVATE_KEY_LEN, btc_dev_mgmt_info->private_key, PSEUDO_RAND_NUM);
      generate_rand_num(PRIVATE_KEY_LEN_256, btc_dev_mgmt_info->private_key_p_256, PSEUDO_RAND_NUM);

      btc_dev_mgmt_info->private_key[PRIVATE_KEY_LEN - 1] &= 0x0F;
      btc_dev_mgmt_info->private_key_256[PRIVATE_KEY_LEN_256 - 1] &= 0x0F;
      dm_generate_public_key(ulc);
      btc_dev_mgmt_info->dev_status_flags |= PUBLIC_KEY_CALC_PENDING;
      btc_dev_mgmt_info->dev_status_flags &= ~PUBLIC_KEY_CALC_PENDING;
    } else {
      status = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
    }
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, WRITE_SIMPLE_PAIRING_DEBUG_MODE_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_SIMPLE_PAIRING_DEBUG_MODE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}

