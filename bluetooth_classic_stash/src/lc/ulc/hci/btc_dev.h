/*******************************************************************************
 * @file  btc_dev.h
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs central Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/central-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef _BTC_DEV_H_
#define _BTC_DEV_H_
#include "stdint.h"
#include "btc_queue.h"
#include "pkb_mgmt_struct.h"


#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define BT_DBG_STATS_ASSIGN_VAL(var, value) var = value;

#define SCO_LT BIT(4)  // 5th position for sco_handle in slave_info
#define ESCO_LT BIT(7) // 6th position for esco_handle in slave_info

#define TRUE_RAND_NUM BIT(0)
#define PSEUDO_RAND_NUM BIT(1)

#define BT_CLOCK_OFFSET_VALID BIT(15)

#define ADDRESSES_ARE_EQUAL 0
#define ADDRESSES_ARE_UNEQUAL 1

#define BTC_HDR_PKT_LEN 2
#define INVALID_PEER_ID 0xFF

/*In the handle we encode, Bits 0, 1, 2, 3 are reserved for peripheral ID's from 1 to 8.
 * Bit 4, 5, 6, 7 and 8 are also reserved as stated above. In the event, handle can be of 2 bytes. So, if we want we can extend */
// #define CONN_HANDLE_TO_SLAVE_ID(x)         bt_conn_handle_to_slave_id(bt_common_info, x)
#define SCO_CONN_HANDLE_TO_SCO_HANDLE(x) ((x & 0x20) >> 5)
#define ESCO_CONN_HANDLE_TO_ESCO_HANDLE(x) ((x & 0x40) >> 6)
// #define BREDR_SLAVE_ID_TO_CONN_HANDLE(x)   bt_slave_id_to_conn_handle(bt_common_info, x) //(x ? x : 8)
#define BREDR_SCO_PEER_ID_TO_CONN_HANDLE(x, y) \
  (x ? x : 8) | SCO_LT | (y << 5) // y is 1 most of the times. 1 implies sco_handle in slave_info
#define BREDR_ESCO_PEER_ID_TO_CONN_HANDLE(x, y) \
  (x ? x : 8) | ESCO_LT | (y << 6) // y is 1 most of the times. 1 implies esco_handle in slave_info

#define BT_LAP_LEN 3

/* ARQ Flush */
#define FIRST_NON_FLUSHABLE_PACKET 0x00
#define CONTINUATION_PACKET 0x01
#define FIRST_FLUSHABLE_PACKET 0x02
#define COMPLETE_L2CAP_PDU 0x03

#define BTC_TX_PKT_HDR_LEN 2

#define BT_PKT_FLUSHED_WHILE_WAITING_FOR_ACK 1

#define ACL_CONTINUATION_LLID 1
#define ACL_FIRST_PKT_LLID 2
#define ACLC_LLID 3
/* Defines */

/* Maximum payload lenghts of various packet types */
#define BT_POLL_PAYLOAD_MAX_LEN 0
#define BT_NULL_PAYLOAD_MAX_LEN 0
#define BT_FHS_PKT_PAYLOAD_LEN 18
#define BT_DM1_PAYLOAD_MAX_LEN 17
#define BT_DM3_PAYLOAD_MAX_LEN 121
#define BT_DM5_PAYLOAD_MAX_LEN 224
#define BT_DH1_PAYLOAD_MAX_LEN 27
#define BT_DH3_PAYLOAD_MAX_LEN 183
#define BT_DH5_PAYLOAD_MAX_LEN 339
#define BT_2DH1_PAYLOAD_MAX_LEN 54
#define BT_2DH3_PAYLOAD_MAX_LEN 367
#define BT_2DH5_PAYLOAD_MAX_LEN 679
#define BT_3DH1_PAYLOAD_MAX_LEN 83
#define BT_3DH3_PAYLOAD_MAX_LEN 552
#define BT_3DH5_PAYLOAD_MAX_LEN 1021

/* Maximum RX pkt payload length */
#define BT_MAX_RX_PKT_ALLOWED 880
#define BT_INVALID_PACKET 1030

#define MAX_TX_TIME_OF_3DH1 227
#define MAX_TX_TIME_OF_3DH3 1472
#define MAX_TX_TIME_OF_3DH5 2728

#define PTYPE_2DH1_MAY_NOT_BE_USED 0X0002
#define PTYPE_3DH1_MAY_NOT_BE_USED 0X0004
#define PTYPE_DM1_MAY_BE_USED 0X0008
#define PTYPE_DH1_MAY_BE_USED 0X0010
#define PTYPE_2DH3_MAY_NOT_BE_USED 0X0100
#define PTYPE_3DH3_MAY_NOT_BE_USED 0X0200
#define PTYPE_DM3_MAY_BE_USED 0X0400
#define PTYPE_DH3_MAY_BE_USED 0X0800
#define PTYPE_2DH5_MAY_NOT_BE_USED 0X1000
#define PTYPE_3DH5_MAY_NOT_BE_USED 0X2000
#define PTYPE_DM5_MAY_BE_USED 0X4000
#define PTYPE_DH5_MAY_BE_USED 0X8000

/* Bluetooth Protocol classic mode packet types
 * ACL and SCO dont have conflicts
 * ACL and eSCO dont have conflicts
 * SCO and eSCO have conflicts
 * ACL has conflicts in itself
 * SCO dont have conflicts in itself
 * eSCO has conflicts in itslef
 * */
#define BT_NULL_PKT_TYPE 0x0
#define BT_POLL_PKT_TYPE 0x1
#define BT_FHS_PKT_TYPE 0x2
#define BT_DM1_PKT_TYPE 0x3
#define BT_DH1_PKT_TYPE 0x4
#define BT_2DH1_PKT_TYPE 0x4
#define BT_HV1_PKT_TYPE 0x5
#define BT_HV2_PKT_TYPE 0x6
#define BT_2EV3_PKT_TYPE 0x6
#define BT_HV3_PKT_TYPE 0x7
#define BT_EV3_PKT_TYPE 0x7
#define BT_3EV3_PKT_TYPE 0x7
#define BT_DV_PKT_TYPE 0x8
#define BT_3DH1_PKT_TYPE 0x8
#define BT_AUX_PKT_TYPE 0x9
#define BT_DM3_PKT_TYPE 0xA
#define BT_2DH3_PKT_TYPE 0xA
#define BT_DH3_PKT_TYPE 0xB
#define BT_3DH3_PKT_TYPE 0xB
#define BT_EV4_PKT_TYPE 0xC
#define BT_2EV5_PKT_TYPE 0xC
#define BT_EV5_PKT_TYPE 0xD
#define BT_3EV5_PKT_TYPE 0xD
#define BT_2DH5_PKT_TYPE 0xE
#define BT_DM5_PKT_TYPE 0xE
#define BT_3DH5_PKT_TYPE 0xF
#define BT_DH5_PKT_TYPE 0xF

#define MAC_ADDR_LEN 6
#define DEFAULT_MAX_NUM_OF_STORED_LINK_KEY 7

#define PAGE_MODE_0 0
#define PAGE_MODE_1 1
#define PAGE_MODE_2 2

#define FEATURE_MASK_LENGTH 8
#define EXT_FEATURE_MASK_LENGTH 8
#define MAX_NUM_ACL_CONNS 2
#define MAX_NUM_OF_STORED_LINK_KEY 10
#define BT_NUM_OF_SUPP_IAC 1
#define MIN_ALLOWED_POWER_INDEX 00
#define MAX_ALLOWED_POWER_INDEX 10
#define DEFAULT_EDR_POWER_INDEX 10
#define GOLDEN_RECEIVE_POWER_RANGE_UPPER_LIMIT -70
#define GOLDEN_RECEIVE_POWER_RANGE_LOWER_LIMIT -30
#define DEFAULT_CLOCK_DRIFT 250
#define DEFAULT_CLOCK_JITTER 10
#define DEFAULT_LINK_SUPERVISION_TIMEOUT 0x2580 // In number of slots = 6Seconds
#define DEFAULT_POLL_INTERVAL 40
#define DEFAULT_NBC 0
#define MAX_NUM_SUPPORTED_FEATURE_PAGE 2
#define MAX_EVENT_MASK_LENGTH 8
#define DEFAULT_ACL_PACKET_LENGTH 17
#define MAX_ACL_PACKET_LENGTH 1021
#define MAX_SCO_PACKET_LENGTH 30
#define MAX_SCO_PACKET_LENGTH_RAM 64
#define CLASS_OF_DEV_LEN 3
#define HCI_CMD_MAP_LENGTH 64
#define BT_RX_MAX_PKT_LEN 1028
#define BT_TX_MAX_PKT_LEN 1025
#define LINK_KEY_LEN 16
#define ENC_KEY_KC_MAX_LEN 16
#define ENC_KEY_KC_MIN_LEN 7
#define RAND_NUM_SIZE 16
#define MAX_PIN_CODE_LEN 16
#define PUBLIC_KEY_LEN 48
#define PUBLIC_KEY_LEN_256 64
#define SIZE_OF_C_VAL 16
#define SIZE_OF_DHKEY 16
#define SIZE_OF_CONFIRMATION_VAL 16
#define MAX_PRIVATE_KEY_LEN 24
#define NUM_OF_DWORDS_IN_G 6
#define PRIVATE_KEY_LEN 24
#define PRIVATE_KEY_LEN_256 32
#define LEN_OF_GX 24
#define LEN_OF_GY 24
#define NUM_OF_DWORDS_IN_B 6
#define NUM_OF_DWORDS_IN_A 6
#define SIZE_OF_R1 24
#define NUM_OF_DWORDS_IN_P 6
#define NUM_DWORDS_IN_PUBLIC_KEY 12
#define NUM_DWORDS_IN_PRIVATE_KEY 6
#define SIZE_OF_R 16
#define DHKEY_LEN 24
#define DHKEY_LEN_256 32
#define SIZE_OF_KEY_ID 4
#define SIZE_OF_SCALAR 24

