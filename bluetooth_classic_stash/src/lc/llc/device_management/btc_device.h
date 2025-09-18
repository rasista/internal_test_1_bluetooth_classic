#ifndef BTC_DEVICE_H_
#define BTC_DEVICE_H_
#include "btc_common_defines.h"
#include "btc_rail.h"
#include "inquiry.h"
#include "inquiry_scan.h"
#include "sl_btc_seq_interface.h"
#include "page.h"
#include "page_scan.h"
#include "esco.h"
#include "acl_common.h"
#include "string.h"
#include "btc_mem_mgmt.h"
#include "test_mode.h"
#include <stdint.h>

#ifdef SIM
#define BTC_DEV_PTR get_btc_device_pointer_addr()
#else
#define BTC_DEV_PTR (&btc_dev)
#endif

#define BTC_CENTRAL               1
#define BTC_PERIPHERAL            0
#define SCALING_FACTOR            2
#define BTC_TX_PKT_LEN            (276 * SCALING_FACTOR)
#define BTC_RAIL_RX_TIMEOUT       (60 * SCALING_FACTOR)
#define BIT(X)                    (1 << X)         // Used to set BITS 0,1,2 etc
#define IS_BIT_SET(A, B)          (A & BIT(B))     // Used to check if a particular Bit is set or not
#define SET_BIT(X, N)             (X |= BIT(N))    // used to set the BITS
#define CLEAR_BIT(X, N)           (X &= ~(BIT(N))) // used to clear the exisisting values of BITS(reset)
#define MAX_NUMBER_BTC_DEVICES    4
#define EVENT_BUFF_LEN            100
// TODO: Revert back again to previous value (3)
#define MAX_NUMBER_OF_CONNECTIONS 2
#define CONN_SUCCESS              1
#define CONN_FAILURE              0
#define FHS_PACKET_LENGTH         276
#define NAK                       0
#define ACK                       1
#define TRUE                      1
#define FALSE                     0
#define MASK_2NP0                0x03
#define RAIL_ASSERT              btc_rail_assert(__FILE__, __LINE__)

#define TOGGLE_BIT(A, B) (A ^= (BIT(B)))

#define ID_1_TX_FAIL_BIT 0
#define ID_2_TX_FAIL_BIT 1
#define FHS_TX_FAIL_BIT  2

#define MAX_BD_CLK_COUNT_HANDLES 8
#define TESTMODE_ENABLED         0 //Testmodes is enabled.
#define LOOPBACKMODE             0 //loopback mode: the received packet along with the header needs to be added to the Tx queue.
#define CONTINUOUS_RX            0 //continuous rx: It will go in schedule rx state continuously.

#define NO_SYNC_LINK

#ifdef NO_SYNC_LINK
#define R0_N_PAGE 1
#define R1_N_PAGE 128
#define R2_N_PAGE 256
#else
#ifdef ONE_SYNC_LINK
#define R0_N_PAGE 2
#define R1_N_PAGE 256
#define R2_N_PAGE 512
#else
#define R0_N_PAGE 3
#define R1_N_PAGE 384
#define R2_N_PAGE 768
#endif
#endif

#define STANDARD_SCAN   0
#define INTERLACED_SCAN 1
#define GENERIC_INQUIRY_ADDRESS 0x9E8B33
#define BTC_CHANNELS_COUNT 79

#define BT_CLOCK_MASK 0x0FFFFFFF

#define BT_ADD_CLOCK(x, y) ((uint32_t)(((x) + (y)) & BT_CLOCK_MASK))
#define BT_SUB_CLOCK(x, y) ((uint32_t)(((x) - (y)) & BT_CLOCK_MASK))

#define CLK1_GT_CLK2 1
#define EQL_CLKS     2
#define CLK1_LT_CLK2 3

