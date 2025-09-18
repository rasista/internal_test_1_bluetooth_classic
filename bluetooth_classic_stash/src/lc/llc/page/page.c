/**
 * @file page.c
 * @brief Bluetooth Classic Page procedure implementation
 *
 * This file implements the Bluetooth Classic Page procedure as specified in
 * the Bluetooth Core Specification. The page procedure allows a central device
 * to establish a connection with a specific peripheral device by transmitting ID
 * packets containing the peripheral's address, followed by FHS packets for
 * synchronization, and finally establishing a connection.
 *
 * The page process follows a specific state machine sequence:
 * 1. Send ID packets on page hopping frequencies
 * 2. Listen for ID responses from the target device
 * 3. Send FHS packet with timing and connection parameters
 * 4. Exchange POLL/NULL packets to establish active connection
 *
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 */

 #include "page.h"
 #include "btc_device.h"
 #include "btc_rail.h"
 #include "page_sm_autogen.h"
 #include "string.h"
 #ifdef SIM
 #include "rail_simulation.h"
 #else
 #include "btc_rail_intf.h"
 #endif
 #include "lpw_scheduler.h"
 #include "lpw_to_host_interface.h"
 #include "sl_btc_seq_interface.h"
 #include "debug_logs.h"

/**
 * @brief Checks operation status and triggers appropriate state machine action
 *
 * Helper function that examines the status returned by radio operations and
 * triggers the appropriate state machine step if an error occurred.
 *
 * @param status Status code returned from radio operations
 * @param page_p Pointer to page procedure structure
 * @param trigger State machine trigger to invoke on error
 */
static void check_status(uint8_t status, page_t *page_p, uint8_t trigger)
{
  if (BTC_RAIL_PASS_STATUS != status) {
    SM_STEP(PAGE_SM, page_p->page_sm, trigger);
  }
}

static page_t* get_page_p(void)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  return &btc_dev_p->page;
}

/**
 * @brief Updates channel information for Central Page Response (PGRSM) - Unfrozen path
 *
 * Used before the peripheral's ID is received and the PGRSM parameters are frozen.
 * Note: Intentionally uses PAGE channel mapping pre-freeze (setBtcFhKosAndN(PAGE,...))
 * to keep 2NP0/2NP1 channel selection stable until CLKE/kOffset/knudge are frozen.
 * ============================================================================
 * FREQUENCY HOPPING SEQUENCE MANAGEMENT
 * ============================================================================
 *

 * This section implements Bluetooth Classic frequency hopping sequences per
 * the Bluetooth Core Specification Vol 2, Part B, Section 2.6.4.

 * SEQUENCE TYPES:
 * - PAGE:  Central device paging sequence (Eq. 2-4)
         Uses CLKE (estimate of peripheral's clock), train toggle, knudge
 * - PGRSM: Central Page Response sequence (Eq. 6)
         Frozen parameters after peripheral ID reception, N counter tracking
 * - BASIC: Connection hopping sequence (Eq. 1)
         Slot-aligned clock, train toggle based on CLKE12

 * TRAIN SELECTION:
 * - Train A: kOffset=1 (maps to 24 in helper function)
 * - Train B: kOffset=0 (maps to 8 in helper function)
 * - Toggle: n_val XOR 1 for PAGE/BASIC sequences

 * TIMING PARAMETERS:
 * - CLKE: Pager's estimate of peripheral's clock (live for PAGE, frozen for PGRSM)
 * - N counter: 5-bit counter (mod 32) for PGRSM sequence tracking
 * - Knudge: Even values only, 0 for first 2×Npage, then adjustable

 * ============================================================================
 */
static void update_channel_info_pgrsm_unfrozen()
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_p->btc_fh_data;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;

  const uint32_t clke_prefreeze = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;
  const uint8_t n_val = (clke_prefreeze >> 12) & 0x01;
  fh_config->masterClk = clke_prefreeze;    // live CLKE
  // Use current kOffset (set by PAGE logic) so 2NP0/2NP1 map to distinct channels
  fh_config->kosAndN = setBtcFhKosAndN(PAGE, n_val, page_p->kOffset);
  // Push to hardware
  update_btc_channel_number(btc_trx_configs, fh_config);
}

/**
 * @brief Updates channel information for Central Page Response (PGRSM) - Frozen path
 *
 * Used after the peripheral's ID is received and parameters are frozen per spec Eq(6).
 */
static void update_channel_info_pgrsm_frozen()
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_p->btc_fh_data;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;

  // Use frozen clock and parameters from when Peripheral's ID was received
  fh_config->masterClk = page_p->btc_fh_data.frozenClk;
  fh_config->frozenClk = page_p->btc_fh_data.frozenClk;

  // Increment N when CLKE1 transitions 1->0 (start of Central TX slot)
  const uint32_t clke = page_p->trx_config.btc_rail_trx_config.clk + page_p->clk_e_offset;
  if (((clke & BT_CLKE1_MASK) == 0) && ((page_p->prc_last_n_clk & BT_CLKE1_MASK) != 0)) {
    page_p->prc_n = (uint8_t)((page_p->prc_n + 1) & BT_N_COUNTER_MASK);  // mod 32
  }
  page_p->prc_last_n_clk = clke;

  // Use frozen kOffset for PGRSM sequence
  fh_config->kosAndN = setBtcFhKosAndN(PGRSM, page_p->prc_n, page_p->kOffsetFrozen);

  // Use frozen knudge per spec Eq(6) - frozen at the values when peripheral's ID was received
  fh_config->knudgeAndIlos = setBtcFhKnudgeAndIlos(page_p->kNudgeFrozen, 0);

  // ---- Push to hardware ----
  update_btc_channel_number(btc_trx_configs, fh_config);
}

/**
 * @brief Updates channel information for BASIC hopping sequence
 *
 * BASIC sequence uses slot-aligned clock and toggles train based on CLKE12.
 * Knudge is not used.
 *
 * @param page_sm Pointer to the page state machine
 */
static void update_channel_info_basic()
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_p->btc_fh_data;

  const uint32_t clkn_slot_aligned = (btc_trx_configs->btc_rail_trx_config.clk & ~0x3);
  const uint8_t n_val = (clkn_slot_aligned >> 12) & 0x01;
  const uint8_t kOffset = n_val ^ 1;  // Train toggle A/B

  fh_config->masterClk = clkn_slot_aligned;
  fh_config->kosAndN = setBtcFhKosAndN(BASIC, n_val, kOffset);
  fh_config->knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
  update_btc_channel_number(btc_trx_configs, fh_config);
}