#define BT_STATUS_SUCCESS 0
#define BT_STATUS_FAILURE 1
#define BT_NODE_NOT_FOUND 0xFF
#define BT_NODE_FOUND 0x00

#define ACCEPT_PARAMS_SET BIT(0)
#define REJECT_PARAMS_SET BIT(1)
#define SEND_ANOTHER_PARAMS_SET BIT(2)

#define EVENT_COMPLETED 0
#define EVENT_PENDING 1

#define PAGE_SCAN_MODE_R0 0
#define PAGE_SCAN_MODE_R1 1
#define PAGE_SCAN_MODE_R2 2

#define DEFAULT_PSCAN_INTERVAL 0x0800 /* 1.28Sec interval or 2048 slots */
#define DEFAULT_PSCAN_WINDOW 0x0012   /* 11.25ms or 18 slots*/
#define DEFAULT_ISCAN_INTERVAL 0x0800 /* 1.28Sec interval or 2048 slots */
#define DEFAULT_ISCAN_WINDOW 0x0012   /* 11.25ms or 18 slots*/

#define AFH_INTERVAL_500_MS 800
#define MIN_GOOD_CHANNEL_CNT 20

#define ENC_ENABLED 1
#define ENC_DISABLED 0

/* SCO link related defines */
#define MAX_NUM_SCO_HANDLES 2

/* eSCO link related defines */
#define MAX_NUM_ESCO_HANDLES 2

typedef struct conn_pwr_ctrl_info_s{
  /* This variables are used to store rssi related
   * info -present rssi,rssi avg
   */
  int8_t rssi;
#define RSSI_AVG_MAX_PACKET_COUNT 100
  int8_t rssi_average;
  uint8_t rssi_avg_pkt_count;

  /* These are used in the power control whether to increase/decrease
   * the power levels
   */
  uint8_t max_power_reached;
  uint8_t min_power_reached;
  /* tx power level */
  uint8_t tx_power_index;
  uint8_t min_allowed_power_index;
  uint8_t max_allowed_power_index;
#define ENABLE_POWER_CNTRL 1
#define DISABLE_POWER_CNTRL 0
  uint8_t enable_power_control;
  int16_t rssi_average_sum;
} conn_pwr_ctrl_info_t;

typedef struct afh_classification_stats_s{
#define NUM_OF_MAX_CHANNELS 80
  uint16_t num_of_tx_retries[NUM_OF_MAX_CHANNELS];
  uint16_t num_of_rx_retries[NUM_OF_MAX_CHANNELS];
} afh_classification_stats_t;

typedef struct btc_periodic_inq_params_s{
  uint8_t inquiry_len;
  uint8_t num_resp;
  uint16_t max_period_len;
  uint16_t min_period_len;
  uint32_t next_start_instant;
} btc_periodic_inq_params_t;

typedef struct inquiry_params_s{
  /* This variable indicate the status whether we are waiting for the eir or not */
#define WAIT_FOR_EIR_PKT 1
#define NO_WAIT_FOR_EIR_PKT 0

#define INQUIRY_ENABLED 1
#define INQUIRY_DISABLED 0
  uint8_t inq_enable;
  uint8_t eir_pkt_to_be_rxd;

  /* This variable contains after how many num_resp inquiry should be stopped */
  uint8_t num_resp;

  /* The variable is used to maintain presently how many resp are recevied */
  uint8_t num_resp_cmpltd;

  uint8_t inquiry_length;
  /*These variables are used for the storage of the access addresses
   * during general,dedicated and from the host
   */
#define GENERIC_INQUIRY_ADDRESS 0x9E8B33
  uint8_t inquiry_address_generic[MAC_ADDR_LEN];   // TODO
  uint8_t inquiry_address_from_host[MAC_ADDR_LEN]; // TODO
  uint8_t inquiry_address_dedicated[MAC_ADDR_LEN];

  /* This is used while forming the eir packet to use which packet types
   */
  uint8_t fec_requirement_for_eir;

  uint8_t eir_set;
  uint8_t eir_length;

  /* This structure contains the info regarding the perodic inquiry
   */
#define PERIODIC_INQUIRY_ENABLED 1
#define PERIODIC_INQUIRY_DISABLED 0
  uint8_t periodic_inq_enable;

  /* This variable stores the inquiry type used */
#define BT_MAX_INQ_RESP_DURATION 0xE //  In terms of half slots (312.5 us)
#define INQUIRY_RESULT_FORMAT 0
#define INQUIRY_RESULT_WITH_RSSI_FORMAT 1
#define EXTND_INQUIRY_RESULT_FORMAT 2
  uint8_t inquiry_mode;
  uint8_t *resp_addr_list;
  uint16_t ext_inq_len;
  uint32_t ext_inquiry_start;

  uint32_t inquiry_expiry_time;

  uint32_t inquiry_start_time;

  btc_periodic_inq_params_t periodic_inq_params; // TODO
} inquiry_params_t;

typedef struct inquiry_scan_params_s{
  /* Type of inq scan standard or interlaced
   */
  uint8_t curr_scan_type;

#define STANDARD_INQUIRY_SCAN 0
#define INTERLACED_INQUIRY_SCAN 1
  uint8_t inquiry_scan_type;
  uint8_t inquiry_scan_pending;

  /* The number of slots for which the
   * continuously visible */
#define BT_MINIMUM_ISCAN_CONTINUOUS_VISIBILITY_DURATION 18 // In terms of full slots
  uint16_t min_cont_visible_cnt;

  /* Stores the value of the inquiry scan interval */
  uint16_t inquiry_scan_interval; // given by host;N - 0x12 to 0x1000

  /* Stores the values of the inquiry scan window */
  uint16_t inquiry_scan_window; // given by host;N - 0x11 to 0x1000

  /* Stores the value when next inquriy scan starts */
  uint32_t next_inquiry_scan_timestamp; // native_bt_clock+(inquiry_scan_interval<<1)

  uint32_t num_slots; // num_slots = inquiry_scan_window*2;in terms of full slots

  /* Clock at which inq_scan starts */
  uint32_t inq_scan_start_instant;

  /* Clock at which inq_scan window ends */
  uint32_t inq_scan_window_end; // native_bt_clock+(inquiry_scan_window<<1)

  /* This variable gives the status no of slots inquiry scan is completed */
  uint32_t cmpltd_slots;
} inquiry_scan_params_t;

typedef struct page_params_s{
  /* This indicates the page scan rep mode */
  // 0-R0, 1-R1, 2-R2
  uint8_t page_scan_rep_mode;

  uint8_t current_paged_conn_id;

  /* This flag is used to update the page start time for the very first time
   * and reset this flag after completing paging or cancel connection command
   * handling
   */
  uint8_t page_start;

  /* The tout used when the temporary connection is madde */
#define REMOTE_NAME_TOUT 0x4000
  uint16_t remote_name_req_tout_count;

  /* This variable gives the status no of slots paging is completed */
  uint16_t cmpltd_half_slots;

  /* The tout used in the paging state */
#define DEFAULT_PAGE_TOUT 0x2000  // 5.12 sec
#define EXTENDED_PAGE_TOUT 0x4000 // 10.24 sec
  uint16_t page_tout;

  uint16_t ext_page_tout;
  uint32_t ext_page_start_instant;
  /* The instant paging should be stopped */
  uint32_t stop_instant;

  /* The instant paging should be started */
  uint32_t page_start_time;
} page_params_t;

