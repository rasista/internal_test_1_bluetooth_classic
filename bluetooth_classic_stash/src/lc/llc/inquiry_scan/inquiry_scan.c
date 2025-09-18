/**
 * @file inquiry_scan.c
 * @brief Bluetooth Classic Inquiry Scan procedure implementation
 *
 * This file implements the Bluetooth Classic Inquiry Scan procedure as specified in
 * the Bluetooth Core Specification. The inquiry scan procedure allows a peripheral device
 * to listen for inquiry requests from central devices, respond with FHS packets containing
 * device information, and optionally send Extended Inquiry Response (EIR) data.
 *
 * The inquiry scan process follows a specific state machine sequence:
 * 1. Listen for ID packets on inquiry scan frequency
 * 2. Respond with FHS packet containing device information when ID is received
 * 3. Optionally transmit EIR packet with additional device information
 * 4. Return to scanning state to listen for more inquiry requests
 *
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2024
 */

#include "inquiry_scan.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "inquiry_scan_sm_autogen.h"
#include "state_machine.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "btc_rail_intf.h"
#endif
#include "debug_logs.h"
#include "lpw_scheduler.h"
#include "lpw_to_host_interface.h"

static void check_status_inquiry_scan(uint8_t status, inquiry_scan_t *inquiry_scan_p, uint8_t trigger)
{
  if (BTC_RAIL_PASS_STATUS != status) {
    SM_STEP(INQUIRY_SCAN_SM, inquiry_scan_p->inquiry_scan_sm, trigger);
  }
}

static inquiry_scan_t* get_inquiry_scan_p(void)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  return &btc_dev_p->inquiry_scan;
}

/**
 * @brief Updates channel information for inquiry scan hopping sequence
 *
 * Helper function that updates frequency hopping selection based on the inquiry scan type
 * (standard or interlaced) to ensure correct channel selection. This function maintains
 * the proper GIAC/DIAC inquiry access code and hopping sequence parameters.
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine
 */
static void update_channel_info_inquiry_scan(btc_trx_configs_t *btc_trx_configs)
{
  inquiry_scan_t *inquiry_scan_p = get_inquiry_scan_p();

  // Frequency hopping inputs for Inquiry Scan (receiver role):
  // - ulap: Use GIAC/DIAC to derive inquiry scan listening frequencies.
  // - masterClk: Current native clock selects the hop index for the RX window.
  // - seqSelect: Selects which scan sequence to follow:
  //     IQSN  = Standard inquiry scan (single-slot cadence)
  //     IIQSN = Interlaced inquiry scan (alternates slot parity for faster discovery)
  //     IQRS  = Response sub-sequence used after receiving an ID (for FHS/EIR timing)
  // - NVal: Response-sequence counter advanced after each FHS/EIR completes; used
  //   to compute timing/channel for subsequent responses in IQRS.
  // - kOffset: Always 0 for scan; train selection (A/B) is handled by the inquirer.
  //   The scanner listens on the channels determined from seqSelect+NVal.
  // - kosAndN: Encodes seqSelect and N into the hop generator; set via setBtcFhKosAndN.
  //   When in IQRS (after ID reception), N is the active NVal; when scanning (IQSN/IIQSN)
  //   we use N=0 per the scan definition.

  inquiry_scan_p->btc_fh_data.ulap = GENERIC_INQUIRY_ADDRESS;

  // Align clock to slot boundary (native clock)
  inquiry_scan_p->btc_fh_data.masterClk = btc_trx_configs->btc_rail_trx_config.clk; // 2-slot align (preferred)

  const uint8_t k_offset = 0;   // train select for scan path
  const uint8_t N_scan   = 0;   // N=0 during passive scanning sequences

  if (inquiry_scan_p->seqSelect == IQRS) {
    // After ID is received, switch to the response sub-sequence (IQRS):
    // keep channel context coherent for FHS/EIR and drive timing via NVal.
    inquiry_scan_p->btc_fh_data.kosAndN = setBtcFhKosAndN(IQRS, (inquiry_scan_p->NVal & 0x1F), k_offset);
    inquiry_scan_p->btc_fh_data.knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
    // NOTE: NVal is incremented AFTER each response completes (FHS or EIR),
    //       so we do not bump it here.
  } else if (inquiry_scan_p->curr_scan_type == STANDARD_SCAN) {
    // Standard scan: remain in IQSN with N=0
    inquiry_scan_p->seqSelect = IQSN;
    inquiry_scan_p->btc_fh_data.kosAndN = setBtcFhKosAndN(IQSN, N_scan, k_offset);
  } else {
    // Interlaced scan: alternate slot parity via IIQSN to improve discovery latency
    inquiry_scan_p->seqSelect = IIQSN;
    inquiry_scan_p->btc_fh_data.kosAndN = setBtcFhKosAndN(IIQSN, N_scan, k_offset);
  }
  update_channel_info(btc_trx_configs, &inquiry_scan_p->btc_fh_data);
}

