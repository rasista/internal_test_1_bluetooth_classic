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
#include "btc.h"
#include "usched_pi.h"
#include "btc_dm_authentication.h"
#include "btc_dm_simple_secure_pairing.h"


void hci_cmd_inquiry_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  uint8_t status;
  if (btc_dev_info_p->inquiry_params.inq_enable == INQUIRY_DISABLED) {
    btc_dev_info_p->inquiry_params.inq_enable = INQUIRY_ENABLED;
    btc_dev_info_p->inquiry_params.inquiry_length = GET_INQUIRY_INQUIRY_LENGTH(pkb->data);
    /* Filling Inquiry Parameters sent from host */
    memcpy(btc_dev_info_p->inquiry_params.inquiry_address_from_host, GET_INQUIRY_LAP(pkb->data), BT_LAP_LEN);
    memset(btc_dev_info_p->inquiry_params.inquiry_address_from_host + BT_LAP_LEN, 0, BT_LAP_LEN);
    usched_pi_parameters_init(&(ulc->scheduler_info), btc_dev_info_p->inquiry_params.inquiry_length, INQUIRY_ROLE);
    status = BT_SUCCESS_STATUS;
  } else {
    status = COMMAND_DISALLOWED;
  }
  if (status == BT_SUCCESS_STATUS) {
    // Allocate memory for the global buffer
    allocate_resp_addr_list(btc_dev_info_p);
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_inquiry_cancel_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  uint8_t status;
  if (btc_dev_info_p->inquiry_params.inq_enable == INQUIRY_ENABLED) {
    usched_pi_stop_handler(NULL, &ulc->scheduler_info.inquiry_role_cb.task);
    status = BT_SUCCESS_STATUS;
  } else {
    status = COMMAND_DISALLOWED;
  }
  //Free the resp_addr_list buffer
  if (status == BT_SUCCESS_STATUS) {
    free_resp_addr_list(btc_dev_info_p);
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), INQUIRY_CANCEL_EVENT_PARAM_LENGTH);
  SET_CE_INQUIRY_CANCEL_STATUS(cmd_complete_event->data, status);
  btc_dev_info_p->inquiry_params.inq_enable = INQUIRY_DISABLED;
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_periodic_inquiry_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  uint8_t status;
  uint16_t max_period_len = GET_PERIODIC_INQUIRY_MODE_MAX_PERIOD_LENGTH(pkb->data);
  uint16_t min_period_len = GET_PERIODIC_INQUIRY_MODE_MIN_PERIOD_LENGTH(pkb->data);
  uint8_t inquiry_length = GET_PERIODIC_INQUIRY_MODE_INQUIRY_LENGTH(pkb->data);

  if (btc_dev_info_p->inquiry_params.periodic_inq_enable == PERIODIC_INQUIRY_DISABLED) {
    btc_dev_info_p->inquiry_params.periodic_inq_enable = PERIODIC_INQUIRY_ENABLED;
    if ((max_period_len > min_period_len) && (min_period_len > inquiry_length)) {
      memcpy(btc_dev_info_p->inquiry_params.inquiry_address_from_host, GET_PERIODIC_INQUIRY_MODE_LAP(pkb->data), BT_LAP_LEN);
      memset(btc_dev_info_p->inquiry_params.inquiry_address_from_host + BT_LAP_LEN, 0, BT_LAP_LEN);
      btc_dev_info_p->inquiry_params.periodic_inq_params.inquiry_len = inquiry_length;
      btc_dev_info_p->inquiry_params.num_resp = GET_PERIODIC_INQUIRY_MODE_NUM_RESPONSES(pkb->data);
      /* calling inquiry routine to start inquiry */
      usched_pi_parameters_init(&(ulc->scheduler_info), btc_dev_info_p->inquiry_params.inquiry_length, INQUIRY_ROLE);
      status = BT_SUCCESS_STATUS;
    } else {
      status = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
    }
  } else {
    status = COMMAND_DISALLOWED;
  }
  if (status == BT_SUCCESS_STATUS) {
    // Allocate memory for the global buffer
    allocate_resp_addr_list(btc_dev_info_p);
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), PERIODIC_INQUIRY_MODE_EVENT_PARAM_LENGTH);
  SET_CE_PERIODIC_INQUIRY_MODE_STATUS(cmd_complete_event->data, status);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_exit_periodic_inquiry_mode_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  uint8_t status;
  if (btc_dev_info_p->inquiry_params.periodic_inq_enable == PERIODIC_INQUIRY_ENABLED) {
    //"TO-DO"
    usched_pi_stop_handler(NULL, &ulc->scheduler_info.inquiry_role_cb.task);
    status = BT_SUCCESS_STATUS;
  } else {
    status = COMMAND_DISALLOWED;
  }
  if (status == BT_SUCCESS_STATUS) {
    free_resp_addr_list(btc_dev_info_p);
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), EXIT_PERIODIC_INQUIRY_MODE_EVENT_PARAM_LENGTH);
  SET_CE_EXIT_PERIODIC_INQUIRY_MODE_STATUS(cmd_complete_event->data, status);
  btc_dev_info_p->inquiry_params.periodic_inq_enable = PERIODIC_INQUIRY_DISABLED;
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_create_connection_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status;
  status = bt_allow_conn_req_from_host(ulc);
  if (status == BT_SUCCESS_STATUS) {
    conn_params_t conn_params;
    memcpy(conn_params.bd_addr, GET_CREATE_CONNECTION_BD_ADDR(pkb->data), 6);
    conn_params.pkt_type = GET_CREATE_CONNECTION_PKT_TYPE(pkb->data);
    conn_params.page_scan_rep_mode = GET_CREATE_CONNECTION_PAGE_SCAN_REP_MODE(pkb->data);
    conn_params.reserved = GET_CREATE_CONNECTION_RESERVED(pkb->data);
    conn_params.clk_offset = GET_CREATE_CONNECTION_CLOCK_OFFSET(pkb->data);
    conn_params.allow_role_switch = GET_CREATE_CONNECTION_ALLOW_ROLE_SWITCH(pkb->data);
    /* Calling create connection routine in LC */
    status = bt_create_conn(ulc, &conn_params, pkb);
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_disconnect_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status;
  uint16_t conn_handle = GET_DISCONNECT_CONNECTION_HANDLE(pkb->data);
  uint8_t reason = GET_DISCONNECT_REASON(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id < INVALID_PEER_ID) {
    status = bt_disconnect(ulc, peer_id, reason);
  } else {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_create_connection_cancel_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_CREATE_CONNECTION_CANCEL_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  /* calling create_conn cancel routine */
  status = bt_create_connection_cancel(ulc, bd_addr);
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), CREATE_CONNECTION_CANCEL_EVENT_PARAM_LENGTH);
  SET_CE_CREATE_CONNECTION_CANCEL_STATUS(cmd_complete_event->data, status);
  SET_CE_CREATE_CONNECTION_CANCEL_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_accept_connection_request_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_ACCEPT_CONNECTION_REQUEST_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), BT_SUCCESS_STATUS);
  send_hci_response_packet(cmd_status_event);
  dm_accept_connection_from_hci(ulc, bd_addr, GET_ACCEPT_CONNECTION_REQUEST_ROLE(pkb->data));
}