typedef struct page_scan_params_s{
  /* This indicates whether the peripheral receied the poll in the connection state */
#define MASTER_POLL_HAS_BEEN_RCVD 1
#define MASTER_POLL_NOT_RCVD_YET 0
  uint8_t master_poll_rcvd; // TODO

#define STANDARD_PAGE_SCAN 0
#define INTERLACED_PAGE_SCAN 1
  uint8_t page_scan_type;
  uint8_t page_scan_pending;

  uint8_t curr_scan_type;

  /* This variables indicate the page scan mode,interval and window */
  // 0-R0, 1-R1, 2-R2
  uint8_t page_scan_rep_mode;

  uint16_t page_scan_interval;
  uint16_t page_scan_window;
  /* The number of slots for which the
   * continuously visible */
#define BT_MINIMUM_PSCAN_CONTINUOUS_VISIBILITY_DURATION 18 // In terms of full slots
  uint16_t min_cont_visible_cnt;

  uint32_t next_page_scan_timestamp;
  uint32_t page_scan_window_end;
#define NEGATIVE_CLOCK_OFFSET BIT(31)
  uint32_t connection_clock_offset;

  /* The no of slots for which page scan should be done */
  uint32_t num_slots;

  /* This variable gives the status no of slots page scan is completed */
  uint32_t cmpltd_slots;
} page_scan_params_t;

typedef struct btc_hci_params_s{
  /* Event map mask for which the events should not be given to the host */
  uint8_t event_map_mask[MAX_EVENT_MASK_LENGTH];

  /* inidcates the num of hci commands it can receive */
  uint8_t num_of_hci_cmd_pkts;

  uint8_t bd_addr[MAC_ADDR_LEN];
  /* Stores the supported commands bitmap
   */
  uint8_t hci_cmd_bitmap[HCI_CMD_MAP_LENGTH];
} btc_hci_params_t;

typedef struct btc_hci_debug_stats_s{
  /* HCI layer */
  uint32_t total_hci_command_pkts_rcvd;
  uint32_t total_hci_acldata_pkts_rcvd;
  uint32_t num_tx_pkt_cmpltd_events_sent;
  uint32_t total_hci_scodata_pkts_rcvd;
  uint32_t total_hci_vendor_pkts_rcvd;
} btc_hci_debug_stats_t;

typedef struct btc_lmp_debug_stats_s{
  /* LMP layer */
  uint16_t lmp_pkts_recvd;
  uint16_t lmp_pkts_sent;
  uint16_t undeveloped_opcode_rcvd;
  uint16_t rx_pkts_dropped;
  uint16_t rx_pkts_dropped_insufficient_rx_pool;
} btc_lmp_debug_stats_t;

/* eSCO params structure */
#define COPY_ESCO_PARAMS 0
#define RESET_ESCO_PARAMS 1
#define COPY_N_RESET_ESCO_PARAMS 2

/* Decide the link type */
#define BT_SCO  0
#define BT_ACL  1
#define BT_ESCO 2

#define BASIC_DATA_RATE    0 // 1Mbps
#define ENHANCED_DATA_RATE 1 // 2/3 Mbps

#define ONE_SLOT_PACKET   1
#define THREE_SLOT_PACKET 3
#define FIVE_SLOT_PACKET  5

typedef struct esco_link_params_s{
  /* Variable to check the status of eSCO connection */
#define ESCO_HANDLE_OCCUPIED 1
#define ESCO_HANDLE_NOT_OCCUPIED 0
  uint8_t esco_handle_occupied;

  /* Connection handle for eSCO link between controller and host.
   * It is generated using esco_handle. */
  uint8_t esco_conn_handle;

  /*eSCO LT_ADDR*/
#define ESCO_LT_ADDR 2
  uint8_t esco_lt_addr;
  /* Timing control flag, Initialisation flag */
#define CLK_INIT_1_MODE 0
#define CLK_INIT_2_MODE 0x2
  uint8_t initialisation_flag;

  /* Air mode */
#define MU_LAW_MODE 0
#define A_LAW_MODE 1
#define CVSD_MODE 2
#define TRANSPARENT_MODE 3
  uint8_t air_mode;

  /* Negotiation state: set as '0' by the initialing LM. Shall not be set as '0' after that.*/
#define ESCO_UNDEFINED_STATE 1
#define ESCO_RESERVED_SLOT_VIOLATION 2
#define ESCO_LATENCY_VOILATION 3
#define ESCO_CONFIG_NOT_SUPPORTED 4
  uint8_t negotiation_state;

  /* Desco value, only even values allowed */
  uint8_t esco_offset;

  /* Tesco value, only even values allowed */
  uint8_t esco_interval;

  /* Wesco value, only even values allowed */
  uint8_t esco_window;

  /*upper limit of Tesco in ms*/
  uint16_t max_latency;

  /* central to peripheral packet type to be used - LMP level pkt type*/
  uint16_t central_to_peripheral_pkt_type;

  /* peripheral to central packet type to be used - LMP level pkt type*/
  uint16_t peripheral_to_central_pkt_type;

  /* Packet length central to peripheral*/
  uint16_t central_to_peripheral_pkt_len;

  /* Packet length peripheral to central */
  uint16_t peripheral_to_central_pkt_len;

  /* Supported packet types in eSCO from HCI. This variable is bitmap.*/
  uint16_t esco_pkt_type;

  /*Retransmission effort as given by host*/
  uint16_t retry_effort;

  uint16_t master_given_esco_handle; /*It has the eSCO handle given by central. The value ranges from 1 to 127.
                                        Since we cant hold such a big array, we are rewriting that to 1. But
                                        master_given_esco_handle is needed when we request for
                                        change of packet type req/DISCONNECT */

  /* TX bandwidth */
  uint32_t tx_bandwidth;

  /* RX bandwidth */
  uint32_t rx_bandwidth;

  /*no of master and slave reserved slots including window.
   * In the units of 625us */
  uint8_t esco_reserved_slot_num;

  /* Queue for esco data packets */ /*FIXME */
  // bt_tx_queue_t tx_esco_q;
} esco_link_params_t;

/* SCO params structure */
typedef struct sco_link_params_s{
  /* Queue for sco data packets */ /* FIXME */
  // bt_tx_queue_t tx_sco_q;
  /* Variable to check the status of SCO connection */
#define SCO_HANDLE_OCCUPIED 1
#define SCO_HANDLE_NOT_OCCUPIED 0
  uint8_t sco_handle_occupied;

  /* Tsco value, only even values allowed */
  uint8_t sco_interval;

  /* Dsco value, only even values allowed */
  uint8_t sco_offset;

  /* Packet type to be used */
  uint8_t sco_pkt_type;

  /* Initialisation flag */
#define CLK_INIT_1_MODE 0
#define CLK_INIT_2_MODE 0x2
  uint8_t initialisation_flag;

  /* Air mode */
#define MU_LAW_MODE 0
#define A_LAW_MODE 1
#define CVSD_MODE 2
#define TRANSPARENT_MODE 3
  uint8_t air_mode;

#define BT_SCO_SELF_HV_PKT 1
#define BT_SCO_NO_SELF_HV_PKT 0
#define MAX_SELF_HV_PKTS_PER_SESSION \
  60000 /* SIG_REVIEW Fix it properly to send self packets and to give the ack with HV1 for received DV */ /* Session means, after create/change link */
  uint8_t self_hv_pkt;

  /* For debugging */
  uint8_t form_sco_self_hv_pkt_called;
  uint8_t sco_link_lmp_transactions;
  uint16_t self_hv_pkt_sent;
  /* Connection handle for SCO link */
  uint16_t sco_conn_handle;

  uint16_t master_given_sco_handle; /*It has the SCO handle given by central. The value ranges from 1 to 127.
                                       Since we cant hold such a big array, we are rewriting that to 1. But
                                       master_given_sco_handle is needed when we request for
                                       HCI_OP_CHANGE_CONN_PKT_TYPE/DISCONNECT */

  /* TX bandwidth */
  uint16_t tx_bandwidth;

  /* RX bandwidth */
  uint16_t rx_bandwidth;

  /* Next M->S TX SCO reserved slot number */
  uint32_t sco_master_reserved_slot_num;

  /* Next S->M TX SCO reserved slot number */
  uint32_t sco_slave_reserved_slot_num;
} sco_link_params_t;

/* SCO Update params structure */
typedef struct sco_update_params_s{
  /* Packet type to be used */
  uint8_t sco_pkt_type;

  /* Tsco value, only even values allowed */
  uint8_t sco_interval;
} sco_update_params_t;

