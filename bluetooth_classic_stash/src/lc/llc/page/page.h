#ifndef PAGE_H_
#define PAGE_H_
#include "btc_common_defines.h"
#include "sl_btc_seq_interface.h"
#include "state_machine.h"
#include "btc_rail.h"

#define PAGE_SCAN_MODE_R0 0
#define PAGE_SCAN_MODE_R1 1
#define PAGE_SCAN_MODE_R2 2

// ---- Frequency Hopping Constants ----
#define BT_CLKE1_MASK              0x2     // Mask for CLKE1 bit (bit 1)
#define BT_CLKE_16_12_MASK         0x1F000 // Mask for CLKE[16:12] bits (5 bits)
#define BT_CLKE_16_12_SHIFT        12      // Shift to extract CLKE[16:12]
#define BT_KNUDGE_EVEN_MASK        0xFE    // Mask to ensure even knudge values
#define BT_N_COUNTER_MASK          0x1F    // Mask for N counter (5 bits, mod 32)
#define BT_NPAGE_REPETITIONS       16      // Npage value (16 repetitions)
#define BT_FIRST_KNUDGE_THRESHOLD  (2 * BT_NPAGE_REPETITIONS) // 2×Npage threshold

typedef struct page_s {
  uint8_t remote_scan_rep_mode;
  uint8_t page_start_slot;
  uint8_t train_start_slot;
  uint8_t tx_fail_stats;
  uint8_t btc_txrx_fail_stats;
  uint8_t page_bd_address[BD_ADDR_LEN];
  uint8_t priority;
  uint8_t step_size;
  uint8_t hci_trigger;  //flag hci_trigger(page)
  uint8_t lt_addr;
  uint32_t end_tsf;
  uint32_t page_start_time; //stores value when page started for the first time from upper layer
  uint32_t clk_e_offset;
  uint32_t procedure_tout_instant; // specifies the clock value instant for timeout cases
  uint32_t page_target_slot;
  uint32_t page_abort_slot;
  uint32_t sync_word[2];
  RAIL_SEQ_BtcFhConfig_t btc_fh_data;
  BtcSeqSelect_t seqSelect;
  uint8_t kOffset;
  /* Knudge tracking per spec Eq(4) */
  uint16_t page_repetition_count;  // Track number of page repetitions
  uint8_t knudge_offset;           // Current knudge offset value
  /* Central Page Response (PGRSM) bookkeeping */
  uint8_t prc_n;
  uint32_t prc_last_n_clk;
  uint8_t kOffsetFrozen;
  uint8_t kNudgeFrozen;
  sm_t *page_sm;
  btc_trx_configs_t trx_config;
} page_t;

void btc_config_tx_pkt(bt_tx_pkt_t *tx_pkt);
void page_start_procedure_handler(sm_t *page_sm);
void page_first_id_tx_done_handler(sm_t *page_sm);
void page_first_id_tx_abort_handler(sm_t *page_sm);
void page_second_id_tx_done_handler(sm_t *page_sm);
void page_second_id_tx_abort_handler(sm_t *page_sm);
void page_first_id_rx_done_handler(sm_t *page_sm);
void page_first_id_rx_timeout_handler(sm_t *page_sm);
void page_second_id_rx_done_handler(sm_t *page_sm);
void page_second_id_rx_timeout_handler(sm_t *page_sm);
void page_fhs_tx_done_handler(sm_t *page_sm);
void page_fhs_tx_abort_handler(sm_t *page_sm);
void page_peripheral_response_id_rx_done_handler(sm_t *page_sm);
void page_peripheral_response_id_rx_timeout_handler(sm_t *page_sm);
void page_central_response_poll_tx_done_handler(sm_t *page_sm);
void page_central_response_poll_tx_abort_handler(sm_t *page_sm);
void page_peripheral_response_null_rx_done_handler(sm_t *page_sm);
void page_peripheral_response_null_rx_timeout_handler(sm_t *page_sm);
void page_activity_complete(sm_t *page_sm);

#endif