/**
 * @brief Updates channel information for page hopping sequence
 *
 * Helper function that updates hopping sequence parameters based on
 * the scan repetition mode of the target device and elapsed time since
 * page procedure start.
 * PAGE only (spec: Vol 2, Part B, 2.6.4.2; Eq. 2/3/4)
 * - Xp uses CLKE (pager's estimate of the peripheral's clock)
 * - Trains: koffset = 24 (A) or 8 (B) -> your helper maps 1->24, 0->8
 * - knudge = 0 for first 2×Npage; later an even value if you need to shift the train
 *
 * @param page_sm Pointer to the page state machine
 */
static void update_channel_info_page(sm_t *page_sm)
{
  (void)page_sm;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_p->btc_fh_data;

  // Spec: paging device uses the estimate of the paged device's clock (CLKE).
  const uint32_t clke = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;
  fh_config->masterClk = clke;

  // ---- Train selection (kOffset bit -> helper maps 0→8(B), 1→24(A)) ----
  // For PAGE sequence, use train toggle like working diff: nVal ^ 1
  const uint8_t n_val = (clke >> 12) & 0x01;
  if (page_p->seqSelect == PAGE) {
    page_p->kOffset = n_val ^ 1;  // Train toggle for PAGE sequence
  } else {
    // For other sequences, use dwell based on remote Page Scan Repetition Mode (R0/R1/R2)
    const uint32_t dt = clke - page_p->page_start_time;
    switch (page_p->remote_scan_rep_mode) {
      case PAGE_SCAN_MODE_R0: page_p->kOffset = ((dt & 0x3F)   < 0x20)  ? 1 : 0; break;
      case PAGE_SCAN_MODE_R1: page_p->kOffset = ((dt & 0x1FFF) < 0x1000) ? 1 : 0; break;
      case PAGE_SCAN_MODE_R2: page_p->kOffset = ((dt & 0x3FFF) < 0x2000) ? 1 : 0; break;
      default:                page_p->kOffset = ((clke >> 12) & 0x1) ? 1 : 0;    break;  // simple 1.28 s toggle
    }
  }

  // ---- Calculate knudge per spec Eq(4) ----
  // knudge = 0 during first 2×Npage repetitions, even value during all other repetitions
  uint8_t knudge_value = 0;
  if (page_p->page_repetition_count >= BT_FIRST_KNUDGE_THRESHOLD) {
    // After first 2×Npage repetitions, use even knudge to shift train
    knudge_value = page_p->knudge_offset & BT_KNUDGE_EVEN_MASK;  // Ensure even value
  }

  fh_config->kosAndN = setBtcFhKosAndN(PAGE, n_val, page_p->kOffset);
  fh_config->knudgeAndIlos = setBtcFhKnudgeAndIlos(knudge_value, 0);

  // ---- Push to hardware ----
  update_btc_channel_number(btc_trx_configs, fh_config);
}

/**
 * @brief Configures radio parameters and updates channel information
 *
 * Helper function that sets up slot timing parameters and updates
 * frequency hopping channel information for the next operation.
 *
 * @param page_p Pointer to page procedure structure
 * @param slot_type Slot timing identifier for scheduling
 */
static void configure_and_update_channel(page_t *page_p, uint8_t slot_type)
{
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  configure_nonconn_slot_params(btc_trx_configs, slot_type);
  page_p->btc_fh_data.masterClk = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;
  // Use appropriate channel update function based on sequence selection
  if (page_p->seqSelect == PGRSM) {
    if (page_p->btc_fh_data.frozenClk == 0) {
      update_channel_info_pgrsm_unfrozen();
    } else {
      update_channel_info_pgrsm_frozen();
    }
  } else if (page_p->seqSelect == BASIC) {
    update_channel_info_basic();
  } else {
    update_channel_info_page(page_p->page_sm);
  }
}

/**
 * @brief Handles scheduling of transmission operations
 *
 * Helper function that configures radio, updates channel information,
 * schedules a transmission, and handles state transitions and error checking.
 *
 * @param page_p Pointer to page procedure structure
 * @param slot_type Slot timing identifier for scheduling
 * @param next_state State to transition to upon successful scheduling
 * @param failure_step Trigger to invoke if scheduling fails
 */
static void handle_tx_schedule(page_t *page_p, uint8_t slot_type, uint8_t next_state, uint8_t failure_step)
{
  uint32_t status;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  configure_and_update_channel(page_p, slot_type);
  if (next_state == PAGE_SM_FHS_TX_SCHEDULED) {
    update_fhs_pkt_clk(&page_p->trx_config, btc_dev_p->fhs_pkt, page_p->lt_addr, page_p->trx_config.btc_rail_trx_config.clk);
#ifdef SIM
    page_p->trx_config.tx_window = BTC_TX_PKT_LEN;
 #endif
  }
  status = schedule_tx(&page_p->trx_config);
  SM_STATE(page_p->page_sm, next_state);
  check_status(status, page_p, failure_step);
}

/**
 * @brief Handles scheduling of reception operations
 *
 * Helper function that configures radio, updates channel information,
 * schedules a reception window, and handles state transitions and error checking.
 *
 * @param page_p Pointer to page procedure structure
 * @param slot_type Slot timing identifier for scheduling
 * @param next_state State to transition to upon successful scheduling
 */
static void handle_rx_schedule(page_t *page_p, uint8_t slot_type, uint8_t next_state)
{
  uint32_t status;
  configure_and_update_channel(page_p, slot_type);
  page_p->trx_config.rx_window = BTC_RAIL_RX_TIMEOUT;
  status = schedule_rx(&page_p->trx_config);
  SM_STATE(page_p->page_sm, next_state);
  check_status(status, page_p, RX_TIMEOUT);
}

/**
 * @brief Schedules transmission of first ID packet
 *
 * Helper function that configures and schedules transmission of the first
 * ID packet in the page sequence, with state transition and error handling.
 *
 * @param page_p Pointer to page procedure structure
 */
static void schedule_first_id_packet(page_t *page_p)
{
  uint32_t status;
  SET_CONF_FLAG_ID(page_p->trx_config.btc_rail_trx_config.specialPktType);
  configure_and_update_channel(page_p, BTC_BD_CLK_2NP0);
  status = schedule_tx(&page_p->trx_config);
  SM_STATE(page_p->page_sm, PAGE_SM_FIRST_ID_TX_SCHEDULED);
  check_status(status, page_p, TX_ABORT);
}

/**
 * @brief Resets page procedure to unfrozen PAGE sequence state
 *
 * Helper function that resets all frozen parameters and sequence state
 * when returning to PAGE sequence from PGRSM after timeouts/aborts.
 *
 * @param page_p Pointer to page procedure structure
 */
