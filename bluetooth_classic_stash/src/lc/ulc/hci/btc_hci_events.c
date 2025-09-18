#include "btc_dev.h"
#include "btc_hci.h"
#include "hci.h"
#include "btc.h"
#include "btc_hci_events.h"
#include "hci_cmd_get_defines_autogen.h"
#include "hci_handlers_autogen.h"
#include "hci_cmd_set_defines_status_event_autogen.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "hci_events_ptl_autogen.h"
#include "ulc.h"
#include "btc_process_hci_cmd.h"
#include "btc_lmp.h"
#include "hci_events_set_defines_autogen.h"

void hci_inquiry_complete_indication(ulc_t *ulc, uint8_t event_map_mask, int8_t status)
{
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  if (event_map_mask & INQ_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_INQUIRY_COMPLETE_EVENT, SET_PTL_OF_INQUIRY_COMPLETE_EVENT);
    /* Filling the Event Parameters */
    SET_INQUIRY_COMPLETE_STATUS(pkb->data, status);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  free_resp_addr_list(btc_dev_info_p);
  return;
}

void hci_inquiry_result(ulc_t *ulc, hci_inq_event_params_t *hci_inq_event_params)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  if (btc_dev_info->btc_hci_params.event_map_mask[0] & INQ_RESULT_EVENT) {
    /* Memory allocation for Event packet */

    pkb_t *pkb = prepare_hci_event(HCI_INQUIRY_RESULT_EVENT, SET_PTL_OF_INQUIRY_RESULT_EVENT);

    SET_INQUIRY_RESULT_NUM_RESP(pkb->data, 1);
    SET_INQUIRY_RESULT_BD_ADDR(pkb->data, hci_inq_event_params->bd_addr);

    SET_INQUIRY_RESULT_PAGE_SCAN_REP_MODE(pkb->data, hci_inq_event_params->psr_mode);
    SET_INQUIRY_RESULT_RESERVED(pkb->data, 0);

    SET_INQUIRY_RESULT_CLASS_OF_DEVICE(pkb->data, hci_inq_event_params->class_of_device);
    SET_INQUIRY_RESULT_CLOCK_OFFSET(pkb->data, hci_inq_event_params->clock_offset);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }

  return;
}

void hci_conn_request_indication(ulc_t *ulc, uint8_t peer_id, uint8_t link_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[0] & CONN_REQ_EVENT) {
    /* Memory allocation for Event packet */

    pkb_t *pkb = prepare_hci_event(HCI_CONN_REQUEST_EVENT, SET_PTL_OF_CONN_REQUEST_EVENT);

    SET_CONN_REQUEST_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);
    SET_CONN_REQUEST_CLASS_OF_DEVICE(pkb->data, acl_peer_info->acl_dev_mgmt_info.class_of_dev);
    SET_CONN_REQUEST_LINK_TYPE(pkb->data, link_type);
    send_hci_response_packet(pkb);
  } else {
    acl_peer_info->acl_link_mgmt_info.received_req_flag |= ACL_CONNECTION_REQ;
    /* FIXME Make this function as function pointer since this can go to the ROM */
    /*Irrespective auto_accept flag we are telling role as master*/
    lmp_accept_connection_req_from_dm(ulc, peer_id, 0);
  }

  return;
}

void hci_auth_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  if (btc_dev_info->btc_hci_params.event_map_mask[0] & AUTH_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */

    pkb_t *pkb = prepare_hci_event(HCI_AUTHENTICATION_COMPLETE_EVENT, SET_PTL_OF_AUTHENTICATION_COMPLETE_EVENT);

    SET_AUTHENTICATION_COMPLETE_STATUS(pkb->data, status);
    SET_AUTHENTICATION_COMPLETE_CONN_HANDLE(pkb->data, btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.conn_handle);
    send_hci_response_packet(pkb);
  }
}

void hci_disconn_complete_indication(ulc_t *ulc, uint16_t conn_handle, uint8_t status, uint8_t reason)
{
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  if (btc_dev_info_p->btc_hci_params.event_map_mask[0] & DISCONN_COMPLETE_EVENT) {
    pkb_t *pkb = prepare_hci_event(HCI_DISCONNECTION_COMPLETE_EVENT, SET_PTL_OF_DISCONNECTION_COMPLETE_EVENT);
    /* Filling the Event Parameters */
    SET_DISCONNECTION_COMPLETE_STATUS(pkb->data, status);
    SET_DISCONNECTION_COMPLETE_CONN_HANDLE(pkb->data, conn_handle);
    SET_DISCONNECTION_COMPLETE_REASON(pkb->data, reason);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
}

void hci_remote_dev_name_recvd_indication(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[0] & REMOTE_NAME_REQ_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT, SET_PTL_OF_REMOTE_NAME_REQUEST_COMPLETE_EVENT);
    SET_REMOTE_NAME_REQUEST_COMPLETE_STATUS(pkb->data, status);
    SET_REMOTE_NAME_REQUEST_COMPLETE_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);
    SET_REMOTE_NAME_REQUEST_COMPLETE_REMOTE_NAME(pkb->data, acl_peer_info->acl_dev_mgmt_info.device_name);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_change_conn_link_key_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  if (btc_dev_info->btc_hci_params.event_map_mask[1] & CHANGE_CONN_LINK_KEY_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT, SET_PTL_OF_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT);

    SET_CHANGE_CONNECTION_LINK_KEY_COMPLETE_STATUS(pkb->data, status);
    SET_CHANGE_CONNECTION_LINK_KEY_COMPLETE_CONN_HANDLE(pkb->data, btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.conn_handle);

    send_hci_response_packet(pkb);
  }

  return;
}

