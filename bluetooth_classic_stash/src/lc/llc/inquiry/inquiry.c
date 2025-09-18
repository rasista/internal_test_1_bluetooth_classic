/**
 * @file inquiry.c
 * @brief Bluetooth Classic Inquiry procedure implementation
 *
 * This file implements the Bluetooth Classic Inquiry procedure as specified in
 * the Bluetooth Core Specification. The inquiry procedure allows a device to
 * discover other Bluetooth devices within range by transmitting ID packets and
 * receiving FHS responses, potentially followed by Extended Inquiry Response (EIR)
 * packets containing additional device information.
 *
 * The inquiry process follows a specific timing pattern using Bluetooth slots,
 * with a state machine controlling transmit and receive operations in a sequence:
 * 1. Send ID packets on inquiry hopping frequencies
 * 2. Listen for FHS responses from scanning devices
 * 3. Optionally receive EIR data with additional device information
 *
 * @version 0.1
 * @date 2023-01-18
 *
 * @copyright Copyright (c) 2023-2025
 */
 #include "inquiry.h"
 #include "btc_device.h"
 #include "btc_rail.h"
 #include "inquiry_sm_autogen.h"
 #ifdef SIM
 #include "rail_simulation.h"
 #else
 #include "btc_rail_intf.h"
 #endif
 #include "lpw_scheduler.h"
 #include "lpw_to_host_interface.h"
 #include "debug_logs.h"

static const uint8_t slot_lookup_table[] = {
  BTC_BD_CLK_2NP0,  // INQUIRY_ID_1_TX_SCHEDULED
  BTC_BD_CLK_2NP1,  // INQUIRY_ID_2_TX_SCHEDULED
  BTC_BD_CLK_2NP2,  // INQUIRY_RX_1_SCHEDULED
  BTC_BD_CLK_2NP3   // INQUIRY_RX_2_SCHEDULED
};

static void check_status_inquiry(uint8_t status, inquiry_t *inquiry_p, uint8_t trigger)
{
  if (BTC_RAIL_PASS_STATUS != status) {
    SM_STEP(INQUIRY_SM, inquiry_p->inquiry_sm, trigger);
  }
}

static inquiry_t* get_inquiry_p(void)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  return &btc_dev_p->inquiry;
}

static void configure_and_schedule_rx(inquiry_t *inquiry_p, inquiry_sm_states_t state, uint8_t rx_slot)
{
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;

  configure_nonconn_slot_params(btc_trx_configs, rx_slot);
  // Frequency hopping note:
  // - For EIR RX, we intentionally preserve the same channel as used for the FHS
  //   exchange. The peer sends EIR immediately following FHS using the same hop
  //   context, so we skip updating the hop (no new train/channel calculation here).
  if (state != INQUIRY_EIR_RX_SCHEDULED) {
    update_channel_info_inquiry(btc_trx_configs);
  }
  configure_trx_acl_rx(btc_trx_configs, BASIC_RATE, BTC_RAIL_RX_TIMEOUT);
  btc_trx_configs->rx_window = BTC_RAIL_RX_TIMEOUT;
  status = schedule_rx(btc_trx_configs);    // Update inquiry state machine to INQUIRY_FIRST_ID_RX_SCHEDULED
  SM_STATE(inquiry_p->inquiry_sm, state);    // Update inquiry state machine to
  check_status_inquiry(status, inquiry_p, RX_TIMEOUT);
}

/**
 * @brief Transitions the inquiry state machine to the specified next state
 *
 * This function configures the radio for the next operation in the inquiry
 * sequence based on the provided state. It handles:
 * - Configuring proper slot timing parameters
 * - Channel hopping configuration
 * - Scheduling either TX or RX operations as needed
 * - State machine transitions
 * - Error handling for failed scheduling
 *
 * States 1-2 (INQUIRY_ID_1/2_TX_SCHEDULED) schedule ID packet transmissions
 * States 3-4 (INQUIRY_RX_1/2_SCHEDULED) schedule reception windows
 *
 * @param inquiry_p Pointer to the inquiry structure
 * @param state Next state to transition to (1-4 corresponding to slot_lookup_table)
 */