static void reset_to_page_sequence(page_t *page_p)
{
  // Reset sequence to PAGE (unfrozen)
  page_p->seqSelect = PAGE;

  // Reset frozen parameters to unfrozen state
  page_p->btc_fh_data.frozenClk = 0;  // Clear frozen clock sentinel
  page_p->kOffsetFrozen = 0;          // Clear frozen kOffset
  page_p->kNudgeFrozen = 0;           // Clear frozen knudge
  page_p->prc_n = 0;                  // Reset N counter
  page_p->prc_last_n_clk = 0;         // Reset N tracking clock

  // CRITICAL FIX: Reset train index/repetition counters to prevent stale knudge
  page_p->page_repetition_count = 0;  // Reset repetition counter
  page_p->knudge_offset = 0;          // Reset knudge offset to 0

  // CRITICAL FIX: Reset kOffset to default train selection (will be recalculated)
  // Note: kOffset will be properly set by update_channel_info_page() based on current CLKE
  page_p->kOffset = 0;  // Clear current kOffset - will be recalculated

  // Clear any "ID Response detected" latch/debounce state
  page_p->tx_fail_stats = 0;  // Clear TX failure tracking
}

/**
 * @brief Handles transmission abort recovery
 *
 * Helper function that determines whether to retry transmission or
 * complete the page activity based on remaining time in the page window.
 *
 * @param page_p Pointer to page procedure structure
 */
static void handle_tx_abort(page_t *page_p)
{
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&page_p->trx_config.bd_clk_counter_handle), page_p->end_tsf)) {
    // Increment page repetition count for knudge tracking
    page_p->page_repetition_count++;

    // Update knudge offset if we've exceeded first 2×Npage repetitions
    if (page_p->page_repetition_count >= BT_FIRST_KNUDGE_THRESHOLD) {
      // Use even knudge values to shift train by multiples of 1.25ms
      page_p->knudge_offset = (page_p->knudge_offset + 2) & BT_KNUDGE_EVEN_MASK;  // Ensure even, wrap at 254
    }

    schedule_first_id_packet(page_p);
  } else {
    page_activity_complete(page_p->page_sm);
  }
}

/**
 * @brief Sends page event notification to upper layers
 *
 * Helper function that prepares and sends notifications about page procedure
 * events to the Upper Link Controller (ULC), including completion status.
 *
 * @param status Status code indicating type of page notification
 */
static void send_page_notification_to_ulc(btc_internal_event_status_t status, hss_event_page_complete_t* page_complete)
{
  uint8_t* buffer = NULL;
  pkb_t *pkb;

  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_PAGE_COMPLETE, status);
  if (status == BTC_CONNECTION_COMPLETED) {
    prep_hss_page_complete_event(buffer, page_complete);
  }
  btc_lpw_activity_complete_indication_to_ull(pkb);
}

