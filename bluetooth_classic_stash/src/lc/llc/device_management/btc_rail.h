/**
 * @file btc_rail.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#ifndef BTC_RAIL_H_
#define BTC_RAIL_H_ \

#include "host_to_lpw_interface.h"
#include "btc_common_defines.h"
#include "stdint.h"

#ifdef SIM
// #include "rail_simulation.h"
#include "protocol_btc_rail.h"
#else
#include "protocol_btc.h"
#define sim_printf(msg, ...) \
  while (0)                  \
  ;
#endif

// These functions are used in log_to_vcd
/*
 #ifdef SIM
 #define LLC_ASSERT(msg, ...)                                                          \
   fprintf(stdout, "\n   %s  :  %d  - " msg " \n", __FILE__, __LINE__, ##__VA_ARGS__); \
   while (1)                                                                           \
   ;

 #else
 #define LLC_ASSERT(msg, ...) \
   while (1)                  \
   ;
 #endif
 */

/**
 * @brief
 *
 */
#define BTC_RAIL_FAIL_STATUS 2
#define BTC_RAIL_PASS_STATUS 0
#define PAGE_RESP_TIME_OUT   4
#define NEW_CONN_TIME_OUT    16
#define BUFC_SIZE            250
#define BTC_CLK_FRACTIONAL_THRESHOLD 12 // Remaining available time is: 312.5 - (12 * (312.5 / 16)) us = 78.125 us.

#define SET_CONF_FLAG_TX(var) (var = false)
#define SET_CONF_FLAG_RX(var) (var = true)
#define SET_CONF_FLAG_ID(var) (var = SL_RAIL_BTC_ID_PKT)
#define SET_CONF_FLAG_PERIODIC(var) (var = var | BIT(2))
#define SET_CONF_FLAG_QUEUED(var) (var = var | BIT(3))
#define SET_CONF_FLAG_DEQUEUED(var) (var = var & ~(BIT(3)))
#define SET_CONF_FLAG_NEW_WINDOW(var) (var = var | BIT(4))
#define RESET_CONF_FLAG_NEW_WINDOW(var) (var = var & ~(BIT(4)))

#define IS_CONF_FLAG_TX(var) (var == false)
#define IS_CONF_FLAG_RX(var) (var == true)
#define IS_CONF_FLAG_ID(var) (var == SL_RAIL_BTC_ID_PKT)
#define IS_CONF_FLAG_PERIODIC(var) (var & BIT(2))
#define IS_CONF_FLAG_NOT_QUEUED(var) (~(var & BIT(3)))
#define IS_CONF_FLAG_NEW_WINDOW(var) (var & BIT(4))
#define IS_CONF_FLAG_APERIODIC(var) (!(var & BIT(2)))

typedef struct bt_tx_pkt_s bt_tx_pkt_t;

/**
 * @brief identifies bd_clk boundary location
 * @details BT Classic bd_clk numbers are identified by a 27 MSBs of a 28 bit BD
 * clock value the BD Clock  ticks in evry 312.5 us hence the bd_clk duration is
 * 625us depending procedures have special significance of BD clock boundaries,
 * and hence the clock boundaries are identified as below
 *
 */
typedef enum btc_bd_clk_identifier_e {
  BTC_BD_CLK_2NP0, // BD clock value is of format 2N + 0(start of an even
                   // bd_clk)

  BTC_BD_CLK_2NP1, // BD clock value is of format 2N + 1(second half of an even
                   // bd_clk)

  BTC_BD_CLK_2NP2, // BD clock value is of format 2N + 2(start of an odd bd_clk)

  BTC_BD_CLK_2NP3 // BD clock value is of format 2N + 3(second_half of an odd
                  // bd_clk)
} btc_bd_clk_identifier_t;

/**
 * @brief Identifies the procedure for Channel Selection algorithm
 * @details different procedures that act as input to channel selection
 * algorithm are listed in this enum
 *
 */
typedef enum btc_csa_procedure_e {
  BTC_PAGE,         // Pagging procedure
  BTC_PAGE_SCAN,    // Page scan procedure
  BTC_INQUIRY,      // Inquiry procedure
  BTC_INQUIRY_SCAN, // Inquiry scan procedure
  BTC_CONNECTION,   // Connection procedure
  BTC_CONNECTED     // Connected procedure
} btc_csa_procedure_t;

/**
 * @brief Structure for tx packet
 *
 */

typedef struct bt_tx_pkt_s {
  uint8_t *pkt_start;
  uint16_t pkt_len;
  uint16_t retry_cnt;
} bt_tx_pkt_t;

/**
 * @brief Structure to populate parameters for TXRX activity
 *
 */