void hci_read_remote_dev_vers_info_complete(ulc_t *ulc, uint16_t conn_handle, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);

  if (btc_dev_info->btc_hci_params.event_map_mask[1] & READ_REMOTE_VERSION_INFO_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */

    pkb_t *pkb = prepare_hci_event(HCI_READ_REMOTE_VERS_INFO_EVENT, SET_PTL_OF_READ_REMOTE_VERSION_INFO_COMPLETE_EVENT);
    SET_READ_REMOTE_VERSION_INFO_COMPLETE_STATUS(pkb->data, status);
    SET_READ_REMOTE_VERSION_INFO_COMPLETE_CONN_HANDLE(pkb->data, conn_handle);
    SET_READ_REMOTE_VERSION_INFO_COMPLETE_VERSION(pkb->data, acl_dev_mgmt_info->vers_number);
    SET_READ_REMOTE_VERSION_INFO_COMPLETE_COMPANY_IDENTIFIER(pkb->data, acl_dev_mgmt_info->comp_id);
    SET_READ_REMOTE_VERSION_INFO_COMPLETE_SUB_VERSION(pkb->data, acl_dev_mgmt_info->sub_vers_number);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_remote_dev_features_recvd(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[1] & READ_REMOTE_SUPPORTED_FEAURES_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */

    pkb_t *pkb = prepare_hci_event(HCI_READ_REMOTE_SUPP_FTRS_EVENT, SET_PTL_OF_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT);
    SET_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_STATUS(pkb->data, status);
    SET_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_CONN_HANDLE(pkb->data, acl_peer_info->acl_dev_mgmt_info.conn_handle);
    SET_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_LMP_FEATURES(pkb->data, acl_peer_info->acl_dev_mgmt_info.features_bitmap);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }

  return;
}

void hci_qos_setup_complete_indication(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[1] & QOS_SETUP_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_QOS_SETUP_COMPLETE_EVENT, SET_PTL_OF_QOS_SETUP_COMPLETE_EVENT);

    SET_QOS_SETUP_COMPLETE_STATUS(pkb->data, status);
    SET_QOS_SETUP_COMPLETE_CONN_HANDLE(pkb->data, acl_peer_info->acl_dev_mgmt_info.conn_handle);
    SET_QOS_SETUP_COMPLETE_UNUSED(pkb->data, 0x0);
    SET_QOS_SETUP_COMPLETE_SERVICE_TYPE(pkb->data, acl_peer_info->acl_dev_mgmt_info.service_type);
    SET_QOS_SETUP_COMPLETE_TOKEN_RATE(pkb->data, 0);
    SET_QOS_SETUP_COMPLETE_PEAK_BANDWIDTH(pkb->data, 0);
    SET_QOS_SETUP_COMPLETE_LATENCY(pkb->data, acl_peer_info->acl_dev_mgmt_info.latency);
    SET_QOS_SETUP_COMPLETE_DELAY_VARIATION(pkb->data, 0xFFFFFFFF);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }

  return;
}

void hci_return_link_keys_event(ulc_t *ulc, uint8_t no_of_link_keys, uint8_t *bd_addrs)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  uint16_t rx_pkt_len = 0;
  uint8_t ii;

  if (btc_dev_info->btc_hci_params.event_map_mask[2] & RETURN_LINK_KEYS_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
    pkb->data[rx_pkt_len] = HCI_PACKET_TYPE_EVENT;
    rx_pkt_len++;
    pkb->data[rx_pkt_len] = HCI_RETURN_LINK_KEYS_EVENT;
    rx_pkt_len += 2;
    /* Filling the Event Parameters */
    pkb->data[rx_pkt_len] = no_of_link_keys;
    rx_pkt_len += 1;
    for (ii = 0; ii < no_of_link_keys; ii++) {
      memcpy(&pkb->data[rx_pkt_len], (bd_addrs + (ii * MAC_ADDR_LEN)), MAC_ADDR_LEN);
      rx_pkt_len += MAC_ADDR_LEN;
      memset(&pkb->data[rx_pkt_len], 0, LINK_KEY_LEN);
      rx_pkt_len += LINK_KEY_LEN;
    }
    pkb->data[2] = rx_pkt_len - 3;

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_pin_code_req(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  if (btc_dev_info->btc_hci_params.event_map_mask[2] & PIN_CODE_REQUEST_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_PIN_CODE_REQUEST_EVENT, SET_PTL_OF_PIN_CODE_REQUEST_EVENT);

    /* Filling the Event Parameters */
    SET_PIN_CODE_REQUEST_BD_ADDR(pkb->data, btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.bd_addr);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }

  return;
}