static void next_state(inquiry_t *inquiry_p, uint32_t state)
{
  uint32_t status;
  uint32_t trigger;
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  uint32_t slot = slot_lookup_table[state - 1];
  sm_t *inquiry_sm_p = inquiry_p->inquiry_sm;

  if (inquiry_sm_p->last_rx_slots > 1) {
    uint32_t BtcDate;
    uint64_t ProtimerDate;
    RAIL_SEQ_BTC_GetLastRxDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, &BtcDate, &ProtimerDate);
    btc_trx_configs->btc_rail_trx_config.clk =  BtcDate + (inquiry_sm_p->last_rx_slots * SLOT_WIDTH);
    configure_nonconn_slot_params_clk(btc_trx_configs, slot);
    inquiry_sm_p->last_rx_slots = 0;
  } else {
    configure_nonconn_slot_params(btc_trx_configs, slot);
  }
  update_channel_info_inquiry(btc_trx_configs);
  if (state > INQUIRY_ID_2_TX_SCHEDULED) {
    status = schedule_rx(btc_trx_configs);
    trigger = RX_TIMEOUT;
  } else {
    SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
    status = schedule_tx(btc_trx_configs);
    trigger = TX_ABORT;
  }

  SM_STATE(inquiry_sm_p, state);
  check_status_inquiry(status, inquiry_p, trigger);
}