/* x < y ? */
#define IS_BT_CLOCK_LT(x, y) (comp_clock(x, y) == CLK1_LT_CLK2)
/* x <= y ? */
#define IS_BT_CLOCK_LTE(x, y) (comp_clock(x, y) >= EQL_CLKS)

/* x > y ? */
#define IS_BT_CLOCK_GT(x, y) (comp_clock(x, y) == CLK1_GT_CLK2)

/* x >= y ? */
#define IS_BT_CLOCK_GTE(x, y) (comp_clock(x, y) <= EQL_CLKS)

/* MIN(x,y) */
#define BT_MIN_CLOCK(x, y) ((IS_BT_CLOCK_LT(x, y)) ? x : y)

#define BTC_TSF_DIFF(a, b) ((a - b) & 0x7FFFFFFF)

#define CLASS_OF_DEV_LEN          3

/* FHS Packet fields */
#define FHS_EIR_BIT              BIT(10)
#define FHS_RESERVED_FIELD       0x290
#define MASK_8_BITS              0xFF         //Mask for lower 8 bits
#define MASK_3_BITS              0x7          // Mask for 3 least significant bits
#define MAX_16_BIT_VALUE         0xFFFF       // Maximum value for a 16-bit unsigned integer
#define MASK_14_BITS             0x3FFF       // Mask for upper 14 bits of a 16-bit value
#define MASK_13_BITS             0x1FFF       // Mask for the upper 13 bits of a 16-bit value
#define MASK_LOWER_30_BITS       0xFFFFFFC    // Mask for the lower 30 bits of a 32-bit value
#define MAX_15_BIT_VALUE         0x7FFF       // Maximum value for a 15-bit unsigned integer
#define MASK_15_BITS             0x7FFF
#define CLK_MASK_27_2_BITS       0x03FFFFFF   // Mask to extract the bits 27-2 in clock
#define BTC_DEFAULT_SP_IN_FHS  2
#define CONFIGURE_COD_NAP_UAP(cod, nap, uap) (((cod) << 24) | ((nap) << 8) | (uap))
#define CONFIGURE_MODE_CLOCK_LTADDR(mode, clock, lt_addr) \
  ((((mode) & 0x07) << 29) | (((clock >> 2) & 0x3FFFFFF) << 3) | ((lt_addr) & 0x07))

#define CONFIGURE_FHS_HEADER(reserved, sr, sp, eir, lap, parity)            \
  ((((sp) & 0x3) << 30) | (((sr) & 0x3) << 28) | (((reserved) & 0x1) << 27) \
   | (((eir) & 0x1) << 26) | (((lap) & 0xFFFFFF) << 2) | ((parity) & 0x3))

#define FHS_LENGTH              5
#define MANDATORY_SCAN_MODE     0
#define BT_TX_PKT_HDR_LEN       2 // Bytes
#define BT_FHS_PKT_PAYLOAD_LEN  18
#define BT_FHS_PKT_LEN          BT_TX_PKT_HDR_LEN + BT_FHS_PKT_PAYLOAD_LEN

#define BASIC_RATE              0
#define ENHANCED_RATE           1
#define BASE_FREQUENCY 2402

#ifdef SIM
#define CLK_SCALE_FACTOR 0
#define TSF_SCALE_FACTOR 0
#define HALF_SLOT_FACTOR 1
#else
#define CLK_SCALE_FACTOR 4
#define TSF_SCALE_FACTOR 3
#define HALF_SLOT_FACTOR 2
#endif

#define HARD_RX_END_DISABLED                   0  // TEMP to keep in scheduled Rx mode only instead of normal Rx
#define RX_TRANSITIONS_END_SCHEDULE_DISABLED   0  // TEMP to define hard window end boundary for the reception
#define SLOT_WIDTH                             2  // defines slot interval, 2 half slots = 625 us

