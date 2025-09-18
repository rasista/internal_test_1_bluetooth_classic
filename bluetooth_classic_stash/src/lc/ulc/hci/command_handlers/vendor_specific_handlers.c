#include "hci.h"
#include "btc.h"
#include "btc_hci.h"
#include "hci_cmd_set_defines_complete_event_autogen.h"
#include "ulc.h"
#include "usched_vendor_specific.h"
#include "btc_hci_rx_pkt_handlers.h"
#include "hci_cmd_get_defines_autogen.h"

/* Function to handle vendor specific HCI commands that loops back the data
 */
void hci_cmd_vendor_echo_handler(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  pkb_t *pkb_event = NULL;
  uint8_t *response_params = NULL;
  uint8_t *echo_data = GET_HCI_OCF_VENDOR_ECHO_PARAMS(pkb->data);
  uint8_t echo_data_len = GET_HCI_CMD_PARAM_LEN(pkb->data);
  BTC_PRINT("echo_data_len %d", echo_data_len);
  if (echo_data[0] == 0xff) {
    BTC_ASSERT(ADHOC_ABORT);
  }
  pkb_event = prepare_hci_command_status_event((uint16_t)GET_HCI_CMD_OPCODE(pkb->data), 0x00);
  enqueue(&ulc.hci_event_pending_q, &pkb_event->queue_node);
  pkb_event = prepare_hci_command_complete_event(
    (uint16_t)GET_HCI_CMD_OPCODE(pkb->data), ECHO_EVENT_PARAM_LENGTH);
  response_params = GET_HCI_COMMAND_COMPLETE_RESPONE_PARAMS(pkb_event->data);
  SET_HCI_CMD_PARAM_LEN(pkb_event->data, echo_data_len);
  memcpy(response_params, echo_data, echo_data_len);
  send_hci_response_packet(pkb_event);
}

#ifdef SCHED_SIM
void virtual_disconnection_complete_handler(void *ctx, uint8_t status)
{
  pkb_t *recvd_data_pkt   = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  recvd_data_pkt->data[0] = HCI_PACKET_TYPE_EVENT;
  recvd_data_pkt->data[1] = HCI_VIRTUAL_DISCONNECTION_COMPLETE_EVENT;
  recvd_data_pkt->data[2] = 0x02; // length
  recvd_data_pkt->data[3] = 0x00; // status
  recvd_data_pkt->data[4] = 0x00; // handle
  recvd_data_pkt->pkt_len = 0x05;
  send_hci_response_packet(recvd_data_pkt);
}

void virtual_connection_complete_handler(void *ctx,uint8_t status)
{
  pkb_t *recvd_data_pkt   = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  recvd_data_pkt->data[0] = HCI_PACKET_TYPE_EVENT;
  recvd_data_pkt->data[1] = HCI_VIRTUAL_CONN_COMPLETE_EVENT;
  recvd_data_pkt->data[2] = 0x02; // length
  recvd_data_pkt->data[3] = 0x00; // status
  recvd_data_pkt->data[4] = 0x00; // handle
  recvd_data_pkt->pkt_len = 0x05;
  send_hci_response_packet(recvd_data_pkt);
}

/* Function to handle vendor specific HCI commands that starts page scan
 */
void hci_cmd_vendor_virtual_listen_connection_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  pkb_t *pkb_event = NULL;
  //! get the port number from the command on whihch vendor page scan is to be
  //! started
  uint16_t vendor_page_scan_port =
    ((uint16_t)(GET_HCI_OCF_VENDOR_PAGE_SCAN_PORT_LOW(pkb->data) << 8)
     | GET_HCI_OCF_VENDOR_PAGE_SCAN_PORT_HIGH(pkb->data));

  //! prepare command complete event
  pkb_event = prepare_hci_command_complete_event((uint16_t)GET_HCI_CMD_OPCODE(pkb->data), VIRTUAL_LISTEN_CONNECTION_EVENT_PARAM_LENGTH);
  BTC_HEX_DUMP("Vendor listen connection - Command complete event packet",
               pkb_event->data, 6);
  send_hci_response_packet(pkb_event);
  ulc->cmd_complete_handler = virtual_connection_complete_handler;
  //! call usched API to start page scan
  usched_api_start_vendor_page_scan(vendor_page_scan_port);
}

/* Function to handle vendor specific HCI commands that cancels page scan
 */
void hci_cmd_vendor_virtual_cancel_listen_handler(void *ctx, pkb_t *pkb)
{
  (void)pkb;
  ulc_t *ulc = (ulc_t *)ctx;
  ulc->cmd_complete_handler = virtual_disconnection_complete_handler;
  //! call usched API to cancel page scan
  usched_api_cancel_vendor_page_scan();
}

/* Function to handle vendor specific HCI commands that starts page on a
   specific IP and port The IP and port are passed as parameters in the HCI
   Command
 */