/**
 * @brief Initiates the Bluetooth inquiry procedure
 *
 * This function is called in response to an HCI Inquiry command to start the
 * inquiry procedure with previously configured parameters. The function:
 * 1. Sets inquiry parameters from the command (priority, step size, timeout)
 * 2. Initializes and configures the inquiry state machine
 * 3. Updates frequency hopping sequence for inquiry train
 * 4. Sets up Bluetooth clock counter and synchronization
 * 5. Calculates the end time for the procedure based on the requested duration
 * 6. Triggers the inquiry process by stepping to START_PROCEDURE state
 *
 * The inquiry procedure will continue until either:
 * - The specified inquiry duration has elapsed
 * - A stop_inquiry command is received
 *
 * @param hss_cmd Pointer to the HCI command buffer containing inquiry parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void start_inquiry(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint32_t btcReference = 0;
  uint64_t protimerReference = 0;
  btc_dev_p->inquiry.priority = *HSS_CMD_START_INQUIRY_PRIORITY(hss_cmd);
  btc_dev_p->inquiry.step_size = *HSS_CMD_START_INQUIRY_STEP_SIZE(hss_cmd);
  btc_dev_p->inquiry.end_tsf = *HSS_CMD_START_INQUIRY_END_TSF(hss_cmd);
  btc_dev_p->inquiry.inquiry_sm = &inquiry_sm;
  inquiry_sm.current_state = inquiry_sm_idle;
  btc_dev_p->inquiry.btc_fh_data.knudgeAndIlos = setBtcFhKnudgeAndIlos(0, 0);
  update_aperiodic_role(btc_dev_p, btc_dev_p->inquiry.inquiry_sm);
  btc_dev_p->inquiry.btc_fh_data.kosAndN = setBtcFhKosAndN(IQRY, 0, 0);   //update sequence selection to INQUIRY_STATE
 #ifdef SIM
  btc_dev_p->inquiry.trx_config.bd_clk_counter_handle.bd_clk_counter_id = btc_init_bd_clk_counter(0);
  log2vcd_prints_wrapper(btc_dev_p->inquiry.inquiry_sm);
 #else
  btc_dev_p->inquiry.trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
 #endif
  RAIL_SEQ_BTC_InitBtcDate(BTC_NATIVE_CLK, btcReference, protimerReference);
  btc_dev_p->inquiry.end_tsf = btc_get_current_bd_clk_cnt(&btc_dev_p->inquiry.trx_config.bd_clk_counter_handle) + (btc_dev_p->inquiry.end_tsf * HALF_SLOT_FACTOR);  //calculating end tsf and adding tsf that has been elapsed. (Procedure activity duration)
  // Update the end_tsf in terms of bluetooth clock ticks
  RAIL_SEQ_BTC_setAccessAddress(btc_dev_p->inquiry.sync_word);
  RAIL_SEQ_BTC_setHecInit(BT_INQUIRY_HEC_INIT_VALUE);
  RAIL_SEQ_BTC_setCrcInit(BT_INQUIRY_CRC_INIT_VALUE);
  SM_STEP(INQUIRY_SM, btc_dev_p->inquiry.inquiry_sm, START_PROCEDURE);
}

/**
 * @brief Configures parameters for a subsequent inquiry procedure
 *
 * This function configures the inquiry parameters in preparation for running an
 * inquiry procedure via start_inquiry(). Parameters include:
 * - Length: Defines duration of inquiry procedure (timeout)
 * - Clock offset: Used for inquiry train hopping sequence
 * - EIR value: Flag indicating whether Extended Inquiry Response is enabled
 * - TX power: Transmit power level for inquiry packets
 * - HCI trigger: Flag indicating if the inquiry should start immediately
 * - LAP address: General/Dedicated Inquiry Access Code (GIAC/DIAC)
 * - Sync word: Synchronization word for ID packets
 *
 * These parameters control the behavior and timing of the inquiry procedure
 * when it is started via start_inquiry().
 *
 * @param hss_cmd Pointer to the HCI command buffer containing parameters
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void set_inquiry(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  btc_dev_p->inquiry.length = *HSS_CMD_SET_INQUIRY_LENGTH(hss_cmd);
  btc_dev_p->inquiry.EIR_value = *HSS_CMD_SET_INQUIRY_EIR_VALUE(hss_cmd);
  btc_dev_p->non_connected_tx_pwr_index = *HSS_CMD_SET_INQUIRY_TX_PWR_INDEX(hss_cmd);
  btc_dev_p->inquiry.hci_trigger = *HSS_CMD_SET_INQUIRY_HCI_TRIGGER(hss_cmd);
  memcpy(btc_dev_p->inquiry.LAP_address, HSS_CMD_SET_INQUIRY_LAP_ADDRESS(hss_cmd), LAP_ADDR_LEN);
  memcpy(btc_dev_p->inquiry.sync_word, HSS_CMD_SET_INQUIRY_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
}

/**
 * @brief Stops an ongoing inquiry procedure
 *
 * This function terminates an active inquiry procedure, typically in response to
 * an HCI Inquiry_Cancel command. It performs the following actions:
 * 1. Immediately transitions the state machine to idle state
 * 2. Aborts any pending radio operations using RAIL_SEQ_Idle
 * 3. Completes the inquiry activity and notifies upper layers
 * 4. Releases radio resources and returns to idle state
 *
 * The function ensures all resources are properly released and the radio
 * is returned to an idle state to conserve power.
 *
 * @param hss_cmd Pointer to the HCI command buffer (not used)
 * @param btc_dev_p Pointer to the Bluetooth Classic device structure
 */
void stop_inquiry(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)hss_cmd;
  btc_common_stop(btc_dev_p->inquiry.inquiry_sm, inquiry_activity_complete, inquiry_sm_idle);
}

/**
 * @brief Handles the START_PROCEDURE trigger for the inquiry state machine
 *
 * This function initializes the inquiry procedure after being triggered by
 * the state machine. It:
 * 1. Configures transmit power and scrambler seed settings
 * 2. Records the inquiry start time for train hopping calculations
 * 3. Updates channel information for inquiry hopping pattern
 * 4. Checks if current time is within the inquiry duration window
 * 5. Either schedules first ID packet transmission or completes activity if expired
 *
 * This is the entry point for the active inquiry procedure after parameters
 * have been set and the state machine has been initialized.
 *
 * @param inquiry_sm_p Pointer to the inquiry state machine structure
 */
