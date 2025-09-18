#ifndef PAGE_SCAN_H_
#define PAGE_SCAN_H_
#include "btc_common_defines.h"
#include "sl_btc_seq_interface.h"
#include "page_scan_sm_autogen.h"
#include "state_machine.h"
#include "btc_rail.h"

#define CURRENT_SLOT_1 (curr_slot + 1) & MASK_2NP0
#define CURRENT_SLOT_2 (curr_slot + 2) & MASK_2NP0
#define CURRENT_SLOT_3 (curr_slot + 3) & MASK_2NP0
#define CURRENT_SLOT_4 (curr_slot + 4) & MASK_2NP0

// ---- Frequency Hopping Constants ----
#define BT_CLKE1_MASK              0x2     // Mask for CLKE1 bit (bit 1)
#define BT_N_COUNTER_MASK          0x1F    // Mask for N counter (5 bits, mod 32)

#define STATE_ENUM_BITS 3   // 0-8 range needs 3 bits
#define SLOT_BITS 2         // 4 possible slot numbers need 2 bits
#define IS_TX_BITS 1        // 0 or 1 needs 1 bit

typedef struct page_scan_s {
  uint8_t pscan_type; //specifies scan type- Standard scan(0) and Interlaced Scan(1)
  uint8_t page_scan_bd_address[BD_ADDR_LEN];
  uint8_t tx_fail_stats;
  uint8_t btc_txrx_fail_stats;
  uint8_t priority;
  uint8_t step_size;
  uint32_t end_tsf;
  uint32_t interval;
  uint32_t window;
  uint32_t page_scan_target_slot;
  uint32_t page_scan_abort_slot;
  uint32_t page_scan_end_time;
  uint32_t procedure_tout_instant; // specifies the clock value instant for timeout cases
  uint32_t sync_word[2];
  uint8_t remote_bd_address[BD_ADDR_LEN];
  RAIL_SEQ_BtcFhConfig_t btc_fh_data;
  BtcSeqSelect_t seqSelect;
  /* Base clock captured for Peripheral Page Response (PGRSS) N counting */
  uint32_t prp_base_clk;
  /* N counter and last update clock for PGRSS (peripheral response) */
  uint8_t prp_n;
  uint32_t prp_last_n_clk;
  sm_t *page_scan_sm;
  btc_trx_configs_t trx_config;
  btc_fhs_rx_pkt_t received_rx_fhs_pkt; // FHS packet recpetion
} page_scan_t;

void page_scan_start_procedure_handler(sm_t *page_scan_sm);
void page_scan_id_rx_done_handler(sm_t *page_scan_sm);
void page_scan_id_rx_timeout_handler(sm_t *page_scan_sm);
void page_scan_id_tx_done_handler(sm_t *page_scan_sm);
void page_scan_id_tx_abort_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_done_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_1_timeout_handler(sm_t *page_scan_sm);
void page_scan_fhs_rx_2_timeout_handler(sm_t *page_scan_sm);
void page_scan_fhs_response_tx_done_handler(sm_t *page_scan_sm);
void page_scan_fhs_response_tx_abort_handler(sm_t *page_scan_sm);
void page_scan_central_poll_rx_done_handler(sm_t *page_scan_sm);
void page_scan_central_poll_rx_timeout_handler(sm_t *page_scan_sm);
void page_scan_peripheral_null_tx_done_handler(sm_t *page_scan_sm);
void page_scan_peripheral_null_tx_abort_handler(sm_t *page_scan_sm);
void page_scan_activity_complete(sm_t *page_scan_sm);
#endif
