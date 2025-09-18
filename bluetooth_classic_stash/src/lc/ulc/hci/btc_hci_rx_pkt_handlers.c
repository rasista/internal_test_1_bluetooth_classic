#include "btc_hci_rx_pkt_handlers.h"
#include "btc_hci_events.h"
#include "btc_process_hci_cmd.h"
#include "btc_lmp.h"
#include "lmp.h"
#include "usched_vendor_specific.h"
#include "btc_lmp_power_control.h"
#include "btc_lmp_test_mode.h"
#include "btc_dm_sniff.h"

/**
 * @brief Handles the completion of a page connection from low-power mode.
 *
 * This function identifies the peer device using its Bluetooth address and
 * starts a connection if the device is in peripheral mode. It also updates
 * the connection status to "already connected."
 *
 * @param ctx Pointer to the ULC context.
 * @param bd_addr Pointer to the Bluetooth address of the peer device.
 */
void page_conn_done_from_lpw(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *bt_dev_info = &(ulc->btc_dev_info);
  uint8_t peer_id = GET_PEER_ID(pkb);

  // Validate parameters before proceeding
  if (peer_id >= MAX_NUM_ACL_CONNS) {
    BTC_PRINT("ERROR: Invalid peer_id %d >= MAX_NUM_ACL_CONNS %d\n", peer_id, MAX_NUM_ACL_CONNS);
    return;
  }
  
  acl_peer_info_t *acl_peer_info = bt_dev_info->acl_peer_info[peer_id];
  acl_peer_info->acl_dev_mgmt_info.mode = BT_PERIPHERAL_MODE;
  lmp_start_connection(ulc, peer_id);
  acl_peer_info->acl_dev_mgmt_info.connection_status = ALREADY_CONNECTED;
  /* This below change is for running test cases independent on PAGE  Needs to remove while LPW integration*/
#ifdef SCHED_SIM
  acl_peer_info->acl_link_mgmt_info.acl_pkt_type = ALL_ACL_DATA_PKTS_VALID;
#endif
}

void inquiry_completion_indication(void *ctx)
{
  ulc_t *ulc = (ulc_t *)ctx;
  btc_dev_info_t *btc_dev_info_p = &(ulc->btc_dev_info);
  free_resp_addr_list(btc_dev_info_p);
  if (btc_dev_info_p->inquiry_params.inq_enable == INQUIRY_ENABLED ) {
    btc_dev_info_p->inquiry_params.inq_enable = INQUIRY_DISABLED;
  } else if (btc_dev_info_p->inquiry_params.periodic_inq_enable != PERIODIC_INQUIRY_ENABLED) {
    return;
  }
  hci_inquiry_complete_indication(ctx, INQ_COMPLETE_EVENT, BT_SUCCESS_STATUS);
}

/**
 * @brief Handles the completion of a page scan connection from low-power mode.
 *
 * This function updates the Bluetooth address of the peer device in the
 * device information structure.
 *
 * @param ctx Pointer to the ULC context.
 * @param bd_addr Pointer to the Bluetooth address of the peer device.
 */
void page_scan_conn_done_from_lpw(void *ctx, pkb_t *pkb)
{
  (void) ctx;
  btc_dev_info_t *bt_dev_info = &(ulc.btc_dev_info);
  uint8_t *bd_addr = &GET_LPW_RX_PKT_BD_ADDR(pkb);
  uint8_t peer_id = GET_PEER_ID(pkb);
  
  // Validate parameters before proceeding
  if (peer_id >= MAX_NUM_ACL_CONNS) {
    BTC_PRINT("ERROR: Invalid peer_id %d >= MAX_NUM_ACL_CONNS %d\n", peer_id, MAX_NUM_ACL_CONNS);
    return;
  }

  memcpy(bt_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.bd_addr, bd_addr, MAC_ADDR_LEN);
  bt_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.mode = BT_CENTRAL_MODE;
  bt_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.conn_id_occupied = 1;
  bt_dev_info->acl_peer_info[peer_id]->acl_dev_mgmt_info.connection_status = ALREADY_CONNECTED;
  /* This below change is for running test cases independent on PAGE  Needs to remove while LPW integration*/
#ifdef SCHED_SIM
  bt_dev_info->acl_peer_info[peer_id]->acl_link_mgmt_info.acl_pkt_type = ALL_ACL_DATA_PKTS_VALID;
#endif
}