void inquiry_start_procedure_handler(sm_t *inquiry_sm_p)
{
  btc_dev_t *btc_dev_p = BTC_DEV_PTR;
  inquiry_t *inquiry_p = &btc_dev_p->inquiry;
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = btc_dev_p->non_connected_tx_pwr_index;
  btc_trx_configs->ignore_whitening = FALSE; // TODO: Remove this once hopping is properly tested in statemachines
  inquiry_sm_p->last_tx_slots = 0;
  inquiry_sm_p->last_rx_slots = 0;

  if (inquiry_p->hci_trigger) {
    inquiry_p->inquiry_start_time = btc_trx_configs->btc_rail_trx_config.clk;  // stores the clk value when inquiry started for the first time
  }

  update_channel_info_inquiry(btc_trx_configs);
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), inquiry_p->end_tsf)) {
    next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
  } else {
    inquiry_activity_complete(inquiry_sm_p);
  }
}

/**
 * @brief Updates channel information for inquiry hopping sequence
 *
 * This function updates the channel selection parameters for inquiry frequency
 * hopping. It:
 * 1. Sets ULAP to Generic Inquiry Address (GIAC)
 * 2. Updates the master clock value for hopping sequence
 * 3. Calculates the kOffset value based on elapsed time since inquiry start
 *    (alternating train A and train B every 2.56 seconds)
 * 4. Updates the kosAndN field with hopping sequence parameters
 *
 * The kOffset determines whether train A (kOffset=0) or train B (kOffset=1)
 * is used for the inquiry hopping sequence.
 *
 * @param inquiry_sm_p Pointer to the inquiry state machine (unused)
 * @param btc_trx_configs Pointer to transaction configuration structure
 */
void update_channel_info_inquiry(btc_trx_configs_t *btc_trx_configs)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  // Frequency hopping inputs for Inquiry (transmitter role):
  // - ulap: Use GIAC/DIAC to derive the inquiry hop frequencies (ID packets).
  // - masterClk: Current Bluetooth native clock drives hop index selection.
  // - kOffset: Selects inquiry train A or B. The lower 14 bits (~2.56 s window)
  //            are compared to 0x2000 to toggle trains every 2.56 s as per spec.
  //            Result: kOffset=1 for first 2.56 s half, then 0 for the next, alternating.
  // - kosAndN (with IQRY): N is not used for the transmitter (set to 0). The
  //   receiver (inquiry scan, sequence IQRS) uses N to track responses.
  inquiry_p->btc_fh_data.ulap = GENERIC_INQUIRY_ADDRESS;
  inquiry_p->btc_fh_data.masterClk = btc_trx_configs->btc_rail_trx_config.clk;
  inquiry_p->kOffset = (((inquiry_p->btc_fh_data.masterClk - inquiry_p->inquiry_start_time) & 0x3FFF) < 0x2000) ? 1
                       : 0;
  inquiry_p->btc_fh_data.kosAndN = setBtcFhKosAndN(IQRY, 0, inquiry_p->kOffset);
  update_channel_info(btc_trx_configs, &inquiry_p->btc_fh_data);
}