//Macros for extraction of bits for Packet header fields
#define PKT_HEADER_PKT_TYPE_MASK  0xF   //4-bit TYPE
#define PKT_HEADER_PKT_TYPE_POS   3     //bits 3-6
#define SET_TX_PKT_HEADER_TYPE(buf, pkt_type)   (buf = buf  | (pkt_type & PKT_HEADER_PKT_TYPE_MASK) << PKT_HEADER_PKT_TYPE_POS)
// Macro to get the packet type from the buffer
#define GET_RX_PKT_HEADER_TYPE(buf)   ((buf >> PKT_HEADER_PKT_TYPE_POS) & PKT_HEADER_PKT_TYPE_MASK)
// Macros to extract length from payload header
#define EXTRACT_EIR_SINGLE_SLOT_PAYLOAD_LENGTH(payload_header) (((payload_header) & 0xF8) >> 3)
#define EXTRACT_EIR_MULTI_SLOT_PAYLOAD_LENGTH(payload_header_ptr) ((((uint16_t)((uint8_t *)(payload_header_ptr))[0]) >> 3) \
                                                                   | ((((uint16_t)((uint8_t *)(payload_header_ptr))[1]) & 0x03) << 8))
#define EXTRACT_LLID(payload_header_ptr) ((((uint8_t *)(payload_header_ptr))[0]) & 0x03)

// Define the number of packet types and data rates
#define NUM_PACKET_TYPES 16
#define NUM_DATA_RATES 2

extern const uint8_t packet_type_slot_lookup_table[NUM_PACKET_TYPES][NUM_DATA_RATES];
#define FIRST_CONN_POLL_PKT 0x0288
#define FIRST_CONN_NULL_PKT 0x0280
#define CONN_NULL_PKT 0x0000

#define EXTRACT_FHS_HEADER(fhs_header, reserved, sr, undefined, eir, lap, parity) \
  do {                                                                            \
    reserved = ((fhs_header >> 30) & 0x3);                                        \
    sr = ((fhs_header >> 28) & 0x3);                                              \
    undefined = ((fhs_header >> 27) & 0x1);                                       \
    eir = ((fhs_header >> 26) & 0x1);                                             \
    lap = ((fhs_header >> 2) & 0xFFFFFF);                                         \
    parity = (fhs_header & 0x3);                                                  \
  } while (0)

#define EXTRACT_LAP_ADDR_BYTE(value) (((value) >> 2) & 0xFF)

#define HEC_PASS 1
#define HEC_FAIL 0
#define CRC_PASS 1
#define CRC_FAIL 0

#define CENTRAL_TO_PERIPHERAL 0 // Direction of the packet from central to peripheral
#define PERIPHERAL_TO_CENTRAL 1 // Direction of the packet from peripheral to central

typedef enum btc_csa_procedure_e btc_csa_procedure_t;
typedef enum btc_bd_clk_identifier_e btc_bd_clk_identifier_t;

typedef struct  lpw_sch_periodic_queue_s{
  btc_trx_configs_t *head;
  uint8_t queue_size;
}lpw_sch_periodic_queue_t;

typedef struct lpw_scheduler_info_s{
  uint8_t is_aperiodic_role_punctured; //   this will help identify at the end of periodic role if abort needs to be sent to aperiodic role
  uint32_t aperiodic_end_tsf;
  sm_t *l1_scheduled_sm; // used to store aperiodic activities scheduled by L1 scheduler
  btc_trx_configs_t *l2_sched_periodic_btc_trx_configs;  //used to store trx configs for periodic activities
  btc_trx_configs_t *l2_sched_aperiodic_btc_trx_configs; //used to store trx configs for aperiodic activities
  lpw_sch_periodic_queue_t lpw_sch_periodic_queue;
}lpw_scheduler_info_t;

typedef enum btc_bd_clk_counter_id {
  BTC_NATIVE_CLK,
  BTC_INIT_CLK,
  BTC_MASTER_CLK
} btc_bd_clk_counter_id_t;