void acl_conn_done_from_lpw(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t peer_id = GET_PEER_ID(pkb);
  dm_acl_conn_indication_from_lm(ulc, peer_id, BT_STATUS_SUCCESS);
}

void btc_process_rx_fhs_packet_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  uint8_t *fhs_pkt;
  uint8_t *stored_addr;
  uint8_t num_resp = 0;
  uint8_t peer_id;
  hci_inq_event_params_t hci_inq_event_params;
  inquiry_params_t *inquiry_params = &ulc_ptr->btc_dev_info.inquiry_params;
  inquiry_params->inquiry_mode = ulc_ptr->btc_dev_info.inquiry_params.inquiry_mode;
  fhs_pkt = &pkb->data[FHS_PKT_START_OFFSET];
  uint32_t LAP = RX_FHS_PKT_LAP(fhs_pkt);
  uint8_t uap = RX_FHS_PKT_UAP(fhs_pkt);
  uint16_t nap = RX_FHS_PKT_NAP(fhs_pkt);
  peer_id = GET_PEER_ID(pkb);

  hci_inq_event_params.psr_mode = RX_FHS_PKT_PSR(fhs_pkt);

  memcpy(hci_inq_event_params.bd_addr, &LAP, BT_LAP_LEN);
  memcpy(hci_inq_event_params.bd_addr + 3, &uap, 1);
  memcpy(hci_inq_event_params.bd_addr + 4, &nap, 2);

  hci_inq_event_params.class_of_device[0]             =  RX_FHS_PKT_CLASS_OF_DEVICE_0(fhs_pkt);
  hci_inq_event_params.class_of_device[1]             =  RX_FHS_PKT_CLASS_OF_DEVICE_1(fhs_pkt);
  hci_inq_event_params.class_of_device[2]             =  RX_FHS_PKT_CLASS_OF_DEVICE_2(fhs_pkt);
  ulc_ptr->btc_dev_info.acl_peer_info[peer_id]->acl_link_mgmt_info.acl_pkt_type = ALL_ACL_DATA_PKTS_VALID;
  if (inquiry_params->resp_addr_list) {
    /* Checking for the redundancy of the response */
    stored_addr = inquiry_params->resp_addr_list;
    num_resp    = inquiry_params->num_resp_cmpltd;
    while (num_resp) {
      if (memcmp(stored_addr, hci_inq_event_params.bd_addr, BD_ADDR_LEN) == ADDRESSES_ARE_EQUAL) {
        break;
      }
      num_resp--;
      stored_addr += BD_ADDR_LEN;
    }
    if (!num_resp) {
      if (inquiry_params->num_resp_cmpltd < MAX_INQUIRY_RESPONSES) {
        memcpy(stored_addr, hci_inq_event_params.bd_addr, BD_ADDR_LEN);
        inquiry_params->num_resp_cmpltd++;
      } else {
        // Address Buffer is full, overwrite the last address
        stored_addr = inquiry_params->resp_addr_list + (MAX_INQUIRY_RESPONSES - 1) * BD_ADDR_LEN;
        memcpy(stored_addr, hci_inq_event_params.bd_addr, BD_ADDR_LEN);
      }
    }
  }
  if (!((inquiry_params->resp_addr_list) && (num_resp))) {
    if (inquiry_params->inquiry_mode == INQUIRY_RESULT_FORMAT) {
      hci_inquiry_result(ulc_ptr, &hci_inq_event_params);
    } else if (inquiry_params->inquiry_mode == EXTND_INQUIRY_RESULT_FORMAT) {
      hci_inq_event_params.eir_len = RX_FHS_PKT_EIR_LENGTH; // ToDo
      //hci_inq_event_params.eir_buf = (uint8_t *)&(RX_FHS_PKT_EIR_BUFFER(fhs_pkt)); //ToDo
      hci_inq_event_params.eir_buf = (uint8_t *)&hci_inq_event_params; //ToDo , proper path needs to be set
      hci_extended_inquiry_result(ulc_ptr, &hci_inq_event_params);
    } else if ((inquiry_params->inquiry_mode == INQUIRY_RESULT_WITH_RSSI_FORMAT) || (!(RX_FHS_PKT_EIR(fhs_pkt) & FHS_EIR_BIT))) {
      hci_inq_event_params.rssi = RX_FHS_PKT_RSSI; // ToDo
      hci_inquiry_result_with_rssi(ulc_ptr, &hci_inq_event_params);
    }
  }
}