void hci_link_key_req(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  if (btc_dev_info->btc_hci_params.event_map_mask[2] & LINK_KEY_REQ_EVENT) {
    /* Memory allocation for Event packet */

    pkb_t *pkb = prepare_hci_event(HCI_LINK_KEY_REQUEST_EVENT, SET_PTL_OF_LINK_KEY_REQUEST_EVENT);

    SET_LINK_KEY_REQUEST_BD_ADDR(pkb->data, btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.bd_addr);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_link_key_notification(ulc_t *ulc, uint8_t peer_id, uint8_t link_key_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  if (btc_dev_info->btc_hci_params.event_map_mask[2] & LINK_KEY_NOTIFICATION_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_LINK_KEY_NOTIFICATION_EVENT, SET_PTL_OF_LINK_KEY_NOTIFICATION_EVENT);

    /* Filling the Event Parameters */
    SET_LINK_KEY_NOTIFICATION_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);
    SET_LINK_KEY_NOTIFICATION_LINK_KEY(pkb->data, acl_peer_info->acl_enc_info.auth_link_key);
    SET_LINK_KEY_NOTIFICATION_KEY_TYPE(pkb->data, link_key_type);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_max_slots_changed_indication(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;

  if (btc_dev_info->btc_hci_params.event_map_mask[3] & MAX_SLOT_CHANGE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_MAX_SLOTS_CHANGE_EVENT, SET_PTL_OF_MAX_SLOTS_CHANGE_EVENT);
    /* Filling the Event Parameters */
    SET_MAX_SLOTS_CHANGE_CONN_HANDLE(pkb->data, acl_dev_mgmt_info->conn_handle);
    SET_MAX_SLOTS_CHANGE_LMPMAX_SLOTS(pkb->data, acl_dev_mgmt_info->tx_max_slots);

    btc_dev_info->btc_dev_mgmt_info.max_acl_pkt_len =
      lc_cal_acl_max_payload_len(acl_peer_info->acl_link_mgmt_info.data_rate_type, acl_peer_info->acl_link_mgmt_info.acl_pkt_type, acl_dev_mgmt_info->tx_max_slots);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_read_clk_offset_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;

  if (btc_dev_info->btc_hci_params.event_map_mask[3] & READ_CLK_OFFSET_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT, SET_PTL_OF_READ_CLOCK_OFFSET_COMPLETE_EVENT);
    SET_READ_CLOCK_OFFSET_COMPLETE_STATUS(pkb->data, status);
    SET_READ_CLOCK_OFFSET_COMPLETE_CONN_HANDLE(pkb->data, acl_dev_mgmt_info->conn_handle);
    SET_READ_CLOCK_OFFSET_COMPLETE_CLOCK_OFFSET(pkb->data, acl_dev_mgmt_info->clk_offset);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_inquiry_result_with_rssi(ulc_t *ulc, hci_inq_event_params_t *hci_inq_event_params)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  if (btc_dev_info->btc_hci_params.event_map_mask[4] & INQ_RESULT_WITH_RSSI_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_INQUIRY_RESULT_WITH_RSSI_EVENT, SET_PTL_OF_INQUIRY_RESULT_RSSI_EVENT);

    /* Filling the Event Parameters */

    /* For now assuming only one response will be sent at a time */
    SET_INQUIRY_RESULT_RSSI_NUM_RESP(pkb->data, 1);
    /* MAC_ADDR Updation */
    SET_INQUIRY_RESULT_RSSI_BD_ADDR(pkb->data, hci_inq_event_params->bd_addr);

    /* Page_Scan_Repitition_Mode Updation */
    SET_INQUIRY_RESULT_RSSI_PAGE_SCAN_REPITION_MODE(pkb->data, hci_inq_event_params->psr_mode);

    /* One byte is reserved */
    SET_INQUIRY_RESULT_RSSI_RESERVED(pkb->data, 0);

    /* Class of Device */
    SET_INQUIRY_RESULT_RSSI_CLASS_OF_DEVICE(pkb->data, hci_inq_event_params->class_of_device);

    /* clock_offset */
    SET_INQUIRY_RESULT_RSSI_CLOCK_OFFSET(pkb->data, hci_inq_event_params->clock_offset);

    /* rssi */
    SET_INQUIRY_RESULT_RSSI_RSSI(pkb->data, hci_inq_event_params->rssi);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_read_remote_ext_features_complete(ulc_t *ulc, uint8_t status, uint8_t peer_id, uint8_t page_num)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;

  if (btc_dev_info->btc_hci_params.event_map_mask[4] & READ_REMOTE_EXT_FEATURES_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_READ_REM_EXTD_FEATURES_COMPLETE_EVENT, SET_PTL_OF_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT);

    SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_STATUS(pkb->data, status);
    SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_CONN_HANDLE(pkb->data, acl_dev_mgmt_info->conn_handle);
    SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_PG_NUM(pkb->data, page_num);
    SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_MAX_PAGE_NUM(pkb->data, acl_dev_mgmt_info->max_page_num);

    if (page_num == 0) {
      SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EXTENDED_LMP_FEATURES(pkb->data, acl_dev_mgmt_info->features_bitmap);
    } else if (page_num == 1) {
      SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EXTENDED_LMP_FEATURES(pkb->data, acl_dev_mgmt_info->ext_features_bitmap);
    } else {
      memset(&pkb->data[8], 0, EXT_FEATURE_MASK_LENGTH);
    }

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_sniff_subrating_event(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  sniff_params_t *sniff_params = &(btc_dev_info->acl_peer_info[peer_id]->sniff_params);
  uint8_t sniff_subrate;
  uint16_t sniff_subrating_tout;

  if (btc_dev_info->btc_hci_params.event_map_mask[5] & SNIFF_SUBRATING_EVENT) {
    if (sniff_params->min_local_sniff_subrating_tout > sniff_params->subrating_tout_rcvd) {
      sniff_subrating_tout = sniff_params->min_local_sniff_subrating_tout;
    } else {
      sniff_subrating_tout = sniff_params->subrating_tout_rcvd;
    }
    if (sniff_params->max_remote_sniff_subrate < sniff_params->max_local_sniff_subrate) {
      sniff_subrate = (sniff_params->max_remote_sniff_subrate * (sniff_params->max_local_sniff_subrate / sniff_params->max_remote_sniff_subrate)); /* Floor of local_subrate/remote_subrate */
    } else {
      sniff_subrate = sniff_params->max_local_sniff_subrate;
    }
    pkb_t *pkb = prepare_hci_event(HCI_SNIFF_SUBRATING_EVENT, SET_PTL_OF_SNIFF_SUBRATING_EVENT);
    SET_SNIFF_SUBRATING_STATUS(pkb->data, status);
    SET_SNIFF_SUBRATING_CONN_HANDLE(pkb->data, btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.conn_handle);
    SET_SNIFF_SUBRATING_MAX_TXLATENCY(pkb->data, (sniff_params->max_remote_sniff_subrate * sniff_params->t_sniff));
    SET_SNIFF_SUBRATING_MAX_RX_LATENCY(pkb->data, (sniff_subrate * sniff_params->t_sniff));
    SET_SNIFF_SUBRATING_MIN_REMOTE_TIMEOUT(pkb->data, sniff_params->min_remote_sniff_subrating_tout);
    SET_SNIFF_SUBRATING_MIN_LOCAL_TIMEOUT(pkb->data, sniff_subrating_tout);
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_extended_inquiry_result(ulc_t *ulc, hci_inq_event_params_t *hci_inq_event_params)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  uint16_t len;

  if (btc_dev_info->btc_hci_params.event_map_mask[5] & EXT_INQ_RESULT_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_EXTENDED_INQUIRY_RESULT_EVENT, SET_PTL_OF_EXTENDED_INQUIRY_RESULT_EVENT);

    /* Filling the Event Parameters */

    /* For now assuming only one response will be sent at a time */
    SET_EXTENDED_INQUIRY_RESULT_NUM_RESP(pkb->data, 1);
    /* MAC_ADDR Updation */
    SET_EXTENDED_INQUIRY_RESULT_BD_ADDR(pkb->data, hci_inq_event_params->bd_addr);

    /* Page_Scan_Repitition_Mode Updation */
    SET_EXTENDED_INQUIRY_RESULT_PAGE_SCAN_REPITION_MODE(pkb->data, hci_inq_event_params->psr_mode);

    /* One byte is reserved */
    SET_EXTENDED_INQUIRY_RESULT_RESERVED(pkb->data, 0);

    /* Class of Device */
    SET_EXTENDED_INQUIRY_RESULT_CLASS_OF_DEVICE(pkb->data, hci_inq_event_params->class_of_device);

    /* clock_offset */
    SET_EXTENDED_INQUIRY_RESULT_CLOCK_OFFSET(pkb->data, hci_inq_event_params->clock_offset);

    /* RSSI */
    SET_EXTENDED_INQUIRY_RESULT_RSSI(pkb->data, hci_inq_event_params->rssi);

    /* EIR packet payload */
    memcpy(&pkb->data[18], hci_inq_event_params->eir_buf, MIN(hci_inq_event_params->eir_len, SIZE_OF_EIR));

    len = SIZE_OF_EIR - hci_inq_event_params->eir_len;

    if (len > 0) {
      memset(&pkb->data[18 + hci_inq_event_params->eir_len], 0, len);
    }

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_io_cap_req_event(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[6] & IO_CAPABILITY_REQ_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_IO_CAPABILITY_REQUEST_EVENT, SET_PTL_OF_IO_CAPABILITY_REQ_EVENT);

    /* Filling the Event Parameters */

    SET_IO_CAPABILITY_REQ_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_io_cap_response_event(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  acl_dev_mgmt_info_t acl_dev_mgmt_info;

  if (btc_dev_info->btc_hci_params.event_map_mask[6] & IO_CAPABILITY_REQ_REPLY_EVENT) {
    acl_dev_mgmt_info = btc_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info;
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_IO_CAPABILITY_RESPONSE_EVENT, SET_PTL_OF_IO_CAPABILITY_RESP_EVENT);

    /* Filling the Event Parameters */
    SET_IO_CAPABILITY_RESP_BD_ADDR(pkb->data, acl_dev_mgmt_info.bd_addr);

    SET_IO_CAPABILITY_RESP_IO_CAPABILITY(pkb->data, acl_dev_mgmt_info.io_capabilities);
    SET_IO_CAPABILITY_RESP_OOB_DATA_PRESENT(pkb->data, acl_dev_mgmt_info.oob_authentication_data);
    SET_IO_CAPABILITY_RESP_AUTHENTICATION_REQUIREMENTS(pkb->data, acl_dev_mgmt_info.authentication_requirement);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_user_confirmation_req_event(ulc_t *ulc, uint8_t peer_id, uint32_t confirmation_val)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[6] & USER_CONFIRM_REQ_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_USER_CONFIRMATION_REQUEST_EVENT, SET_PTL_OF_USER_CONFIRMATION_REQUEST_EVENT);

    /* Filling the Event Parameters */
    SET_USER_CONFIRMATION_REQUEST_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);
    SET_USER_CONFIRMATION_REQUEST_NUMERIC_VALUE(pkb->data, confirmation_val);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_user_passkey_request(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[6] & USER_PASSKEY_REQ_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_USER_PASSKEY_REQUEST_EVENT, SET_PTL_OF_USER_PASSKEY_REQUEST_EVENT);
    /* Filling the Event Parameters */

    /* Device Address */
    SET_USER_PASSKEY_REQUEST_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_remote_oob_data_request(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[6] & REMOTE_OOB_DATA_REQ_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_REMOTE_OOB_DATA_REQUEST_EVENT, SET_PTL_OF_REMOTE_OOB_DATA_REQUEST_EVENT);

    /* Filling the Event Parameters */
    SET_REMOTE_OOB_DATA_REQUEST_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_simple_pairing_complete(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[6] & SIMPLE_PAIRING_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_SIMPLE_PAIRING_COMPLETE_EVENT, SET_PTL_OF_SIMPLE_PAIRING_COMPLETE_EVENT);

    /* Filling the Event Parameters */
    SET_SIMPLE_PAIRING_COMPLETE_STATUS(pkb->data, status);
    SET_SIMPLE_PAIRING_COMPLETE_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
    return;
  }
}

void hci_link_supervision_timeout(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  if (btc_dev_info->btc_hci_params.event_map_mask[6] & LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT, SET_PTL_OF_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT);

    /* Filling the Event Parameters */

    SET_LINK_SUPERVISION_TIMEOUT_CHANGED_CONN_HANDLE(pkb->data, acl_peer_info->acl_dev_mgmt_info.conn_handle);
    SET_LINK_SUPERVISION_TIMEOUT_CHANGED_LINK_SUPERVISION_TIMEOUT(pkb->data, acl_peer_info->acl_link_mgmt_info.link_supervision_timeout);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_user_passkey_notification(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[7] & USER_PASSKEY_NOTIFICATION_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_USER_PASSKEY_NOTIFICATION_EVENT, SET_PTL_OF_USER_PASSKEY_NOTIFICATION_EVENT);
    /* Filling the Event Parameters */
    /* Device Address*/
    SET_USER_PASSKEY_NOTIFICATION_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* Passkey to be displayed */
    SET_USER_PASSKEY_NOTIFICATION_PASSKEY(pkb->data, acl_peer_info->acl_enc_info.passkey_display);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_keypress_notification_event(ulc_t *ulc, uint8_t peer_id, uint8_t notification_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[7] & KEYPRESS_NOTIFICATION_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_KEYPRESS_NOTIFICATION_EVENT, SET_PTL_OF_KEYPRESS_NOTIFICATION_EVENT);
    /* Filling the Event Parameters */
    /* Device Address*/
    SET_KEYPRESS_NOTIFICATION_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);
    SET_KEYPRESS_NOTIFICATION_NOTIFICATION_TYPE(pkb->data, notification_type);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_remote_host_supp_features_notification_event(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  if (btc_dev_info->btc_hci_params.event_map_mask[7] & REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_REM_HOST_SUPP_FEATURES_NOTIFICATION_EVENT, SET_PTL_OF_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT);
    /* Filling the Event Parameters */
    /* Device Address*/
    SET_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);
    /* Filling the Ext. Host Supported Features */
    SET_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_HOST_SUPPORTED_FEATURES(pkb->data, acl_peer_info->acl_dev_mgmt_info.ext_features_bitmap);
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_role_change_event(ulc_t *ulc, uint8_t peer_id, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];

  uint8_t role;

  if (btc_dev_info->btc_hci_params.event_map_mask[2] & ROLE_CHANGE_EVENT) {
    if (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE) {
      role = BT_CENTRAL_MODE;
    } else {
      role = BT_PERIPHERAL_MODE;
    }
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_ROLE_CHANGE_EVENT, SET_PTL_OF_ROLE_CHANGE_EVENT);
    /* Filling the Event Parameters */
    /* Status */
    SET_ROLE_CHANGE_STATUS(pkb->data, status);

    /* Device Address*/
    SET_ROLE_CHANGE_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* Current role */
    SET_ROLE_CHANGE_NEW_ROLE(pkb->data, role);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_mode_change_event(ulc_t *ulc, uint16_t conn_handle, uint8_t status, uint8_t mode, uint16_t interval)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);

  if (btc_dev_info->btc_hci_params.event_map_mask[2] & MODE_CHANGE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_MODE_CHANGE_EVENT, SET_PTL_OF_MODE_CHANGE_EVENT);

    SET_MODE_CHANGE_STATUS(pkb->data, status);
    SET_MODE_CHANGE_CONN_HANDLE(pkb->data, conn_handle);
    SET_MODE_CHANGE_CURRENT_MODE(pkb->data, mode);
    SET_MODE_CHANGE_INTERVAL(pkb->data, interval);

    send_hci_response_packet(pkb);
  }
  return;
}