typedef struct current_slot_info_s{
  uint8_t crc_status; // Variable to store the crc status, which we got from the rail
  uint8_t mic_status; // Variable to store the mic status, which we got from the rail
  uint8_t hec_status; // Variable to store the hec status, which we got from the rail
  uint8_t current_role_acl_lt_addr; // Current Acl LT address
  uint8_t current_role_esco_lt_addr; // Current Esco LT address
  uint8_t current_role_lt_addr; // Current Active Lt_address
}current_slot_info_t;

typedef struct roleswitch_data_s {
  uint8_t new_lt_addr;
  uint8_t old_connection_idx; // old Logical transport address before roleswitch
  uint8_t new_connection_idx;             // new Logical transport address after roleswitch
  uint16_t roleswitch_slot_offset; //specifies the slot offset
  uint32_t roleswitch_instant;    //specifies instant at which roleswitch happens
  uint32_t last_retry_instant;    //specifies the clock value instant after 16 slots[new conn timeout case]
  uint32_t class_of_device;       //Bluetooth device class
  uint32_t new_clk_roleswitch_instant; // new clock value at the time of roleswitch
  btc_fhs_rx_pkt_t received_rx_fhs_pkt; // FHS packet received during roleswitch
} roleswitch_data_t;

typedef struct header_s {
  uint16_t lt_addr : 3;
  uint16_t type : 4;
  uint16_t flow : 1;
  uint16_t arqn : 1;
  uint16_t seqn : 1;
} header_t;

typedef struct btc_dev_cb_s {
  uint8_t bd_addr[BD_ADDR_LEN];
  uint8_t LAP_address[LAP_ADDR_LEN];
  /* Used to store the controller class of device */
  uint8_t class_of_dev[CLASS_OF_DEV_LEN]; // will be updated from upper layer
  uint8_t uap;                   // Upper Access Part (UAP) - 8 bits
  uint8_t sub_state;
  uint8_t non_connected_tx_pwr_index; // Transmit Power variable for the non connected states
  uint8_t connection_info_idx;
  uint8_t fixed_btc_channel;
  uint8_t disable_channel_hopping;
  uint16_t nap;                  // Non-Applicable Part (NAP) - 16 bits
  uint32_t lap;                  // Lower Access Part (LAP) - 24 bits
  uint32_t sync_word[2];
  uint32_t fhs_pkt[FHS_LENGTH];
  uint32_t fhs_rx_pkt[FHS_LENGTH];
  page_t page;
  inquiry_t inquiry;
  page_scan_t page_scan;
  inquiry_scan_t inquiry_scan;
  connection_info_t connection_info[MAX_NUMBER_OF_CONNECTIONS];
  roleswitch_data_t roleswitch_data;
  sm_t *l2_scheduled_sm;
  current_slot_info_t current_slot_info;
  lpw_scheduler_info_t lpw_scheduler_info;
  btc_trx_configs_t *curr_rx_configs;
  header_t rx_pkt_hdr;
  bd_clk_counter_t bd_clk_counter_handle;
} btc_dev_t;