typedef struct btc_trx_configs_s {
  uint8_t flags;   // Tx(or)Rx BIT(0) , ID_PKT_IND BIT(1)
  uint8_t ignore_whitening;
  uint8_t ignore_rx_pkt;
  uint8_t rx_hdr_needed;
  uint8_t is_connected_procedure;
  uint8_t abort_rx_pkt;
  uint8_t *tx_pkt;
  uint8_t lap_addr[LAP_ADDR_LEN];
  uint16_t priority;     // priority to gate with in LPW and also to inform RAIL/DMP
                         // TODO: Check if uint16_t is needed, or more for priority
  uint16_t max_radio_usage_in_us;  // max radio usage time in us
#ifdef SIM
  RAIL_SEQ_BtcTrxConfig_d_t btc_rail_trx_config;
#else
  RAIL_SEQ_BtcTrxConfig_t btc_rail_trx_config;
#endif
  RAIL_SEQ_BtcFhConfig_t *btc_fh_data;
  RAIL_SEQ_BtcFhResult_t btcFhResult;
  struct sm_s *sm;   /* TODO: check if this is still needed */
  struct btc_trx_configs_s *next; // pointer to the next txrx config used in periodic role
  btc_rx_meta_data_t rx_meta_data;
  uint32_t tx_window;
  uint32_t rx_window;
  bd_clk_counter_t bd_clk_counter_handle; //counter_handle
  RAIL_SEQ_E0EncryptionConfig_t *e0EncConfig; // E0 encryption configuration
  RAIL_SEQ_AESEncryptionConfig_t *aesEncConfig; // AES encryption configuration
} btc_trx_configs_t;

typedef struct buffc_s {
  uint32_t data[BUFC_SIZE];
  int head; // Index to add data
  int tail; // Index to remove data
  int size;
  int count;
} buffc_t;

typedef struct btc_fhs_rx_pkt_s {
  uint8_t lt_addr;
  uint8_t fhs_rx_lap_addr[LAP_ADDR_LEN];
  uint8_t fhs_rx_bd_addr[BD_ADDR_LEN];
  uint32_t fhs_rx_clk;
  uint64_t fhs_rx_tsf;
  uint32_t rx_sync_word[2];
} btc_fhs_rx_pkt_t;

uint32_t btc_get_txrx_configs(btc_bd_clk_identifier_t bt_bd_clk_identifier,
                              btc_csa_procedure_t btc_csa_procedure,
                              btc_trx_configs_t *btc_trx_configs);
uint32_t btc_init_bd_clk_counter(uint32_t clk_offset);
uint32_t btc_schedule_id_tx(btc_trx_configs_t *btc_trx_configs);
uint32_t btc_schedule_tx(btc_trx_configs_t *btc_trx_configs);
uint32_t btc_schedule_rx(btc_trx_configs_t *btc_trx_configs);
uint32_t btc_schedule_id_rx(btc_trx_configs_t *btc_trx_configs);
uint32_t btc_get_current_bd_clk_cnt(bd_clk_counter_t *bd_clk_handle);
uint32_t btc_get_target_bd_clk(
  uint32_t current_bd_clk, btc_bd_clk_identifier_t btc_bd_clk_identifier);
uint32_t btc_get_current_clk();
void btc_rx_header_info();
buffc_t *btc_trigger_buffc(buffc_t *buffc, uint8_t length);
uint32_t btc_is_buffc_triggered(buffc_t *buffc);
void btc_arqn_stage1();
void btc_arqn_stage2();
void btc_wait_for_bufc_transfer_status();
void rail_configure_idle();
void rail_lpw_procedure_complete_event(uint8_t *buffer, uint16_t buff_len);
uint8_t btc_get_hec_status();
uint8_t btc_get_crc_status();
uint8_t btc_get_mic_status();
uint32_t btc_get_txrx_clk_configs(btc_bd_clk_identifier_t bt_bd_clk_identifier,
                                  btc_trx_configs_t *btc_trx_configs);
uint32_t btc_get_txrx_clk_configs_with_clk(uint32_t clk, btc_bd_clk_identifier_t bt_bd_clk_identifier,
                                           btc_trx_configs_t *btc_trx_configs);
uint32_t btc_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk);
void btc_rail_assert(char *file, uint32_t line);
void btc_tx_end_handler();
void btc_tx_abort_handler();
void btc_rx_done_handler();
void btc_rx_timeout_handler();
void btc_rx_header_received_handler();
void btc_rx_abort_handler();
void btc_get_txrx_clk_configs_with_target_time(btc_trx_configs_t *btc_trx_configs,
                                               uint32_t target_time);
uint32_t get_target_bd_clk(uint32_t current_bd_clk, btc_bd_clk_identifier_t btc_bd_clk_identifier);
uint32_t rail_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk);
uint32_t rail_init_bd_clk_counter(uint32_t clk_offset);
#endif