void hci_cmd_reject_connection_request_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), REJECT_CONNECTION_REQUEST_EVENT_PARAM_LENGTH);
  SET_SE_REJECT_CONNECTION_REQUEST_STATUS(cmd_status_event->data, BT_SUCCESS_STATUS);
  send_hci_response_packet(cmd_status_event);
  dm_reject_connection_from_hci(ulc, GET_REJECT_CONNECTION_REQUEST_BD_ADDR(pkb->data), GET_REJECT_CONNECTION_REQUEST_REASON(pkb->data)); // TO DO
}

void hci_cmd_link_key_request_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t status = BT_SUCCESS_STATUS;
  memcpy(bd_addr, GET_LINK_KEY_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), LINK_KEY_REQUEST_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_LINK_KEY_REQUEST_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_LINK_KEY_REQUEST_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    memcpy(acl_peer_info->acl_enc_info.link_key, GET_LINK_KEY_REQUEST_REPLY_LINK_KEY(pkb->data), LINK_KEY_LEN);
    lmp_link_key_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_link_key_request_negative_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t status = BT_SUCCESS_STATUS;
  memcpy(bd_addr, GET_LINK_KEY_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), LINK_KEY_REQUEST_NEGATIVE_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_LINK_KEY_REQUEST_NEGATIVE_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_LINK_KEY_REQUEST_NEGATIVE_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_link_key_neg_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_pin_code_request_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_PIN_CODE_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), PIN_CODE_REQUEST_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_PIN_CODE_REQUEST_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_PIN_CODE_REQUEST_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_peer_info->acl_enc_info.pin_length = GET_PIN_CODE_REQUEST_REPLY_PIN_CODE_LENGTH(pkb->data);
    memcpy(acl_peer_info->acl_enc_info.pin, GET_PIN_CODE_REQUEST_REPLY_PIN_CODE(pkb->data), acl_peer_info->acl_enc_info.pin_length);
    lmp_pin_code_req_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_pin_code_request_negative_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t status = BT_SUCCESS_STATUS;
  memcpy(bd_addr, GET_PIN_CODE_REQUEST_NEGATIVE_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), PIN_CODE_REQUEST_NEGATIVE_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_PIN_CODE_REQUEST_NEGATIVE_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_PIN_CODE_REQUEST_NEGATIVE_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_pin_code_req_neg_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_change_connection_packet_type_handler(void *ctx, pkb_t *pkb)
{
  uint16_t sco_handle = 1;
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_CHANGE_CONNECTION_PACKET_TYPE_CONNECTION_HANDLE(pkb->data);
  uint16_t pkt_type = GET_CHANGE_CONNECTION_PACKET_TYPE_PKT_TYPE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    if (conn_handle & SCO_LT) {
      btc_dev_info_p->acl_peer_info[peer_id]->acl_link_mgmt_info.check_pending_req_from_hci |=
        (SYNC_CONN_REQ | CHANGE_PKT_TYPE);
      /* Change the SCO packet type */
      bt_change_sco_conn(ulc, peer_id, sco_handle, pkt_type);
    } else {
      btc_dev_info_p->acl_peer_info[peer_id]->acl_link_mgmt_info.device_status_flags |= UPDATE_PACKET_TYPE;
      btc_dev_info_p->acl_peer_info[peer_id]->acl_link_mgmt_info.check_pending_req_from_hci |= CHANGE_PKT_TYPE;
      lmp_change_conn_pkt_type(ulc, peer_id, pkt_type);
    }
  }
}