/**
 * @brief Initiates the Bluetooth page procedure
 *
 * This function is called in response to an HCI Create Connection command to start
 * the page procedure with previously configured parameters. The function:
 * 1. Sets page parameters from the command (priority, step size, timeout)
 * 2. Initializes and configures the page state machine
 * 3. Updates frequency hopping sequence for page train
 * 4. Sets up Bluetooth clock counter and synchronization
 * 5. Calculates the end time for the procedure based on the requested duration
 * 6. Triggers the page process by stepping to START_PROCEDURE state
 *
 * The page procedure will continue until either:
 * - A connection is established with the target device
 * - The specified page duration has elapsed
 * - A stop_page command is received
 *
 * @param hss_cmd Pointer to the HCI command buffer containing page parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void start_page(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint32_t btcReference = 0;
  uint64_t protimerReference = 0;
  btc_dev_p->page.page_sm  = &page_sm;
  page_sm.current_state = page_sm_idle;
  update_aperiodic_role(btc_dev_p, btc_dev_p->page.page_sm);
  btc_dev_p->page.priority = *HSS_CMD_START_PAGE_PRIORITY(hss_cmd);
  btc_dev_p->page.step_size = *HSS_CMD_START_PAGE_STEP_SIZE(hss_cmd);
  btc_dev_p->page.end_tsf = *HSS_CMD_START_PAGE_END_TSF(hss_cmd);

  // Initialize knudge tracking fields
  btc_dev_p->page.page_repetition_count = 0;
  btc_dev_p->page.knudge_offset = 0;  // Start with no knudge offset
  btc_dev_p->page.btc_fh_data.frozenClk = 0; // sentinel: not frozen yet

  btc_dev_p->page.btc_fh_data.kosAndN = setBtcFhKosAndN(PAGE, 0, btc_dev_p->page.kOffset);      //update sequence selection to PAGE

  // Set ULAP for page frequency hopping - use target device's LAP from BD address
  btc_dev_p->page.btc_fh_data.ulap = (btc_dev_p->page.page_bd_address[0]
                                      | (btc_dev_p->page.page_bd_address[1] << 8)
                                      | (btc_dev_p->page.page_bd_address[2] << 16));
 #ifdef SIM
  btc_dev_p->page.trx_config.bd_clk_counter_handle.bd_clk_counter_id =
    btc_init_bd_clk_counter(btc_dev_p->page.clk_e_offset);
  log2vcd_prints_wrapper(btc_dev_p->page.page_sm);
 #else
  btc_dev_p->page.trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
 #endif
  RAIL_SEQ_BTC_InitBtcDate(BTC_NATIVE_CLK, btcReference, protimerReference);
  btc_dev_p->page.end_tsf = btc_get_current_bd_clk_cnt(&btc_dev_p->page.trx_config.bd_clk_counter_handle) + (btc_dev_p->page.end_tsf * HALF_SLOT_FACTOR);  // calculating end tsf and adding tsf that has been elapsed. (Procedure activity duration)
  // Update the end_tsf in terms of bluetooth clock ticks
  RAIL_SEQ_BTC_setAccessAddress(btc_dev_p->page.sync_word);
  RAIL_SEQ_BTC_setHecInit(btc_dev_p->page.page_bd_address[3]);
  RAIL_SEQ_BTC_setCrcInit(btc_dev_p->page.page_bd_address[3]);
  SM_STEP(PAGE_SM, btc_dev_p->page.page_sm, START_PROCEDURE);
}

/**
 * @brief Configures parameters for a subsequent page procedure
 *
 * This function configures the page parameters in preparation for running a
 * page procedure via start_page(). Parameters include:
 * - Remote scan repetition mode: Controls timing of page scanning by target
 * - Clock offset: Used for page train hopping sequence
 * - TX power: Transmit power level for page packets
 * - BD Address: Bluetooth address of the device being paged
 * - LT_ADDR: Logical transport address for the connection
 * - Sync word: Synchronization word for page packets
 *
 * These parameters control the behavior and timing of the page procedure
 * when it is started via start_page().
 *
 * @param hss_cmd Pointer to the HCI command buffer containing parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void set_page(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  page_t *page = &btc_dev_p->page;
  page->remote_scan_rep_mode = *HSS_CMD_SET_PAGE_REMOTE_SCAN_REP_MODE(hss_cmd);
  page->clk_e_offset = *HSS_CMD_SET_PAGE_CLK_E_OFFSET(hss_cmd);
  btc_dev_p->non_connected_tx_pwr_index = *HSS_CMD_SET_PAGE_TX_PWR_INDEX(hss_cmd);
  memcpy(page->page_bd_address, HSS_CMD_SET_PAGE_BD_ADDRESS(hss_cmd), BD_ADDR_LEN);
  page->lt_addr = *HSS_CMD_SET_PAGE_LT_ADDR(hss_cmd);
  memcpy(page->sync_word, HSS_CMD_SET_PAGE_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
}

/**
 * @brief Stops an ongoing page procedure
 *
 * This function terminates an active page procedure, typically in response to
 * an HCI Cancel Connection command. It performs the following actions:
 * 1. Immediately transitions the state machine to idle state
 * 2. Aborts any pending radio operations using RAIL_SEQ_Idle
 * 3. Completes the page activity and notifies upper layers
 * 4. Releases radio resources and returns to idle state
 *
 * The function ensures all resources are properly released and the radio
 * is returned to an idle state to conserve power.
 *
 * @param hss_cmd Pointer to the HCI command buffer (not used)
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void stop_page(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)hss_cmd;
  btc_common_stop(btc_dev_p->page.page_sm, page_activity_complete, page_sm_idle);
}

/**
 * @brief Handles the START_PROCEDURE trigger for the page state machine
 *
 * This function initializes the page procedure after being triggered by
 * the state machine. It:
 * 1. Configures transmit power and scrambler seed settings
 * 2. Forms the FHS packet for later transmission
 * 3. Records the page start time for train hopping calculations
 * 4. Updates channel information for page hopping pattern
 * 5. Determines the appropriate slot to start transmission
 * 6. Either schedules ID packet transmission or completes activity if expired
 *
 * This is the entry point for the active page procedure after parameters
 * have been set and the state machine has been initialized.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_start_procedure_handler(sm_t *page_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = btc_dev_p->non_connected_tx_pwr_index;
  btc_trx_configs->ignore_whitening = FALSE; // TODO: Remove this once hopping is properly tested in statemachines
  // Form FHS packet
  btc_dev_p->l2_scheduled_sm = page_sm;
  form_fhs_pkt(btc_trx_configs, btc_dev_p->fhs_pkt, page_p->lt_addr, 0);  // Use 0 for old_lt_addr as it is not applicable in page procedure
  if (page_p->hci_trigger) {
    page_p->page_start_time = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;
  }

  // Prepare to transmit ID packet(s) on page train
  page_p->seqSelect = PAGE;
  update_channel_info_page(page_sm);
  // Get current slot
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  if (IS_BT_CLOCK_LT(curr_slot, page_p->end_tsf)) {
    btc_bd_clk_identifier_t next_clk_id;
    page_sm_states_t next_state;

    //  check if the current slot is BTC_BD_CLK_2NP0. If yes then schedule second
    //  ID packet else schedule first ID Packet
    if (((curr_slot) & (0x03)) == BTC_BD_CLK_2NP0) {
      next_clk_id = BTC_BD_CLK_2NP1;
      next_state = PAGE_SM_SECOND_ID_TX_SCHEDULED;
    } else {
      next_clk_id = BTC_BD_CLK_2NP0;
      next_state = PAGE_SM_FIRST_ID_TX_SCHEDULED;
    }
    SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
    handle_tx_schedule(page_p, next_clk_id, next_state, TX_ABORT);
  } else {
    page_activity_complete(page_p->page_sm);
  }
}

/**
 * @brief Handles successful transmission of first ID packet
 *
 * Called when the first ID packet in the page train has been successfully
 * transmitted. The function:
 * 1. Updates hopping sequence to page response mode
 * 2. Clears the ID_1_TX_FAIL_BIT in transmit status register
 * 3. Schedules transmission of second ID packet
 *
 * This function advances the page sequence to maintain the proper timing
 * between consecutive ID packet transmissions.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_first_id_tx_done_handler(sm_t *page_sm)
{
  (void)page_sm;
  page_t *page_p = get_page_p();
  CLEAR_BIT(page_p->tx_fail_stats, ID_1_TX_FAIL_BIT);
  // Schedule second ID on page train
  page_p->seqSelect = PAGE;
  handle_tx_schedule(page_p, BTC_BD_CLK_2NP1, PAGE_SM_SECOND_ID_TX_SCHEDULED, TX_ABORT);
}

/**
 * @brief Handles abortion of first ID packet transmission
 *
 * Called when the first ID packet transmission was aborted or failed. This function:
 * 1. Sets the ID_1_TX_FAIL_BIT in transmit status register to indicate failure
 * 2. Schedules transmission of second ID packet to continue the page sequence
 *
 * The failure status is tracked to determine which response slots to monitor,
 * as the target device response timing depends on which ID packet was received.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_first_id_tx_abort_handler(sm_t *page_sm)
{
  (void)page_sm;
  page_t *page_p = get_page_p();
  SET_BIT(page_p->tx_fail_stats, ID_1_TX_FAIL_BIT);
  page_p->seqSelect = PAGE;
  handle_tx_schedule(page_p, BTC_BD_CLK_2NP1, PAGE_SM_SECOND_ID_TX_SCHEDULED, TX_ABORT);
}

/**
 * @brief Handles successful transmission of second ID packet
 *
 * Called when the second ID packet in the page train has been successfully
 * transmitted. This function:
 * 1. Updates hopping sequence to page response mode
 * 2. Clears the ID_2_TX_FAIL_BIT in transmit status register
 * 3. Schedules appropriate receive window based on which ID packets were
 *    successfully transmitted:
 *    - If first ID failed: Schedule second RX slot
 *    - If first ID succeeded: Schedule first RX slot
 *
 * The function ensures proper timing of receive windows to capture responses
 * from the target device.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_second_id_tx_done_handler(sm_t *page_sm)
{
  (void)page_sm;
  page_t *page_p = get_page_p();
  CLEAR_BIT(page_p->tx_fail_stats, ID_2_TX_FAIL_BIT);

  // Stay in PAGE sequence until peripheral ID response is received
  // Use PAGE pre-freeze math with same kOffset, suppress edge bumps
  const uint32_t clke = page_p->trx_config.btc_rail_trx_config.clk + page_p->clk_e_offset;
  page_p->prc_last_n_clk = clke & ~BT_CLKE1_MASK;  // Suppress unintended N/k bumps

  if (IS_BIT_SET(page_p->tx_fail_stats, ID_1_TX_FAIL_BIT)) {
    handle_rx_schedule(page_p, BTC_BD_CLK_2NP3, PAGE_SM_SECOND_ID_RX_SCHEDULED);
  } else {
    handle_rx_schedule(page_p, BTC_BD_CLK_2NP2, PAGE_SM_FIRST_ID_RX_SCHEDULED);
  }
}

/**
 * @brief Handles abortion of second ID packet transmission
 *
 * Called when the second ID packet transmission was aborted or failed. This function:
 * 1. Sets the ID_2_TX_FAIL_BIT in transmit status register
 * 2. Determines next action based on transmission status:
 *    - If both ID packet transmissions failed: Retry from beginning if time permits
 *    - If first ID packet succeeded: Schedule receive window for potential response
 *
 * This recovery mechanism ensures the page procedure can continue even when
 * individual packet transmissions fail.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_second_id_tx_abort_handler(sm_t *page_sm)
{
  page_t *page_p = get_page_p();
  (void)page_sm;
  SET_BIT(page_p->tx_fail_stats, ID_2_TX_FAIL_BIT);

  if ((page_p->tx_fail_stats & BIT(ID_1_TX_FAIL_BIT)) && (page_p->tx_fail_stats & BIT(ID_2_TX_FAIL_BIT))) {
    if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&page_p->trx_config.bd_clk_counter_handle), page_p->end_tsf)) {
      // Restart ID train on page sequence - reset frozen state
      reset_to_page_sequence(page_p);
      handle_tx_schedule(page_p, BTC_BD_CLK_2NP0, PAGE_SM_FIRST_ID_TX_SCHEDULED, TX_ABORT);
    } else {
      page_activity_complete(page_p->page_sm);
    }
  } else {
    // Prepare to receive on response sequence
    page_p->seqSelect = PGRSM;
    handle_rx_schedule(page_p, BTC_BD_CLK_2NP2, PAGE_SM_FIRST_ID_RX_SCHEDULED);
  }
}

/**
 * @brief Handles reception of ID response in first RX slot
 *
 * Called when an ID response is successfully received in the first receive slot.
 * This function:
 * 1. Freezes CLKE, koffset, and knudge per spec Eq(6) for Central Page Response
 * 2. Initializes PGRSM sequence with N=1 and increments before FHS transmission
 * 3. Calculates target slot for FHS transmission
 * 4. Sets up procedure timeout for monitoring response time
 * 5. Updates FHS packet with current clock value
 * 6. Schedules FHS packet transmission
 *
 * This function transitions the page procedure from the initial ID exchange
 * phase to the FHS transmission phase using PGRSM sequence.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_first_id_rx_done_handler(sm_t *page_sm)
{
  (void)page_sm;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;

  // ---- Central Page Response (PGRSM) freeze per spec Eq(6) ----
  const uint32_t clke = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;

  // Freeze CLKE16−12, koffset, and knudge at the values when Peripheral's ID was received
  page_p->btc_fh_data.frozenClk = clke & ~0x3;   // slot-aligned frozen clock
  page_p->kOffsetFrozen = page_p->kOffset;

  // Freeze current knudge value per spec Eq(6)
  uint8_t current_knudge = 0;
  if (page_p->page_repetition_count >= BT_FIRST_KNUDGE_THRESHOLD) {
    current_knudge = page_p->knudge_offset & BT_KNUDGE_EVEN_MASK;  // Ensure even value
  }
  page_p->kNudgeFrozen = current_knudge;

  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_p->btc_fh_data;
  // Switch to Central Page Response sequence
  page_p->seqSelect     = PGRSM;
  fh_config->masterClk         = page_p->btc_fh_data.frozenClk;
  fh_config->frozenClk         = page_p->btc_fh_data.frozenClk;

  // Initialize N counter: starts at 1, first increment before FHS transmission
  page_p->prc_n = 1;                      // PGRSM N starts at 1
  page_p->prc_last_n_clk = clke & ~0x2;

  fh_config->knudgeAndIlos     = setBtcFhKnudgeAndIlos(page_p->kNudgeFrozen, 0);
  fh_config->kosAndN           = setBtcFhKosAndN(PGRSM, page_p->prc_n, page_p->kOffsetFrozen);
  update_btc_channel_number(btc_trx_configs, fh_config);

  // Calculate target slot and setup FHS specifics
  page_p->page_target_slot = btc_get_target_bd_clk(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), BTC_BD_CLK_2NP0);
  page_p->procedure_tout_instant = BT_ADD_CLOCK(page_p->trx_config.btc_rail_trx_config.clk, (PAGE_RESP_TIME_OUT << 2));
  update_fhs_pkt_clk(btc_trx_configs, btc_dev_p->fhs_pkt, page_p->lt_addr, page_p->trx_config.btc_rail_trx_config.clk);
#ifdef SIM
  btc_trx_configs->tx_window = BTC_TX_PKT_LEN;
 #endif
  handle_tx_schedule(page_p, BTC_BD_CLK_2NP0, PAGE_SM_FHS_TX_SCHEDULED, TX_ABORT);
}

/**
 * @brief Handles timeout during first RX slot
 *
 * Called when no valid ID response is received during the first receive window.
 * This function:
 * 1. Checks if the second ID packet transmission failed
 * 2. If second TX failed: Schedules retransmission if within page duration
 * 3. If second TX succeeded: Schedules receive for second RX slot
 *
 * This function implements recovery logic to maximize the chance of successful
 * page completion despite missed transmissions or receptions.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_first_id_rx_timeout_handler(sm_t *page_sm)
{
  page_t *page_p = get_page_p();
  (void)page_sm;
  uint32_t current_bd_clk = btc_get_current_bd_clk_cnt(&page_p->trx_config.bd_clk_counter_handle);
  if (IS_BIT_SET(page_p->tx_fail_stats, ID_2_TX_FAIL_BIT)) {
    if (IS_BT_CLOCK_LT(current_bd_clk, page_p->end_tsf)) {
      // About to transmit an ID: reset to page sequence
      reset_to_page_sequence(page_p);
      handle_tx_schedule(page_p, BTC_BD_CLK_2NP0, PAGE_SM_FIRST_ID_TX_SCHEDULED, TX_ABORT);
    } else {
      page_activity_complete(page_p->page_sm);
    }
  } else {
    page_p->page_target_slot = btc_get_target_bd_clk(current_bd_clk, BTC_BD_CLK_2NP3);
    page_p->seqSelect = PGRSM;
    handle_rx_schedule(page_p, BTC_BD_CLK_2NP3, PAGE_SM_SECOND_ID_RX_SCHEDULED);
  }
}

/**
 * @brief Handles successful transmission of FHS packet
 *
 * Called when the FHS packet has been successfully transmitted to the target device.
 * This function updates hopping sequence parameters and schedules the reception window
 * to listen for the peripheral's ID response.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_fhs_tx_done_handler(sm_t *page_sm)
{
  (void)page_sm;
  page_t *page_p = get_page_p();
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;

  // Keep Central Page Response sequence until after the immediate response to FHS
  // Hold CLKE1 edge to avoid unintended N bump on scheduling boundary
  {
    const uint32_t clke_now = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;
    page_p->prc_last_n_clk = (clke_now & ~0x2);
  }
  page_p->btc_fh_data.kosAndN = setBtcFhKosAndN(PGRSM, page_p->prc_n, page_p->kOffsetFrozen);
  page_p->page_target_slot = btc_get_target_bd_clk(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), BTC_BD_CLK_2NP2);

  // Special handling for normal packet type
  SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);

  page_p->seqSelect = PGRSM;
  handle_rx_schedule(page_p, BTC_BD_CLK_2NP2, PAGE_SM_PERIPHERAL_RESPONSE_ID_RX_SCHEDULED);
}

/**
 * @brief Handles FHS packet transmission abort during page procedure
 *
 * This function is called when the transmission of an FHS packet fails during
 * the page procedure. It updates failure statistics and handles retry attempts
 * based on timeout conditions:
 * 1. If within procedure timeout - Retries FHS transmission
 * 2. If within end_tsf but past procedure timeout - Reverts to ID transmission
 * 3. If past end_tsf - Completes page activity
 *
 * @param page_sm Pointer to the page state machine
 */
