/**
 * @file rail_simulation.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-01-25
 *
 * @copyright Copyright (c) 2023-2025
 *
 */
#ifndef RAIL_SIMULATION_H
#define RAIL_SIMULATION_H
#include "btc_common_defines.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "stdint.h"
#include "string.h"
#include "sl_host_to_lpw_interface.h"
#include "sl_structs_btc_seq_interface.h"

#define SCALING_FACTOR           2
#define MAX_BD_CLK_COUNT_HANDLES 8

#define SIM_HEC_BIT 1 // Macro mimicing HEC bit is set from rail
#define SIM_MIC_BIT 1 // Macro mimicing MIC bit is set from rail
#define SIM_CRC_BIT 1 // Macro mimicing CRC bit is set from rail
#define MAX_NUMBER_BTC_DEVICES   4
#define BUFC_SIZE                250

typedef enum btc_csa_procedure_e btc_csa_procedure_t;
typedef enum btc_packet_type_enum { FHS, EIR, DH1, DH3, DH5 } btc_packet_type_enum_t;
typedef enum current_slot_s { IS_TX, IS_RX, IS_BACKOFF } current_slot_t;
typedef struct btc_trx_configs_s btc_trx_configs_t;
typedef struct buffc_s buffc_t;
extern uint8_t *received_rx_pkt;
extern uint16_t received_rx_pkt_len;

typedef enum {
  BTCCLKMODE_NONE, // no BTC clock resynchro
  BTCCLKMODE_INIT, // for inquiry scan or page scan : At next Rx packet, set BtcReference=0 and ProtimerReference=packet_Rx_date
  BTCCLKMODE_AUTO  // update ProtimerReference based on next packet Rx date
} BtcClkMode_t;

void BTC_post_notification_to_ulc(pkb_t *pkb);
void RAIL_SEQ_SendMbox(uint32_t event);
void init_btc_device_handler();
btc_dev_t *get_btc_device_pointer_addr();
void log2vcd_prints_wrapper(sm_t *l2_scheduled_sm);
uint32_t RAIL_SEQ_BTC_GetBtcDate(uint8_t ClkId);
uint32_t rail_init_bd_clk_counter(uint32_t clk_offset);
uint32_t RAIL_SEQ_BTC_ConvertBtcDate(uint8_t ClkId, uint32_t btcDate);
uint32_t rail_get_channel(btc_csa_procedure_t csa_procedure, uint32_t bd_clk);
uint32_t rail_schedule_id_tx(btc_trx_configs_t *txrx_configs);
uint32_t rail_schedule_tx(btc_trx_configs_t *txrx_configs);
uint32_t rail_schedule_rx(btc_trx_configs_t *txrx_configs);
uint32_t rail_schedule_id_rx(btc_trx_configs_t *txrx_configs);
void rail_lpw_procedure_complete_event(uint8_t *buffer, uint16_t buff_len);
uint32_t get_current_clk();
void process_rx_header_info();
uint32_t is_buffc_triggered(buffc_t *buffc);
buffc_t *trigger_buffc_if_payload_present(buffc_t *buffc, uint16_t length);
void rail_process_arqn_stage1();
void rail_process_arqn_stage2();
void rail_wait_for_bufc_transfer_status();
uint8_t rail_get_hec_status();
uint8_t rail_get_crc_status();
uint8_t rail_get_mic_status();
RAIL_Status_t RAIL_SEQ_Idle(RAIL_IdleMode_t mode, bool wait);
void rail_assert(char *file, uint32_t line);
void RAIL_SEQ_BTC_setAccessAddress(uint32_t *accessAddress);
void RAIL_SEQ_BTC_FhCopro(RAIL_SEQ_BtcFhConfig_t *btcFhConfig, RAIL_SEQ_BtcFhResult_t *btcFhResult);
void RAIL_SEQ_BTC_InitBtcDate(uint32_t btc_clk_id, uint32_t btcReference, uint32_t protimerReference);
void RAIL_SEQ_BTC_ConfigAutoMode(uint32_t bd_clk_counter_id, BtcClkMode_t mode);
void rail_set_btc_clk_instance(bd_clk_counter_t *bd_clk_counter);
void RAIL_SEQ_BTC_GetLastRxDate(uint32_t bd_clk_counter_id, uint32_t *BtcDate, uint64_t *ProtimerDate);
void RAIL_SEQ_BTC_setHecInit(uint8_t hecInit);
void RAIL_SEQ_BTC_setCrcInit(uint16_t crcInit);
pkb_t* get_buffer_for_notification(uint8_t **buffer);
void RAIL_SEQ_BTC_setE0Encryption(RAIL_SEQ_E0EncryptionConfig_t *e0EncConfig);
void RAIL_SEQ_BTC_setAESEncryption(RAIL_SEQ_AESEncryptionConfig_t *aesEncConfig);
void RAIL_SEQ_SetChannel(uint8_t channel);
uint16_t RAIL_SEQ_BTC_SlotOffsetUs(uint8_t clkIdbforeRoleswitch, uint8_t clkIdafterRoleswitch);
#endif
