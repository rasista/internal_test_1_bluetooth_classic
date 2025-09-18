/**
 * @file page_scan.c
 * @brief Bluetooth Classic Page Scan procedure implementation
 *
 * This file implements the Bluetooth Classic Page Scan procedure as specified in
 * the Bluetooth Core Specification. The page scan procedure allows a peripheral device
 * to listen for connection attempts from central devices, respond to ID packets,
 * process FHS packets, and establish connections through the POLL/NULL packet exchange.
 *
 * The page scan process follows a specific state machine sequence:
 * 1. Listen for ID packets on page scan frequency
 * 2. Respond with ID packet when target address matches
 * 3. Receive FHS packet with timing and connection parameters
 * 4. Exchange POLL/NULL packets to establish active connection
 *
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 */

 #include "page_scan.h"
 #include "btc_device.h"
 #include "btc_rail.h"
 #include "page_scan_sm_autogen.h"
 #include "state_machine.h"
 #include "string.h"
 #ifdef SIM
 #include "rail_simulation.h"
 #else
 #include "btc_rail_intf.h"
 #endif
 #include "lpw_scheduler.h"
 #include "lpw_to_host_interface.h"
 #include "debug_logs.h"
 #include "sl_debug_btc_llc_autogen.h"

/**
 * @brief Checks operation status and triggers appropriate state machine action
 *
 * Helper function that examines the status returned by radio operations and
 * triggers the appropriate state machine step if an error occurred.
 *
 * @param status Status code returned from radio operations
 * @param page_scan_p Pointer to page scan procedure structure
 * @param trigger State machine trigger to invoke on error
 */
static void check_status_page_scan(uint8_t status, page_scan_t *page_scan_p, uint8_t trigger)
{
  if (BTC_RAIL_PASS_STATUS != status) {
    SM_STEP(PAGE_SCAN_SM, page_scan_p->page_scan_sm, trigger);
  }
}

static page_scan_t* get_page_scan_p(void)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  return &btc_dev_p->page_scan;
}

static inline uint32_t live_clkn(page_scan_t *s)
{
  return s->trx_config.btc_rail_trx_config.clk;
}

/*
 * ============================================================================
 * FREQUENCY HOPPING SEQUENCE MANAGEMENT (PERIPHERAL/PAGE SCAN)
 * ============================================================================

 * This section implements Bluetooth Classic frequency hopping sequences for
 * the peripheral (page scan) side per Bluetooth Core Specification.

 * SEQUENCE TYPES:
 * - PGSN:  Standard Page Scan sequence
         Listens on standard page scan frequencies
 * - IPGSN: Interlaced Page Scan sequence
         Listens on interlaced page scan frequencies
 * - PGRSS: Peripheral Page Response sequence
         Response sequence after receiving pager's ID, frozen parameters
 * - BASIC: Connection hopping sequence
         Used after connection establishment, master's LAP for ULAP

 * TRAIN SELECTION:
 * - Page scan uses train B (kOffset=0) for listening
 * - Response uses calculated train based on frozen master clock
 * - BASIC uses train toggle based on master's clock

 * TIMING PARAMETERS:
 * - Frozen clock: Captured at ID reception for response sequence
 * - N counter: Tracks CLKE1 transitions for response sequence
 * - Master LAP: Used for ULAP in BASIC sequence after connection

   ============================================================================
 */
static void reset_to_page_scan_sequence(page_scan_t *page_scan_p)
{
  // Reset sequence to page scan (unfrozen)
  if (page_scan_p->pscan_type == STANDARD_SCAN) {
    page_scan_p->seqSelect = PGSN;
  } else {
    page_scan_p->seqSelect = IPGSN;
  }

  // Reset frozen parameters to unfrozen state
  page_scan_p->btc_fh_data.frozenClk = 0;  // Clear frozen clock sentinel
  page_scan_p->prp_n = 0;                  // Reset N counter
  page_scan_p->prp_last_n_clk = 0;         // Reset N tracking clock

  // Clear any "ID seen" latch/debounce state
  page_scan_p->tx_fail_stats = 0;          // Clear TX failure tracking
  page_scan_p->btc_txrx_fail_stats = 0;    // Clear TRX failure tracking
}

