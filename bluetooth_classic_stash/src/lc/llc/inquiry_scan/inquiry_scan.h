#ifndef INQUIRY_SCAN_H_
#define INQUIRY_SCAN_H_
#include "btc_common_defines.h"
#include "sl_btc_seq_interface.h"
#include "state_machine.h"
#include "btc_rail.h"

#define EIR_PKT_SIZE_IN_WORDS (260 / 4) // 255 bytes Max EIR packet size + 2 payload_header + 2 Tx header + 1 byte offset
#define INQUIRY_LT_ADDR 0x00

#define BT_INQUIRY_SCAN_HEC_INIT_VALUE     0x00
#define BT_INQUIRY_SCAN_CRC_INIT_VALUE     0x00

typedef struct inquiry_scan_s {
  uint8_t LAP_address[LAP_ADDR_LEN];
  uint32_t current_slot;
  uint32_t target_slot;
  uint8_t EIR_value;
  uint8_t curr_scan_type;
  uint8_t priority;
  uint8_t step_size;
  uint8_t eir_pkt_type;
  uint16_t eir_length;
  uint32_t end_tsf;
  uint32_t interval;
  uint32_t window;
  uint32_t scan_end_time;
  uint32_t sync_word[2];
  RAIL_SEQ_BtcFhConfig_t btc_fh_data;
  uint8_t NVal;
  BtcSeqSelect_t seqSelect;
  sm_t *inquiry_scan_sm;
  btc_trx_configs_t trx_config;
  uint32_t eir_pkt[EIR_PKT_SIZE_IN_WORDS];
} inquiry_scan_t;

void inquiry_scan_start_procedure_handler(sm_t *inquiry_scan_sm_p);
void inquiry_scan_rx_done_handler(sm_t *inquiry_scan_sm_p);
void inquiry_scan_rx_timeout_handler(sm_t *inquiry_scan_sm_p);
void inquiry_scan_fhs_tx_done_handler(sm_t *inquiry_scan_sm_p);
void inquiry_scan_fhs_tx_abort_handler(sm_t *inquiry_scan_sm_p);
void inquiry_scan_eir_tx_done_handler(sm_t *inquiry_scan_sm_p);
void inquiry_scan_eir_tx_abort_handler(sm_t *inquiry_scan_sm_p);
uint32_t inquiry_scan_duration_check(uint32_t inquiry_scan_time);
void inquiry_scan_activity_complete(sm_t *inquiry_scan_sm);
#endif