/**
 * @brief Handles the core inquiry scan radio operations
 *
 * This helper function centralizes the common scheduling and configuration logic
 * for inquiry scan operations. It:
 * 1. Configures radio parameters based on the current state
 * 2. Updates channel information for the appropriate hopping sequence
 * 3. Schedules transmit or receive operations as required
 * 4. Updates state machine state and handles status reporting
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine
 * @param btc_trx_configs Pointer to transceiver configuration structure
 * @param inquiry_scan_p Pointer to inquiry scan procedure structure
 * @param slot_param Slot timing identifier for scheduling
 */
static void handle_inquiry_scan(sm_t *inquiry_scan_sm_p, btc_trx_configs_t *btc_trx_configs, inquiry_scan_t *inquiry_scan_p, uint32_t slot_param)
{
  uint32_t status;
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), inquiry_scan_p->end_tsf)) {
    //  all necessary parameters for slot programming
    if (inquiry_scan_sm_p->last_tx_slots > 1) {
      btc_trx_configs->btc_rail_trx_config.clk =  btc_trx_configs->btc_rail_trx_config.clk + inquiry_scan_sm_p->last_tx_slots * SLOT_WIDTH;
      configure_nonconn_slot_params_clk(btc_trx_configs, slot_param);
      inquiry_scan_sm_p->last_tx_slots = 0;
    } else {
      configure_nonconn_slot_params(btc_trx_configs, slot_param);
    }
    update_channel_info_inquiry_scan(btc_trx_configs);
    btc_trx_configs->rx_window = inquiry_scan_p->window * BT_SLOT_WIDTH;
    SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
    status = schedule_rx(btc_trx_configs);
    SM_STATE(inquiry_scan_sm_p, INQUIRY_SCAN_ID_RX_SCHEDULED);
    check_status_inquiry_scan(status, inquiry_scan_p, RX_TIMEOUT);
  } else {
    SM_STATE(inquiry_scan_sm_p, inquiry_scan_sm_idle);
    inquiry_scan_activity_complete(inquiry_scan_sm_p);
  }
}

/**
 * @brief Prepares Extended Inquiry Response (EIR) packet
 *
 * Helper function that formats and prepares the Extended Inquiry Response (EIR)
 * packet based on the configured EIR data. The EIR packet contains additional
 * information about the device such as device name, supported services,
 * and other discoverable attributes.
 *
 * @param inquiry_scan Pointer to the inquiry scan procedure structure
 * @param btc_trx_configs Pointer to the transceiver configuration structure
 * @return Pointer to the prepared EIR packet buffer
 */
static uint8_t* trigger_eir_packet(inquiry_scan_t *inquiry_scan, btc_trx_configs_t *btc_trx_configs)
{
  if (inquiry_scan == NULL || inquiry_scan->eir_length == 0) {
    // If the pointer to inquiry_scan or eir_pkt is NULL, return NULL
    return NULL;
  }
  uint16_t *pkt_hdr = (uint16_t *)inquiry_scan->eir_pkt;
  uint8_t pkt_type = GET_RX_PKT_HEADER_TYPE(*pkt_hdr);
  inquiry_scan->eir_pkt_type = pkt_type;
  btc_trx_configs->tx_pkt = (uint8_t *)inquiry_scan->eir_pkt;   // Assign the allocated buffer to tx_pkt
  btc_trx_configs->btc_rail_trx_config.txPacketLen = inquiry_scan->eir_length; //updating eir packet length
  btc_trx_configs->btc_rail_trx_config.packetConfig.packetType = pkt_type; // Set the packet type for EIR
  // Update packet configuration
  configure_trx_acl_common(btc_trx_configs, BASIC_RATE);
  // Return the pointer to the EIR packet buffer
  return btc_trx_configs->tx_pkt;
}