static void update_channel_info_prp()
{
  page_scan_t *page_scan_p = get_page_scan_p();
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_scan_p->btc_fh_data;
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;

  uint32_t current_clkn = live_clkn(page_scan_p);
  if (((current_clkn & BT_CLKE1_MASK) == 0) && ((page_scan_p->prp_last_n_clk & BT_CLKE1_MASK) != 0)) {
    page_scan_p->prp_n = (uint8_t)((page_scan_p->prp_n + 1) & BT_N_COUNTER_MASK);
  }
  page_scan_p->prp_last_n_clk = current_clkn;

  uint8_t k_response = (uint8_t)(((page_scan_p->btc_fh_data.frozenClk >> 12) & 0x1) ^ 1);

  fh_config->frozenClk = page_scan_p->btc_fh_data.frozenClk;
  fh_config->kosAndN   = setBtcFhKosAndN(PGRSS, page_scan_p->prp_n, k_response);
  fh_config->knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
  update_btc_channel_number(btc_trx_configs, fh_config);
}

static void update_channel_info_basic_scan()
{
  page_scan_t *page_scan_p = get_page_scan_p();
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_scan_p->btc_fh_data;
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;

  uint32_t central_clkn_slot_aligned = live_clkn(page_scan_p) & ~0x3;
  uint8_t n_basic = (uint8_t)((central_clkn_slot_aligned >> 12) & 0x1);
  uint8_t k_basic = (uint8_t)(n_basic ^ 1);
  fh_config->masterClk = central_clkn_slot_aligned;
  fh_config->kosAndN   = setBtcFhKosAndN(BASIC, n_basic, k_basic);
  fh_config->knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
  update_btc_channel_number(btc_trx_configs, fh_config);
}

/**
 * @brief Updates channel information for page scan hopping sequence
 *
 * Helper function that updates frequency hopping selection based on the page scan type
 * (standard or interlaced) to ensure correct channel selection.
 *
 * @param page_scan_sm Pointer to the page scan state machine
 */
static void update_channel_info_page_scan()
{
  page_scan_t *page_scan_p = get_page_scan_p();
  RAIL_SEQ_BtcFhConfig_t *fh_config = &page_scan_p->btc_fh_data;
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;

  if (page_scan_p->seqSelect == PGRSS) {
    update_channel_info_prp();
  } else if (page_scan_p->seqSelect == BASIC) {
    update_channel_info_basic_scan();
  } else {
    uint32_t current_clkn = live_clkn(page_scan_p);
    uint8_t scan_sequence_n = (uint8_t)((current_clkn >> 12) & 0x01);
    uint8_t train_toggle_k = 0;
    fh_config->masterClk = (current_clkn & ~0x3);
    fh_config->knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
    uint8_t listen_seq = (page_scan_p->seqSelect == PGSN || page_scan_p->seqSelect == IPGSN)
                         ? page_scan_p->seqSelect
                         : (page_scan_p->pscan_type == STANDARD_SCAN ? PGSN : IPGSN);
    fh_config->kosAndN = setBtcFhKosAndN(listen_seq, scan_sequence_n, train_toggle_k);
    update_btc_channel_number(btc_trx_configs, fh_config);
  }
}

/**
 * @brief Schedules a receive window for ID packets
 *
 * Helper function that configures the radio for ID packet reception,
 * updates channel information, and schedules the receive window.
 *
 * @param page_scan_p Pointer to page scan procedure structure
 */
static void page_scan_schedule_id_rx(page_scan_t *page_scan_p)
{
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t status;

  SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
  update_channel_info_page_scan();
#ifdef SIM
  btc_trx_configs->rx_window = page_scan_p->end_tsf;
#else
  btc_trx_configs->rx_window = (page_scan_p->window * BT_SLOT_WIDTH);
#endif
  status = schedule_rx(btc_trx_configs);
  SM_STATE(page_scan_p->page_scan_sm, PAGE_SCAN_ID_RX_SCHEDULED);
  check_status_page_scan(status, page_scan_p, RX_TIMEOUT);
}

/**
 * @brief Configures and schedules ID packet reception
 *
 * Helper function that checks if the page scan activity should continue,
 * configures radio parameters, and schedules ID packet reception.
 *
 * @param page_scan_p Pointer to page scan procedure structure
 * @param slot Slot timing identifier for scheduling
 * @param curr_slot Current Bluetooth clock value
 */
static void page_scan_configure_schedule_id_rx(page_scan_t *page_scan_p, uint8_t slot, uint32_t curr_slot)
{
  RAIL_SEQ_BTC_ConfigAutoMode(BTC_NATIVE_CLK, BTCCLKMODE_INIT);
  rail_set_btc_clk_instance(&page_scan_p->trx_config.bd_clk_counter_handle);
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->end_tsf)) {
    configure_nonconn_slot_params(btc_trx_configs, slot);
    page_scan_schedule_id_rx(page_scan_p);
  } else {
    page_scan_activity_complete(page_scan_p->page_scan_sm);
  }
}

