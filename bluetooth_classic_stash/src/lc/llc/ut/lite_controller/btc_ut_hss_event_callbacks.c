#include "btc_ut_hss_event_callbacks.h"
#include "btc_riscvseq_address_map.h"
#include "host_to_lpw_interface.h" // for status codes

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

uint32_t rtt_event_buffer[RTT_EVENT_BUFFER_SIZE];
extern uint8_t lpw_hss_event_logger_enabled;
extern hss_cmd_init_btc_device_t init_btc_device;
hss_cmd_start_acl_central_t start_acl_central_page;
extern hss_cmd_set_device_parameters_acl_t set_acl_parameters;
hss_cmd_start_acl_peripheral_t start_acl_peripheral_page_scan;
hss_cmd_start_roleswitch_central_t start_roleswitch_central_page;
hss_cmd_set_roleswitch_parameters_t set_roleswitch_params_central;
hss_cmd_set_roleswitch_parameters_t set_roleswitch_params_peripheral;
hss_cmd_start_roleswitch_peripheral_t start_roleswitch_peripheral_page_scan;
// hss_event_btc_slot_offset_t hss_event_roleswitch_compute_offset;

// Shared memory
extern shared_mem_t queue;
extern RAIL_Handle_t btc_rail_handle;
extern uint8_t g_trigger_roleswitch;
uint8_t peer_role = BTC_CENTRAL; // Default role is central

acl_traffic_pattern_t acl_traffic_pattern;
extern uint32_t end_tsf_acl;
extern uint8_t connection_device_index_central;
extern uint8_t connection_device_index_peripheral;
uint16_t roleswitch_slot_offset; // Slot offset for roleswitch
uint8_t pkt_type;
uint8_t lt_addr;
static inline void log_lpw_hss_event(uint8_t *hss_event, uint32_t event_size);
static inline void log_lpw_hss_packet(uint8_t *hss_event, uint32_t event_size);
uint8_t g_instance_id = 0; // Global instance ID for ACL traffic pattern and roleswitch

void update_lt_addr(uint8_t* tx_pkt)
{
  tx_pkt[0] = tx_pkt[0] | (lt_addr);
}

void update_slot_offset(uint8_t* tx_pkt)
{
  tx_pkt[4] = roleswitch_slot_offset & 0x00FF;
  tx_pkt[5] = (roleswitch_slot_offset >> 8) & 0x00FF;
}

void hss_event_tx_pkt_ack_received_handler(uint8_t *hss_event, uint32_t event_size)
{
  log_lpw_hss_event(hss_event, event_size);
  uint8_t read_index = 0;
  uint8_t write_index = 0;
  uint8_t free_buffer_index = 0;
  uint32_t pkt_len = 0;
  linear_buffer_t *buffer_ptr = NULL;
  for (uint8_t i = 0; i < fifo_manager.active_instances; i++) {
    read_index = fifo_manager.fifo_instances[i].read_index;
    write_index = fifo_manager.fifo_instances[i].write_index;
    free_buffer_index = (read_index + HSS_LPWSS_FIFO_SIZE - 1) & HSS_LPWSS_FIFO_MASK;
    buffer_ptr = (linear_buffer_t *)fifo_manager.fifo_instances[i].data_buffer[free_buffer_index];
    pkt_len = buffer_ptr->length;
    while (free_buffer_index != (write_index & HSS_LPWSS_FIFO_MASK) && pkt_len != 0 ) {
      if (!verify_buffer_and_free(i, fifo_manager.fifo_instances[i].data_buffer[free_buffer_index])) {
        responsePrintError("hss_event_tx_pkt_ack_received_handler", 0xFF, "Failed to free data_buffer for instance %d", i);
        return;
      }
      free_buffer_index = (free_buffer_index + HSS_LPWSS_FIFO_SIZE - 1) & HSS_LPWSS_FIFO_MASK;
      if (fifo_manager.fifo_instances[i].data_buffer[free_buffer_index] == 0) {
        break;
      }
      buffer_ptr = (linear_buffer_t *)fifo_manager.fifo_instances[i].data_buffer[free_buffer_index];
      pkt_len = buffer_ptr->length;
    }
  }
}

void hss_event_inquiry_complete_handler(uint8_t *hss_event, uint32_t event_size)
{
  log_lpw_hss_event(hss_event, event_size);
}