/**
 * @brief Handles successful transmission of first ID packet
 *
 * Called when the first ID packet in the inquiry train has been successfully
 * transmitted. The function:
 * 1. Clears the ID_1_TX_FAIL_BIT in transmit status register
 * 2. Schedules transmission of second ID packet
 *
 * This function advances the inquiry sequence to maintain the proper timing
 * between consecutive ID packet transmissions.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_id_1_tx_done_handler(sm_t *inquiry_sm)
{
  (void)inquiry_sm;
  inquiry_t *inquiry_p = get_inquiry_p();
  CLEAR_BIT(inquiry_p->tx_fail_status, 0);  // reset(clearing tx_fail_stats first bit to zero)
  next_state(inquiry_p, INQUIRY_ID_2_TX_SCHEDULED);
}

/**
 * @brief Handles abortion of first ID packet transmission
 *
 * Called when the first ID packet transmission was aborted or failed. This function:
 * 1. Sets the ID_1_TX_FAIL_BIT in transmit status register to indicate failure
 * 2. Schedules transmission of second ID packet to continue the inquiry sequence
 *
 * The failure status is tracked to determine which response slots to monitor,
 * as devices responding to the missed ID packet would respond in different slots.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_id_1_tx_abort_handler(sm_t *inquiry_sm)
{
  (void)inquiry_sm;
  inquiry_t *inquiry_p = get_inquiry_p();
  SET_BIT(inquiry_p->tx_fail_status, 0);  // setting first bit of tx_fail_stats to show first ID packet is missed
  next_state(inquiry_p, INQUIRY_ID_2_TX_SCHEDULED);
}

/**
 * @brief Handles successful transmission of second ID packet
 *
 * Called when the second ID packet in the inquiry train has been successfully
 * transmitted. This function:
 * 1. Clears the ID_2_TX_FAIL_BIT in transmit status register
 * 2. Configures radio parameters for receiving FHS response packets
 * 3. Schedules appropriate receive window based on which ID packets were
 *    successfully transmitted:
 *    - If both ID packets were sent: Schedule RX_1 window
 *    - If only second ID packet was sent: Schedule RX_2 window
 *
 * The function ensures proper timing of receive windows to capture responses
 * from devices that received either ID packet.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_id_2_tx_done_handler(sm_t *inquiry_sm)
{
  (void)inquiry_sm;
  inquiry_t *inquiry_p = get_inquiry_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  CLEAR_BIT(inquiry_p->tx_fail_status, 1);  // reset(clearing tx_fail_stats second bit to zero)
  configure_trx_acl_rx(btc_trx_configs, BASIC_RATE, BTC_RAIL_RX_TIMEOUT);
  btc_trx_configs->rx_window = BTC_RAIL_RX_TIMEOUT;
  if (inquiry_p->tx_fail_status == 0) {     // no ID packets are missed
    next_state(inquiry_p, INQUIRY_RX_1_SCHEDULED);
  } else if (inquiry_p->tx_fail_status == 1) {    // First ID packet missed
    next_state(inquiry_p, INQUIRY_RX_2_SCHEDULED);
  }
}

/**
 * @brief Handles abortion of second ID packet transmission
 *
 * Called when the second ID packet transmission was aborted or failed. This function:
 * 1. Sets the ID_2_TX_FAIL_BIT in transmit status register
 * 2. Determines next action based on whether first ID packet was transmitted:
 *    - If first ID also failed: Restart inquiry sequence if within duration
 *    - If first ID succeeded: Schedule receive window for potential responses
 *
 * This recovery mechanism ensures the inquiry procedure can continue even when
 * individual packet transmissions fail.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_id_2_tx_abort_handler(sm_t *inquiry_sm)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  // uint32_t status;
  SET_BIT(inquiry_p->tx_fail_status, 1);  // setting second bit of tx_fail_stats after second ID packet is missed
  //  Schedule first reception id packet to RAIL
  if (IS_BIT_SET(inquiry_p->tx_fail_status, ID_1_TX_FAIL_BIT)) {        //  check if First ID scheduled
    if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), inquiry_p->end_tsf)) {
      next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
    } else {
      inquiry_activity_complete(inquiry_sm);
    }
  } else {
    configure_and_schedule_rx(inquiry_p, INQUIRY_RX_1_SCHEDULED, BTC_BD_CLK_2NP2);
  }
}
/**
 * @brief function handles the reception of EIR response from the peripheral
 * device if first id packet was received. machine
 *
 * @param inquiry_sm-structure of Inquiry containing information of Lap address,
 * length and number of response.
 */
void inquiry_rx_1_done_handler(sm_t *inquiry_sm)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  if (inquiry_p->EIR_value == 0) {    //  EIR not enabled in FHS packet
    if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), inquiry_p->end_tsf)) {
      next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
    } else {
      inquiry_activity_complete(inquiry_sm);
    }
  } else {  // EIR enabled in FHS packet
    configure_and_schedule_rx(inquiry_p, INQUIRY_EIR_RX_SCHEDULED, BTC_BD_CLK_2NP2);
  }
}