void hci_cmd_authentication_requested_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_AUTHENTICATION_REQUESTED_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
  if (status == BT_SUCCESS_STATUS) {
    lmp_auth_req_from_hci(ulc, peer_id);
  }
}

void hci_cmd_set_connection_encryption_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  acl_link_mgmt_info_t *acl_link_mgmt_info = NULL;
  acl_peer_info_t *acl_peer_info = NULL;
  uint16_t conn_handle = GET_SET_CONNECTION_ENCRYPTION_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (!(btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.features_bitmap[0] & LMP_ENCRYPT)) {
    status = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
  } else if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  /* Filling the return parameters */
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), SET_CONNECTION_ENCRYPTION_EVENT_PARAM_LENGTH);
  SET_SE_SET_CONNECTION_ENCRYPTION_STATUS(cmd_status_event->data, status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    acl_link_mgmt_info = &(acl_peer_info->acl_link_mgmt_info);

    if (GET_SET_CONNECTION_ENCRYPTION_ENCRYPTION_ENABLE(pkb->data) == 0x01) { // 0x01 for enable
      if (!(acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED)) {
        lmp_start_enc_from_hci(ulc, peer_id);
      }
    } else {
      if (acl_link_mgmt_info->device_status_flags & DEVICE_ENC_ENABLED) {
        lmp_stop_enc_from_hci(ulc, peer_id);
      }
    }
  }
}