void hss_event_inquiry_scan_complete_handler(uint8_t *hss_event, uint32_t event_size)
{
  log_lpw_hss_event(hss_event, event_size);
}

void hss_event_activity_complete_handler(uint8_t *hss_event, uint32_t event_size)
{
  uint8_t event_status = *HSS_EVENT_EVENT_STATUS(hss_event);
  log_lpw_hss_event(hss_event, event_size);
  (void)hss_event;
  if (g_trigger_roleswitch) {
    if (peer_role == BTC_CENTRAL) {
      start_roleswitch_central_procedure();
    } else {
      start_roleswitch_peripheral_procedure();
    }
  }
}

void hss_event_role_switch_complete_handler(uint8_t *hss_event, uint32_t event_size)
{
  log_lpw_hss_event(hss_event, event_size);
}

void hss_event_page_complete_handler(uint8_t *hss_event, uint32_t event_size)
{
  uint8_t event_status = *HSS_EVENT_EVENT_STATUS(hss_event);
  uint8_t connection_idx = *HSS_EVENT_PAYLOAD_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_event);
  log_lpw_hss_event(hss_event, event_size);
  if (event_status != BTC_CONNECTION_COMPLETED) {
    return;
  }
  uint32_t cmd = (uint32_t)&queue;

  // Update the packet length to inform the LPW that data is available for transmission
  lt_addr = set_page.lt_addr;

  // Set device parameters for ACL central
  set_acl_parameters.connection_device_index = *HSS_EVENT_PAYLOAD_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_event);
  set_acl_parameters.peer_role = BTC_CENTRAL;
  peer_role = BTC_CENTRAL;
  init_acl_tx_data_path();
  connection_device_index_central = *HSS_EVENT_PAYLOAD_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_event);

  // pkb_alloc, fill the macros and enqueue
  set_device_parameters_acl_cmd(&set_acl_parameters, (shared_mem_t *)&queue);

  RAIL_USER_SendMbox(btc_rail_handle, cmd);
  // Set parameters to start ACL central
  start_acl_central_page.connection_device_index = *HSS_EVENT_PAYLOAD_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_event);
  start_acl_central_page.priority = 1;
  start_acl_central_page.step_size = 1;
  start_acl_central_page.end_tsf = end_tsf_acl;
  memcpy(start_acl_central_page.bd_address, init_btc_device.bd_address, sizeof(init_btc_device.bd_address));
  start_acl_central_cmd(&start_acl_central_page, (shared_mem_t *)&queue);
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
}

void hss_event_page_scan_complete_handler(uint8_t *hss_event, uint32_t event_size)
{
  uint8_t event_status = *HSS_EVENT_EVENT_STATUS(hss_event);
  uint8_t connection_idx = *HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_event);
  log_lpw_hss_event(hss_event, event_size);
  if (event_status != BTC_CONNECTION_COMPLETED) {
    return;
  }

  uint32_t cmd = (uint32_t)&queue;
  set_acl_parameters.connection_device_index = *HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_event);
  set_acl_parameters.peer_role = BTC_PERIPHERAL;
  peer_role = BTC_PERIPHERAL;
  init_acl_tx_data_path();

  // Set device parameters for ACL peripheral

  connection_device_index_peripheral = *HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_event);

  // pkb_alloc, fill the macros and enqueue
  set_device_parameters_acl_cmd(&set_acl_parameters, (shared_mem_t *)&queue);
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
  // Set parameters to start ACL peripheral
  start_acl_peripheral_page_scan.connection_device_index = *HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_event);
  start_acl_peripheral_page_scan.priority = 1;
  start_acl_peripheral_page_scan.step_size = 1;
  start_acl_peripheral_page_scan.end_tsf = end_tsf_acl;

  // pkb_alloc, fill the macros and enqueue
  start_acl_peripheral_cmd(&start_acl_peripheral_page_scan, (shared_mem_t *)&queue);
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
}