void page_fhs_tx_abort_handler(sm_t *page_sm)
{
  (void)page_sm;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  uint32_t current_clk = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);

  SET_BIT(page_p->tx_fail_stats, FHS_TX_FAIL_BIT);
  page_p->page_abort_slot = current_clk;

  if (IS_BT_CLOCK_LT(page_p->trx_config.btc_rail_trx_config.clk, page_p->procedure_tout_instant)) {
    // Handle FHS retry
    page_p->page_target_slot = btc_get_target_bd_clk(current_clk, BTC_BD_CLK_2NP0);

    // Special FHS packet handling
    configure_nonconn_slot_params(btc_trx_configs, BTC_BD_CLK_2NP0);
    // Use PGRSM update on FHS retries; do not overwrite masterClk
    page_p->seqSelect = PGRSM;
    update_channel_info_pgrsm_frozen();
    update_fhs_pkt_clk(btc_trx_configs, btc_dev_p->fhs_pkt, page_p->lt_addr, page_p->trx_config.btc_rail_trx_config.clk);
#ifdef SIM
    btc_trx_configs->tx_window = BTC_TX_PKT_LEN;
#endif
    handle_tx_schedule(page_p, BTC_BD_CLK_2NP0, PAGE_SM_FHS_TX_SCHEDULED, TX_ABORT);
  } else if (IS_BT_CLOCK_LT(current_clk, page_p->end_tsf)) {
    send_page_notification_to_ulc(BTC_CONNECTION_RESP_TOUT, NULL);
    reset_to_page_sequence(page_p);
    handle_tx_schedule(page_p, BTC_BD_CLK_2NP0, PAGE_SM_FIRST_ID_TX_SCHEDULED, TX_ABORT);
  } else {
    page_activity_complete(page_p->page_sm);
  }
}

