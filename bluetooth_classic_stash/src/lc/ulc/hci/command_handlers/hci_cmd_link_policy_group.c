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
#include "btc_hci_events.h"
#include "btc_lmp_qos.h"
#include "btc_lmp_sniff.h"
#include "btc_dm_sniff.h"

void hci_cmd_sniff_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  uint8_t status;
  uint16_t conn_handle = GET_SNIFF_MODE_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id < INVALID_PEER_ID) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);
    if (IS_IN_SNIFF_STATE(acl_link_mgmt_info)) {
      status = COMMAND_DISALLOWED;
    } else {
      status = dm_enable_sniff_mode_from_hci(ulc, peer_id, pkb);
    }
  } else {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
  if (status == BT_SUCCESS_STATUS) {
    lmp_enable_or_disable_sniff_mode(ulc, peer_id, SNIFF_MODE_ENABLE);
  }
}

void hci_cmd_exit_sniff_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  uint8_t status;
  uint16_t conn_handle = GET_EXIT_SNIFF_MODE_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id < INVALID_PEER_ID) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

    if (acl_link_mgmt_info->connected_sub_state != CONNECTED_SNIFF_MODE) {
      status = COMMAND_DISALLOWED;
    } else {
      lmp_enable_or_disable_sniff_mode(ulc, peer_id, SNIFF_MODE_DISABLE);
      status = BT_SUCCESS_STATUS;
    }
  } else {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_sniff_subrating_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status;
  uint16_t conn_handle = GET_SNIFF_SUBRATING_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id < INVALID_PEER_ID) {
    status = lmp_change_sniff_subrating_params_from_hci(ulc, peer_id, pkb);
  } else {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), SNIFF_SUBRATING_EVENT_PARAM_LENGTH);
  SET_CE_SNIFF_SUBRATING_STATUS(cmd_complete_event->data, status);
  SET_CE_SNIFF_SUBRATING_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_qos_setup_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = NULL;
  uint8_t status;
  uint16_t conn_handle = GET_QOS_SETUP_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
    acl_dev_mgmt_info->service_type = GET_QOS_SETUP_SERVICE_TYPE(pkb->data);
    acl_dev_mgmt_info->latency = *GET_QOS_SETUP_LATENCY(pkb->data);
    lmp_qos_setup(ulc, peer_id);
    status = BT_SUCCESS_STATUS;
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_read_link_policy_settings_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t link_policy_settings = 0;
  uint8_t status;

  uint16_t conn_handle = GET_READ_LINK_POLICY_SETTINGS_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    link_policy_settings = bt_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.link_policy_settings;
    status = BT_SUCCESS_STATUS;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), READ_LINK_POLICY_SETTINGS_EVENT_PARAM_LENGTH);
  SET_CE_READ_LINK_POLICY_SETTINGS_STATUS(cmd_complete_event->data, status);
  SET_CE_READ_LINK_POLICY_SETTINGS_CONNECTION_HANDLE(cmd_complete_event, conn_handle);
  SET_CE_READ_LINK_POLICY_SETTINGS_LINK_POLICY_SETTINGS(cmd_complete_event, link_policy_settings);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_link_policy_settings_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_WRITE_LINK_POLICY_SETTINGS_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    bt_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.link_policy_settings = GET_WRITE_LINK_POLICY_SETTINGS_LINK_POLICY_SETTINGS(pkb->data);
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), WRITE_LINK_POLICY_SETTINGS_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_LINK_POLICY_SETTINGS_STATUS(cmd_complete_event->data, status);
  SET_CE_WRITE_LINK_POLICY_SETTINGS_CONNECTION_HANDLE(cmd_complete_event, conn_handle);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_default_link_policy_settings_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint16_t link_policy_settings = bt_dev_info->btc_dev_mgmt_info.link_policy_settings;
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), READ_DEFAULT_LINK_POLICY_SETTINGS_EVENT_PARAM_LENGTH);
  SET_CE_READ_DEFAULT_LINK_POLICY_SETTINGS_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  SET_CE_READ_DEFAULT_LINK_POLICY_SETTINGS_DEFAULT_LINK_POLICY_SETTINGS(cmd_complete_event, link_policy_settings);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_write_default_link_policy_settings_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  bt_dev_info->btc_dev_mgmt_info.link_policy_settings = GET_WRITE_DEFAULT_LINK_POLICY_SETTINGS_DEFAULT_LINK_POLICY_SETTINGS(pkb->data);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), WRITE_DEFAULT_LINK_POLICY_SETTINGS_EVENT_PARAM_LENGTH);
  SET_CE_WRITE_DEFAULT_LINK_POLICY_SETTINGS_STATUS(cmd_complete_event->data, BT_SUCCESS_STATUS);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_role_discovery_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = NULL;
  uint8_t peer_id = 0xFF;
  uint16_t conn_handle;
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  conn_handle = GET_ROLE_DISCOVERY_CONNECTION_HANDLE(pkb->data);
  peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id == 0xFF) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  /* Filling the return parameters */
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(hciOpcode, ROLE_DISCOVERY_EVENT_PARAM_LENGTH);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
    if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
      SET_CE_ROLE_DISCOVERY_CURRENT_ROLE(cmd_complete_event->data, 0); /*< central-0, Pheripheral-1 */
    } else {
      SET_CE_ROLE_DISCOVERY_CURRENT_ROLE(cmd_complete_event->data, 1);
    }
  }
  SET_CE_ROLE_DISCOVERY_CONNECTION_HANDLE(cmd_complete_event->data, conn_handle);
  SET_CE_ROLE_DISCOVERY_STATUS(cmd_complete_event->data, status);

  /* Calling the command complete event */
  send_hci_response_packet(cmd_complete_event);
  return;
}