void parse_fhs_pkt(uint8_t *fhs_ptr)
{
  uint32_t *fhs_pkt = (uint32_t *)fhs_ptr; // Cast to uint32_t*
  uint8_t lap[3];
  // Access individual bytes safely
  uint8_t *fhs_pkt_bytes = (uint8_t *)fhs_pkt;
  // Extract LAP values directly from individual bytes
  lap[0] = EXTRACT_LAP_ADDR_BYTE(fhs_pkt_bytes[4]);
  lap[1] = EXTRACT_LAP_ADDR_BYTE(fhs_pkt_bytes[5]);
  lap[2] = EXTRACT_LAP_ADDR_BYTE(fhs_pkt_bytes[6]);

  // Extract LT address
  lt_addr = fhs_pkt_bytes[14] & 0x07; // Mask to get the last 3 bits

  uint32_t cod = fhs_pkt_bytes[11] | (fhs_pkt_bytes[12] << 8) | (fhs_pkt_bytes[13] << 16);

  uint8_t major_service_class = (cod >> 13) & 0x7FF; // Extract bits 13-23
  uint8_t major_device_class = (cod >> 8) & 0x1F;    // Extract bits 8-12
  uint8_t minor_device_class = (cod >> 2) & 0x3F;    // Extract bits 2-7
  // Update the BD_ADDRESS of the central device to trigger ACL peripheral
  start_acl_peripheral_page_scan.bd_address[0] = lap[0];
  start_acl_peripheral_page_scan.bd_address[1] = lap[1];
  start_acl_peripheral_page_scan.bd_address[2] = lap[2];
  start_acl_peripheral_page_scan.bd_address[3] = fhs_pkt_bytes[8];
  start_acl_peripheral_page_scan.bd_address[4] = fhs_pkt_bytes[9];
  start_acl_peripheral_page_scan.bd_address[5] = fhs_pkt_bytes[10];

  (void)major_device_class;
  (void)minor_device_class;
  (void)major_service_class;
}

void parse_rx_pkt_handler(uint8_t *hss_event, uint32_t event_size)
{
  uint16_t *pkt_hdr = (uint16_t *)hss_event;
  uint8_t pkt_type = GET_RX_PKT_HEADER_TYPE(*pkt_hdr);
  if (pkt_type == BT_FHS_PKT_TYPE) {
    parse_fhs_pkt(hss_event + 3); // Skip the first 3 bytes (header)
  } else if (pkt_type == 0x04) {
    roleswitch_slot_offset = hss_event[5] | (hss_event[6] << 8);
  } else {
  }
  log_lpw_hss_packet(hss_event, event_size);
  // For all other packet types, do nothing - let the regular HSS event handlers handle them
}

/**
 * @brief Initialize the data path for the TX data path handler.
 *
 * This function initializes the data path for the TX data path handler by
 * getting a free FIFO instance ID and setting the global current FIFO instance ID.
 * It also initializes the data path with a specific buffer address.
 */
void init_acl_tx_data_path(void)
{
  // Get a free FIFO instance ID
  uint8_t instance_id = get_free_fifo_instance_id();
  g_instance_id = instance_id;
  // Use the global variable for buffer address (ACL as example)
  uint32_t acl_buffer_address = BTC_RISCVSEQ_G_TX_ACL_BUFFER_ADDRESS;
  // Call the new data path initialization function
  if (!init_data_path(instance_id, acl_buffer_address)) {
    responsePrintError("init_data_path", 0xA, "Could not initialize data path");
    return;
  }
  // Enqueue 3 data packets to the FIFO for initial testing
  uint8_t k[3] = { 0x04, 0x0B, 0x0F }; // DH1, DH3, DH5 for Basic Rate. 2DH1, 3DH3, 3DH5 for Enhanced Rate.
  uint16_t pkt_len[3] = { 17, 17, 200 }; // 17 bytes for DH1/2DH1, DH3/3DH3, 200 bytes for DH5/3DH5 for basic rate/enhanced rate.
  for (uint8_t i = 0; i < 3; i++) {
    linear_buffer_t* buffer_ptr = data_path_create_linear_buffer();
    if (buffer_ptr != NULL) {
      uint32_t len = form_data_pkt(k[i], pkt_len[i], buffer_ptr->data, set_acl_parameters.br_edr_mode);
      update_lt_addr(buffer_ptr->data);
      buffer_ptr->length = len;
      if (!enqueue_to_fifo(instance_id, buffer_ptr)) {
        responsePrintError("init_data_path", 0xFC, "Failed to enqueue to FIFO");
        return;
      }
    } else {
      responsePrintError("init_data_path", 0xFE, "Failed to create linear buffer");
      return;
    }
  }
}