/**
 * @brief Initiates the Bluetooth inquiry scan procedure
 *
 * This function is called in response to an HCI command to start the inquiry scan
 * procedure with previously configured parameters. The function:
 * 1. Sets inquiry scan parameters from the command (priority, end time)
 * 2. Initializes and configures the inquiry scan state machine
 * 3. Initializes the Bluetooth clock counter
 * 4. Calculates the end time for the procedure
 * 5. Configures the inquiry scan access address (sync word)
 * 6. Triggers the inquiry scan process by stepping to START_PROCEDURE state
 *
 * The inquiry scan procedure will continue until either:
 * - The specified inquiry scan duration has elapsed
 * - A stop_inquiry_scan command is received
 *
 * @param hss_cmd Pointer to the HCI command buffer containing inquiry scan parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void start_inquiry_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint32_t btcReference = 0;
  uint64_t protimerReference = 0;
  btc_dev_p->inquiry_scan.priority = *HSS_CMD_START_INQUIRY_SCAN_PRIORITY(hss_cmd);
  btc_dev_p->inquiry_scan.end_tsf = *HSS_CMD_START_INQUIRY_SCAN_END_TSF(hss_cmd);
  btc_dev_p->inquiry_scan.inquiry_scan_sm = &inquiry_scan_sm;
  inquiry_scan_sm.current_state = inquiry_scan_sm_idle;

  update_aperiodic_role(btc_dev_p, btc_dev_p->inquiry_scan.inquiry_scan_sm);
  btc_dev_p->inquiry_scan.seqSelect = IQSN;
  btc_dev_p->inquiry_scan.btc_fh_data.kosAndN = setBtcFhKosAndN(IQSN, 0, 0); //update sequence selection to Inquiry scan
  // Ensure knudge/ILOS are cleared for inquiry scan procedures
  btc_dev_p->inquiry_scan.btc_fh_data.knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
  btc_dev_p->inquiry_scan.NVal = 0;
  btc_dev_p->inquiry_scan.scan_end_time = (btc_dev_p->inquiry_scan.window * BT_SLOT_WIDTH)
                                          + btc_get_current_clk();
#ifdef SIM
  log2vcd_prints_wrapper(btc_dev_p->inquiry_scan.inquiry_scan_sm);
#else
  btc_dev_p->inquiry_scan.trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
#endif
  RAIL_SEQ_BTC_InitBtcDate(BTC_NATIVE_CLK, btcReference, protimerReference);
  RAIL_SEQ_BTC_ConfigAutoMode(BTC_NATIVE_CLK, BTCCLKMODE_INIT);
  rail_set_btc_clk_instance(&btc_dev_p->inquiry_scan.trx_config.bd_clk_counter_handle);
  btc_dev_p->inquiry_scan.end_tsf = btc_get_current_bd_clk_cnt(&btc_dev_p->inquiry_scan.trx_config.bd_clk_counter_handle) + (btc_dev_p->inquiry_scan.end_tsf * HALF_SLOT_FACTOR); //calculating end tsf and adding tsf that has been elapsed. (Procedure activity duration)
  RAIL_SEQ_BTC_setAccessAddress(btc_dev_p->inquiry_scan.sync_word);
  RAIL_SEQ_BTC_setHecInit(BT_INQUIRY_SCAN_HEC_INIT_VALUE);
  RAIL_SEQ_BTC_setCrcInit(BT_INQUIRY_SCAN_CRC_INIT_VALUE);
  SM_STEP(INQUIRY_SCAN_SM, btc_dev_p->inquiry_scan.inquiry_scan_sm, START_PROCEDURE);
}

/**
 * @brief Configures parameters for a subsequent inquiry scan procedure
 *
 * This function configures the inquiry scan parameters in preparation for running an
 * inquiry scan procedure via start_inquiry_scan(). Parameters include:
 * - Window: Duration to listen on each inquiry scan channel
 * - Interval: Time between consecutive scan windows
 * - Scan type: Standard or interlaced frequency hopping pattern
 * - TX power: Transmit power level for response packets
 * - BD Address: Bluetooth address of the scanning device
 * - Sync word: Synchronization word for inquiry scan packets
 * - EIR data: Extended Inquiry Response data (if applicable)
 *
 * @param hss_cmd Pointer to the HCI command buffer containing parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void set_inquiry_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  inquiry_scan_t *inquiry_scan = &btc_dev_p->inquiry_scan;
  inquiry_scan->window = *HSS_CMD_SET_INQUIRY_SCAN_WINDOW(hss_cmd);
  inquiry_scan->curr_scan_type = *HSS_CMD_SET_INQUIRY_SCAN_CURR_SCAN_TYPE(hss_cmd);
  inquiry_scan->interval = *HSS_CMD_SET_INQUIRY_SCAN_INTERVAL(hss_cmd);
  inquiry_scan->EIR_value = *HSS_CMD_SET_INQUIRY_SCAN_EIR_VALUE(hss_cmd);
  inquiry_scan->eir_length = *HSS_CMD_SET_INQUIRY_SCAN_EIR_DATA_LENGTH(hss_cmd);
  memcpy(inquiry_scan->LAP_address, HSS_CMD_SET_INQUIRY_SCAN_LAP_ADDRESS(hss_cmd), LAP_ADDR_LEN);
  memcpy(inquiry_scan->sync_word, HSS_CMD_SET_INQUIRY_SCAN_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
  memcpy(inquiry_scan->eir_pkt, HSS_CMD_SET_INQUIRY_SCAN_EIR_DATA(hss_cmd), inquiry_scan->eir_length);
}

/**
 * @brief Stops an ongoing inquiry scan procedure
 *
 * This function terminates an active inquiry scan procedure. It performs the following actions:
 * 1. Immediately transitions the state machine to idle state
 * 2. Aborts any pending radio operations using RAIL_SEQ_Idle
 * 3. Completes the inquiry scan activity and notifies upper layers
 * 4. Releases radio resources and returns to idle state
 *
 * @param hss_cmd Pointer to the HCI command buffer (not used)
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void stop_inquiry_scan(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)hss_cmd;
  btc_common_stop(btc_dev_p->inquiry_scan.inquiry_scan_sm, inquiry_scan_activity_complete, inquiry_scan_sm_idle);
}

/**
 * @brief Handles the START_PROCEDURE trigger for the inquiry scan state machine
 *
 * This function initializes the inquiry scan procedure after being triggered by
 * the state machine. It configures transmit power, updates channel information
 * for inquiry scan hopping pattern, and schedules the first ID packet reception window.
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_start_procedure_handler(sm_t *inquiry_scan_sm_p)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  inquiry_scan_t* inquiry_scan_p = &btc_dev_p->inquiry_scan;
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = btc_dev_p->non_connected_tx_pwr_index;
  btc_trx_configs->ignore_whitening = FALSE;    // TODO: Remove this once hopping is properly tested in statemachines
  inquiry_scan_sm_p->last_tx_slots = 0;
  inquiry_scan_sm_p->last_rx_slots = 0;

  update_channel_info_inquiry_scan(btc_trx_configs);
  handle_inquiry_scan(inquiry_scan_sm_p, btc_trx_configs, inquiry_scan_p, BTC_BD_CLK_2NP0);
}

/**
 * @brief Handles successful reception of ID packet from an inquiring device
 *
 * Called when an ID packet from an inquiring device is received. The function:
 * 1. Captures timing information from the received packet
 * 2. Configures radio parameters for transmitting the FHS response
 * 3. Prepares device information in the FHS packet
 * 4. Schedules FHS packet transmission as response
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_rx_done_handler(sm_t *inquiry_scan_sm_p)
{
  inquiry_scan_t *inquiry_scan_p = get_inquiry_scan_p();
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  btc_dev_t *dev = BTC_DEV_PTR;

  inquiry_scan_p->seqSelect = IQRS;
  inquiry_scan_p->current_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  inquiry_scan_p->target_slot = inquiry_scan_p->current_slot + 2;

  btc_trx_configs->btc_rail_trx_config.clk = inquiry_scan_p->target_slot;

  // All necessary parameters for slot programming
  configure_nonconn_slot_params(btc_trx_configs, (inquiry_scan_p->target_slot) & 0x03);
  update_channel_info_inquiry_scan(btc_trx_configs);
#ifdef SIM
  // FHS packet formation
  packetization_nonconn(btc_trx_configs, TRUE);
#endif
  // Scheduling tx for FHS packet formed
  form_fhs_pkt(btc_trx_configs, dev->fhs_pkt, INQUIRY_LT_ADDR ,0); // Use 0 for old_lt_addr as it is not applicable in inquiry scan
  debug_log_event(DEBUG_PTI, PTI_SCHEDULED_TX_FHS);

  status = schedule_tx(btc_trx_configs);
  SM_STATE(inquiry_scan_sm_p, INQUIRY_SCAN_FHS_TX_CONFIGURED);
  check_status_inquiry_scan(status, inquiry_scan_p, TX_ABORT);
}

/**
 * @brief Handles timeout during ID packet reception window
 *
 * Called when no ID packet is received during the configured inquiry scan window.
 * This function:
 * 1. Checks if the scan procedure should continue based on end time
 * 2. For standard scan: Schedules next scan after (interval-window) slots
 * 3. For interlaced scan: Ensures interval is at least twice the window
 * 4. Completes the activity if past end time
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_rx_timeout_handler(sm_t *inquiry_scan_sm_p)
{
  inquiry_scan_t *inquiry_scan_p = get_inquiry_scan_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  uint32_t curr_slot = btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle);
  uint32_t min_interval_slots = (2 * inquiry_scan_p->window);
  #ifndef SIM
  uint32_t status;
  #endif
  if (IS_BT_CLOCK_LT(curr_slot, inquiry_scan_p->end_tsf)) {
    if (((inquiry_scan_p->curr_scan_type == 1) && (inquiry_scan_p->interval >= min_interval_slots))
        || (inquiry_scan_p->curr_scan_type == 0)) {
    #ifdef SIM
      handle_inquiry_scan(inquiry_scan_sm_p, btc_trx_configs, inquiry_scan_p, BTC_BD_CLK_2NP0);
    #else
      // Schedule next scan after (interval - window) time
      uint32_t sleep_slots = inquiry_scan_p->interval - inquiry_scan_p->window;
      btc_trx_configs->btc_rail_trx_config.clk = curr_slot + (sleep_slots * SLOT_WIDTH);
      // Configure and schedule
      configure_nonconn_slot_params_clk(btc_trx_configs, BTC_BD_CLK_2NP0);
      update_channel_info_inquiry_scan(btc_trx_configs);
      btc_trx_configs->rx_window = inquiry_scan_p->window * BT_SLOT_WIDTH;
      SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
      status = schedule_rx(btc_trx_configs);
      SM_STATE(inquiry_scan_sm_p, INQUIRY_SCAN_ID_RX_SCHEDULED);
      check_status_inquiry_scan(status, inquiry_scan_p, RX_TIMEOUT);
    #endif
    }
  } else {
    inquiry_scan_activity_complete(inquiry_scan_sm_p);
  }
}

/**
 * @brief Handles successful transmission of FHS packet
 *
 * Called when the FHS response packet has been successfully transmitted to the
 * inquiring device. This function:
 * 1. Checks if EIR is supported and configured for this device
 * 2. If EIR is enabled, prepares and schedules EIR packet transmission
 * 3. If EIR is not enabled, returns to the scanning state to listen for more inquiries
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_fhs_tx_done_handler(sm_t *inquiry_scan_sm_p)
{
  inquiry_scan_t* inquiry_scan_p = get_inquiry_scan_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  uint32_t status;
  if (inquiry_scan_p->EIR_value == 1) {
    //  all necessary parameters for slot programming
    configure_nonconn_slot_params(btc_trx_configs, (inquiry_scan_p->target_slot + 4) & 0x03);
    update_channel_info_inquiry_scan(btc_trx_configs);
    // EIR packet formation
#ifdef SIM
    packetization_nonconn(btc_trx_configs, FALSE);
#endif
    trigger_eir_packet(inquiry_scan_p, btc_trx_configs);
    // scheduling tx for EIR packet formed
    inquiry_scan_sm_p->last_tx_slots = packet_type_slot_lookup_table[inquiry_scan_p->eir_pkt_type][0];
    // debug_log_event(DEBUG_PTI, PTI_SCHEDULED_TX_EIR);
    status = schedule_tx(btc_trx_configs);
    SM_STATE(inquiry_scan_sm_p, INQUIRY_SCAN_EIR_TX_CONFIGURED);
    check_status_inquiry_scan(status, inquiry_scan_p, TX_ABORT);
  } else {
    // No EIR: response complete after FHS, increment N and return to scan
    inquiry_scan_p->NVal = (inquiry_scan_p->NVal + 1) & 0x1F;
    // Restore appropriate scan sequence based on scan type
    inquiry_scan_p->seqSelect = (inquiry_scan_p->curr_scan_type == STANDARD_SCAN) ? IQSN : IIQSN;
    handle_inquiry_scan(inquiry_scan_sm_p, btc_trx_configs, inquiry_scan_p, BTC_BD_CLK_2NP0);
  }
}

/**
 * @brief Handles abortion of FHS packet transmission
 *
 * Called when the FHS response packet transmission fails. This function
 * reconfigures the device to continue listening for ID packets on the
 * inquiry scan frequency, implementing the required backoff algorithm
 * as specified in the Bluetooth Core Specification.
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_fhs_tx_abort_handler(sm_t *inquiry_scan_sm_p)
{
  inquiry_scan_t *inquiry_scan_p = get_inquiry_scan_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  // FHS failed: response sequence aborted, don't increment N, return to scan
  // Restore appropriate scan sequence based on scan type
  inquiry_scan_p->seqSelect = (inquiry_scan_p->curr_scan_type == STANDARD_SCAN) ? IQSN : IIQSN;
  handle_inquiry_scan(inquiry_scan_sm_p, btc_trx_configs, inquiry_scan_p, BTC_BD_CLK_2NP0);
}

/**
 * @brief Handles successful transmission of EIR packet
 *
 * Called when the Extended Inquiry Response (EIR) packet has been successfully
 * transmitted following the FHS packet. This function:
 * 1. Configures radio parameters for resuming scan operation
 * 2. Schedules the next ID packet reception window
 * 3. Updates statistics counters for successful EIR transmissions
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_eir_tx_done_handler(sm_t *inquiry_scan_sm_p)
{
  inquiry_scan_t *inquiry_scan_p = get_inquiry_scan_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  // EIR complete: response sequence finished, increment N and return to scan
  inquiry_scan_p->NVal = (inquiry_scan_p->NVal + 1) & 0x1F;   // advance after EIR
  // Restore appropriate scan sequence based on scan type
  inquiry_scan_p->seqSelect = (inquiry_scan_p->curr_scan_type == STANDARD_SCAN) ? IQSN : IIQSN;
  handle_inquiry_scan(inquiry_scan_sm_p, btc_trx_configs, inquiry_scan_p, BTC_BD_CLK_2NP0);
}

/**
 * @brief Handles abortion of EIR packet transmission
 *
 * Called when the EIR packet transmission fails. This function:
 * 1. Updates transmission failure statistics
 * 2. Reconfigures the device to continue listening for ID packets
 * 3. Implements any required backoff algorithm for transmission retries
 *
 * @param inquiry_scan_sm_p Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_eir_tx_abort_handler(sm_t *inquiry_scan_sm_p)
{
  inquiry_scan_t *inquiry_scan_p = get_inquiry_scan_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_scan_p->trx_config;
  // EIR failed but response sequence complete: increment N and return to scan
  inquiry_scan_p->NVal = (inquiry_scan_p->NVal + 1) & 0x1F;
  // Restore appropriate scan sequence based on scan type
  inquiry_scan_p->seqSelect = (inquiry_scan_p->curr_scan_type == STANDARD_SCAN) ? IQSN : IIQSN;
  handle_inquiry_scan(inquiry_scan_sm_p, btc_trx_configs, inquiry_scan_p, BTC_BD_CLK_2NP0);
}

/**
 * @brief Completes the inquiry scan activity and performs necessary cleanup
 *
 * This function is called when the inquiry scan procedure completes, either successfully
 * or due to timeout/error. It:
 * 1. Logs completion event for debugging
 * 2. Notifies upper layers that activity is complete
 * 3. Stops the aperiodic role (releases radio resources)
 * 4. Sets state machine to idle state
 * 5. Configures radio to idle state to save power
 * 6. Resets procedure-specific variables to prepare for next operation
 *
 * @param inquiry_scan_sm Pointer to the inquiry scan state machine structure
 */
void inquiry_scan_activity_complete(sm_t *inquiry_scan_sm)
{
  uint8_t *buffer = NULL;
  pkb_t *pkb;
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;

  debug_log_event(DEBUG_PTI, PTI_INQUIRY_SCAN_COMPLETED);

  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_INQUIRY_SCAN_COMPLETE, BTC_ACTIVITY_DURATION_COMPLETED);
  btc_lpw_activity_complete_indication_to_ull(pkb);

  stop_aperiodic_role(btc_dev_p);
  SM_STATE(inquiry_scan_sm, inquiry_scan_sm_idle);
  rail_configure_idle();
}