/**
 * @brief Handles peripheral response ID reception timeout during page procedure
 *
 * Called when no ID packet is received from the peripheral in response to the
 * FHS packet. Depending on timing constraints, the function either:
 * 1. Retries FHS packet transmission if within procedure timeout window
 * 2. Handles timeout notification and returns to ID transmission if still within page window
 * 3. Completes page activity if outside the page window
 *
 * @param page_sm Pointer to the page state machine
 */
void page_peripheral_response_id_rx_timeout_handler(sm_t *page_sm)
{
  uint32_t status = BTC_RAIL_PASS_STATUS;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  uint32_t current_bd_clk = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  page_p->page_abort_slot = current_bd_clk;
  configure_nonconn_slot_params(btc_trx_configs, BTC_BD_CLK_2NP0);
  if (IS_BT_CLOCK_LT(btc_trx_configs->btc_rail_trx_config.clk, page_p->procedure_tout_instant)) {
    page_p->page_target_slot = btc_get_target_bd_clk(current_bd_clk, BTC_BD_CLK_2NP0);
    page_p->seqSelect = PGRSM;
    update_channel_info_pgrsm_frozen();
    update_fhs_pkt_clk(btc_trx_configs, btc_dev_p->fhs_pkt, page_p->lt_addr, page_p->trx_config.btc_rail_trx_config.clk);
 #ifdef SIM
    btc_trx_configs->tx_window = BTC_TX_PKT_LEN;
 #endif
    status = schedule_tx(btc_trx_configs);
    SM_STATE(page_sm, PAGE_SM_FHS_TX_SCHEDULED);
  } else {
    send_page_notification_to_ulc(BTC_CONNECTION_RESP_TOUT, NULL);
    reset_to_page_sequence(page_p);
    handle_tx_abort(page_p);
  }
  check_status(status, page_p, TX_ABORT);
}

/**
 * @brief Handles successful reception of ID packet from peripheral after FHS transmission
 *
 * This function is called when the peripheral responds to our FHS packet with an ID packet,
 * indicating connection establishment is in progress. The function:
 * 1. Switches to BASIC hopping for the ensuing POLL/NULL exchange
 * 2. Configures parameters for connection
 * 3. Notifies upper layers that connection has been created
 * 4. Schedules POLL packet transmission as next step in connection establishment
 *
 * Note: Transition to BASIC happens immediately after the FHS response (central path),
 * before the POLL/NULL exchange.
 *
 * @param page_sm Pointer to the page state machine
 */