void hci_cmd_switch_role_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = NULL;
  uint8_t peer_id;
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  uint8_t switch_to_mode = 0; /* DUT new role to switch*/
  peer_id = bt_mac_addr_to_peer_id(ulc, GET_SWITCH_ROLE_BD_ADDR(pkb->data));

  if ((peer_id == 0xFF)
      //&& (btc_dev_info->btc_dev_mgmt_info.no_of_periph_connected >= 1)
      ) {
    status = COMMAND_DISALLOWED;
  }
  //! Based on number of slaves & masters connected, and switch request disallow the role switch
  acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
  /* Call function in LMP */
  if (GET_SWITCH_ROLE_ROLE(pkb->data) == 0x0) {
    if (acl_dev_mgmt_info->mode == BT_CENTRAL_MODE) {
    
      if (btc_dev_info->btc_dev_mgmt_info.no_of_periph_connected == MAX_NUM_ACL_CONNS){
        //! Slave to master switch. ALready DUT connected as master for max connections, so not possible to switch to master
         //! If max masters are connected then don't allow role switch to new master
        status = COMMAND_DISALLOWED;
      } else

      {
        switch_to_mode = BT_CENTRAL_MODE;
      }
    }
  } else {
    if (
      acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) { //! If max slaves are connected then don't allow role switch to new slave. FIXME: Need to handle for multiple slaves
      // #ifdef BT_MS_ENABLE
      // #ifndef BT_2_SLAVE_SUPPORT
      //           if (bt_info->bt_context_info_p->bt_roles & BT_ROLE_SLAVE)
      // #else
      //           //! Master to slave switch. ALready DUT connected as slave for max connections, so not possible to switch to slave
      //           if (bt_ram_info->no_of_masters_connected == BT_MAX_SUPPORTED_ACL_LINKS)
      // #endif
      //           {
      //             status = COMMAND_DISALLOWED;
      //           } else
      // #endif
      {
        switch_to_mode = BT_PERIPHERAL_MODE;
      }
    }
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(hciOpcode, SWITCH_ROLE_EVENT_PARAM_LENGTH);
  SET_CE_SWITCH_ROLE_STATUS(cmd_status_event->data, status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    void lmp_cmd_lmp_switch_req_handler(void *ctx, pkb_t *pkb);
  }

  if (switch_to_mode == 0) {                       //! Role switch requested for the same role
    hci_role_change_event(ulc, peer_id, status);   // TODO
  } else if (switch_to_mode == BT_CENTRAL_MODE) {
    lmp_switch_to_central(ulc, peer_id);   // TODO
  } else if (switch_to_mode == BT_PERIPHERAL_MODE) {
    lmp_switch_to_peripheral(ulc, peer_id);   // TODO
  }

  return;
}