void hci_tx_pkts_flush_occurred_event(ulc_t *ulc, uint8_t peer_id, uint8_t event_type)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  pkb_t *pkb;

  if (((btc_dev_info->btc_hci_params.event_map_mask[2] & FLUSH_OCCURED_EVENT) && (event_type == HCI_FLUSH_OCCURRED_EVENT)) || ((btc_dev_info->btc_hci_params.event_map_mask[7] & ENHANCED_FLUSH_COMPLETE_EVENT) && (event_type == HCI_ENHANCED_FLUSH_COMPLETE_EVENT))) {
    if (event_type == HCI_FLUSH_OCCURRED_EVENT) {
      pkb = prepare_hci_event(HCI_FLUSH_OCCURRED_EVENT, SET_PTL_OF_FLUSH_OCCURRED_EVENT);
      SET_FLUSH_OCCURRED_HANDLE(pkb->data, acl_peer_info->acl_dev_mgmt_info.conn_handle);
    } else {
      pkb = prepare_hci_event(HCI_ENHANCED_FLUSH_COMPLETE_EVENT, SET_PTL_OF_ENHANCED_FLUSH_COMPLETE_EVENT);
      SET_ENHANCED_FLUSH_COMPLETE_HANDLE(pkb->data, acl_peer_info->acl_dev_mgmt_info.conn_handle);
    }

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_conn_complete_indication(ulc_t *ulc, uint8_t peer_id, uint8_t status, uint8_t conn_type)
{
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info_p->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t *acl_dev_mgmt_info = &(acl_peer_info->acl_dev_mgmt_info);

  sco_link_params_t *sco_link_params;
  sco_link_params = &acl_peer_info->sco_link_params[acl_dev_mgmt_info->sco_handle];

  /*
   * This Condition is required since the conn_handle_bitmap needs to assigned only
   * when the new connection handle is created.
   *
   */
  if ((conn_type == 1) && (status == BT_STATUS_SUCCESS)) {                     // BT_ACL = 1
    acl_dev_mgmt_info->conn_handle = bt_peer_id_to_conn_handle(ulc, peer_id); // QUERY
  }

  /* If the link type is SCO, connection handle must be referring to the SCO link */
  if (conn_type == 0) { // BT_SCO = 0
    sco_link_params->sco_conn_handle = BREDR_SCO_PEER_ID_TO_CONN_HANDLE(peer_id, acl_peer_info->sco_link_params->sco_conn_handle);
  }

  if (btc_dev_info_p->btc_hci_params.event_map_mask[0] & CONN_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    // pkt = bt_allocate_hci_rx_pkt(bt_common_info, &rx_pkt, MAX_HCI_EVENT_PKT_LENGTH);
    pkb_t *pkb = prepare_hci_event(HCI_CONN_COMPLETE_EVENT, SET_PTL_OF_CONN_COMPLETE_EVENT);

    /* Filling the Event Parameters */
    SET_CONN_COMPLETE_STATUS(pkb->data, status);

    if (conn_type == BT_ACL) {
      SET_CONN_COMPLETE_CONN_HANDLE(pkb->data, acl_peer_info->acl_dev_mgmt_info.conn_handle);
    } else {
      SET_CONN_COMPLETE_CONN_HANDLE(pkb->data, sco_link_params->sco_conn_handle);
    }

    SET_CONN_COMPLETE_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    SET_CONN_COMPLETE_LINK_TYPE(pkb->data, conn_type);

    if (acl_peer_info->acl_link_mgmt_info.device_status_flags & DEVICE_ENC_ENABLED) {
      SET_CONN_COMPLETE_ENCRYPTION_ENABLE(pkb->data, 1);
    } else {
      SET_CONN_COMPLETE_ENCRYPTION_ENABLE(pkb->data, 0);
    }

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_conn_pkt_type_changed_event(ulc_t *ulc, uint8_t peer_id, uint8_t link_type, uint8_t status)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  acl_dev_mgmt_info_t acl_dev_mgmt_info = acl_peer_info->acl_dev_mgmt_info;

  if (btc_dev_info->btc_hci_params.event_map_mask[3] & CONN_PKT_TYPE_CHANGED_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT, SET_PTL_OF_CONN_PKT_TYPE_CHANGED_EVENT);
    /* Filling the Event Parameters */

    SET_CONN_PKT_TYPE_CHANGED_STATUS(pkb->data, status);

    if (link_type == BT_SCO) {
      sco_link_params_t *sco_link_params = &acl_peer_info->sco_link_params[acl_peer_info->sco_link_params->sco_conn_handle];
      /* SCO connection handle */
      SET_CONN_PKT_TYPE_CHANGED_CONN_HANDLE(pkb->data, sco_link_params->sco_conn_handle);

      if (sco_link_params->sco_pkt_type == BT_HV1_PKT_TYPE) {
        SET_CONN_PKT_TYPE_CHANGED_PKT_TYPE(pkb->data, ADD_SCO_HV1);
      }
      if (sco_link_params->sco_pkt_type == BT_HV2_PKT_TYPE) {
        SET_CONN_PKT_TYPE_CHANGED_PKT_TYPE(pkb->data, ADD_SCO_HV2);
      }
      if (sco_link_params->sco_pkt_type == BT_HV3_PKT_TYPE) {
        SET_CONN_PKT_TYPE_CHANGED_PKT_TYPE(pkb->data, ADD_SCO_HV3);
      }
    } else {
      SET_CONN_PKT_TYPE_CHANGED_CONN_HANDLE(pkb->data, acl_dev_mgmt_info.conn_handle);

      SET_CONN_PKT_TYPE_CHANGED_PKT_TYPE(pkb->data, (acl_peer_info->acl_link_mgmt_info.acl_pkt_type | btc_dev_info->btc_dev_mgmt_info.pkt_mask_map));

      btc_dev_info->btc_dev_mgmt_info.max_acl_pkt_len = lc_cal_acl_max_payload_len(acl_peer_info->acl_link_mgmt_info.data_rate_type,
                                                                                   acl_peer_info->acl_link_mgmt_info.acl_pkt_type, acl_dev_mgmt_info.tx_max_slots);
    }
    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_sync_conn_complete(ulc_t *ulc, uint8_t peer_id, uint8_t link_type, uint8_t status)
{
  esco_link_params_t *esco_link_params = NULL;
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sco_link_params_t *sco_link_params = NULL;

  if (link_type == BT_ESCO) {
    esco_link_params = &acl_peer_info->esco_link_params[esco_link_params->esco_conn_handle];
    esco_link_params->esco_conn_handle =
      BREDR_ESCO_PEER_ID_TO_CONN_HANDLE(peer_id, esco_link_params->esco_conn_handle);
  } else {
    sco_link_params = &acl_peer_info->sco_link_params[sco_link_params->sco_conn_handle];
    sco_link_params->sco_conn_handle = BREDR_SCO_PEER_ID_TO_CONN_HANDLE(peer_id, sco_link_params->sco_conn_handle);
  }
  if (btc_dev_info->btc_hci_params.event_map_mask[5] & SYNC_CONN_COMPLETE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT, SET_PTL_OF_SYNC_CONN_COMPLETE_EVENT);

    /* Filling the Event Parameters */
    /* status */
    SET_SYNC_CONN_COMPLETE_STATUS(pkb->data, status);

    if (link_type == BT_ESCO) {
      /* eSCO connection handle */
      SET_SYNC_CONN_COMPLETE_CONN_HANDLE(pkb->data, esco_link_params->esco_conn_handle);
    } else {
      /* SCO connection handle */
      SET_SYNC_CONN_COMPLETE_CONN_HANDLE(pkb->data, sco_link_params->sco_conn_handle);
    }

    /* BD address */
    SET_SYNC_CONN_COMPLETE_BD_ADDR(pkb->data, acl_peer_info->acl_dev_mgmt_info.bd_addr);

    /* Link type */
    SET_SYNC_CONN_COMPLETE_LINK_TYPE(pkb->data, link_type);

    if (link_type == BT_SCO) {
      /* Transmission interval */
      SET_SYNC_CONN_COMPLETE_TRANSMISSION_INTERVAL(pkb->data, 0);
      /* Retransmission window */
      SET_SYNC_CONN_COMPLETE_RETRANSMISSIONWINDOW(pkb->data, 0);

      /* RX_packet_length */
      SET_SYNC_CONN_COMPLETE_RX_PKT_LENGTH(pkb->data, 0);

      /* TX_packet_length */
      SET_SYNC_CONN_COMPLETE_TX_PKT_LENGTH(pkb->data, 0);
      /* Air mode */
      SET_SYNC_CONN_COMPLETE_AIR_MODE(pkb->data, sco_link_params->air_mode);
    } else if (link_type == BT_ESCO) {
      /* Transmission interval */
      SET_SYNC_CONN_COMPLETE_TRANSMISSION_INTERVAL(pkb->data, esco_link_params->esco_interval);

      /* Retransmission window */
      SET_SYNC_CONN_COMPLETE_RETRANSMISSIONWINDOW(pkb->data, esco_link_params->esco_window);

      /* RX_packet_length */
      SET_SYNC_CONN_COMPLETE_RX_PKT_LENGTH(pkb->data, esco_link_params->central_to_peripheral_pkt_len); //! FIXME length here is wrong

      /* TX_packet_length */
      SET_SYNC_CONN_COMPLETE_TX_PKT_LENGTH(pkb->data, esco_link_params->peripheral_to_central_pkt_len); //! FIXME length is wrong

      /* Air mode */
      SET_SYNC_CONN_COMPLETE_AIR_MODE(pkb->data, esco_link_params->air_mode);
    }

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
  return;
}

void hci_sync_conn_changed(ulc_t *ulc, uint8_t peer_id, uint8_t link_type, uint8_t status)
{
  // TODO complete implementation can be done after discussing

  esco_link_params_t *esco_link_params = NULL;

  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sco_link_params_t *sco_link_params = NULL;
  uint16_t sync_handle = 0;

  if (link_type == BT_ESCO) {
    esco_link_params = &acl_peer_info->esco_link_params[esco_link_params->esco_conn_handle];
    esco_link_params->esco_conn_handle =
      BREDR_ESCO_PEER_ID_TO_CONN_HANDLE(peer_id, esco_link_params->esco_conn_handle);
    sync_handle = esco_link_params->esco_conn_handle;
  } else {
    sco_link_params = &acl_peer_info->sco_link_params[sco_link_params->sco_conn_handle];
    sco_link_params->sco_conn_handle = BREDR_SCO_PEER_ID_TO_CONN_HANDLE(peer_id, sco_link_params->sco_conn_handle);
    sync_handle = sco_link_params->sco_conn_handle;
  }

  if (btc_dev_info->btc_hci_params.event_map_mask[5] & SYNC_CONN_CHANGED_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT, SET_PTL_OF_SYNC_CONN_CHANGED_EVENT);

    /* Filling the Event Parameters */

    /* status */
    SET_SYNC_CONN_CHANGED_STATUS(pkb->data, status);

    /* SCO/eSCO connection handle */
    SET_SYNC_CONN_CHANGED_CONN_HANDLE(pkb->data, sync_handle);

    if (link_type == BT_SCO) {
      /* Transmission interval */
      SET_SYNC_CONN_CHANGED_TRANSMISSION_INTERVAL(pkb->data, sco_link_params->sco_interval);

      /* Retransmission window */
      SET_SYNC_CONN_CHANGED_RETRANSMISSIONWINDOW(pkb->data, 0);

      /* RX_packet_length */
      SET_SYNC_CONN_CHANGED_RX_PKT_LENGTH(pkb->data, 0);

      /* TX_packet_length */
      SET_SYNC_CONN_CHANGED_TX_PKT_LENGTH(pkb->data, 0);
    } else if (link_type == BT_ESCO) {
      /* Transmission interval */
      SET_SYNC_CONN_CHANGED_TRANSMISSION_INTERVAL(pkb->data, esco_link_params->esco_interval);

      /* Retransmission window */
      SET_SYNC_CONN_CHANGED_RETRANSMISSIONWINDOW(pkb->data, esco_link_params->esco_window);

      /* RX_packet_length */
      SET_SYNC_CONN_CHANGED_RX_PKT_LENGTH(pkb->data, esco_link_params->central_to_peripheral_pkt_len);

      /* TX_packet_length */
      SET_SYNC_CONN_CHANGED_TX_PKT_LENGTH(pkb->data, esco_link_params->peripheral_to_central_pkt_len);
    }

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
}

void hci_enc_changed_event(ulc_t *ulc, uint16_t conn_handle, uint8_t status, uint8_t enc_enabled)
{
  btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
  if (btc_dev_info->btc_hci_params.event_map_mask[0] & ENCRYPT_CHANGE_EVENT) {
    /* Memory allocation for Event packet */
    pkb_t *pkb = prepare_hci_event(HCI_ENCRYPTION_CHANGE_EVENT, SET_PTL_OF_ENCRYTPION_CHANGE_V1_EVENT);

    /* Filling the event parameters */
    SET_ENCRYTPION_CHANGE_V1_STATUS(pkb->data, status);

    SET_ENCRYTPION_CHANGE_V1_CONN_HANDLE(pkb->data, conn_handle);

    SET_ENCRYTPION_CHANGE_V1_ENCRYPTION_ENABLE(pkb->data, enc_enabled);

    /* queues command complete event packet to host */
    send_hci_response_packet(pkb);
  }
}

void hci_vendor_event(uint8_t *data)
{
  pkb_t *pkb = prepare_hci_event(HCI_VENDOR_SPECIFIC_EVENT, SET_PTL_OF_VENDOR_EVENT_EVENT);
  /* Filling the Event Parameters */
  SET_VENDOR_EVENT_SUBEVENT_CODE(pkb->data, data[0]);
  SET_VENDOR_EVENT_VENDOR_SPECIFIC_PARAMS(pkb->data, data[1]);

  send_hci_response_packet(pkb);
}