void acl_traffic_pattern_cb(RAIL_Handle_t btc_rail_handle)
{
  uint32_t timestamp = (uint32_t)(PROTIMER_GetPTicks() >> TSF_SCALE_FACTOR);
  linear_buffer_t* buffer_ptr = data_path_create_linear_buffer();
  if (buffer_ptr != NULL) {
    uint32_t len = form_data_pkt((uint8_t)acl_traffic_pattern.packet_type, acl_traffic_pattern.payload_length, buffer_ptr->data, set_acl_parameters.br_edr_mode);
    update_lt_addr(buffer_ptr->data);
    buffer_ptr->length = len;
    if (!enqueue_to_fifo(g_instance_id, buffer_ptr)) {
      responsePrintError("acl_traffic_pattern_cb", 0xFC, "Failed to enqueue to FIFO");
      return;
    }
  } else {
    responsePrintError("acl_traffic_pattern_cb", 0xFE, "Failed to create linear buffer");
    return;
  }

  RAIL_SetTimer(btc_rail_handle, acl_traffic_pattern.packet_interval_tsf + timestamp, RAIL_TIME_ABSOLUTE, &acl_traffic_pattern_cb);
}

void lpw_hss_event_rtt_init(void)
{
  // Clear the RTT buffer before initialization
  memset(rtt_event_buffer, 0, sizeof(rtt_event_buffer));
  SEGGER_RTT_Init();
  SEGGER_RTT_ConfigUpBuffer(SEGGER_RTT_CHANNEL_ID, "BTC_UT_EVENT_LOG_BUFFER", rtt_event_buffer, sizeof(rtt_event_buffer), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
}

// Create properly aligned linear buffer for RTT logging
static inline void log_lpw_hss_event(uint8_t *hss_event, uint32_t event_size)
{
  if (!lpw_hss_event_logger_enabled || hss_event == NULL) {
    return;
  }
  // Get timestamp from PROTIMER , timestamp is in microseconds
  uint32_t timestamp = (uint32_t)(PROTIMER_GetPTicks() >> TSF_SCALE_FACTOR);
  uint8_t event_linear_buffer[RTT_EVENT_LINEAR_BUFFER_SIZE];

  // Create a linear buffer with proper byte alignment //TODO: this is not required once HSS_EVENT bug is fixed
  uint8_t buffer_index = 0;
  memset(event_linear_buffer, 0, sizeof(event_linear_buffer));

  // Regular HSS event handling
  // Extract event type and status using macros
  uint8_t event_type = *HSS_EVENT_EVENT_TYPE(hss_event);
  uint8_t event_status = *HSS_EVENT_EVENT_STATUS(hss_event);

  // Build linear buffer: [event_type][event_status][payload...][timestamp(4 bytes)]
  event_linear_buffer[buffer_index++] = event_type;
  event_linear_buffer[buffer_index++] = event_status;

  // Add payload based on event type
  if (event_type == 0x02) {  // page_complete
    uint8_t connection_idx = *HSS_EVENT_PAYLOAD_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_event);
    event_linear_buffer[buffer_index++] = connection_idx;
  } else if (event_type == 0x03) {  // page_scan_complete
    uint8_t connection_idx = *HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_event);
    uint8_t *remote_bd_addr = HSS_EVENT_PAYLOAD_PAGE_SCAN_COMPLETE_REMOTE_BD_ADDRESS(hss_event);
    event_linear_buffer[buffer_index++] = connection_idx;
    // Copy 6-byte BD address
    for (int i = 0; i < 6; i++) {
      event_linear_buffer[buffer_index++] = remote_bd_addr[i];
    }
  }
  // Data packets are now handled by log_lpw_hss_packet function
  // This function only handles regular HSS events
  // Add timestamp as last 4 bytes (little-endian format)
  event_linear_buffer[buffer_index++] = (uint8_t)(timestamp & 0xFF);
  event_linear_buffer[buffer_index++] = (uint8_t)((timestamp >> 8) & 0xFF);
  event_linear_buffer[buffer_index++] = (uint8_t)((timestamp >> 16) & 0xFF);
  event_linear_buffer[buffer_index++] = (uint8_t)((timestamp >> 24) & 0xFF);

  // Validate buffer size before writing
  if (buffer_index > 0 && buffer_index <= 36) {
    uint32_t bytesWritten = SEGGER_RTT_Write(SEGGER_RTT_CHANNEL_ID, event_linear_buffer, buffer_index);

    // Check if write was successful
    if (bytesWritten != buffer_index) {
      responsePrintError("log_lpw_hss_event", 0xFE, "RTT write failed: expected %d bytes, wrote %d bytes", buffer_index, bytesWritten);
    }
  }
}