void hci_cmd_change_connection_link_key_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status;
  uint16_t conn_handle = GET_CHANGE_CONNECTION_LINK_KEY_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    status = lmp_change_conn_link_key_from_hci(ulc, peer_id); // TODO
  }
  /* Filling the return parameters */
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), CHANGE_CONNECTION_LINK_KEY_EVENT_PARAM_LENGTH);
  SET_SE_CHANGE_CONNECTION_LINK_KEY_STATUS(cmd_status_event->data, status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_remote_name_request_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t page_scan_rep_mode;
  uint16_t clk_offset;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t status = bt_allow_conn_req_from_host(ulc);

  if (status == BT_STATUS_SUCCESS) {
    memcpy(bd_addr, GET_REMOTE_NAME_REQUEST_BD_ADDR(pkb->data), MAC_ADDR_LEN);
    page_scan_rep_mode = GET_REMOTE_NAME_REQUEST_PAGE_SCAN_REP_MODE(pkb->data);
    clk_offset = GET_REMOTE_NAME_REQUEST_CLK_OFFSET(pkb->data);
    status = dm_remote_dev_name_req_from_hci(ulc, bd_addr, page_scan_rep_mode, clk_offset);
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_remote_name_request_cancel_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t status;
  memcpy(bd_addr, GET_REMOTE_NAME_REQUEST_CANCEL_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  status = bt_remote_name_req_cancel(ulc, bd_addr); 
  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), REMOTE_NAME_REQUEST_CANCEL_EVENT_PARAM_LENGTH);
  SET_CE_REMOTE_NAME_REQUEST_CANCEL_STATUS(cmd_complete_event->data, status);
  SET_CE_REMOTE_NAME_REQUEST_CANCEL_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);
}

void hci_cmd_read_remote_supported_features_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_READ_REMOTE_SUPPORTED_FEATURES_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_remote_dev_features_req_from_hci(ulc, peer_id);
  }
}

void hci_cmd_read_remote_extended_features_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_READ_REMOTE_EXTENDED_FEATURES_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);
  uint8_t page_num = GET_READ_REMOTE_EXTENDED_FEATURES_PAGE_NUMBER(pkb->data);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_remote_ext_features_req(ulc, peer_id, page_num);
  }
}

void hci_cmd_read_remote_version_information_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_READ_REMOTE_VERSION_INFORMATION_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }
  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    /* calling Read remote version information Routine in LMP */
    lmp_read_remote_version_req_from_hci(ulc, peer_id);
  }
}

void hci_cmd_read_clock_offset_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status = BT_SUCCESS_STATUS;
  uint16_t conn_handle = GET_READ_CLOCK_OFFSET_CONNECTION_HANDLE(pkb->data);
  uint8_t peer_id = bt_conn_handle_to_peer_id(ulc, conn_handle);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), status);
  send_hci_response_packet(cmd_status_event);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);
    if (acl_dev_mgmt_info->mode == BT_PERIPHERAL_MODE) {
      lmp_read_clk_offset_req_from_hci(ulc, peer_id);
    } else {
      hci_read_clk_offset_complete(ulc, peer_id, BT_SUCCESS_STATUS);
    }
  }
}

void hci_cmd_setup_synchronous_connection_handler(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  (void)pkb;
  // ulc_t *ulc = (ulc_t *)ctx;
  // uint16_t hciOpcode = GET_HCI_CMD_OPCODE(pkb->data);
  // uint8_t status = BT_SUCCESS_STATUS;

  // pkb_t *cmd_status_event = prepare_hci_command_status_event(hciOpcode,SETUP_SYNCHRONOUS_CONNECTION_EVENT_PARAM_LENGTH);
  // SET_CE_SETUP_SYNCHRONOUS_CONNECTION_STATUS(cmd_status_event->data,status);
  // send_hci_response_packet(cmd_status_event);
}

void hci_cmd_accept_synchronous_connection_request_handler(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  // ulc_t *ulc = (ulc_t *)ctx;
  // btc_dev_info_t *bt_dev_info =  &(ulc->btc_dev_info); //TODO
  uint8_t status = BT_SUCCESS_STATUS;

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_EVENT_PARAM_LENGTH);
  SET_SE_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_STATUS(cmd_status_event->data, status);
  send_hci_response_packet(cmd_status_event);
}
void hci_cmd_reject_synchronous_connection_request_handler(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  uint8_t status = BT_SUCCESS_STATUS;

  pkb_t *cmd_status_event = prepare_hci_command_status_event(GET_HCI_CMD_OPCODE(pkb->data), REJECT_SYNCHRONOUS_CONNECTION_REQUEST_EVENT_PARAM_LENGTH);
  SET_SE_REJECT_SYNCHRONOUS_CONNECTION_REQUEST_STATUS(cmd_status_event->data, status);
  send_hci_response_packet(cmd_status_event);
}