/**
 * @brief Handles abortion of packet reception in first RX slot
 *
 * Called when packet reception in first RX slot is aborted (typically due to
 * reception errors). Delegates to inquiry_rx_1_handler with appropriate slot
 * parameters for recovery.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_rx_1_abort_handler(sm_t *inquiry_sm)
{
  inquiry_rx_1_handler(inquiry_sm, BTC_BD_CLK_2NP0, BTC_BD_CLK_2NP0);
}

/**
 * @brief Handles timeout during first RX slot
 *
 * Called when no valid packet is received during the first receive window.
 * Delegates to inquiry_rx_1_handler with appropriate slot parameters
 * to schedule the next reception attempt.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_rx_1_timeout_handler(sm_t *inquiry_sm)
{
  inquiry_rx_1_handler(inquiry_sm, BTC_BD_CLK_2NP0, BTC_BD_CLK_2NP3);
}

/**
 * @brief Common handler for first RX slot processing
 *
 * This helper function implements common logic for handling the first receive slot
 * after ID packet transmissions. It:
 * 1. Checks if second ID packet transmission failed
 * 2. If second TX failed, schedules retransmission
 * 3. If second TX succeeded, schedules receive for second RX slot
 * 4. Handles error conditions for scheduling failures
 *
 * Used by both inquiry_rx_1_abort_handler and inquiry_rx_1_timeout_handler
 * to avoid code duplication and centralize common error recovery logic.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 * @param tx_slot Transmit slot identifier for scheduling
 * @param rx_slot Receive slot identifier for scheduling
 */
void inquiry_rx_1_handler(sm_t *inquiry_sm, uint8_t tx_slot, uint8_t rx_slot)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  uint32_t status;
  if (IS_BIT_SET(inquiry_p->tx_fail_status, ID_2_TX_FAIL_BIT)) {
    // check if second id is scheduled or not
    // ID packet will be scheduled if the Inquiry duration is not completed
    // all necessary parameters for slot programming
    configure_nonconn_slot_params(btc_trx_configs, tx_slot);
    update_channel_info_inquiry(btc_trx_configs);
    // Schedule first ID packet TX
    SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
    status = schedule_tx(btc_trx_configs);    // ID packets will be scheduled if the inquiry
    // duration is not completed.
    SM_STATE(inquiry_sm, INQUIRY_ID_2_TX_SCHEDULED);
    check_status_inquiry(status, inquiry_p, TX_ABORT);
  } else {
    configure_and_schedule_rx(inquiry_p, INQUIRY_RX_2_SCHEDULED, rx_slot);
  }
}