/**
 * @brief Log data packets specifically to RTT with packet-specific format.
 *
 * This function creates a linear buffer containing event_type, event_status,
 * packet_type (first 2 bytes), HEC (3rd byte), and the entire payload.
 * It's specifically designed for data packets (DH1, DH3, DH5).
 *
 * @param hss_event Pointer to the HSS event data
 * @param event_size Size of the event data
 */
static inline void log_lpw_hss_packet(uint8_t *hss_event, uint32_t event_size)
{
  if (!lpw_hss_event_logger_enabled || hss_event == NULL) {
    return;
  }
  // Get timestamp from PROTIMER, timestamp is in microseconds
  uint32_t timestamp = (uint32_t)(PROTIMER_GetPTicks() >> TSF_SCALE_FACTOR);
  uint8_t linear_buffer[1152];  // event_type + event_status + br_edr_mode + event_size(4) + timestamp(4)
  // Create a linear buffer with proper byte alignment
  uint32_t buffer_index = 0;

  // Clear the global buffer before use to prevent data corruption
  memset(linear_buffer, 0, sizeof(linear_buffer));

  linear_buffer[buffer_index++] = BTC_DATA_PKT_RECEIVED;
  linear_buffer[buffer_index++] = BTC_DATA_PKT_RECEIVED_SUCCESS;
  linear_buffer[buffer_index++] = set_acl_parameters.br_edr_mode;

  // Write event_size as 4 bytes (little-endian)
  *(uint32_t *)&linear_buffer[buffer_index] = event_size;
  buffer_index += 4;

  for (int i = 0; i < event_size; i++) {
    linear_buffer[buffer_index++] = hss_event[i];
  }
  // Add timestamp as last 4 bytes (little-endian format)
  linear_buffer[buffer_index++] = (uint8_t)(timestamp & 0xFF);
  linear_buffer[buffer_index++] = (uint8_t)((timestamp >> 8) & 0xFF);
  linear_buffer[buffer_index++] = (uint8_t)((timestamp >> 16) & 0xFF);
  linear_buffer[buffer_index++] = (uint8_t)((timestamp >> 24) & 0xFF);

  // Validate buffer size before writing
  if (buffer_index > 0) {
    uint32_t bytesWritten = SEGGER_RTT_Write(SEGGER_RTT_CHANNEL_ID, linear_buffer, buffer_index);

    // Check if write was successful
    if (bytesWritten != buffer_index) {
      responsePrintError("log_lpw_hss_packet", 0xFE, "RTT write failed: expected %d bytes, wrote %d bytes", buffer_index, bytesWritten);
    }
  }
}

/**
 * @brief Form a complete Bluetooth data packet with headers
 *
 * This function constructs a properly formatted Bluetooth data packet including
 * packet header, payload header, and payload data. It handles different packet
 * types (DM1, DH1, DM3, DH3, DM5, DH5) with appropriate header lengths and
 * formats according to the Bluetooth specification.
 *
 * The packet structure includes:
 * - Packet header with type information
 * - Payload header (1 or 2 bytes depending on packet type)
 * - Payload data (filled with zeros for testing)
 *
 * Payload header format varies by packet type:
 * - DM1/DH1: 1-byte header with length, flow, and LLID
 * - DM3/DH3/DM5/DH5: 2-byte header with extended length field
 *
 * @param pkt_type Bluetooth packet type identifier
 * @param pkt_len Length of payload data in bytes
 * @param data_pkt_buf_ptr Buffer to store the constructed packet
 *
 * @return Total packet length including headers and payload
 *
 * @note The function uses LLID=2 (Start of L2CAP PDU) and flow=1 (Go) as defaults
 */
