/**
 * @file btc_rail_intf.h
 * @author Hasan Ali Stationwala
 * @brief The file shall list the interfacing APIs for RAIL. Incase actuall RAIL
 *        library is not present, the APIs shall link against a mock in
 * UT/Simulaion environment
 * @version 0.1
 * @date 2023-09-27
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#ifndef __BTC_RAIL_INTF_H__
#define __BTC_RAIL_INTF_H__

#include "btc_common_defines.h"
#include "btc_device.h"
#ifdef RUN_TEST_MODE
  #define TEST_MODE_STATS_MSG 100
#endif
#ifndef SIM
#include "rail_seq.h"
#include "em_device.h"
#else
#include "protocol_btc_rail.h"
#endif
#include "sl_host_to_lpw_interface.h"
#include "sl_structs_btc_seq_interface.h"
#include "data_path.h"

#define BTC_MAX_CP_REQUESTS 6
#define BTC_RAIL_PKT_CB_THRESHOULD 4
#define BTC_MIN_RAIL_RX_TIMEOUT 166 // in microseconds, minimum duration to receive the packet header
#define BTC_MIN_RAIL_ID_RX_TIMEOUT 100 // in microseconds, minimum duration to receive the ID packet

typedef struct btc_rail_seq_intfc {
  uint32_t status;
} btc_rail_seq_intfc_t;

typedef enum btc_error_codes {
  NO_ERROR,
  ZERO_LEN_PKT,
  NO_BUFFERS_AVAILBLE,
  NO_BUFFERS_QUEUE_AVAILBLE,
  DMA_TRIGGERED_ALREADY,
  DMA_TRANSFER_DONE,
  DMA_TRIGGER_FAILED
}btc_error_codes_t;

void init_btc_device_handler();
extern uint8_t *received_rx_pkt;
extern uint16_t received_rx_pkt_len;
extern btc_dev_t btc_dev; // Global variable for the Bluetooth device structure
/* Global variable for buffer address communication between ULC and LLC */
extern volatile uint32_t g_tx_acl_buffer_address;

void BTC_post_notification_to_ulc(pkb_t *pkb);
void rail_btc_mbox(uint32_t msg_data);
void rail_user_btc_int(void);
void rail_set_btc_clk_instance(bd_clk_counter_t *bd_clk_counter);
void BTC_RAIL_SEQ_txCb(RAIL_SeqTxPacketInfo_t *pTxPktInfo);
void BTC_RAIL_SEQ_rxCb(RAIL_RxPacketInfo_t *pRxPkt, RAIL_RxPacketDetails_t *pRxPktDetails);
void BTC_RAIL_SEQ_rxSearchTimeoutCb(uint8_t rxTimeoutCause);
uint8_t BTC_RAIL_SEQ_RxPktCb(uint8_t *packet, uint32_t *pRxBytes);
uint32_t rail_schedule_id_tx(btc_trx_configs_t *btc_trx_configs);
uint32_t rail_schedule_id_rx(btc_trx_configs_t *btc_trx_configs);
uint32_t rail_schedule_tx(btc_trx_configs_t *btc_trx_configs);
uint32_t rail_schedule_rx(btc_trx_configs_t *btc_trx_configs);
uint32_t get_current_clk();
uint32_t rail_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk);
uint32_t rail_init_bd_clk_counter(uint32_t clk_offset);
void rail_process_rx_header_info();
buffc_t *trigger_buffc_if_payload_present(buffc_t *buffc, uint16_t length);
uint32_t rail_is_buffc_triggered();
void rail_process_arqn_stage1();
void rail_process_arqn_stage2();
void rail_wait_for_bufc_transfer_status();
uint8_t rail_get_mic_status();
uint8_t rail_get_crc_status();
uint8_t rail_get_hec_status();
void btc_rail_dma_done_cb(const void *pCpReq, uint32_t reqStatus);
pkb_t *get_buffer_for_notification(uint8_t **buffer);
void  *get_tx_from_fifo_if_available(void);
#endif