typedef struct test_mode_params_s{
  pkb_t *tx_pkt;

#define PAUSE_TEST_MODE 0
#define TRANSMITTER_TEST_0 1
#define TRANSMITTER_TEST_1 2
#define TRANSMITTER_TEST_2 3
#define TRANSMITTER_TEST_PRBS 4
#define LOOPBACK_ACL 5
#define LOOPBACK_SCO 6
#define LOOPBACK_ACL_NO_WHITENING 7
#define LOOPBACK_SCO_NO_WHITENING 8
#define TRANSMITTER_TEST_F0 9
#define EXIT_TEST_MODE 255
  uint8_t test_scenario;

#define TX_RX_ON_SINGLE_FREQ 0
#define NORMAL_HOPPING 1
  uint8_t hopping_mode;
  uint8_t tx_freq;
  uint8_t rx_freq;
#define FIXED_TX_PWR 0
#define ADAPTIVE_PWR_CNTRL 1
  uint8_t pwr_cntrl_mode;
  uint8_t poll_period;
  uint8_t pkt_type;
  uint8_t link_type;
#define WHITENING_DISABLED 1
#define WHITENING_ENABLED 2
  uint8_t whitening_enable;
#define INVALID_SEQUENCE 0xFF
#define SEQUENCE_0 0
#define SEQUENCE_1 1
#define SEQUENCE_2 2
#define SEQUENCE_F0 3
#define SEQUENCE_PRBS 4

  /* pkt_type : bits 7-4*/
#define TEST_ACL ACL_TEST_MODE
#define TEST_SCO SCO_TEST_MODE
#define TEST_ESCO ESCO_TEST_MODE
  uint8_t test_lt_type;

#define TEST_NOT_STARTED 0
#define TEST_STARTED 1
  uint8_t test_started;

  /* When test_mode_type = 1(SCO_TEST_MODE), we need to xmit SCO packet.
   * When test_mode_type = 2(ESCO_TEST_MODE), we need to xmit eSCO packet.
   * In transmitter test - once the test is started, eSCO/SCO pkt is to be txd until exit/pause_test_mode is rcvd in RX.
   * So for transmitter case this variable is set at the start and reset only when exit/pause is rcvd.
   * In Loopback test - For every rcvd eSCO/SCO pkt, we have to xmit the rcvd pkt in loopback.
   * So for loopback case this variable is set at the start of the test mode and also set every time eSCO/SCO pkt is rcvd.
   * It is reset every time a non-eSCO/SCO pkt is rcvd.
   * */
#define ACL_TEST_MODE 0
#define SCO_TEST_MODE BIT(0)
#define ESCO_TEST_MODE BIT(1)
  uint8_t test_mode_type;

  uint16_t len_of_seq;

} test_mode_params_t;

typedef struct sniff_params_s{
#define SNIFF_MODE_DISABLE 0
#define SNIFF_MODE_ENABLE 1
  /* Initialisation procedure flag */
#define INIT_PROCEDURE_1 0
#define INIT_PROCEDURE_2 1
  uint8_t init_flag;

#define SNIFF_SUBR_ENABLE 1
#define SNIFF_SUBR_DISABLE 0
  uint8_t sniff_subrating_enable;
  uint8_t max_remote_sniff_subrate; // we calculate for peer, remote_max_sniff_latency/t_sniff
  uint8_t max_local_sniff_subrate;  // received from peer	uint8_t sniff_subrate;
  uint8_t sniff_subrate;
#define ENABLE_SUBRATE 1
#define DISABLE_SUBRATE 0
  uint8_t set_subrate;
#define SNIFF_SUBRATE_PARAMS_NOT_SET 0
#define SNIFF_SUBRATE_PARAMS_SET 1
  uint8_t sniff_subrate_params_set;
  /* Sniff interval */
#define SNIFF_INTERVAL_200_MS 0x140
  uint16_t t_sniff; // In terms of full slots (1 full slot = 625 us)

  uint16_t remote_max_sniff_latency;        // received from HCI through Sniff subrate command
  uint16_t min_remote_sniff_subrating_tout; // received from HCI - send to peer
  uint16_t min_local_sniff_subrating_tout;  // received from HCI
  uint16_t subrating_tout_rcvd;             // received from peer
  uint16_t sniff_subrating_tout;            // max(min_local_sniff_subrating_tout, subrating_tout_rcvd)
  /* Used to determine when to transition form Sniff to Subrate */
  uint16_t sniff_subrating_tout_count; // reset same way as sniff_tout_count
  uint32_t sniff_subrating_instant;
  /* Sniff offset */
  uint16_t d_sniff;
  uint16_t sniff_attempt;
  uint16_t sniff_tout;
  uint16_t sniff_tout_count;
} sniff_params_t;

typedef struct esco_info_s{
  uint8_t esco_lt_addr;
  /* This is the index of esco_link_params in use. */
  uint8_t esco_handle;
  /*no of central and peripheral reserved slots + window.
   * In the units of 625us */
  uint8_t reserved_slot_num;
  /*No of retx window slots.TU=625us */
  uint8_t window_slot_num;

  /* A valid packet is already received in this eSCO window */
#define NOT_RECEIVED 0
#define RECEIVED 1
  uint8_t vld_pkt_rcvd_in_esco;

#define DO_ESCO_RETRY 1
#define NO_RETRY 0
  uint8_t retry_flag;
  uint8_t data_rate_type;
#define GOOD_DATA 0x00                 // CRC pass
#define DATA_WITH_POSSIBLE_ERRORS 0x01 // CRC fail
#define NO_ESCO_DATA 0x02              // No packet rcvd, HEC fail
#define PARTIALLY_LOST 0x03
  uint8_t pkt_sts_flag;
  /* eSCO pkt type. This variable is updated when the link is established or when pkt type is changed.
   * It contains on air LC TX/RX esco pkt type.*/
  uint8_t esco_pkt_type;
  /* This variable is used to indicate the length of esco rx pkt. */

  uint8_t esco_three_slot_pkt;

  uint16_t esco_rx_packet_len;
  uint16_t esco_tx_tout;
  uint16_t max_self_esco_pkts;
} esco_info_t;

typedef struct acl_dev_mgmt_info_s{
  /* Stores the mac address of the remote device */
  uint8_t bd_addr[MAC_ADDR_LEN];

  /* Indicates the peer_info be already used */
  uint8_t conn_id_occupied;
#define ALREADY_CONNECTED 1
#define NOT_CONNECTED 0

  /*To indicate controller entered into the connected state */
  uint8_t connection_status;

  /*QOS parameters*/
#define NO_TRAFFIC 0
#define BEST_EFFORT 1
#define GUARANTEED 2
  uint8_t service_type;

  /*This is used to store the lmp connection status of the controller */
#define LM_LEVEL_CONN_DONE 1
#define LM_LEVEL_CONN_NOT_DONE 0
  uint8_t lm_connection_status;

  /* To indicate whether the peer device is peripheral/central */
#define BT_DEVICE_MODE 0
#define BT_CENTRAL_MODE 1
#define BT_PERIPHERAL_MODE 2
  uint8_t mode;

  /* To store the remote device class of device info
   */
  uint8_t class_of_dev[CLASS_OF_DEV_LEN];

#define MAX_BT_DEVICE_NAME_LEN 248
  /* This variable is used to contain the remote device name */
  uint8_t device_name[MAX_BT_DEVICE_NAME_LEN];

  /* This variable stores the device name length of the remote device */
  uint8_t device_name_len;

  /*Stores the drift and jitter of the remote device used in the sniff mode
   */
  uint8_t drift;
  uint8_t jitter;

  /* Stores the io_capablities of the remote device */
  uint8_t io_capabilities;
  uint8_t oob_authentication_data;
  uint8_t authentication_requirement;

  /* features info */
  uint8_t features_bitmap[FEATURE_MASK_LENGTH];

  /* extended features info */
  uint8_t ext_features_bitmap[EXT_FEATURE_MASK_LENGTH];
  uint8_t ext_features_bitmap_page2[EXT_FEATURE_MASK_LENGTH];
  uint8_t max_page_num;

  /* version info */
  uint8_t vers_number;

  /* ptt info and requested ptt of the remote device */
#define BT_BR_MODE 0x0
#define BT_EDR_MODE 0x1
  uint8_t ptt;
  uint8_t requested_ptt;

  /*This will be used channel driven quality rate feature*/
  uint8_t preferred_rate_received;
  uint8_t preferred_rate;

  /* Max slots info of the remote device */
#define ONE_SLOT_PKT 1
#define THREE_SLOT_PKT 3
#define FIVE_SLOT_PKT 5
  uint8_t tx_max_slots;
  uint8_t requested_max_slot;

#define AFH_DISABLE 0
#define AFH_ENABLE 1
#define CHANNEL_MAP_BYTES 10
  uint8_t afh_channel_map_in_use[CHANNEL_MAP_BYTES];
  uint8_t afh_channel_map_new[CHANNEL_MAP_BYTES];
#define AFH_REPORTING_DISABLED 0
#define AFH_REPORTING_ENABLED 1
  uint8_t afh_reporting_mode;

  uint8_t no_of_valid_afh_channels;

  /* The below variables contains the information relating to the afh_channel_map now use and new received
   * afh interval during classification
   */
  uint8_t afh_channel_classification[CHANNEL_MAP_BYTES];

  /* This variable contains the info relating to the sco handle */
  uint8_t sco_handle;

  /* This variable stores the air mode used */
#define U_LAW 0
#define A_LAW 1
#define CVSD 2
#define TRANSPARENT_DATA 3
  uint8_t air_mode;

  // 0-R0, 1-R1, 2-R2
  uint8_t page_scan_rep_mode;

  uint16_t conn_handle;

  /* This variable is used to store the clock offset info of the remote device
   */
  uint16_t clk_offset;

  uint16_t comp_id;
  uint16_t sub_vers_number;

  uint16_t afh_min_interval;
  uint16_t afh_max_interval;
  uint16_t afh_interval;

  /* This variables are used to store the link policy settings and connected sub state(sniff,active)
   * sniff params,sniff tout,sniff poll transition and sniff subtrating
   */
#define DISABLE_ALL_LM_MODES 0x0000
#define ENABLE_ROLE_SWITCH BIT(0)
#define ENABLE_HOLD_MODE BIT(1)
#define ENABLE_SNIFF_MODE BIT(2)
#define ENABLE_PARK_STATE BIT(3)
  uint16_t link_policy_settings;

  uint32_t latency;
  uint8_t local_lk_rand[RAND_NUM_SIZE];
} acl_dev_mgmt_info_t;