void hci_cmd_vendor_virtual_connect_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  pkb_t *pkb_event = NULL;
  //! get the IP and port number from the command on whihch vendor page is to be
  //! started
  uint8_t vendor_page_ip_0 = GET_HCI_OCF_VENDOR_PAGE_IP_0(pkb->data);
  uint8_t vendor_page_ip_1 = GET_HCI_OCF_VENDOR_PAGE_IP_1(pkb->data);
  uint8_t vendor_page_ip_2 = GET_HCI_OCF_VENDOR_PAGE_IP_2(pkb->data);
  uint8_t vendor_page_ip_3 = GET_HCI_OCF_VENDOR_PAGE_IP_3(pkb->data);
  uint16_t vendor_page_port =
    (uint16_t)((GET_HCI_OCF_VENDOR_PAGE_PORT_LOW(pkb->data) << 8)
               | (GET_HCI_OCF_VENDOR_PAGE_PORT_HIGH(pkb->data)));
  BTC_PRINT("%s, %d.%d.%d.%d:%d\n", __FUNCTION__, vendor_page_ip_0,
            vendor_page_ip_1, vendor_page_ip_2, vendor_page_ip_3,
            vendor_page_port);
  //! prepare command complete event
  pkb_event = prepare_hci_command_complete_event(
    (uint16_t)GET_HCI_CMD_OPCODE(pkb->data),
    VIRTUAL_CONNECT_EVENT_PARAM_LENGTH);
  BTC_HEX_DUMP("Vendor connect - Command complete event packet",
               pkb_event->data, 6);
  send_hci_response_packet(pkb_event);

  ulc->cmd_complete_handler = virtual_connection_complete_handler;
  //! call usched API to start page
  usched_api_start_vendor_page(vendor_page_ip_0, vendor_page_ip_1,
                               vendor_page_ip_2, vendor_page_ip_3,
                               vendor_page_port);
}

/* Function to handle vendor specific HCI commands that cancels page. IP and
 * port are not required for this command
 */
void hci_cmd_vendor_virtual_cancel_connect_handler(void *ctx, pkb_t *pkb)
{
  (void)pkb;
  ulc_t *ulc = (ulc_t *)ctx;
  ulc->cmd_complete_handler = virtual_disconnection_complete_handler;
  usched_api_cancel_vendor_page();
}

void hci_cmd_vendor_kill_firmware_handler(void *ctx, pkb_t *pkb)
{
  (void)pkb;
  (void)ctx;
  BTC_PRINT("Killing Firmware \n");
  killFirmware();
}

/* Function to handle vendor specific HCI commands that sets local LMP features.
   The local features are passed as parameters in the HCI command and copied to
   the device's features bitmap
 */
void hci_cmd_vendor_set_local_lmp_features_handler(void *ctx, pkb_t *pkb)
{
  ulc_t *ulc = (ulc_t *)ctx;
  uint8_t *local_features_from_vendor_cmd = GET_SET_LOCAL_LMP_FEATURES_LOCAL_FEATURES(pkb->data);
  uint8_t *local_features_on_device = &ulc->btc_dev_info.btc_dev_mgmt_info.features_bitmap[0];
  memcpy(local_features_on_device, local_features_from_vendor_cmd, FEATURE_MASK_LENGTH);

  pkb_t *pkb_event = NULL;
  //! prepare command complete event
  pkb_event = prepare_hci_command_complete_event(
    (uint16_t)GET_HCI_CMD_OPCODE(pkb->data), SET_LOCAL_LMP_FEATURES_EVENT_PARAM_LENGTH);
  send_hci_response_packet(pkb_event);
}
#endif

void hci_cmd_vendor_internal_event_trigger_from_lpw_handler(void *ctx, pkb_t *pkb)
{
  (void)ctx;
  pkb_t *internal_pkb = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  memcpy(internal_pkb->data, pkb->data + 4, VENDOR_COMMAND_INTERNAL_PKT_LEN(pkb));
  internal_pkb->pkt_len = VENDOR_COMMAND_INTERNAL_PKT_LEN(pkb);

  enqueue(&ulc.lpw_rx_done_q, &internal_pkb->queue_node);
  btc_event_loop_set_event(&ulc.event_loop, BTC_EVENT_LPW_RX_DONE);
  btc_plt_remove_from_pend(&ulc.event_loop_pend);

#ifdef SCHED_SIM
  if (GET_LPW_RX_NOTIFICATION_TYPE(internal_pkb) == POWER_CONTROL_RSSI_FROM_LPW) {
    pkb_t *pkb_event = NULL;
    //! prepare command complete event
    pkb_event = prepare_hci_command_complete_event(
      (uint16_t)(GET_HCI_CMD_OPCODE(pkb->data)),
      VIRTUAL_CANCEL_CONNECT_EVENT_PARAM_LENGTH);
    send_hci_response_packet(pkb_event);
  }
#endif
}