void hci_cmd_io_capability_request_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t status;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_IO_CAPABILITY_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);
  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  } else {
    bt_dev_info->btc_dev_mgmt_info.io_capabilities = GET_IO_CAPABILITY_REQUEST_REPLY_IO_CAPABILITY(pkb->data);
    bt_dev_info->btc_dev_mgmt_info.oob_authentication_data = GET_IO_CAPABILITY_REQUEST_REPLY_OOB_DATA_PRESENT(pkb->data);
    bt_dev_info->btc_dev_mgmt_info.authentication_requirement = GET_IO_CAPABILITY_REQUEST_REPLY_AUTHENTICATION_REQUIREMENTS(pkb->data);
    status = BT_SUCCESS_STATUS;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), IO_CAPABILITY_REQUEST_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_IO_CAPABILITY_REQUEST_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_IO_CAPABILITY_REQUEST_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_io_cap_req_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_user_confirmation_request_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_USER_CONFIRMATION_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);
  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), USER_CONFIRMATION_REQUEST_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_USER_CONFIRMATION_REQUEST_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_USER_CONFIRMATION_REQUEST_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_user_confirm_req_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_user_confirmation_request_negative_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_user_confirm_req_neg_reply_recvd_from_hci(ulc, peer_id);
  }
}

void hci_cmd_user_passkey_request_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_USER_PASSKEY_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), USER_PASSKEY_REQUEST_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_USER_PASSKEY_REQUEST_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_USER_PASSKEY_REQUEST_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    acl_peer_info->acl_enc_info.passkey_display = *(GET_USER_PASSKEY_REQUEST_REPLY_NUMERIC_VALUE(pkb->data));
    lmp_user_passkey_req_reply_rcvd(ulc, peer_id);
  }
}

void hci_cmd_user_passkey_request_negative_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), USER_PASSKEY_REQUEST_NEGATIVE_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_user_passkey_req_neg_reply_rcvd(ulc, peer_id);
  }
}

void hci_cmd_remote_oob_data_request_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = NULL;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_REMOTE_OOB_DATA_REQUEST_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), REMOTE_OOB_DATA_REQUEST_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_REMOTE_OOB_DATA_REQUEST_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_REMOTE_OOB_DATA_REQUEST_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
    memcpy(acl_peer_info->acl_enc_info.r, GET_REMOTE_OOB_DATA_REQUEST_REPLY_R(pkb->data), SIZE_OF_R);
    lmp_remote_oob_data_req_reply_rcvd(ulc, peer_id, GET_REMOTE_OOB_DATA_REQUEST_REPLY_C(pkb->data));
  }
}

void hci_cmd_remote_oob_data_request_negative_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);

  if (status == BT_SUCCESS_STATUS) {
    lmp_remote_oob_data_req_neg_reply_rcvd(ulc, peer_id);
  }
}

void hci_cmd_io_capability_request_negative_reply_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t status = BT_SUCCESS_STATUS;
  uint8_t bd_addr[MAC_ADDR_LEN];
  memcpy(bd_addr, GET_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_BD_ADDR(pkb->data), MAC_ADDR_LEN);
  uint8_t peer_id = bt_mac_addr_to_peer_id(ulc, bd_addr);

  if (peer_id == INVALID_PEER_ID) {
    status = UNKNOWN_CONNECTION_IDENTIFIER;
  }

  pkb_t *cmd_complete_event = prepare_hci_command_complete_event(GET_HCI_CMD_OPCODE(pkb->data), IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_EVENT_PARAM_LENGTH);
  SET_CE_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_STATUS(cmd_complete_event->data, status);
  SET_CE_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_BD_ADDR(cmd_complete_event->data, bd_addr);
  send_hci_response_packet(cmd_complete_event);
  if (status == BT_SUCCESS_STATUS) {
    lmp_io_cap_neg_reply_rcvd_from_hci(ulc, peer_id, GET_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_REASON(pkb->data));
  }
}