typedef struct acl_link_mgmt_info_s{
  // struct bt_tx_pkt_s *tx_frag_pkt; /*FIXME */

  /* State */
#define LMP_STANDBY_STATE 0
#define START_CONNECTION 1
#define NAME_REQ_SENT_STATE 2
#define FEATURES_REQ_SENT_STATE 3
#define EXT_FEATURES_REQ_SENT_STATE 4
#define VERSION_REQ_SENT_STATE 5
#define HOST_CONNECTION_REQ_SENT_STATE 6
#define HCI_LINK_KEY_REQUEST_SENT_STATE 7
#define HCI_PIN_CODE_REQUEST_SENT_STATE 8
#define LMP_IN_RAND_SENT_STATE 9
#define LMP_COMB_KEY_SENT_STATE 10
#define LMP_AU_RAND_SENT_STATE 11
#define ENC_MODE_REQ_SENT_STATE 12
#define ENC_KEY_SIZE_REQ_SENT_STATE 13
#define START_ENCRYPTION_SENT_STATE 14
#define SETUP_COMPLETE_SENT_STATE 15
#define PTT_REQ_SENT_STATE 16
#define TIMING_ACCURACY_REQ_SENT_STATE 17
#define MAX_SLOT_REQ_SENT_STATE 18
#define HCI_IO_CAP_REQ_SENT_STATE 19
#define LMP_IO_CAP_REQ_SENT_STATE 20
#define LMP_ENCAP_HEADER_SENT_STATE 21
#define LMP_ENCAP_PAYLOAD_SENT_STATE 22
#define PUBLIC_KEY_SENT_STATE 23
#define LMP_SIMPLE_PAIRING_NUM_SENT_STATE 24
#define HCI_USER_CONFIRMATION_REQ_SENT_STATE 25
#define LMP_DHKEY_CHECK_SENT_STATE 26
#define HCI_PASSKEY_REQ_SENT_STATE 27
#define LMP_SIMPLE_PAIRING_CONFIRM_SENT_STATE 28
#define EXT_FEATURES_REQ_PAGE_2_SENT_STATE 29

  uint8_t lmp_connection_state;

#define SLAVE_PROCESS_CONN_START 0
#define SLAVE_PTT_REQ_SENT_STATE 1
#define SLAVE_SETUP_COMPLETE_SENT_STATE 2
#define SLAVE_TIMING_ACCURACY_REQ_SENT_STATE 3
#define SLAVE_FEATURE_REQ_SENT_STATE 4
#define SLAVE_EXT_FEATURES_REQ_SENT_STATE 5
#define SLAVE_VERSION_REQ_SENT_STATE 6
#define SLAVE_MAX_SLOT_REQ_SENT_STATE 7
#define SLAVE_EXT_FEATURES_REQ_PAGE2_SENT_STATE 8

  uint8_t lmp_slave_connection_state;

  /* Flow  control bit */
#define ACL_FLOW_CNTRL_STOP 0
#define ACL_FLOW_CNTRL_GO 1
  uint8_t acl_flow; // Flow control is not done for synchronous data

  /* This is used whther is connection general or reading name remote name request command */
#define CONN_CANCEL_FROM_REMOTE_NAME_TOUT 1
#define CONN_CANCEL_FROM_PAGE_TOUT 2
  uint8_t conn_cancel_purpose;

  /* Flag to give events to the host */
#define LINK_KEY_CHANGED BIT(0)
#define ENC_KEY_REFRESH BIT(1)
#define ENC_MODE_CHANGE BIT(2)
#define CONNECTION_COMPLETE BIT(3)
#define DISCONNNECTION_COMPLETE BIT(4)
#define ROLE_CHANGE BIT(5)
#define DISCONNECT_COMPLETE_EVENT_GIVEN_TO_HOST BIT(6)
  uint8_t host_event_notification_flags;

  uint8_t link_quality;
  /* used in the lmp pending state machines */
  uint8_t lmp_tx_pending;
  uint8_t lmp_tx_pending_opcode;
  uint8_t lmp_tx_pending_ext_opcode;

  /* From which queue was the last packet transmitted to this remote device */
#define ACL_C_QUEUE 1
#define ACL_D_QUEUE 2
#define SCO_QUEUE 3
#define ESCO_QUEUE 4
  uint8_t last_txd_pkt_q;

#define PKT_NOT_RCVD_IN_LAST_SLOT 0
#define PKT_RCVD_IN_LAST_SLOT 1
  /* Used only when we are peripheral */
  uint8_t pkt_rcvd_in_last_rx_slot;

#define NO_ROLE_SWITCH 0x0
#define ALLOW_ROLE_SWITCH 0x1
  uint8_t allow_role_switch;

#define LINK_SUPERVISION_ENABLED 1
#define LINK_SUPERVISION_DISABLED 0
  uint8_t link_supervision_enabled;

#define CONN_FOR_DATA_PKTS_EXCHANGE 0x0
#define TEMPORARY_CONN_FOR_REMOTE_NAME_RES 0x1
  uint8_t conn_purpose;

  uint8_t no_of_tx_completed_pkts;
  uint8_t no_of_tx_completed_sco_pkts;

  /* Connection state frequency hop type (Basic or Adapted) */
#define BASIC_PICONET 0
#define ADAPTED_PICONET 1
  uint8_t piconet_chnl_type;

  /* This are used during the loop back test mode used in the bt-sig */
#define TEST_MODE_DISABLED 0
#define TEST_MODE_ENABLED 1
  uint8_t dev_in_test_mode;

#define TEST_IN_PAUSE 0
#define TRANSMITTER_TEST 1
#define LOOPBACK_TEST 2
  uint8_t test_type;

#define TEST_NOT_IN_PROGRESS 0
#define TEST_IN_PROGRESS 1
  uint8_t test_in_progress;

#define CONNECTED_ACTIVE_MODE 0
#define CONNECTED_HOLD_MODE 1
#define CONNECTED_SNIFF_MODE 2
#define PARK_STATE 3
#define CONNECTED_SNIFF_TRANSITION_MODE 4
  uint8_t connected_sub_state;

  uint8_t afh_poll_num; /* Number of polls at AFH instant */

#define AUTOMATICALLY_FLUSHABLE 0
  uint8_t enhanced_flush_pkt_type;
  uint8_t enhanced_flush_pending;

  /* These are used in the bt_flush stores the timeout and othe related info */
  /* in ms = auto_flush_timeout*0.625 */
  uint16_t auto_flush_timeout;     // default = 0 ->infinite
  uint16_t failed_contact_counter; // This is not yet implemented

  uint16_t sniff_mode_tout;
  uint16_t sniff_subrating;
  uint16_t sniff_poll_transition;
  /* Stores the poll interval and tx payload len */
  uint16_t poll_interval; // In terms of full slots

#define SLAVE_PTT_REQ_SENT BIT(0)
#define SLAVE_SETUP_COMPLETE_SENT BIT(1)
#define SLAVE_AU_RAND_SENT BIT(2)
#define SLAVE_TIMING_ACCURACY_REQ_SENT BIT(3)
#define SLAVE_MAX_SLOT_REQ_SENT BIT(4)
  uint16_t check_slave_pending_msg_flag;

  /* Pending events flag*/
#define ACL_CONNECTION_REQ BIT(0)
#define SCO_CONNECTION_REQ BIT(1)
#define ESCO_CONNECTION_REQ BIT(2)
  uint16_t received_req_flag;

#define BR_PKT_SLOT_CHECK 0x06  //! For BIT(1)& BIT(2)
#define EDR_PKT_SLOT_CHECK 0x60 //! For BIT(5)& BIT(6)
#define EDR_RATE_CHECK 0x18     //! For BIT(3)& BIT(4)

#define DONT_USE_FEC BIT(0)
#define BR_FEC_PKT_TYPE 0x770E
#define BR_NON_FEC_PKT_TYPE 0xBB16
#define BR_ALL_SLOT_PKT_TYPE 0xFF1E
#define BR_ONE_SLOT_PKT_TYPE 0x331E
#define BR_THREE_SLOT_PKT_TYPE 0x3F06
#define BR_FIVE_SLOT_PKT_TYPE 0xF306

#define EDR_ONE_SLOT_PKT_TYPE 0x3300
#define EDR_THREE_SLOT_PKT_TYPE 0x3006
#define EDR_FIVE_SLOT_PKT_TYPE 0x0306

#define BR_ALL_SLOT_PREFER 0x00
#define BR_ONE_SLOT_PKTS 0x01
#define BR_THREE_SLOT_PKTS 0x02
#define BR_FIVE_SLOT_PKTS 0x03

#define ALL_ACL_DATA_PKTS_VALID 0xCC18
#define ONLY_ONE_MBPS_PKTS_VALID 0xFF1E
#define ONLY_TWO_MBPS_PKTS_VALID 0x2204
#define ONLY_THREE_MBPS_PKTS_VALID 0x1102
#define EDR_PACKETS_NOT_ALLOWED 0x3306
#define EDR_PACKETS_ALLOWED 0x0000

#define USE_DM1_PKTS 0x00
#define USE_EDR_2MBPS_PKTS 0x01
#define USE_EDR_3MBPS_PKTS 0x02

#define EDR_ALL_SLOT_PREFER 0x00
#define EDR_ONE_SLOT_PKTS 0x01
#define EDR_THREE_SLOT_PKTS 0x02
#define EDR_FIVE_SLOT_PKTS 0x03

  uint16_t acl_pkt_type;
  uint16_t max_acl_pkt_len;
  uint16_t new_pkt_type;
  /* Valid SCO packet types */
  uint16_t sco_pkt_type;

  uint16_t link_supervision_timeout;
  uint8_t data_rate_type;

  /* Slot offset */
  uint16_t slot_offset;

  uint16_t poll_interval_pending;

  uint32_t auto_flush_timeout_in_us;
  /* in units of us*/
  uint32_t hci_tx_pkt_expiry_time;

  /* Used in the poll interval mechanism in the central mode */
  uint32_t poll_instant;

  /* Pending response from peripheral flag */
#define NAME_REQ_SENT BIT(0)
#define CONN_FEATURES_REQ_SENT BIT(1)
#define EXT_FEATURES_REQ_SENT BIT(2)
#define VERSION_REQ_SENT BIT(3)
#define HOST_CONNECTION_REQ_SENT BIT(4)
#define SETUP_COMPLETE_SENT BIT(5)
#define SYNC_LINK_REQ_SENT BIT(6)
#define HCI_LINK_KEY_REQ_SENT BIT(7)
#define PIN_CODE_REQ_TO_HCI_SENT BIT(8)
#define LMP_IN_RAND_SENT BIT(9)
#define LMP_COMB_KEY_SENT BIT(10)
#define LMP_AU_RAND_SENT BIT(11)
#define ENCRYPTION_MODE_REQ_SENT BIT(12)
#define ENC_KEY_SIZE_REQ_SENT BIT(13)
#define START_ENCRYPTION_SENT BIT(14)
#define TIMING_ACCURACY_REQ_SENT BIT(15)
#define PTT_REQ_SENT BIT(16)
#define IO_CAP_REQ_TO_HCI_SENT BIT(17)
#define LMP_IO_CAP_REQ_SENT BIT(18)
#define LMP_ENCAP_HEADER_SENT BIT(19)
#define LMP_ENCAP_PAYLOAD_SENT BIT(20)
#define LMP_SIMPLE_PAIRING_NUM_SENT BIT(21)
#define HCI_USER_CONFIRMATION_REQ_SENT BIT(22)
#define LMP_DHKEY_CHECK_SENT BIT(23)
#define HCI_PASSKEY_REQ_SENT BIT(24)
#define LMP_SIMPLE_PAIRING_CONFIRM_SENT BIT(25)
#define DHKEY_CHECK_PKT_RCVD BIT(26)
#define USER_CONFIRMATION_RCVD BIT(27)
#define SSP_NUM_RCVD BIT(28)
#define REMOTE_OOB_DATA_REQ_SENT BIT(29)
#define MAX_SLOT_REQ_SENT BIT(30)
  /* In case of esco, the flag is set only when the LMP_ESCO_LINK_REQ is rcvd. */
#define ESCO_SCO_CHANGE_PKT_REQ_SENT BIT(31)
  uint32_t check_pending_msg_flag; //! LMP level flag

#define FEATURES_REQ_SENT BIT(0)
#define FEATURES_REQ_EXT_SENT BIT(1)
#define HCI_NAME_REQ_SENT BIT(2)
#define HCI_VERSION_REQ_SENT BIT(3)
#define START_ENCRYPTION BIT(4)
#define STOP_ENCRYPTION BIT(5)
#define CHANGE_CONN_LINK_KEY BIT(6)
#define REFRESH_ENC_KEY BIT(7)
#define HCI_READ_CLK_OFFSET BIT(8)
#define AUTH_REQ_FROM_HCI BIT(9)
#define SYNC_CONN_REQ BIT(10)
#define NORMAL_CONN_REQ BIT(11)
#define SCO_DISCONNECT_RCVD BIT(12)
#define ROLE_SWITCH_REQ BIT(13)
#define CHANGE_PKT_TYPE BIT(14)
#define ESCO_LINK_REQ BIT(15)
#define ESCO_DISCONNECT_RCVD BIT(16)
#define CHANGE_ESCO_PKT_TYPE BIT(17)
#define CREATE_CON_CAN_EVENT_PEDNING BIT(18)
#define REMOTE_NAME_REQ_CAN_EVENT_PENDING BIT(19)
#define PAGE_TIMEOUT_EVENT_TO_HOST_PENDING BIT(20)
#define CHANGE_PKT_TYPE_MAX_SLOT_PENDING BIT(21)
#define MASTER_BR_EDR_PTT_IND BIT(22)
#define SLAVE_BR_EDR_PTT_IND BIT(23)
  uint32_t check_pending_req_from_hci;
  /* This variable stores the flags which contains the state machines in the lmp exchanges
   */
#define SETUP_COMPLETE BIT(0)
#define DEVICE_FEATURES_RCVD BIT(1)
#define DEVICE_EXT_FEATURES_RCVD BIT(2)
#define DEVICE_VERSION_INFO_RCVD BIT(3)
#define DEVICE_NAME_RCVD BIT(4)
#define SRES_RCVD BIT(5)
#define DEVICE_LINK_KEY_EXISTS BIT(6)
#define SRES_SENT BIT(7)
#define DEVICE_PTT_RCVD BIT(8)
#define DEVICE_AUTO_RATE_ENABLED BIT(9)
#define AFH_ENABLED BIT(10)
#define AUTHENTICATION_COMPLETE BIT(11)
#define PAUSE_ACLD_TRAFFIC BIT(12)
#define DEVICE_ENC_ENABLED BIT(13)
#define PUBLIC_KEY_SENT BIT(14)
#define PUBLIC_KEY_RCVD BIT(15)
#define SIMPLE_PAIRING_ENABLED BIT(16)
#define SLOT_OFFSET_RCVD BIT(18)
#define SENT_SIMPLE_PAIRING_NUM_ACCEPTED BIT(19)
#define USER_PASSKEY_RCVD BIT(20)
#define PENDING_SIMPLE_PAIRING_CONFIRM BIT(21)
#define DEVICE_ENC_PAUSED BIT(22)
#define CHANNEL_CLASSIFICATION_REQ_SENT BIT(23)
#define UPDATE_PACKET_TYPE BIT(24)
#define CHANNEL_CLASSIFICATION_RECVD BIT(25)
#define HOST_CONN_REQ_COMPLETED BIT(26)
  uint32_t device_status_flags;

  /* Link supervision timer*/
  uint32_t link_supervision_count;

  /* Role switch instant */
  uint32_t role_switch_instant;

  uint32_t master_native_clk;

  /* Updates Pending */
#define UPDATE_AFH_PENDING BIT(0)
#define ROLE_SWITCH_PENDING_INFO BIT(1)
#define UPDATE_AFH_CLASSIFICATION_PENDING BIT(2)
#define ACCEPT_CONN_TIMEOUT_PENDING BIT(3)
  uint32_t updates_pending;
  /* Stores of the afh instant it should follow */
  uint32_t afh_instant;

  uint8_t role_switch_progress;
  uint8_t tdd_switch_timeout;

  uint32_t afh_classification_instant;

#define ACCEPT_TIMEOUT_PENDING BIT(0)
  uint16_t accept_timeout_flags;
  uint32_t accept_timeout_instant;

  uint8_t lmp_ext_tx_pending;

  /* Security State machine flags */
#define SENT_AU_RAND BIT(0)
#define AURAND_NOT_ACCEPTED BIT(1)
#define RECVD_AU_RAND BIT(2)
#define RECVD_SRES BIT(3)
#define START_ENC_REQ_RCVD BIT(4)
#define SET_FLAG(a, b) (a |= b)
  uint8_t sec_sm_flags_1;

  uint8_t remote_initiated_conn;
  uint8_t dut_initiated_conn;

  uint32_t lmp_detach_instant;
} acl_link_mgmt_info_t;

