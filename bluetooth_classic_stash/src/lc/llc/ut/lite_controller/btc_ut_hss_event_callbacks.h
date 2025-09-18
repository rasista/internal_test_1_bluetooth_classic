#ifndef __BTC_UT_HSS_EVENT_CALLBACKS_H_
#define __BTC_UT_HSS_EVENT_CALLBACKS_H_

#include "stdint.h"
#include "pkb_mgmt.h"
#include "host_to_lpw_interface.h"
#include "rail_seq.h"
#include "data_path.h"
#include "SEGGER_RTT.h"
#include "sl_sleeptimer.h"
#include "em_device.h"

#define RTT_EVENT_BUFFER_SIZE 2048
#define EXTRACT_LAP_ADDR_BYTE(value) (((value) >> 2) & 0xFF)
#define TSF_SCALE_FACTOR 3
#define SEGGER_RTT_CHANNEL_ID 6
#define RTT_EVENT_LINEAR_BUFFER_SIZE 64 // Maximum event size + 4 bytes for timestamp

//Macros for extraction of bits for Packet header fields
#define PKT_HEADER_PKT_TYPE_MASK  0xF   //4-bit TYPE
#define PKT_HEADER_PKT_TYPE_POS   3     //bits 3-6
// Macro to get the packet type from the buffer
#define GET_RX_PKT_HEADER_TYPE(buf)   ((buf >> PKT_HEADER_PKT_TYPE_POS) & PKT_HEADER_PKT_TYPE_MASK)

#define BT_FHS_PKT_TYPE 0x02

#define BT_DM1_PKT_TYPE  0x03

// Data packet types for BASIC_RATE and ENHANCED_RATE
#define BT_DH1_PKT_TYPE 0x04
#define BT_2DH1_PKT_TYPE 0x04
#define BT_DH3_PKT_TYPE 0x0B
#define BT_3DH3_PKT_TYPE 0x0B
#define BT_DH5_PKT_TYPE 0x0F
#define BT_3DH5_PKT_TYPE 0x0F

#define BTC_CENTRAL               1
#define BTC_PERIPHERAL            0

#define BASIC_RATE              0
#define ENHANCED_RATE           1
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/

typedef struct acl_traffic_pattern_s{
  uint32_t packet_interval;        /**< Packet Interval in slots(1 slot = 625 microseconds) */
  uint32_t packet_type;            /**< Packet Type */
  uint32_t payload_length;        /**< Payload Length */
  uint32_t packet_interval_tsf;   /**< Packet Interval in microseconds */
} acl_traffic_pattern_t;

extern hss_cmd_set_page_t set_page;
extern acl_traffic_pattern_t acl_traffic_pattern;
void parse_rx_pkt_handler(uint8_t *hss_event, uint32_t event_size);
void hss_event_page_complete_handler(uint8_t *hss_event, uint32_t event_size);
void hss_event_page_scan_complete_handler(uint8_t *hss_event, uint32_t event_size);
void hss_event_inquiry_complete_handler(uint8_t *hss_event, uint32_t event_size);
void hss_event_inquiry_scan_complete_handler(uint8_t *hss_event, uint32_t event_size);
void hss_event_activity_complete_handler(uint8_t *hss_event, uint32_t event_size);
void hss_event_role_switch_complete_handler(uint8_t *hss_event, uint32_t event_size);
void update_lt_addr(uint8_t* tx_pkt);
void init_acl_tx_data_path(void);
void hss_event_tx_pkt_ack_received_handler(uint8_t *hss_event, uint32_t event_size);
void lpw_hss_event_rtt_init(void);
void acl_traffic_pattern_cb(RAIL_Handle_t btc_rail_handle);
uint32_t form_data_pkt(uint8_t pkt_type, uint16_t pkt_len, uint8_t* data_pkt_buf_ptr, uint8_t br_edr_mode);
void hss_event_clk_notification_handler(uint8_t *hss_event);
void start_roleswitch_central_procedure(void);
void start_roleswitch_peripheral_procedure(void);

#endif