/**
 * @brief Configures and schedules TX or RX operations
 *
 * Helper function that configures radio parameters and schedules a transmit
 * or receive operation based on the is_tx parameter.
 *
 * @param page_scan_p Pointer to page scan procedure structure
 * @param state_enum State to transition to upon successful scheduling
 * @param slot Slot timing identifier for scheduling
 * @param is_tx Flag to indicate TX (1) or RX (0) operation
 */
static void configure_and_schedule(page_scan_t *page_scan_p, uint8_t state_enum, uint8_t slot, uint8_t is_tx)
{
  uint32_t status = BTC_RAIL_PASS_STATUS;
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;

  configure_nonconn_slot_params(btc_trx_configs, slot);
  update_channel_info_page_scan();
  if (is_tx) {
    status = schedule_tx(btc_trx_configs);
  } else {
    status = schedule_rx(btc_trx_configs);
  }

  SM_STATE(page_scan_p->page_scan_sm, state_enum);
  if (is_tx) {
    check_status_page_scan(status, page_scan_p, TX_ABORT);
  } else {
    check_status_page_scan(status, page_scan_p, RX_TIMEOUT);
  }
}

/**
 * @brief Configures and schedules a reception operation with ACL parameters
 *
 * Helper function that prepares the radio for receiving ACL packets by:
 * 1. Updating channel information for the current frequency hop
 * 2. Setting appropriate timeout for reception window
 * 3. Configuring the receiver for basic rate reception
 * 4. Scheduling the reception and updating state machine state
 *
 * @param btc_trx_configs Pointer to the transceiver configuration structure
 * @param page_scan_p Pointer to page scan procedure structure
 * @param page_scan_sm Pointer to page scan state machine
 * @param next_state State to transition to upon successful scheduling
 */
static void configure_and_schedule_rx(btc_trx_configs_t *btc_trx_configs, page_scan_t *page_scan_p, sm_t *page_scan_sm, uint8_t next_state)
{
  uint32_t status = BTC_RAIL_PASS_STATUS;
  uint32_t rxTimeout;
  update_channel_info_page_scan();
#ifdef SIM
  rxTimeout = page_scan_p->end_tsf;
#else
  rxTimeout = BTC_RAIL_RX_TIMEOUT;
#endif
  configure_trx_acl_rx(btc_trx_configs, BASIC_RATE, rxTimeout);
  btc_trx_configs->rx_window = rxTimeout;
  status = schedule_rx(btc_trx_configs);
  SM_STATE(page_scan_sm, next_state);
  check_status_page_scan(status, page_scan_p, RX_TIMEOUT);
}

/**
 * @brief Sends page scan event notification to upper layers
 *
 * Helper function that prepares and sends notifications about page scan procedure
 * events to the Upper Link Controller (ULC), including completion status.
 *
 * @param status Status code indicating type of page scan notification
 */
static void send_page_scan_notification_to_ulc(btc_internal_event_status_t status, hss_event_page_scan_complete_t *page_scan_complete)
{
  uint8_t* buffer = NULL;
  pkb_t *pkb;

  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_PAGE_SCAN_COMPLETE, status);
  if (status == BTC_CONNECTION_COMPLETED) {
    prep_hss_page_scan_complete_event(buffer, page_scan_complete);
  }
  btc_lpw_activity_complete_indication_to_ull(pkb);
}