void init_btc_device(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void btc_activity_complete(sm_t *sm, void (*notify_func)(btc_internal_event_status_t, void *), uint32_t debug_event, uint32_t sm_idle_state);
void btc_common_stop(sm_t *sm, void (*activity_complete)(sm_t *), uint32_t idle_state);
void start_inquiry(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void start_inquiry_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void start_page_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void start_page(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void acl_central_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void acl_peripheral_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void esco_central_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void esco_peripheral_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void roleswitch_central_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_test_mode_encryption(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_encryption_parameters(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void update_encryption_parameters(connection_info_t *connection_info, btc_trx_configs_t *btc_trx_configs, uint8_t is_tx, uint8_t direction);
void update_acl_enc_pkt_count(uint8_t encryption_mode, uint8_t *pkt_count);
void roleswitch_peripheral_start_connection(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void start_test_mode(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_page(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_inquiry(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_page_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_inquiry_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_device_parameters_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_afh_parameters_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_sniff_parameters_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_device_parameters_esco(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_device_parameters_roleswitch(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_roleswitch_parameters(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void set_fixed_btc_channel(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_page(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_inquiry(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_inquiry_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_page_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_acl(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_acl_peripheral(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_esco(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_esco_peripheral(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_roleswitch(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void stop_roleswitch_peripheral(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
void btc_config_tx_pkt(bt_tx_pkt_t *tx_pkt);
uint32_t schedule_tx(btc_trx_configs_t *btc_trx_configs);
uint32_t schedule_rx(btc_trx_configs_t *btc_trx_configs);

void lpw_procedure_complete_event(uint8_t *hss_cmd, uint16_t buff_len);
bt_tx_pkt_t *get_pkt_from_tx_queue();
void configure_conn_slot_params(connection_info_t *connection_info_p, btc_bd_clk_identifier_t btc_bd_clk_identifier, btc_trx_configs_t *btc_trx_configs);
uint8_t comp_clock(uint32_t clock1, uint32_t clock2);
void configure_nonconn_slot_params(btc_trx_configs_t *btc_trx_configs, btc_bd_clk_identifier_t bt_bd_clk_identifier);
void update_channel_info(btc_trx_configs_t *btc_trx_configs, RAIL_SEQ_BtcFhConfig_t *btcFhData);
void configure_nonconn_slot_params_clk(btc_trx_configs_t *btc_trx_configs, btc_bd_clk_identifier_t btc_bd_clk_identifier);
void packetization_nonconn(btc_trx_configs_t *btc_trx_configs, uint8_t is_fhs_pkt);
uint8_t get_no_of_active_channels(uint8_t *addr);

uint32_t schedule_tx_rx_activity(btc_trx_configs_t *btc_trx_configs);
void procedure_activity_complete(sm_t *connection_info_sm, uint8_t state);
void configure_conn_slot_params_for_esco_window_start(connection_info_t *connection_info_p,
                                                      btc_trx_configs_t *btc_trx_configs,
                                                      esco_info_t *esco_info_p);
uint8_t is_crc_check_passed();
uint8_t is_hec_check_passed();
void form_fhs_pkt(btc_trx_configs_t *btc_trx_configs, uint32_t *fhs_pkt, uint8_t lt_addr, uint8_t old_lt_addr);

void llc_process_rx_fhs_pkt(uint32_t *fhs_pkt, btc_fhs_rx_pkt_t *fhs_rx_pkt, uint8_t *remote_bd_address);
void update_fhs_pkt_clk(btc_trx_configs_t *btc_trx_configs, uint32_t *fhs_pkt, uint8_t lt_addr, uint32_t clk);
void form_poll_and_null_pkt(btc_trx_configs_t *btc_trx_configs, uint8_t lt_addr, uint8_t pkt_type);
void configure_trx_acl_rx(btc_trx_configs_t *btc_trx_configs, uint8_t rate, uint32_t rxTimeout);
connection_info_t* get_connection_info(sm_t *connection_info_sm);
void configure_trx_acl_common(btc_trx_configs_t *btc_trx_configs, uint8_t rate);
void tx_packetization(connection_info_t *connection_info, btc_trx_configs_t *btc_trx_configs, uint8_t* tx_pkt, uint16_t tx_pkt_len);
void form_null_arqn_flow(btc_trx_configs_t *btc_trx_configs, connection_info_t *connection_info_p);
// void send_tx_pkt_ack_received_event(uint32_t status)
void send_tx_pkt_ack_notification_to_ulc();
void update_btc_channel_number(btc_trx_configs_t *btc_trx_configs, RAIL_SEQ_BtcFhConfig_t *btcFhData);
void get_roleswitch_parameter_slot_offset(uint8_t *hss_cmd, btc_dev_t *btc_dev_p);
#endif