uint32_t form_data_pkt(uint8_t pkt_type, uint16_t pkt_len, uint8_t* data_pkt_buf_ptr, uint8_t br_edr_mode)
{
  uint32_t idx = 0;
  uint8_t payload_hdr_len = 0;
  uint8_t llid = 2;
  uint8_t flow = 1;
  // Enhanced Rate handling
  if (((pkt_type == 3) || (pkt_type == 4)) && (br_edr_mode == BASIC_RATE)) {
    payload_hdr_len = 1;  // 1-byte header for DM1/DH1 in Basic Rate ONLY
  } else if ((pkt_type > 0x02)) {
    payload_hdr_len = 2;  // 2-byte header for ALL other cases (Enhanced Rate + multi-slot)
  } else {
    payload_hdr_len = 1;  // Default for other packet types
  }

  // Update packet type
  data_pkt_buf_ptr[idx++] = pkt_type;
  data_pkt_buf_ptr[idx++] = payload_hdr_len;
  // Update payload header
  if (payload_hdr_len == 1) {
    data_pkt_buf_ptr[idx++] = (pkt_len << 3) | (flow << 2) | llid;
  } else if (payload_hdr_len == 2) {
    *(uint16_t *)&data_pkt_buf_ptr[idx] = llid /*ACL START */ | (flow << 2 /* FLOW GO */) | (pkt_len << 3);
    idx += 2;
  }

  // Set all the eir data as 0s
  for (uint16_t i = 0; i < pkt_len; i++) {
    data_pkt_buf_ptr[idx++] = 0;
  }

  *(uint16_t *)&data_pkt_buf_ptr[0] = 0;   // Reset the Header portion.

  // Forming the Packet Header
  data_pkt_buf_ptr[0] |= pkt_type << 3; // Set the packet type in the header
  return idx;
}

void hss_event_clk_notification_handler(uint8_t *hss_event)
{
  (void)hss_event;
  roleswitch_slot_offset = *HSS_EVENT_BTC_SLOT_OFFSET_ROLESWITCH_SLOT_OFFSET(hss_event);
  linear_buffer_t* buffer_ptr = data_path_create_linear_buffer();
  if (buffer_ptr != NULL) {
    uint32_t len = form_data_pkt(4, 10, buffer_ptr->data, set_acl_parameters.br_edr_mode);
    update_lt_addr(buffer_ptr->data);
    update_slot_offset(buffer_ptr->data);
    buffer_ptr->length = len;
    if (!enqueue_to_fifo(g_instance_id, buffer_ptr)) {
      responsePrintError("init_data_path", 0xFC, "Failed to enqueue to FIFO");
      return;
    }
  }
}

void start_roleswitch_central_procedure(void)
{
  uint32_t cmd = (uint32_t)&queue;
  set_roleswitch_params_central.connection_device_index = connection_device_index_central;
  set_roleswitch_params_central.roleswitch_instant = 40000;
  set_roleswitch_params_central.roleswitch_slot_offset = roleswitch_slot_offset;
  set_roleswitch_params_central.new_lt_addr = 2;
  set_roleswitch_parameters_cmd(&set_roleswitch_params_central, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
  start_roleswitch_central_page.connection_device_index = connection_device_index_central;
  start_roleswitch_central_page.priority = 1;
  start_roleswitch_central_page.step_size = 1;
  start_roleswitch_central_page.end_tsf = 26384;
  start_roleswitch_central_cmd(&start_roleswitch_central_page, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
}

void start_roleswitch_peripheral_procedure(void)
{
  uint32_t cmd = (uint32_t)&queue;
  set_roleswitch_params_peripheral.connection_device_index = connection_device_index_peripheral;
  set_roleswitch_params_peripheral.roleswitch_instant = 40000;
  set_roleswitch_params_peripheral.roleswitch_slot_offset = roleswitch_slot_offset;
  set_roleswitch_params_peripheral.new_lt_addr = 2;
  set_roleswitch_parameters_cmd(&set_roleswitch_params_peripheral, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
  start_roleswitch_peripheral_page_scan.connection_device_index = connection_device_index_peripheral;
  start_roleswitch_peripheral_page_scan.priority = 1;
  start_roleswitch_peripheral_page_scan.step_size = 1;
  start_roleswitch_peripheral_page_scan.end_tsf = 26384;
  start_roleswitch_peripheral_cmd(&start_roleswitch_peripheral_page_scan, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  RAIL_USER_SendMbox(btc_rail_handle, cmd);
}