/**
 * @brief Processes received LMP (Link Manager Protocol) packets.
 *
 * This function decodes the LMP packet, identifies the peer device, and
 * invokes the appropriate handler based on the transaction ID.
 *
 * @param ctx Pointer to the ULC context.
 * @param pkb Pointer to the received packet buffer.
 */
void btc_handle_received_lmp_packet(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  call_lmp_rx_handler(ulc_ptr, pkb);
}


/**
 * @brief Handles received ACL data packets from the Bluetooth link.
 *
 * This function is responsible for processing ACL data packets received over
 * the Bluetooth link. Currently, it frees the packet buffer as the data path
 * implementation is pending. In the future, this function will be enhanced to:
 * - Process ACL data packets
 * - Handle data routing to upper layers
 *
 * @param ctx Pointer to the ULC context (currently unused).
 * @param pkb Pointer to the received packet buffer, which is freed after processing.
 */
void btc_handle_received_data_packet(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  (void)pkb;
  // This function is a placeholder for handling received data packets.
  // It currently does not perform any operations but is intended to be
  // implemented in the future to process data packets received over the
  // Bluetooth link. 
  
  /* TODO: will be updated along with data path */
  scatter_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]);  // Using scatter_free as used elsewhere in the code
}


#ifdef SCHED_SIM
/**
 * @brief Triggers the test input for RSSI averaging.
 *
 * This function processes the received packet to extract RSSI values and
 * simulates the reception of these values by calling a callback function simulating RSSI updated data packets.
 * It also handles the case where enhanced power control is disabled.
 *
 * @param ctx Pointer to the ULC context.
 * @param pkb Pointer to the received packet buffer containing RSSI data.
 */
void trigger_test_input_rssi_averaging(void *ctx, pkb_t *pkb)
{
  uint8_t peer_id = GET_PEER_ID(pkb);
  uint8_t pkb_data[8] = {0};
  pkb_data[2] = peer_id;
  uint8_t last_rssi_index = GET_LPW_RX_RSSI_SAMPLE_COUNT(pkb) + GET_LPW_RX_RSSI_INPUT_START_INDEX;
  for (int i = GET_LPW_RX_RSSI_INPUT_START_INDEX; i < last_rssi_index; i++) {
    VENDOR_UPDATE_TEST_PACKET_RSSI(pkb_data, pkb->data[i]);
    VENDOR_UPDATE_TEST_PACKET_PACKET_TYPE(pkb_data, BT_DM5_PKT_TYPE);
    /* Now triggering the callback to simulate the RSSI updated data packets */
    receive_done_callback(pkb_data, sizeof(pkb_data));
  }
}
#endif