void page_peripheral_response_id_rx_done_handler(sm_t *page_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;

  // Switch both sides to BASIC right after FHS response RX (central path)
  page_p->seqSelect = BASIC;
  page_p->btc_fh_data.frozenClk = 0;
  // Set FH ULAP to master's LAP (central BD_ADDR) for BASIC hopping
  page_p->btc_fh_data.ulap = (btc_dev_p->bd_addr[0]
                              | (btc_dev_p->bd_addr[1] << 8)
                              | (btc_dev_p->bd_addr[2] << 16));

  page_p->page_target_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);

  // Schedule POLL Tx packet to RAIL
  //  all necessary parameters for slot programming
  configure_nonconn_slot_params(btc_trx_configs, BTC_BD_CLK_2NP0);
  update_channel_info_basic();
  RAIL_SEQ_BTC_setAccessAddress(btc_dev_p->sync_word);
  RAIL_SEQ_BTC_setHecInit(btc_dev_p->bd_addr[3]);
  RAIL_SEQ_BTC_setCrcInit(btc_dev_p->bd_addr[3]);
  send_page_notification_to_ulc(BTC_CONNECTION_CREATED, NULL);
  page_p->procedure_tout_instant = BT_ADD_CLOCK(btc_trx_configs->btc_rail_trx_config.clk, (NEW_CONN_TIME_OUT << 2));
  // Schedule POLL packet TX
  form_poll_and_null_pkt(btc_trx_configs, page_p->lt_addr, BT_POLL_PKT_TYPE);
  status = schedule_tx(btc_trx_configs);
  // Update page state machine to CONN_POLL_TX_SCHEDULED
  SM_STATE(page_sm, PAGE_SM_CENTRAL_RESPONSE_POLL_TX_SCHEDULED);

  check_status(status, page_p, TX_ABORT);
}

/**
 * @brief Handles successful transmission of POLL packet during connection establishment
 *
 * After the peripheral has acknowledged our FHS packet with an ID packet, we send a POLL packet.
 * This function handles the successful transmission of that POLL packet and sets up the device
 * to listen for the peripheral's NULL response, which will complete the connection establishment.
 *
 * @param page_sm Pointer to the page state machine
 */
void page_central_response_poll_tx_done_handler(sm_t *page_sm)
{
  page_t *page_p = get_page_p();
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  uint32_t status;
  // calculating and storing next tx/rx slot
  page_p->page_target_slot =
    btc_get_target_bd_clk(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), BTC_BD_CLK_2NP2);
  configure_nonconn_slot_params(btc_trx_configs, BTC_BD_CLK_2NP2);
  // Use BASIC after FHS response to schedule NULL RX
  update_channel_info_basic();
  configure_trx_acl_rx(btc_trx_configs, BASIC_RATE, BTC_RAIL_RX_TIMEOUT);
  status = schedule_rx(btc_trx_configs);
  SM_STATE(page_sm, PAGE_SM_PERIPHERAL_RESPONSE_NULL_RX_SCHEDULED);
  check_status(status, page_p, RX_TIMEOUT);
}

/**
 * @brief Handles POLL packet transmission failure during connection establishment
 *
 * When POLL packet transmission fails during the connection establishment phase of paging,
 * this function attempts to recover by:
 * 1. Retransmitting POLL if within NEW_CONN_TIME_OUT window
 * 2. Aborting connection attempt and notifying upper layers if timeout expired
 *
 * This is a critical step in connection establishment as both devices need to synchronize
 * their frequency hopping sequence for ongoing communication.
 *
 * @param page_sm Pointer to the page state machine
 */
void page_central_response_poll_tx_abort_handler(sm_t *page_sm)
{
  uint32_t status = BTC_RAIL_PASS_STATUS;
  page_t *page_p = get_page_p();
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  uint32_t current_bd_clk = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  page_p->page_abort_slot = current_bd_clk;
  configure_conn_slot_params(&container_of(page_p, btc_dev_t, page)->connection_info[0], BTC_BD_CLK_2NP0, btc_trx_configs);
  if (IS_BT_CLOCK_LT(btc_trx_configs->btc_rail_trx_config.clk, page_p->procedure_tout_instant)) {
    page_p->page_target_slot = btc_get_target_bd_clk(current_bd_clk, BTC_BD_CLK_2NP0);
    page_p->seqSelect = BASIC;
    update_channel_info_basic();
    form_poll_and_null_pkt(btc_trx_configs, page_p->lt_addr, BT_POLL_PKT_TYPE);
    status = schedule_tx(btc_trx_configs);
    SM_STATE(page_sm, PAGE_SM_CENTRAL_RESPONSE_POLL_TX_SCHEDULED);
  } else {
    send_page_notification_to_ulc(BTC_NEW_CONNECTION_TOUT, NULL);
    reset_to_page_sequence(page_p);
    handle_tx_abort(page_p);
  }
  check_status(status, page_p, TX_ABORT);
}

/**
 * @brief Handles successful reception of NULL packet from peripheral completing connection
 *
 * This function is called when the NULL packet from the peripheral is successfully received,
 * which completes the connection establishment process. The function:
 * 1. Transitions from PGRSM to BASIC sequence (Central Page Response complete)
 * 2. Sets flag to ignore packet data transfer (NULL has no payload)
 * 3. Notifies upper layers of successful connection completion
 * 4. Transitions state machine to idle state
 * 5. Logs connection completion event
 *
 * This marks the successful completion of the page procedure with established connection.
 *
 * @param page_sm Pointer to the page state machine
 */
void page_peripheral_response_null_rx_done_handler(sm_t *page_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  hss_event_page_complete_t page_complete;

  // ---- Central Page Response (PGRSM) complete - transition to BASIC ----
  // Per spec: we should already be in BASIC after FHS response. Ensure BASIC.
  update_channel_info_basic();

  btc_trx_configs->ignore_rx_pkt = 1;  // To avoid DMA transfer for the received NULL packet
  if (btc_dev_p->connection_info_idx < MAX_NUMBER_OF_CONNECTIONS) {
    page_complete.connection_info_idx = btc_dev_p->connection_info_idx;
    // Update connection info
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].trx_config.bd_clk_counter_handle = btc_dev_p->page.trx_config.bd_clk_counter_handle;
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].lt_addr = btc_dev_p->page.lt_addr;
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].sync_word[0] = btc_dev_p->page.sync_word[0];
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].sync_word[1] = btc_dev_p->page.sync_word[1];
    btc_dev_p->connection_info_idx++;
    send_page_notification_to_ulc(BTC_CONNECTION_COMPLETED, &page_complete);
  } else {
    // TODO: Handle case where max number of connections is reached
  }
  stop_aperiodic_role(btc_dev_p);
  SM_STATE(page_sm, page_sm_idle);
  debug_log_event(DEBUG_PTI, PTI_CONNECTION_COMPLETED);
}

/**
 * @brief Handles NULL packet reception timeout during connection establishment
 *
 * Called when expected NULL packet from peripheral is not received within timeout.
 * The function implements recovery by:
 * 1. Retransmitting POLL packet if within procedure timeout window
 * 2. Aborting connection with notification if timeout expired
 *
 * This retry mechanism helps improve connection reliability in challenging RF environments.
 *
 * @param page_sm Pointer to the page state machine
 */