/**
 * @brief Initiates the Bluetooth page scan procedure
 *
 * This function is called in response to an HCI command to start the page scan
 * procedure with previously configured parameters. The function:
 * 1. Sets page scan parameters from the command (priority, end time)
 * 2. Initializes and configures the page scan state machine
 * 3. Initializes the Bluetooth clock counter
 * 4. Calculates the end time for the procedure
 * 5. Configures the access address (sync word)
 * 6. Triggers the page scan process by stepping to START_PROCEDURE state
 *
 * The page scan procedure will continue until either:
 * - A connection is established with a central device
 * - The specified page scan duration has elapsed
 * - A stop_page_scan command is received
 *
 * @param hss_cmd Pointer to the HCI command buffer containing page scan parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void start_page_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint32_t btcReference = 0;
  uint64_t protimerReference = 0;
  page_scan_t *page_scan = &btc_dev_p->page_scan;
  page_scan->priority = *HSS_CMD_START_PAGE_SCAN_PRIORITY(hss_cmd);
  page_scan->end_tsf = *HSS_CMD_START_PAGE_SCAN_END_TSF(hss_cmd);
  page_scan->page_scan_sm                                 = &page_scan_sm;
  page_scan_sm.current_state                              = page_scan_sm_idle;
  update_aperiodic_role(btc_dev_p, btc_dev_p->page_scan.page_scan_sm);
  page_scan->seqSelect = (page_scan->pscan_type == STANDARD_SCAN) ? PGSN : IPGSN;
  page_scan->btc_fh_data.kosAndN = setBtcFhKosAndN(page_scan->seqSelect, 0, 0);   // update sequence selection to page scan
  page_scan->prp_n = 0;
  // Set ULAP for page scan frequency hopping - use device's own LAP from BD address
  page_scan->btc_fh_data.ulap = (page_scan->page_scan_bd_address[0]
                                 | (page_scan->page_scan_bd_address[1] << 8)
                                 | (page_scan->page_scan_bd_address[2] << 16));
 #ifdef SIM
  log2vcd_prints_wrapper(btc_dev_p->page_scan.page_scan_sm);
 #else
  btc_dev_p->page_scan.trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
 #endif
  RAIL_SEQ_BTC_InitBtcDate(BTC_NATIVE_CLK, btcReference, protimerReference);
  RAIL_SEQ_BTC_ConfigAutoMode(BTC_NATIVE_CLK, BTCCLKMODE_INIT);
  rail_set_btc_clk_instance(&btc_dev_p->page_scan.trx_config.bd_clk_counter_handle);
  // Update the end_tsf in terms of bluetooth clock ticks
  btc_dev_p->page_scan.end_tsf = btc_get_current_bd_clk_cnt(&btc_dev_p->page_scan.trx_config.bd_clk_counter_handle) + (btc_dev_p->page_scan.end_tsf * HALF_SLOT_FACTOR);  // calculating end tsf and adding tsf that has been elapsed. (Procedure activity duration)
  RAIL_SEQ_BTC_setAccessAddress(btc_dev_p->page_scan.sync_word);
  RAIL_SEQ_BTC_setHecInit(btc_dev_p->page_scan.page_scan_bd_address[3]);
  RAIL_SEQ_BTC_setCrcInit(btc_dev_p->page_scan.page_scan_bd_address[3]);
  SM_STEP(PAGE_SCAN_SM, btc_dev_p->page_scan.page_scan_sm, START_PROCEDURE);
}

/**
 * @brief Configures parameters for a subsequent page scan procedure
 *
 * This function configures the page scan parameters in preparation for running a
 * page scan procedure via start_page_scan(). Parameters include:
 * - Window: Duration to listen on each scan channel
 * - Interval: Time between consecutive scan windows
 * - Scan type: Standard or interlaced frequency hopping pattern
 * - TX power: Transmit power level for response packets
 * - BD Address: Bluetooth address of the scanning device
 * - Sync word: Synchronization word for page scan packets
 *
 * @param hss_cmd Pointer to the HCI command buffer containing parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void set_page_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  btc_dev_p->page_scan.window = *HSS_CMD_SET_PAGE_SCAN_WINDOW(hss_cmd);
  btc_dev_p->page_scan.interval = *HSS_CMD_SET_PAGE_SCAN_INTERVAL(hss_cmd);
  btc_dev_p->page_scan.pscan_type = *HSS_CMD_SET_PAGE_SCAN_CURR_SCAN_TYPE(hss_cmd);
  btc_dev_p->non_connected_tx_pwr_index = *HSS_CMD_SET_PAGE_SCAN_TX_PWR_INDEX(hss_cmd);
  memcpy(btc_dev_p->page_scan.page_scan_bd_address, HSS_CMD_SET_PAGE_SCAN_BD_ADDRESS(hss_cmd), BD_ADDR_LEN);
  memcpy(btc_dev_p->page_scan.sync_word, HSS_CMD_SET_PAGE_SCAN_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
}

/**
 * @brief Stops an ongoing page scan procedure
 *
 * This function terminates an active page scan procedure. It performs the following actions:
 * 1. Immediately transitions the state machine to idle state
 * 2. Aborts any pending radio operations using RAIL_SEQ_Idle
 * 3. Completes the page scan activity and notifies upper layers
 * 4. Releases radio resources and returns to idle state
 *
 * @param hss_cmd Pointer to the HCI command buffer (not used)
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void stop_page_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)hss_cmd;
  btc_common_stop(btc_dev_p->page_scan.page_scan_sm, page_scan_activity_complete, page_scan_sm_idle);
}

/**
 * @brief Handles the START_PROCEDURE trigger for the page scan state machine
 *
 * This function initializes the page scan procedure after being triggered by
 * the state machine. It configures transmit power, updates channel information
 * for page scan hopping pattern, and schedules the first ID packet reception window.
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_start_procedure_handler(sm_t *page_scan_sm)
{
  (void)page_scan_sm;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_scan_t *page_scan_p = &btc_dev_p->page_scan;
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  btc_trx_configs->ignore_whitening = FALSE; // TODO: Remove this once hopping is properly tested in statemachines
  btc_trx_configs->btc_rail_trx_config.txPower = btc_dev_p->non_connected_tx_pwr_index;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  update_channel_info_page_scan();
  page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_1, curr_slot);
}

/**
 * @brief Handles successful reception of ID packet from a paging device
 *
 * Called when an ID packet addressed to this device is received. The function:
 * 1. Captures timing information from the received packet
 * 2. Configures radio parameters for transmitting the ID response
 * 3. Schedules ID packet transmission as response
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_id_rx_done_handler(sm_t *page_scan_sm)
{
  (void)page_scan_sm;
  uint32_t status;
  uint32_t BtcDate = 0;
  uint64_t ProtimerDate = 0;
  uint32_t curr_slot = 0;
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  RAIL_SEQ_BTC_GetLastRxDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, &BtcDate, &ProtimerDate);
  curr_slot = BtcDate;

  page_scan_p->seqSelect = PGRSS;
  // Freeze the native clock window at access code detect slot (slot-aligned) and init N tracking
  page_scan_p->btc_fh_data.frozenClk = (curr_slot & ~0x3);
  page_scan_p->prp_n = 0;   // first response keeps X same as when access code recognized
  page_scan_p->prp_last_n_clk = curr_slot | BT_CLKE1_MASK;

  configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_2);
  page_scan_p->btc_fh_data.masterClk = btc_trx_configs->btc_rail_trx_config.clk;
  // page_scan_p->btc_fh_data.frozenClk = page_scan_p->btc_fh_data.masterClk;
  update_channel_info_page_scan();
  status = schedule_tx(btc_trx_configs);
  SM_STATE(page_scan_p->page_scan_sm, PAGE_SCAN_ID_TX_SCHEDULED);
  check_status_page_scan(status, page_scan_p, TX_ABORT);
}

/**
 * @brief Handles timeout during ID packet reception window
 *
 * Called when no ID packet is received during the configured page scan window.
 * This function:
 * 1. Checks if the scan procedure should continue based on end time
 * 2. For standard scan: Schedules next scan after (interval-window) slots
 * 3. For interlaced scan: Ensures interval is at least twice the window
 * 4. Completes the activity if past end time
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_id_rx_timeout_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  uint32_t min_interval = (2 * page_scan_p->window);
  (void)page_scan_sm;
  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->end_tsf)) {
    if (((page_scan_p->pscan_type == 1) && (page_scan_p->interval >= min_interval)) || (page_scan_p->pscan_type == 0)) {
 #ifdef SIM
      (void)btc_trx_configs;
      configure_and_schedule(page_scan_p, PAGE_SCAN_ID_RX_SCHEDULED, CURRENT_SLOT_1, 0);
 #else
      btc_trx_configs->btc_rail_trx_config.clk =  curr_slot + ((page_scan_p->interval - page_scan_p->window) * SLOT_WIDTH);
      curr_slot = btc_trx_configs->btc_rail_trx_config.clk;
      configure_nonconn_slot_params_clk(btc_trx_configs, CURRENT_SLOT_1);
      page_scan_schedule_id_rx(page_scan_p);
 #endif
    }
  } else {
    page_scan_activity_complete(page_scan_p->page_scan_sm);
  }
}

/**
 * @brief Handles successful transmission of ID response packet
 *
 * Called when the ID response packet has been successfully transmitted to the
 * central device. This function:
 * 1. Calculates and sets page response timeout for subsequent operations
 * 2. Adjusts timing parameters for FHS packet reception
 * 3. Schedules reception window to listen for FHS packet
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_id_tx_done_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  page_scan_p->procedure_tout_instant = BT_ADD_CLOCK(curr_slot, (PAGE_RESP_TIME_OUT << 2));
  page_scan_p->seqSelect = PGRSS;

  configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_1);
  // Configure the BTC clock in auto mode
  RAIL_SEQ_BTC_ConfigAutoMode(BTC_NATIVE_CLK, BTCCLKMODE_AUTO);
  rail_set_btc_clk_instance(&btc_trx_configs->bd_clk_counter_handle);

  page_scan_p->prp_last_n_clk = (btc_trx_configs->btc_rail_trx_config.clk | 0x2);
  page_scan_p->btc_fh_data.masterClk = btc_trx_configs->btc_rail_trx_config.clk;
  //  all necessary parameters for slot programming
  configure_and_schedule_rx(btc_trx_configs, page_scan_p, page_scan_sm, PAGE_SCAN_FHS_RX_1_SCHEDULED);
}

/**
 * @brief Handles abortion of ID response packet transmission
 *
 * Called when the ID response packet transmission fails. This function
 * reconfigures the device to continue listening for ID packets on the
 * page scan frequency.
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_id_tx_abort_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&page_scan_p->trx_config.bd_clk_counter_handle);
  (void)page_scan_sm;
  // Return to page scan sequence after response fails
  reset_to_page_scan_sequence(page_scan_p);
  page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_1, curr_slot);
}

/**
 * @brief Handles successful reception of FHS packet from paging device
 *
 * Called when an FHS packet is successfully received from the central device.
 * This function:
 * 1. Captures timing information and stores FHS packet contents
 * 2. Processes the received FHS packet to extract connection parameters
 * 3. Configures radio for ID response to FHS
 * 4. Schedules transmission of ID response
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_fhs_rx_done_handler(sm_t *page_scan_sm)
{
  (void)page_scan_sm;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  page_scan_t *page_scan_p = &btc_dev_p->page_scan;
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t status;
#ifndef SIM
  memcpy(btc_dev_p->fhs_rx_pkt, received_rx_pkt, received_rx_pkt_len);
#endif
  uint32_t BtcDate = 0;
  uint64_t ProtimerDate = 0;
  RAIL_SEQ_BTC_GetLastRxDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, &BtcDate, &ProtimerDate);
  // Store the clock and TSF values
  page_scan_p->received_rx_fhs_pkt.fhs_rx_tsf = ProtimerDate >> TSF_SCALE_FACTOR;
  llc_process_rx_fhs_pkt(btc_dev_p->fhs_rx_pkt, &page_scan_p->received_rx_fhs_pkt, btc_dev_p->page_scan.remote_bd_address);
  btc_trx_configs->ignore_rx_pkt = 0; // Initiate DMA transfer for the received FHS packet
  SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);

  page_scan_p->seqSelect = PGRSS;
  uint32_t clkn = page_scan_p->trx_config.btc_rail_trx_config.clk;
  page_scan_p->prp_last_n_clk = clkn;
#ifndef SIM
  // Schedule a ID packet tx after 625 us from FHS rx
  btc_trx_configs->btc_rail_trx_config.clk = BT_ADD_CLOCK(btc_trx_configs->btc_rail_trx_config.clk, 2);
#else
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_2);
#endif
  // Update channel info for PRP using the finalized scheduling clock
  update_channel_info_page_scan();
  status = schedule_tx(btc_trx_configs);
  SM_STATE(page_scan_p->page_scan_sm, PAGE_SCAN_FHS_RESPONSE_TX_SCHEDULED);
  check_status_page_scan(status, page_scan_p, TX_ABORT);
}

/**
 * @brief Handles timeout during first FHS packet reception attempt
 *
 * Called when no FHS packet is received during the first reception window.
 * This function:
 * 1. Checks if still within procedure timeout
 * 2. Schedules second FHS reception attempt if within timeout
 * 3. Notifies upper layers and returns to scanning if timeout expired
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_fhs_rx_1_timeout_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);

  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->procedure_tout_instant)) {
    configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_1);
    page_scan_p->prp_last_n_clk = (btc_trx_configs->btc_rail_trx_config.clk | BT_CLKE1_MASK);
    page_scan_p->btc_fh_data.masterClk = btc_trx_configs->btc_rail_trx_config.clk;
    configure_and_schedule_rx(btc_trx_configs, page_scan_p, page_scan_sm, PAGE_SCAN_FHS_RX_2_SCHEDULED);
  } else {
    // Send page response timeout event to ULC
    send_page_scan_notification_to_ulc(BTC_CONNECTION_RESP_TOUT, NULL);
    // Return to page scan sequence after timeout
    reset_to_page_scan_sequence(page_scan_p);
    page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_1, curr_slot);
  }
}

/**
 * @brief Handles timeout during second FHS packet reception attempt
 *
 * Called when no FHS packet is received during the second reception window.
 * This function:
 * 1. Checks if still within procedure timeout
 * 2. Schedules another FHS reception attempt if within timeout
 * 3. Notifies upper layers and returns to scanning if timeout expired
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_fhs_rx_2_timeout_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->procedure_tout_instant)) {
    configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_3);
    configure_and_schedule_rx(btc_trx_configs, page_scan_p, page_scan_sm, PAGE_SCAN_FHS_RX_1_SCHEDULED);
  } else {
    // Send page response timeout event to ULC
    send_page_scan_notification_to_ulc(BTC_CONNECTION_RESP_TOUT, NULL);
    // Return to page scan sequence after timeout
    reset_to_page_scan_sequence(page_scan_p);
    page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_1, curr_slot);
  }
}

/**
 * @brief Handles successful transmission of ID response to FHS packet
 *
 * Called when the ID response to FHS has been successfully transmitted.
 * This function:
 * 1. Initializes Bluetooth clock with master timing from FHS
 * 2. Sets up access address for connection
 * 3. Notifies upper layers of connection creation
 * 4. Configures radio for POLL packet reception
 * 5. Schedules reception window for POLL packet
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_fhs_response_tx_done_handler(sm_t *page_scan_sm)
{
  (void)page_scan_sm;
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t btcReference = page_scan_p->received_rx_fhs_pkt.fhs_rx_clk;
  uint64_t protimerReference = page_scan_p->received_rx_fhs_pkt.fhs_rx_tsf << TSF_SCALE_FACTOR;
  uint32_t rxTimeout;
  btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id = BTC_MASTER_CLK;
  RAIL_SEQ_BTC_InitBtcDate(BTC_MASTER_CLK, btcReference, protimerReference);
  RAIL_SEQ_BTC_ConfigAutoMode(BTC_MASTER_CLK, BTCCLKMODE_AUTO);
  rail_set_btc_clk_instance(&btc_trx_configs->bd_clk_counter_handle);
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  RAIL_SEQ_BTC_setAccessAddress(page_scan_p->received_rx_fhs_pkt.rx_sync_word);
  RAIL_SEQ_BTC_setHecInit(page_scan_p->remote_bd_address[3]);
  RAIL_SEQ_BTC_setCrcInit(page_scan_p->remote_bd_address[3]);
  page_scan_p->procedure_tout_instant = BT_ADD_CLOCK(curr_slot, (NEW_CONN_TIME_OUT << 2));
  page_scan_p->seqSelect = BASIC;
  // Set FH ULAP to master's LAP (remote BD address from FHS) for BASIC hopping
  page_scan_p->btc_fh_data.ulap = (page_scan_p->remote_bd_address[0]
                                   | (page_scan_p->remote_bd_address[1] << 8)
                                   | (page_scan_p->remote_bd_address[2] << 16));
#ifdef SIM
  rxTimeout = page_scan_p->end_tsf;
#else
  rxTimeout = BTC_RAIL_RX_TIMEOUT;
#endif
  configure_trx_acl_rx(btc_trx_configs, BASIC_RATE, rxTimeout);
  configure_and_schedule(page_scan_p, PAGE_SCAN_CENTRAL_POLL_RX_SCHEDULED, CURRENT_SLOT_2, 0);
}

/**
 * @brief Handles abortion of ID response to FHS packet
 *
 * Called when ID response to FHS transmission fails. This function:
 * 1. Checks if still within procedure timeout
 * 2. Attempts to receive the FHS packet again if within timeout
 * 3. Notifies upper layers and returns to scanning if timeout expired
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_fhs_response_tx_abort_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  //  all necessary parameters for slot programming
  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->procedure_tout_instant)) {
    uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
    configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_2);
    //  all necessary parameters for slot programming
    configure_and_schedule_rx(btc_trx_configs, page_scan_p, page_scan_sm, PAGE_SCAN_FHS_RX_1_SCHEDULED);
  } else {
    // Send page response timeout event to ULC
    send_page_scan_notification_to_ulc(BTC_CONNECTION_RESP_TOUT, NULL);
    // Return to page scan sequence after timeout
    reset_to_page_scan_sequence(page_scan_p);
    page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_2, curr_slot);
  }
}

/**
 * @brief Handles successful reception of POLL packet from central
 *
 * Called when a POLL packet is successfully received from the central device.
 * This function:
 * 1. Sets flag to ignore POLL packet data (POLL has no payload)
 * 2. Forms NULL packet for response
 * 3. Schedules transmission of NULL packet
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_central_poll_rx_done_handler(sm_t *page_scan_sm)
{
  (void)page_scan_sm;
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  btc_trx_configs->ignore_rx_pkt = 1;  // To avoid DMA transfer for the received POLL packet

  if (page_scan_p->seqSelect != BASIC) {
    page_scan_p->seqSelect = BASIC;
  }
  update_channel_info_basic_scan();
  form_poll_and_null_pkt(&page_scan_p->trx_config, page_scan_p->received_rx_fhs_pkt.lt_addr, BT_NULL_PKT_TYPE);
  configure_and_schedule(page_scan_p, PAGE_SCAN_PERIPHERAL_NULL_TX_SCHEDULED, CURRENT_SLOT_2, 1);
}

/**
 * @brief Handles timeout during POLL packet reception
 *
 * Called when no POLL packet is received within the timeout period.
 * This function:
 * 1. Checks if still within new connection timeout
 * 2. Schedules another reception attempt if within timeout
 * 3. Notifies upper layers and returns to scanning if timeout expired
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_central_poll_rx_timeout_handler(sm_t *page_scan_sm)
{
  uint32_t status = BTC_RAIL_PASS_STATUS;
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  //  all necessary parameters for slot programming
  configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_4);
  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->procedure_tout_instant)) {
    update_channel_info_page_scan();
    configure_trx_acl_rx(btc_trx_configs, BASIC_RATE, BTC_RAIL_RX_TIMEOUT);
    status = schedule_rx(btc_trx_configs);
    SM_STATE(page_scan_sm, PAGE_SCAN_CENTRAL_POLL_RX_SCHEDULED);
    check_status_page_scan(status, page_scan_p, RX_TIMEOUT);
  } else {
    send_page_scan_notification_to_ulc(BTC_NEW_CONNECTION_TOUT, NULL);
    // Return to page scan sequence after connection timeout
    reset_to_page_scan_sequence(page_scan_p);
    page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_1, curr_slot);
  }
}

/**
 * @brief Handles successful transmission of NULL packet response
 *
 * Called when NULL packet has been successfully transmitted in response to POLL.
 * This function:
 * 1. Notifies upper layers of connection completion
 * 2. Transitions state machine to idle state
 * 3. Logs connection completion event
 *
 * This marks the successful completion of the page scan procedure with established connection.
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_peripheral_null_tx_done_handler(sm_t *page_scan_sm)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  hss_event_page_scan_complete_t page_scan_complete;
  if (btc_dev_p->connection_info_idx < MAX_NUMBER_OF_CONNECTIONS) {
    page_scan_complete.connection_info_idx = btc_dev_p->connection_info_idx;
    memcpy(page_scan_complete.remote_bd_address, btc_dev_p->page_scan.remote_bd_address, BD_ADDR_LEN);
    // Update connection info
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].trx_config.bd_clk_counter_handle = btc_dev_p->page_scan.trx_config.bd_clk_counter_handle;
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].lt_addr = btc_dev_p->page_scan.received_rx_fhs_pkt.lt_addr;
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].sync_word[0] = btc_dev_p->page_scan.received_rx_fhs_pkt.rx_sync_word[0];
    btc_dev_p->connection_info[btc_dev_p->connection_info_idx].sync_word[1] = btc_dev_p->page_scan.received_rx_fhs_pkt.rx_sync_word[1];
    memcpy(btc_dev_p->connection_info[btc_dev_p->connection_info_idx].bd_address, btc_dev_p->page_scan.remote_bd_address, BD_ADDR_LEN);
    btc_dev_p->connection_info_idx++;
    send_page_scan_notification_to_ulc(BTC_CONNECTION_COMPLETED, &page_scan_complete);
  } else {
    // TODO: Handle case where max number of connections is reached
  }
  stop_aperiodic_role(btc_dev_p);
  SM_STATE(page_scan_sm, page_scan_sm_idle);
  debug_log_event(DEBUG_PTI, PTI_CONNECTION_COMPLETED);
}

/**
 * @brief Handles abortion of NULL packet transmission
 *
 * Called when NULL packet transmission fails. This function:
 * 1. Checks if still within new connection timeout
 * 2. Schedules POLL packet reception retry if within timeout
 * 3. Notifies upper layers and returns to scanning if timeout expired
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_peripheral_null_tx_abort_handler(sm_t *page_scan_sm)
{
  page_scan_t *page_scan_p = get_page_scan_p();
  btc_trx_configs_t *btc_trx_configs = &page_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  //  all necessary parameters for slot programming
  configure_nonconn_slot_params(btc_trx_configs, CURRENT_SLOT_2);
  if (IS_BT_CLOCK_LT(curr_slot, page_scan_p->procedure_tout_instant)) {
    configure_and_schedule_rx(btc_trx_configs, page_scan_p, page_scan_sm, PAGE_SCAN_CENTRAL_POLL_RX_SCHEDULED);
  } else {
    send_page_scan_notification_to_ulc(BTC_NEW_CONNECTION_TOUT, NULL);
    // Return to page scan sequence after connection timeout
    reset_to_page_scan_sequence(page_scan_p);
    page_scan_configure_schedule_id_rx(page_scan_p, CURRENT_SLOT_1, curr_slot);
  }
}

/**
 * @brief Completes the page scan activity and performs necessary cleanup
 *
 * This function is called when the page scan procedure completes, either successfully
 * or due to timeout/error. It:
 * 1. Logs completion event for debugging
 * 2. Notifies upper layers that activity is complete
 * 3. Stops the aperiodic role (releases radio resources)
 * 4. Sets state machine to idle state
 * 5. Configures radio to idle state to save power
 *
 * @param page_scan_sm Pointer to the page scan state machine structure
 */
void page_scan_activity_complete(sm_t *page_scan_sm)
{
  btc_activity_complete(page_scan_sm, (void (*)(btc_internal_event_status_t, void *))send_page_scan_notification_to_ulc, PTI_PAGE_SCAN_COMPLETED, page_scan_sm_idle);
}