void btc_process_rx_internal_event_packet_handler(void *ctx, pkb_t *pkb)
{
  uint8_t rx_pkt_start =  GET_LPW_RX_NOTIFICATION_TYPE(pkb);
  switch (rx_pkt_start) {
    case PAGE_SUCCESS_NOTIFICATION_FROM_LPW:
      page_conn_done_from_lpw(ctx, pkb);
      break;
    case PAGE_SCAN_SUCCESS_NOTIFICATION_FROM_LPW:
      page_scan_conn_done_from_lpw(ctx, pkb);
      break;
    case INQUIRY_COMPLETE_INDICATION_FROM_LPW:
      inquiry_completion_indication(ctx);
      break;
    case CONNECTION_COMPLETE_NOTIFICATION_FROM_LPW:
      acl_conn_done_from_lpw(ctx, pkb);
      break;
    case POWER_CONTROL_RSSI_FROM_LPW:
#ifdef SCHED_SIM
      trigger_test_input_rssi_averaging(ctx, pkb);
#endif
      break;
    case DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW:
      ulc_t *ulc = (ulc_t *)ctx;
      uint8_t peer_id = GET_PEER_ID(pkb);
      btc_dev_info_t *btc_dev_info = &(ulc->btc_dev_info);
      acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
      acl_dev_mgmt_info_t *acl_dev_mgmt_info = &acl_peer_info->acl_dev_mgmt_info;
      acl_dev_mgmt_info->connection_status = NOT_CONNECTED;
      bt_reset_peer_info(ulc, peer_id);
      acl_dev_mgmt_info->conn_id_occupied = 0;
      break;
    case SNIFF_MODE_TRANSITION_COMPLETE:
      sniff_role_change_from_lpw(ctx, pkb);
      break;
    case SNIFF_SUBR_PARAMS_SET:
      sniff_subrating_params_update_from_lpw(ctx, pkb);
      break;
    default:
      break;
  }
}

void btc_process_rx_on_air_event_packet_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  uint8_t rx_pkt_type  =  GET_PKT_TYPE(pkb);
  uint8_t peer_id = GET_PEER_ID(pkb);
  int8_t rssi = GET_RSSI(pkb);
  switch (rx_pkt_type) {
    case BT_FHS_PKT_TYPE:
      btc_process_rx_fhs_packet_handler(ctx, pkb);
      break;
    case BT_DM1_PKT_TYPE:
      uint8_t llid_val = GET_LLID(pkb);
      if (llid_val == ACLC_LLID) {
        btc_handle_received_lmp_packet(ctx, pkb);
      }
      break;
    case BT_DM5_PKT_TYPE:
    lmp_power_control_manager(ulc_ptr, peer_id, rssi);
    btc_handle_received_data_packet(ctx, pkb);
    break;
    default:
      break;
  }
}

typedef void (*lpw_event_type_handler_t)(void *ctx, pkb_t *pkb);
lpw_event_type_handler_t lpw_event_type_handler_lut[] = {
  btc_process_rx_internal_event_packet_handler,
  btc_process_rx_on_air_event_packet_handler,
  btc_process_test_mode_packets,
};

void lpw_rx_done_handler(void *ctx)
{
  ulc_t *ulc_ptr = (ulc_t *)ctx;
  queue_t *lpw_rx_done_q = &ulc_ptr->lpw_rx_done_q;
  pkb_t *pkb = NULL;
  uint8_t lpw_event_type;
  lpw_event_type_handler_t handler = NULL;
  while (lpw_rx_done_q->size > 0) {
    pkb = (pkb_t *)dequeue(lpw_rx_done_q);
    lpw_event_type = GET_LPW_EVENT_TYPE(ulc_ptr, pkb);
    handler = lpw_event_type_handler_lut[lpw_event_type];
    if (handler != NULL) {
      handler(ctx, pkb);
    }
    scatter_free(pkb, &btc.pkb_pool[pkb->pkb_chunk_type]);
  }
  btc_event_loop_clear_event(&ulc_ptr->event_loop, BTC_EVENT_LPW_RX_DONE);
}