typedef struct acl_pwr_ctrl_info_s{
  /* This variables are used to store rssi related
   * info -present rssi,rssi avg
   */
  int8_t rssi;
  int8_t rssi_average;
  uint8_t rssi_avg_pkt_count;

  /* These are used in the power control whether to increase/decrease
   * the power levels
   */
  uint8_t max_power_reached;
  uint8_t min_power_reached;
  /* tx power level */
  uint8_t tx_power_index;
  uint8_t min_allowed_power_index;
  uint8_t max_allowed_power_index;
#define ENABLE_POWER_CNTRL 1
#define DISABLE_POWER_CNTRL 0
  uint8_t enable_power_control;
  int16_t rssi_average_sum;
} acl_pwr_ctrl_info_t;

typedef struct acl_enc_info_s{
  /* Security
   * This variables store the encryption related info
   */
#define ENCRYPT_EN BIT(0)
#define DECRYPT_EN BIT(1)
  uint8_t encryption_flags;

  /* This is used in the security process */
  uint8_t change_lk_for_legacy_device;

  uint8_t enc_key_size;

  /* These are used in the pairing process */
  uint8_t pin_length;

  /* Encapsulated PDUs */
  uint8_t encap_payload_len;
  uint8_t encap_payload_cnt;

#define OOB_COMMITMENT_CHECK_PASS 0
#define OOB_COMMITMENT_CHECK_FAIL 1
  uint8_t oob_commitment_check;

#define NUMERIC_COMPARISON 0x1
#define OUT_OF_BAND 0x2
#define PASSKEY_ENTRY 0x3
  uint8_t auth_protocol;
#define AUTH_INITIATOR 0x0
#define AUTH_RESPONDER 0x1
  uint8_t auth_mode;
  uint8_t passkey_entry_round;

  /* Flag decides what paring should be initiated */
#define LEGACY_PAIRING 0
#define SECURE_SIMPLE_PAIRING 1
  uint8_t pairing_procedure;

  /* Flag to remember if we received a positive link key reply
   * or negative link reply, in case of auth req command from host */
#define PAIRING_NOT_REQUIRED 0
#define PAIRING_REQUIRED 1
  uint8_t pairing_requirement;

  /* Flag whether user has confirmed the reply correctly or not */
#define USER_CONFIRMATION_POS_REPLY 0
#define USER_CONFIRMATION_NEG_REPLY 1
  uint8_t user_confirmation_reply;

#define E0_ENCRYPTION 0x0
#define BT_AES_ENCRYPTION 0x1

  uint8_t encryption_type;

  uint8_t link_key[LINK_KEY_LEN];
  uint8_t auth_link_key[LINK_KEY_LEN];
  uint8_t device_key[LINK_KEY_LEN];

  uint8_t enc_key_kc[ENC_KEY_KC_MAX_LEN];
  uint8_t pin[MAX_PIN_CODE_LEN];
  uint8_t ssp_rand_peer[RAND_NUM_SIZE];
  uint8_t ssp_rand_own[RAND_NUM_SIZE];
  uint8_t ssp_commitment[SIZE_OF_C_VAL];
  uint8_t r[SIZE_OF_R];

  uint8_t dh_key[DHKEY_LEN];
  uint8_t public_key[PUBLIC_KEY_LEN_256];

  uint16_t authentication_payload_timeout;

  uint32_t passkey_display;
} acl_enc_info_t;
/* Structures and their member value defines */
/* This structure comprises remote device info elements*/