/**
 * @brief Handles reception of FHS packet in second RX slot
 *
 * Called when an FHS packet is successfully received in the second receive slot.
 * This function:
 * 1. Checks if EIR (Extended Inquiry Response) is enabled
 * 2. If EIR enabled: Schedules reception for EIR packet
 * 3. If EIR disabled: Schedules next inquiry train if within duration
 *
 * This function processes discovery responses that arrive in the second RX slot
 * (typically from devices responding to the second ID packet).
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_rx_2_done_handler(sm_t *inquiry_sm)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  btc_trx_configs_t *btc_trx_configs = &inquiry_p->trx_config;
  if (inquiry_p->EIR_value == 1) {
    configure_and_schedule_rx(inquiry_p, INQUIRY_EIR_RX_SCHEDULED, BTC_BD_CLK_2NP3);
  } else {
    if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&btc_trx_configs->bd_clk_counter_handle), inquiry_p->end_tsf)) {
      next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
    } else {
      inquiry_activity_complete(inquiry_sm);
    }
  }
}

/**
 * @brief Handles timeout in second RX slot
 *
 * Called when no packet is received in the second RX slot. This function:
 * 1. Checks if current time is within the inquiry duration
 * 2. If within duration: Schedules next inquiry train with new ID packets
 * 3. If past duration: Completes inquiry activity
 *
 * This function ensures the inquiry procedure continues or terminates
 * appropriately based on the configured duration. This is part of the
 * inquiry train rotation sequence that alternates between trains A and B
 * to maximize discovery probability.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_rx_2_timeout_handler(sm_t *inquiry_sm)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&inquiry_p->trx_config.bd_clk_counter_handle), inquiry_p->end_tsf)) {
    next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
  } else {
    inquiry_activity_complete(inquiry_sm);
  }
}

/**
 * @brief Handles successful reception of EIR packet
 *
 * Called when an Extended Inquiry Response packet is successfully received.
 * This function:
 * 1. Processes the additional device information in the EIR
 * 2. If within inquiry duration: Schedules next inquiry train
 * 3. If past duration: Completes the inquiry activity
 *
 * The EIR packet contains additional information about the responding device
 * such as device name, supported services, and manufacturer data.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_EIR_rx_done_handler(sm_t *inquiry_sm)
{
  inquiry_t *inquiry_p = get_inquiry_p();
  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&inquiry_p->trx_config.bd_clk_counter_handle), inquiry_p->end_tsf)) {
    next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
  } else {
    inquiry_activity_complete(inquiry_sm);
  }
}

/**
 * @brief Sends inquiry event notification to upper layers
 *
 * This function prepares and sends a notification to the Upper Link Controller (ULC)
 * to report inquiry events such as:
 * - BTC_INQUIRY_COMPLETE: Inquiry procedure completed normally
 * - BTC_INQUIRY_EIR_TIMEOUT: EIR packet reception timed out
 * - BTC_ACTIVITY_DURATION_COMPLETED: Inquiry duration elapsed
 *
 * The ULC uses these notifications to manage higher-level inquiry processing
 * and inform the host of inquiry status.
 *
 * @param status Status code indicating type of inquiry notification
 */
void send_inquiry_notification_to_ulc(btc_internal_event_status_t status, uint8_t *temp)
{
  (void)temp;
  uint8_t* buffer = NULL;
  pkb_t *pkb;

  pkb = get_buffer_for_notification(&buffer);
  prep_hss_internal_event(buffer, BTC_INQUIRY_COMPLETE, status);
  btc_lpw_activity_complete_indication_to_ull(pkb);
}

/**
 * @brief Handles timeout while waiting for EIR packet
 *
 * Called when an expected EIR packet is not received within the timeout period.
 * This function:
 * 1. Sends EIR timeout notification to upper layers
 * 2. If within inquiry duration: Schedules next inquiry train
 * 3. If past duration: Completes the inquiry activity
 *
 * This function ensures the inquiry procedure continues even when expected
 * EIR packets fail to arrive.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_EIR_rx_timeout_handler(sm_t *inquiry_sm)
{
  inquiry_t *inquiry_p = get_inquiry_p();

  if (IS_BT_CLOCK_LT(btc_get_current_bd_clk_cnt(&inquiry_p->trx_config.bd_clk_counter_handle), inquiry_p->end_tsf)) {
    send_inquiry_notification_to_ulc(BTC_INQUIRY_EIR_TIMEOUT, NULL);
    next_state(inquiry_p, INQUIRY_ID_1_TX_SCHEDULED);
  } else {
    inquiry_activity_complete(inquiry_sm);
  }
}

/**
 * @brief Completes the inquiry activity and releases resources
 *
 * This function is called when the inquiry procedure completes, either due to
 * timeout or explicit stop command. It:
 * 1. Stops the aperiodic role and releases radio resources
 * 2. Sends completion notification to upper layers
 * 3. Sets the state machine to idle state
 * 4. Configures radio to idle state to save power
 *
 * This function ensures proper cleanup and notification when inquiry completes.
 *
 * @param inquiry_sm Pointer to the inquiry state machine structure
 */
void inquiry_activity_complete(sm_t *inquiry_sm)
{
  btc_activity_complete(inquiry_sm, (void (*)(btc_internal_event_status_t, void *))send_inquiry_notification_to_ulc, PTI_INQUIRY_COMPLETED, inquiry_sm_idle);
}