void page_peripheral_response_null_rx_timeout_handler(sm_t *page_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  uint32_t status = BTC_RAIL_PASS_STATUS;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  page_p->page_abort_slot   = btc_get_current_bd_clk_cnt(&page_p->trx_config.bd_clk_counter_handle);
  //  all necessary parameters for slot programming
  configure_conn_slot_params(&btc_dev_p->connection_info[0], BTC_BD_CLK_2NP0, btc_trx_configs);
  if (IS_BT_CLOCK_LT(btc_trx_configs->btc_rail_trx_config.clk, page_p->procedure_tout_instant)) {
    update_channel_info_basic();
    form_poll_and_null_pkt(btc_trx_configs, page_p->lt_addr, BT_POLL_PKT_TYPE);
    status = schedule_tx(btc_trx_configs);
    SM_STATE(page_sm, PAGE_SM_CENTRAL_RESPONSE_POLL_TX_SCHEDULED);
    check_status(status, page_p, TX_ABORT);
  } else {
    send_page_notification_to_ulc(BTC_NEW_CONNECTION_TOUT, NULL);
    reset_to_page_sequence(page_p);
    handle_tx_abort(page_p);
    check_status(status, page_p, TX_ABORT);
  }
}

/**
 * @brief Handles successful reception of second ID response during page procedure
 *
 * This function is called when the peripheral responds to our second ID transmission.
 * It prepares and schedules FHS packet transmission as the next step in the page procedure.
 * The function:
 * 1. Freezes CLKE, koffset, and knudge per spec Eq(6) for Central Page Response
 * 2. Initializes PGRSM sequence with N=1 and increments before FHS transmission
 * 3. Calculates appropriate transmission timing
 * 4. Sets up channel information for the next transmission
 * 5. Updates procedure timeout for response monitoring
 * 6. Transitions to FHS_TX_SCHEDULED state
 *
 * @param page_sm Pointer to the page state machine
 */
void page_second_id_rx_done_handler(sm_t *page_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_t *page_p = &btc_dev_p->page;
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &page_p->trx_config;
  configure_nonconn_slot_params(btc_trx_configs, BTC_BD_CLK_2NP0);

  // ---- Central Page Response (PGRSM) freeze per spec Eq(6) ----
  const uint32_t clke = btc_trx_configs->btc_rail_trx_config.clk + page_p->clk_e_offset;

  // Freeze CLKE16−12, koffset, and knudge at the values when Peripheral's ID was received
  page_p->btc_fh_data.frozenClk = clke & ~0x3;   // slot-aligned frozen clock
  page_p->kOffsetFrozen = page_p->kOffset;

  // Freeze current knudge value per spec Eq(6)
  uint8_t current_knudge = 0;
  if (page_p->page_repetition_count >= BT_FIRST_KNUDGE_THRESHOLD) {
    current_knudge = page_p->knudge_offset & BT_KNUDGE_EVEN_MASK;  // Ensure even value
  }
  page_p->kNudgeFrozen = current_knudge;

  // Initialize N counter: starts at 1, first increment before FHS transmission
  page_p->prc_n = 1;
  page_p->prc_last_n_clk = clke;

  // Switch to Central Page Response sequence
  page_p->seqSelect = PGRSM;

  // Increment N before sending FHS (per spec: "first increment shall be done before sending FHS")
  // page_p->prc_n = (uint8_t)((page_p->prc_n + 1) & 0x1F);
  // Prevent a duplicate increment inside update_channel_info_pgrsm() during FHS scheduling
  page_p->prc_last_n_clk = (clke & ~0x2);

  // calculating and storing next tx/rx slot
  page_p->page_target_slot =
    btc_get_target_bd_clk(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), BTC_BD_CLK_2NP0);
  // Schedule first id packet to RAIL
  //  all necessary parameters for slot programming
  configure_and_update_channel(page_p, BTC_BD_CLK_2NP0);   // Use PGRSM channel update
  page_p->procedure_tout_instant = BT_ADD_CLOCK(page_p->trx_config.btc_rail_trx_config.clk, (PAGE_RESP_TIME_OUT << 2));
  // Schedule FHS packet TX
  update_fhs_pkt_clk(btc_trx_configs, btc_dev_p->fhs_pkt, page_p->lt_addr, page_p->trx_config.btc_rail_trx_config.clk);
#ifdef SIM
  btc_trx_configs->tx_window = BTC_TX_PKT_LEN;
 #endif
  // form_fhs_pkt(btc_dev_p, btc_trx_configs, btc_dev_p->fhs_pkt, page_p->lt_addr);
  btc_trx_configs->btc_rail_trx_config.specialPktType = SL_RAIL_BTC_NORMAL_PKT;
  status = schedule_tx(btc_trx_configs);

  //  Update page state machine to PAGE_SM_FHS_TX_SCHEDULED
  SM_STATE(page_sm, PAGE_SM_FHS_TX_SCHEDULED);
  check_status(status, page_p, TX_ABORT);
}

/**
 * @brief Handles timeout during second RX slot
 *
 * Called when no valid ID response is received during the second receive window.
 * This function:
 * 1. Calculates the next slot for transmission
 * 2. Delegates to handle_tx_abort() which either:
 *    - Schedules next ID packet transmission if within page duration
 *    - Completes page activity if past the end time
 *
 * This function ensures the page procedure continues or terminates
 * appropriately based on the configured duration.
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_second_id_rx_timeout_handler(sm_t *page_sm)
{
  (void)page_sm;
  page_t *page_p = get_page_p();
  // calculating and storing next tx/rx slot
  page_p->page_target_slot =
    btc_get_target_bd_clk(btc_get_current_bd_clk_cnt(&page_p->trx_config.bd_clk_counter_handle), BTC_BD_CLK_2NP0);
  reset_to_page_sequence(page_p);
  handle_tx_abort(page_p);
}

/**
 * @brief Completes the page activity and performs necessary cleanup
 *
 * This function is called when the page procedure completes, either successfully
 * or due to timeout/error. It:
 * 1. Stops the aperiodic role (releases radio resources)
 * 2. Notifies upper layers that activity is complete
 * 3. Sets state machine to idle state
 * 4. Configures radio to idle state to save power
 *
 * @param page_sm Pointer to the page state machine structure
 */
void page_activity_complete(sm_t *page_sm)
{
  btc_activity_complete(page_sm, (void (*)(btc_internal_event_status_t, void *))send_page_notification_to_ulc, PTI_PAGE_COMPLETED, page_sm_idle);
}