typedef struct acl_peer_info_s{
  struct afh_classification_stats_s afh_classification_stats;

  struct acl_dev_mgmt_info_s acl_dev_mgmt_info;

  struct acl_link_mgmt_info_s acl_link_mgmt_info;

  struct acl_enc_info_s acl_enc_info;

  struct conn_pwr_ctrl_info_s conn_pwr_ctrl_info;
  /* esco info */
  struct esco_info_s esco_info;

  struct esco_link_params_s esco_link_params[MAX_NUM_ESCO_HANDLES];

  struct sco_link_params_s sco_link_params[MAX_NUM_SCO_HANDLES];

  struct sco_update_params_s sco_update_params[MAX_NUM_SCO_HANDLES];

  struct test_mode_params_s test_mode_params;

  struct sniff_params_s sniff_params;

  /* Queue for acl-c packets */
  queue_t tx_aclc_q;

  /* Queue for acl-d packets */
  queue_t tx_acld_q;
} acl_peer_info_t;

typedef struct link_key_info_s{
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint8_t link_key[LINK_KEY_LEN];
} link_key_info_t;

/* This structure comprises the our device info elements*/
typedef struct btc_dev_mgmt_info_s{
/* To store the device mac addresses */
#define MAC_ADDR_LEN 6
  uint8_t bt_mac_addr[MAC_ADDR_LEN];
  uint8_t channel_assessment_mode;

  /* To keep track of connection handles*/
  uint16_t conn_handle_bitmap;
  uint16_t non_conn_bt_scalar;
  uint16_t non_conn_tx_pwr_value;
  uint16_t bt_fail_cnt;
  uint16_t bt_pass_cnt;
  uint16_t pkt_mask_map;
  uint8_t bt_rate_inx;
  uint8_t no_of_periph_connected;
#define CHANNEL_MAP_BYTES 10
  uint8_t host_classification_channel_map[CHANNEL_MAP_BYTES];
  uint8_t host_ch_classification_cmd;

#define FEATURE_MASK_LENGTH 8
  uint8_t features_bitmap[FEATURE_MASK_LENGTH];

  /* The following info is from site
   * https://www.bluetooth.org/Technical/AssignedNumbers/link_manager.htm
   */
#define BT_SPEC_1_0B 0
#define BT_SPEC_1_1 1
#define BT_SPEC_1_2 2
#define BT_SPEC_2_0_EDR 3
#define BT_SPEC_2_1_EDR 4
#define BT_SPEC_3_0_HS 5
#define BT_SPEC_4_0 6
#ifdef LE_DATA_PKT_LEN_EXT
#define BT_SPEC_4_1 7
#define BT_SPEC_4_2 8
#endif
#if (defined LE_2MBPS || defined LE_LONG_RANGE)
#define BT_SPEC_5_0 9
#endif
  uint8_t vers_number;

  /* Stored the controller revision and version number */
  uint8_t hci_vers_num;
  uint8_t max_acl_buffers;

  /* This bitmap is used to convey the controller extended features to host
   * and a remote device */
#define EXT_FEATURE_MASK_LENGTH 8
  uint8_t ext_features_bitmap[EXT_FEATURE_MASK_LENGTH];

  uint8_t ext_features_bitmap_page2[EXT_FEATURE_MASK_LENGTH];

  /* This key is used in generating the p_256 public key can be used
   * for the bt-classic and bt-le secure connections.
   */
#define PRIVATE_KEY_LEN_P256 32
  uint8_t private_key_p_256[PRIVATE_KEY_LEN_P256];

#define LMP_SUB_VERSION_NUMBER_NUM  0x0100
#define BT_SPEC_5_4   13
  /* This id is to indicate the manufacturer's name
   * Currently we use the special id 65535
   * as Bluetooth SIG didn't assign any ID
   * to Repdine yet */
#define IOT_COMP_ID 0xFFFF /* Reserved for testing*/
  uint16_t comp_id;

  /* This should be increased for every different lmp & bbp implementation of RSI */
#define EVEREST_SUB_VERSION_NUMBER 0              /* For CSR 0x22BB */
#define EVEREST_SUB_VERSION_NUMBER_NON_ROM 0x0100 /* For CSR 0x22BB */
  uint16_t sub_vers_number;
  uint16_t hci_revision;

  /* This is used by the controller to maintain the max supported pages */
  uint8_t max_page_num;

  /* This is used by the controller to maintain its lmp_verison_number */
  uint8_t lmp_vers_num;

  /* To store the device name length */
  uint8_t device_name_len;

  /* This is used by the controller to maintain its max supported key size */
  uint8_t max_supported_enc_key_size;

  uint8_t no_of_link_key_stored;

  /* This is used by the controller to maintain its max slots */
#define FIVE_SLOTS 5
#define THREE_SLOTS 3
#define ONE_SLOT 1
  uint8_t max_slots;

  /* This is used by the controller to tell its drift and jitter to third party
   * in the sniff mode
   */
#define DEFAULT_DRIFT 250
  uint8_t drift;
#define DEFAULT_JITTER 10
  uint8_t jitter;

  /* This is used by the controller to store its io_capablities */
#define DISPLAY_ONLY 0
#define DISPLAY_YES_NO 1
#define KEYBOARD_ONLY 2
#define NO_INPUT_NO_OUTPUT 3
  uint8_t io_capabilities;

  /* This is presently for the hci commands read_auth_enable and write_auth_enable
   * staus
   */
  uint8_t auth_enable;

  /* This are stored by the controller for the default oob_data is present or not
   */
#define OOB_AUTH_DATA_NOT_AVAILABLE 0
#define OOB_AUTH_DATA_AVAILABLE 1
  uint8_t oob_authentication_data;

  /* These are stored by the controller to store the authentication requirment from
   * the host*/
#define MITM_PROTCTN_NOT_REQD_NO_BONDING 0
#define MITM_PROTCTN_REQD_NO_BONDING 1
#define MITM_PROTCTN_NOT_REQD_DEDICATED_BONDING 2
#define MITM_PROTCTN_REQD_DEDICATED_BONDING 3
#define MITM_PROTCTN_NOT_REQD_GEN_BONDING 4
#define MITM_PROTCTN_REQD_GEN_BONDING 5
  uint8_t authentication_requirement;

  /* This variable was for the channel driven quality rate feature
   */
#define AUTO_RATE_DISABLED 0
#define AUTO_RATE_ENABLED 1
  uint8_t auto_rate;

  /* This parameter is used to calculate the poll interval in the quality of service
   * No significant use of this variable is not observed
   */
  uint8_t nbc;

  /* These variable used to store the max sco packet length supported
   * by the controller
   */
  uint8_t max_sco_pkt_len;

#define MANDATORY_SCAN_MODE 0

  /* Scan Parameters  and variable is used whether scanning should be done after disconnection*/
#define NO_SCAN_EN 0
#define INQUIRY_SCAN_EN 1
#define PAGE_SCAN_EN 2
#define INQ_PAGE_SCAN_EN 3
  uint8_t scan_enable;

  /* This variable whether the controller supports fixed/variable pin
   */
#define VARIABLE_PIN 0
#define FIXED_PIN 1
  uint8_t fixed_pin;

  /* This variable is used to store the gc value for the non connected states
   */
  int8_t tx_non_connected_power_index;

  /* Used to store the controller class of device */
  uint8_t class_of_dev[CLASS_OF_DEV_LEN];

  uint8_t class_of_dev_mask[CLASS_OF_DEV_LEN];

  /* To store the device local name */
  uint8_t device_name[MAX_BT_DEVICE_NAME_LEN];

  /* These variables are used to store the inquiry responses info
   */
#define SIZE_OF_EIR 240
  uint8_t extended_inquiry_response[SIZE_OF_EIR];

  /* Voice setting parameter */
#define CVSD_AIR_CODING_FORMAT 0x0
#define MU_LAW_AIR_CODING_FORMAT 0x1
#define A_LAW_AIR_CODING_FORMAT 0x2
#define TRANSPARENT_AIR_CODING_FORMAT 0x3
  uint16_t voice_setting;

  /* This variable is used mainly for the reading and writing the connection
   * accept tout in the hci layer.
   */
  uint16_t conn_accept_tout;

  /* This variable is used to store the default link policy settings set
   * by the host
   */
  uint16_t link_policy_settings;

  /* This is used by the controller to maintain its lmp_sub_verison_number */
  uint16_t lmp_sub_vers_num;

  /* To store the max acl pkt len stored by the controller and give it to host
   */
  uint16_t max_acl_pkt_len;

  /* These variables are used to store the gx,gy and r1 vectors
   * these are independent of the slaves
   */
  uint8_t private_key[PRIVATE_KEY_LEN];
  uint8_t private_key_256[PRIVATE_KEY_LEN_256];


  uint8_t public_key[PUBLIC_KEY_LEN];
  uint8_t debug_public_key[PUBLIC_KEY_LEN];

  uint8_t public_key_256[PUBLIC_KEY_LEN_256];
  uint8_t debug_public_key_256[PUBLIC_KEY_LEN_256];

  uint8_t debug_private_key[PRIVATE_KEY_LEN];

  /* These variables are used to store the ssp debug mode
   */
#define SSP_DEBUG_LINK_KEY 1
#define SSP_COMB_KEY 0
  uint8_t ssp_link_key_type;

  /* This is fixed value used in the key generation the h/w engines in different steps */
  uint8_t key_id[SIZE_OF_KEY_ID];

  /*FIXME:This variable is used many places in the ssp to finally conform
   * these need to understand some more code
   */
  uint8_t r[SIZE_OF_R];

  /* Device for which shared key calculation is going on */
  uint8_t shared_key_calc_pending_slave;

  /* Devices for which shared key calculation is yet to be triggered */
  uint8_t shared_key_trig_pending_slave;

  /* These variables are to sore the ssp debug mode
   */
#define SSP_DEBUG_MODE_ENABLED 1
#define SSP_DEBUG_MODE_DISABLED 0
  uint8_t ssp_debug_mode;

#define DEVICE_TEST_MODE_DISABLED 0
#define DEVICE_TEST_MODE_ENABLED 1
  uint8_t host_dev_test_mode_enable;

  uint8_t testmode_block_host_pkts;
    
  uint8_t bt_tx_power_from_host;

  uint16_t bt_oper_mode;

  uint8_t event_filter_type;
  uint8_t event_filter_cond_type;
  uint8_t event_auto_accept_flag;

  struct link_key_info_s link_key_info[MAX_NUM_OF_STORED_LINK_KEY];

#define LE_MODE_EN BIT(0)
#define SIMUL_LE_BREDR_EN BIT(1)
#define SIMPLE_PAIRING_EN BIT(2)
#define ROLE_CHANGED BIT(3)
#define PUBLIC_KEY_CALC_PENDING BIT(4)
#define SHARED_KEY_CALC_PENDING BIT(5)
#define ROLE_SWITCH_EN BIT(6)
#define SECURE_CONNECTIONS_HOST_SUPPORT BIT(7)

  uint8_t dev_status_flags;
  uint8_t free_lt_addr_bitmap;
  uint16_t afh_min_interval_from_host;
  uint16_t afh_max_interval_from_host;
  uint16_t afh_interval_from_host;
} btc_dev_mgmt_info_t;

typedef struct btc_dev_info_s{
  struct btc_dev_mgmt_info_s btc_dev_mgmt_info;

  struct btc_hci_params_s btc_hci_params;

  /* Inquiry state parameters structure */
  struct inquiry_params_s inquiry_params;

  /* Inquiry scan parameters structure */
  struct inquiry_scan_params_s inquiry_scan_params;

  /* Page parameters */
  struct page_params_s page_params;

  /* Page scan parameters */
  struct page_scan_params_s page_scan_params;

  /* The acl peer info structures are used to contain the
   * required remote connected acl peer information
   */
  struct acl_peer_info_s *acl_peer_info[MAX_NUM_ACL_CONNS];

  /* Statistics */
  struct btc_hci_debug_stats_s btc_hci_debug_stats;
  struct btc_lmp_debug_stats_s btc_lmp_debug_stats;
} btc_dev_info_t;

typedef struct conn_params_s{
  uint8_t bd_addr[MAC_ADDR_LEN];
  uint16_t pkt_type;
  uint8_t page_scan_rep_mode;
  uint8_t reserved;
  uint16_t clk_offset;
  uint8_t allow_role_switch;
} conn_params_t;

#endif
