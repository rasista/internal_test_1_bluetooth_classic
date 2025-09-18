/***************************************************************************//**
 * @file lite_controller.c
 * @brief Bluetooth Classic Controller Testing Framework - CLI Command Handlers
 *
 * This file implements the command line interface handlers for the Bluetooth
 * Classic controller testing framework. It serves as the primary interface
 * between user commands and the controller's internal operations.
 *
 * ## System Overview
 *
 * The testing framework operates through a structured command processing pipeline:
 *
 * 1. **SLCP Project Configuration**: Defines project settings, components, and
 *    CLI commands, linking them to handler functions in this file.
 *
 * 2. **CLI Command Processing**: Functions in this file process CLI arguments,
 *    configure settings, and prepare commands for the Bluetooth controller.
 *
 * 3. **Command Preparation**: Commands are structured using packet buffers,
 *    prepared with appropriate parameters, and enqueued in shared memory.
 *
 * 4. **Controller Communication**: RAIL_USER_SendMbox notifies the controller,
 *    triggering rail_btc_mbox in btc_rail_intf.c to process messages.
 *
 * 5. **Message Decoding**: message_decode.c decodes and handles messages,
 *    mapping command IDs to handler functions that initiate procedures.
 *
 * 6. **Autonomous Operation**: Procedures run autonomously using state machines
 *    until completion, with events reported back via registered callbacks.
 *
 * ## Key Features
 *
 * - **Non-connected States**: Inquiry, inquiry scan, page, page scan procedures
 * - **Connected States**: ACL, eSCO connections and role switching
 * - **Test Modes**: Transmit/receive testing with various packet types
 * - **Encryption**: E0 and AES encryption configuration and testing
 * - **Debugging**: PTI, logging, and statistics collection
 * - **Buffer Management**: Packet buffer pools and queue management
 *
 * ## Thread Safety
 *
 * The framework uses RAIL semaphores for thread-safe queue operations and
 * shared memory access between the application and controller contexts.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "lite_controller.h"
#include "btc_ut_hss_event_callbacks.h"
uint32_t BT_SLOT_WIDTH_HSS = 625;
uint32_t default_eir_pkt[5] = { 0x008E0018, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
uint32_t eir_pkt[64];

// Variable definitions
uint8_t g_trigger_roleswitch = 0;
RAIL_Handle_t btc_rail_handle;
uint32_t rxFifo[RX_FIFO_SIZE / 4];
uint32_t medium_pkt_heap[48][pkb_chunk_size_medium / sizeof(uint32_t)];
uint32_t short_pkt_heap[8][pkb_chunk_size_small / sizeof(uint32_t)];
uint8_t lpw_hss_event_logger_enabled = 0;
uint32_t end_tsf_acl = 0;
uint8_t connection_device_index_central;
uint8_t connection_device_index_peripheral;
shared_mem_t queue;

// Command structure definitions
hss_cmd_init_btc_device_t init_btc_device;
hss_cmd_start_test_mode_t start_test_mode;
hss_cmd_set_inquiry_t set_inquiry;
hss_cmd_set_inquiry_scan_t set_inquiry_scan;
hss_cmd_set_page_t set_page;
hss_cmd_set_page_scan_t set_page_scan;
hss_cmd_set_device_parameters_acl_t set_device_parameters_acl;
hss_cmd_set_afh_parameters_acl_t set_afh_parameters_acl;
hss_cmd_set_sniff_parameters_acl_t set_sniff_parameters_acl;
hss_cmd_set_device_parameters_esco_t set_device_parameters_esco;
hss_cmd_set_device_parameters_roleswitch_t set_device_parameters_roleswitch;
hss_cmd_set_roleswitch_parameters_t set_roleswitch_parameters;
hss_cmd_start_inquiry_t start_inquiry;
hss_cmd_start_inquiry_scan_t start_inquiry_scan;
hss_cmd_start_page_t start_page;
hss_cmd_start_page_scan_t start_page_scan;
hss_cmd_start_acl_central_t start_acl_central;
hss_cmd_start_acl_peripheral_t start_acl_peripheral;
hss_cmd_start_esco_central_t start_esco_central;
hss_cmd_start_esco_peripheral_t start_esco_peripheral;
hss_cmd_start_roleswitch_central_t start_roleswitch_central;
hss_cmd_start_roleswitch_peripheral_t start_roleswitch_peripheral;
hss_cmd_set_e0_encryption_parameters_t e0_enc_params;
hss_cmd_set_aes_encryption_parameters_t aes_enc_params;
hss_cmd_set_device_parameters_acl_t set_acl_parameters;
hss_cmd_set_fixed_btc_channel_t set_fixed_btc_channel;

bool stats_check = false;
hss_event_btc_slot_offset_t hss_event_roleswitch_compute_offset;

/**
 * @brief Simple hello command handler for testing CLI connectivity
 *
 * This is a basic test command that responds with "Hello FPGA" to verify
 * that the CLI interface is working properly and the command routing
 * through the testing framework is functional.
 *
 * @param args CLI command arguments structure containing the parsed command
 *
 * @note This function is primarily used for initial system verification
 *       and debugging of the CLI command processing pipeline
 */
void btc_hello(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "Hello FPGA");
}

void enable_slow_hopping(sl_cli_command_arg_t *args)
{
  BT_SLOT_WIDTH_HSS = 1250;
  responsePrint(sl_cli_get_command_string(args, 0), "Slow hopping enabled");
}

/**
 * @brief Configure encryption parameters for Bluetooth Classic test mode
 *
 * This function configures either E0 or AES encryption parameters for the
 * Bluetooth controller testing framework. It supports both default values
 * per Bluetooth specification and custom user-provided parameters.
 *
 * ## Supported Encryption Types:
 * - **E0 encryption**: Legacy Bluetooth encryption (Vol 2, Part G, Section 1.1.5)
 * - **AES encryption**: Enhanced encryption (Vol 2, Part G, Section 1.2.1)
 *
 * ## Command Processing:
 * After configuring the parameters, it prepares and sends the encryption
 * command to the controller via the shared memory queue system using either
 * set_e0_encryption_cmd() or set_aes_encryption_cmd().
 *
 * ## Error Handling:
 * - Invalid encryption types are reported via responsePrint
 * - RAIL communication errors are handled by send_command()
 * - Parameter validation is performed before command preparation
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             [1]: encryption_type - E0 (0) or AES (1)
 *             For E0 custom: [2-9]: masterClock, encKey0-3, bdAddr0-1, keyLength
 *             For AES custom: [2-11]: aesKeyBytes_0_3 through aesPldCntr2
 *
 * @retval None - Status and errors reported via responsePrint/responsePrintError
 *
 * @note The encryption configuration is applied immediately to the controller
 *       through the RAIL_USER_SendMbox interface. Default values comply with
 *       Bluetooth specification test vectors.
 *
 * @see set_e0_encryption_cmd, set_aes_encryption_cmd, send_command
 */
void cli_test_mode_set_encryption(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  uint8_t encryption_type = sl_cli_get_argument_uint8(args, 1);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    if (encryption_type == TEST_ENCRYPTION_MODE_E0) {
      // Default values per spec: Vol 2, Part G, Section 1.1.5
      e0EncConfig.masterClock = 0x400034BE;
      e0EncConfig.encKey0    = 0x4AF08721;
      e0EncConfig.encKey1    = 0xD03190BA;
      e0EncConfig.encKey2    = 0x534C0D78;
      e0EncConfig.encKey3    = 0x633A15E0;
      e0EncConfig.bdAddr0    = 0x56947F2C;
      e0EncConfig.bdAddr1    = 0x1B0F;
      e0EncConfig.keyLength  = 64;
      responsePrint(sl_cli_get_command_string(args, 0), "Default E0 encryption configured");
    } else if (encryption_type == TEST_ENCRYPTION_MODE_AES) {
      // Default values per spec: Vol 2, Part G, Section 1.2.1
      aesEncConfig.aesKeyBytes_0_3 = 0x89678967;
      aesEncConfig.aesKeyBytes_4_7 = 0x89678967;
      aesEncConfig.aesKeyBytes_8_11 = 0x45234523;
      aesEncConfig.aesKeyBytes_12_15 = 0x45234523;
      aesEncConfig.aesIv1         = 0x66778899;
      aesEncConfig.aesIv2         = 0xaabbccdd;
      aesEncConfig.dayCountAndDir = 0;
      aesEncConfig.zeroLenAclW    = 0;
      aesEncConfig.aesPldCntr1    = 0x00bc614e;
      aesEncConfig.aesPldCntr2    = 0;
      responsePrint(sl_cli_get_command_string(args, 0), "Default AES encryption configured");
    } else {
      responsePrint(sl_cli_get_command_string(args, 0), "Unknown encryption type: %d", encryption_type);
    }
  } else {
    // Custom encryption parameters provided via CLI.
    if (encryption_type == TEST_ENCRYPTION_MODE_E0) {
      // Expected arguments:
      // [2]: masterClock, [3]: encKey0, [4]: encKey1, [5]: encKey2, [6]: encKey3,
      // [7]: bdAddr0, [8]: bdAddr1, [9]: keyLength
      e0EncConfig.masterClock = sl_cli_get_argument_uint32(args, 2);
      e0EncConfig.encKey0    = sl_cli_get_argument_uint32(args, 3);
      e0EncConfig.encKey1    = sl_cli_get_argument_uint32(args, 4);
      e0EncConfig.encKey2    = sl_cli_get_argument_uint32(args, 5);
      e0EncConfig.encKey3    = sl_cli_get_argument_uint32(args, 6);
      e0EncConfig.bdAddr0    = sl_cli_get_argument_uint32(args, 7);
      e0EncConfig.bdAddr1    = sl_cli_get_argument_uint16(args, 8);
      e0EncConfig.keyLength  = sl_cli_get_argument_uint8(args, 9);
      responsePrint(sl_cli_get_command_string(args, 0), "Custom E0 encryption configured");
    } else if (encryption_type == TEST_ENCRYPTION_MODE_AES) {
      // Expected arguments:
      // [2]: aesKeyBytes_0_3, [3]: aesKeyBytes_4_7, [4]: aesKeyBytes_8_11, [5]: aesKeyBytes_12_15,
      // [6]: aesIv1, [7]: aesIv2, [8]: dayCountAndDir, [9]: zeroLenAclW,
      // [10]: aesPldCntr1, [11]: aesPldCntr2
      aesEncConfig.aesKeyBytes_0_3 = sl_cli_get_argument_uint32(args, 2);
      aesEncConfig.aesKeyBytes_4_7 = sl_cli_get_argument_uint32(args, 3);
      aesEncConfig.aesKeyBytes_8_11 = sl_cli_get_argument_uint32(args, 4);
      aesEncConfig.aesKeyBytes_12_15 = sl_cli_get_argument_uint32(args, 5);
      aesEncConfig.aesIv1         = sl_cli_get_argument_uint32(args, 6);
      aesEncConfig.aesIv2         = sl_cli_get_argument_uint32(args, 7);
      aesEncConfig.dayCountAndDir = sl_cli_get_argument_uint16(args, 8);
      aesEncConfig.zeroLenAclW    = sl_cli_get_argument_uint16(args, 9);
      aesEncConfig.aesPldCntr1    = sl_cli_get_argument_uint32(args, 10);
      aesEncConfig.aesPldCntr2    = sl_cli_get_argument_uint8(args, 11);
      responsePrint(sl_cli_get_command_string(args, 0), "Custom AES encryption configured");
    } else {
      responsePrint(sl_cli_get_command_string(args, 0), "Unknown encryption type: %d", encryption_type);
    }
  }

  if (encryption_type == TEST_ENCRYPTION_MODE_E0) {
    set_e0_encryption_cmd(&e0EncConfig, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  } else if (encryption_type == TEST_ENCRYPTION_MODE_AES) {
    set_aes_encryption_cmd(&aesEncConfig, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  } else {
    return; // Unknown type; already reported
  }
  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure encryption parameters for Bluetooth Classic connections
 *
 * This function configures encryption parameters for Bluetooth Classic connections, supporting
 * both E0 and AES encryption modes. The function can use default parameters or
 * accept custom encryption parameters via CLI arguments.
 *
 * ## Encryption Modes:
 * - **ENCRYPTION_MODE_OFF**: No encryption (default)
 * - **ENCRYPTION_MODE_E0**: E0 encryption with custom parameters
 * - **ENCRYPTION_MODE_AES**: AES encryption with custom parameters
 *
 * ## Default Mode:
 * When is_default is set to TRIGGER_CMD_WITH_DEFAULTS (99), the function uses
 * default parameters: device index 0 and encryption mode OFF.
 *
 * ## Custom E0 Encryption Parameters:
 * For E0 encryption, the following parameters are expected:
 * [2]: masterClock, [3]: encKey0, [4]: encKey1, [5]: encKey2, [6]: encKey3,
 * [7]: bdAddr0, [8]: bdAddr1, [9]: keyLength
 *
 * ## Custom AES Encryption Parameters:
 * For AES encryption, the following parameters are expected:
 * [2]: aesKeyBytes_0_3, [3]: aesKeyBytes_4_7, [4]: aesKeyBytes_8_11, [5]: aesKeyBytes_12_15,
 * [6]: aesIv1, [7]: aesIv2, [8]: dayCountAndDir, [9]: zeroLenAclW,
 * [10]: aesPldCntr1, [11]: aesPldCntr2
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (99) or custom (0)
 *             For custom: [0]: device_index, [1]: encryption_mode,
 *                        [2-11]: encryption-specific parameters
 *
 * @note The function automatically selects the appropriate encryption command
 *       (E0 or AES) based on the encryption mode and sends it to the controller.
 *
 * @note Default command: set_encryption_parameters 99
 *       E0 command: set_encryption_parameters [device_index] [1] [masterClock] [encKey0] [encKey1] [encKey2] [encKey3] [bdAddr0] [bdAddr1] [keyLength]
 *       AES command: set_encryption_parameters [device_index] [2] [aesKeyBytes_0_3] [aesKeyBytes_4_7] [aesKeyBytes_8_11] [aesKeyBytes_12_15] [aesIv1] [aesIv2] [dayCountAndDir] [zeroLenAclW] [aesPldCntr1] [aesPldCntr2]
 */
void cli_set_encryption_parameters(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index;
  uint8_t encryption_mode;
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    current_device_index = 0;  // Assuming 0 is the default device index
    encryption_mode = ENCRYPTION_MODE_OFF;
  } else {
    current_device_index = sl_cli_get_argument_uint8(args, 0);
    encryption_mode = sl_cli_get_argument_uint8(args, 1);
    // Custom encryption parameters provided via CLI.
    if (encryption_mode == ENCRYPTION_MODE_E0) {
      // Expected arguments:
      // [2]: masterClock, [3]: encKey0, [4]: encKey1, [5]: encKey2, [6]: encKey3,
      // [7]: bdAddr0, [8]: bdAddr1, [9]: keyLength
      e0_enc_params.connection_device_index = current_device_index;
      e0_enc_params.encryption_mode = encryption_mode;
      e0_enc_params.masterClock = sl_cli_get_argument_uint32(args, 2);
      e0_enc_params.encKey0    = sl_cli_get_argument_uint32(args, 3);
      e0_enc_params.encKey1    = sl_cli_get_argument_uint32(args, 4);
      e0_enc_params.encKey2    = sl_cli_get_argument_uint32(args, 5);
      e0_enc_params.encKey3    = sl_cli_get_argument_uint32(args, 6);
      e0_enc_params.bdAddr0    = sl_cli_get_argument_uint32(args, 7);
      e0_enc_params.bdAddr1    = sl_cli_get_argument_uint16(args, 8);
      e0_enc_params.keyLength  = sl_cli_get_argument_uint8(args, 9);
      responsePrint(sl_cli_get_command_string(args, 0), "Custom E0 encryption configured");
    } else if (encryption_mode == ENCRYPTION_MODE_AES) {
      // Expected arguments:
      // [2]: aesKeyBytes_0_3, [3]: aesKeyBytes_4_7, [4]: aesKeyBytes_8_11, [5]: aesKeyBytes_12_15,
      // [6]: aesIv1, [7]: aesIv2, [8]: dayCountAndDir, [9]: zeroLenAclW,
      // [10]: aesPldCntr1, [11]: aesPldCntr2
      aes_enc_params.connection_device_index = current_device_index;
      aes_enc_params.encryption_mode = encryption_mode;
      aes_enc_params.aesKeyBytes_0_3 = sl_cli_get_argument_uint32(args, 2);
      aes_enc_params.aesKeyBytes_4_7 = sl_cli_get_argument_uint32(args, 3);
      aes_enc_params.aesKeyBytes_8_11 = sl_cli_get_argument_uint32(args, 4);
      aes_enc_params.aesKeyBytes_12_15 = sl_cli_get_argument_uint32(args, 5);
      aes_enc_params.aesIv1         = sl_cli_get_argument_uint32(args, 6);
      aes_enc_params.aesIv2         = sl_cli_get_argument_uint32(args, 7);
      aes_enc_params.dayCountAndDir = sl_cli_get_argument_uint16(args, 8);
      aes_enc_params.zeroLenAclW    = sl_cli_get_argument_uint16(args, 9);
      aes_enc_params.aesPldCntr1    = sl_cli_get_argument_uint32(args, 10);
      aes_enc_params.aesPldCntr2    = sl_cli_get_argument_uint8(args, 11);
      responsePrint(sl_cli_get_command_string(args, 0), "Custom AES encryption configured");
    } else {
      responsePrint(sl_cli_get_command_string(args, 0), "Unknown encryption type: %d", encryption_mode);
    }
  }

  if (encryption_mode == ENCRYPTION_MODE_E0) {
    set_e0_encryption_parameters_cmd(&e0_enc_params, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  } else if (encryption_mode == ENCRYPTION_MODE_AES) {
    set_aes_encryption_parameters_cmd(&aes_enc_params, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "Encryption mode OFF; no command sent");
    return;
  }
  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Setting encryption parameters on device: %d",
                current_device_index);
}

/**
 * @brief Configure and start Bluetooth Classic test mode
 *
 * This function configures the Bluetooth controller for various test modes
 * including transmit/receive testing, packet generation, and loopback testing.
 * It supports different BR/EDR modes, packet types, payload patterns, and
 * power levels for comprehensive testing scenarios.
 *
 * ## Key Initialization Steps:
 * - Sets up RAIL state transitions for receive mode when not in loopback
 * - Initializes shared memory queue metadata (PKB_DATA_OFFSET)
 * - Configures semaphore locks for thread-safe queue operations
 * - Prepares test mode command using start_test_mode_cmd()
 *
 * ## Supported Test Modes:
 * - Transmit testing with configurable packet types and patterns
 * - Receive testing with automatic RAIL state transitions
 * - Loopback testing for internal verification
 * - BR/EDR mode testing for different data rates
 *
 * @param args CLI command arguments containing:
 *             [0]: transmit_mode - TX/RX mode selection (0=RX, 1=TX)
 *             [1]: br_edr_mode - Basic Rate (0) or Enhanced Data Rate (1)
 *             [2]: pkt_type - Bluetooth packet type (DM1, DH1, etc.)
 *             [3]: pkt_len - Packet length in bytes
 *             [4]: payload_type - Test payload pattern type
 *             [5]: no_of_pkts - Number of packets to transmit (0=continuous)
 *             [6]: inter_packet_gap - Gap between packets in microseconds
 *             [7]: link_type - Link type (ACL, SCO, eSCO)
 *             [8]: tx_power - Transmit power level index
 *             [9]: hopping_type - Frequency hopping configuration
 *             [10]: loopback_mode - Enable (1) or disable (0) loopback testing
 *             [11]: scrambler_seed - Scrambler seed
 *             [12]: rx_window - Receive window in seconds
 *             [12-:17]: bd_address - 6-byte Bluetooth device address
 *
 * @retval None - Status reported via responsePrint/responsePrintError
 *
 * @note This function initializes the queue metadata and locking mechanisms
 *       required for proper operation of the testing framework. For receive
 *       mode, RAIL state transitions are configured to maintain RX state.
 *
 * @see start_test_mode_cmd, RAIL_SetRxTransitions, btc_plt_acquire_lock
 */
void cli_start_test_mode_handler(sl_cli_command_arg_t *args)
{
  start_test_mode.transmit_mode = sl_cli_get_argument_uint8(args, 0);
  start_test_mode.br_edr_mode = sl_cli_get_argument_uint8(args, 1);
  start_test_mode.pkt_type = sl_cli_get_argument_uint8(args, 2);
  start_test_mode.pkt_len = sl_cli_get_argument_uint16(args, 3);
  start_test_mode.payload_type = sl_cli_get_argument_uint8(args, 4);
  start_test_mode.no_of_pkts = sl_cli_get_argument_uint32(args, 5);
  start_test_mode.inter_packet_gap = sl_cli_get_argument_uint32(args, 6);
  start_test_mode.link_type = sl_cli_get_argument_uint8(args, 7);
  start_test_mode.tx_power = sl_cli_get_argument_uint8(args, 8);
  start_test_mode.hopping_type = sl_cli_get_argument_uint8(args, 9);
  start_test_mode.loopback_mode = sl_cli_get_argument_uint8(args, 10);
  start_test_mode.scrambler_seed = sl_cli_get_argument_uint8(args, 11);
  start_test_mode.rx_window = sl_cli_get_argument_uint32(args, 12);
  for (int i = 0; i < 6; i++) {
    start_test_mode.bd_address[i] = sl_cli_get_argument_uint8(args, 13 + i);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "mode:%d,br_edr_mode:%d,pkt_type:%d,pkt_len:%d,payload_type:%d,num_pkts:%d,inter_pkt_gap:%d,link_type:%d,scrambler_seed:%d,rx_window:%d",
                start_test_mode.transmit_mode, start_test_mode.br_edr_mode, start_test_mode.pkt_type, start_test_mode.pkt_len, start_test_mode.payload_type, start_test_mode.no_of_pkts, start_test_mode.inter_packet_gap, start_test_mode.link_type, start_test_mode.scrambler_seed, start_test_mode.rx_window);

  RAIL_StateTransitions_t rx_transitions = {
    .success = RAIL_RF_STATE_RX,
    .error = RAIL_RF_STATE_RX,
  };
  if (!start_test_mode.loopback_mode) {
    RAIL_SetRxTransitions(btc_rail_handle, &rx_transitions);
  }

  queue.metadata_size = PKB_DATA_OFFSET;
  queue.pending_queue.plt_lock.acquire_lock = btc_plt_acquire_lock;
  queue.pending_queue.plt_lock.release_lock = btc_plt_release_lock;
  queue.processed_queue.plt_lock.acquire_lock = btc_plt_acquire_lock;
  queue.processed_queue.plt_lock.release_lock = btc_plt_release_lock;

  start_test_mode_cmd(&start_test_mode, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Set common command parameters for Bluetooth procedures
 *
 * This utility function sets common scheduling and timing parameters that are
 * used across multiple Bluetooth procedures. It provides a standardized way
 * to configure priority, step size, and end timestamp for various operations
 * like inquiry, page, etc.
 *
 * @param cmd_struct Pointer to command structure to be configured
 * @param priority Command priority level for scheduling
 * @param step_size Step size for frequency hopping or timing intervals
 * @param end_tsf End timestamp (Time Synchronization Function) for the procedure
 *
 * @note This function assumes the command structure starts with the generic
 *       parameter layout (priority, step_size, end_tsf)
 */
void set_command_parameters(void *cmd_struct, uint8_t priority, uint8_t step_size, uint32_t end_tsf)
{
  struct generic_cmd_params {
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
  } *params = (struct generic_cmd_params *)cmd_struct;

  params->priority = priority;
  params->step_size = step_size;
  params->end_tsf = end_tsf;
}

/**
 * @brief Set a default Bluetooth device address pattern
 *
 * This utility function generates a predictable default BD_ADDR pattern
 * for testing purposes. The generated address follows the pattern:
 * 0x11, 0x22, 0x33, 0x44, 0x55, 0x66
 *
 * @param bd_address Pointer to 6-byte array to store the BD_ADDR
 *
 * @note This is used when default parameters are requested in CLI commands
 *       to ensure consistent and recognizable test addresses
 */
void set_default_bd_address(uint8_t *bd_address)
{
  for (int i = 0; i < 6; i++) {
    bd_address[i] = 0x11 + i * 0x11;
  }
}

/**
 * @brief Send a prepared command to the Bluetooth controller
 *
 * This function serves as the final step in the command processing pipeline,
 * sending the prepared command to the Bluetooth controller via the RAIL
 * mailbox interface. It handles error reporting and success confirmation.
 *
 * ## Command Processing Flow:
 * 1. Command is prepared using specific *_cmd() functions (e.g., start_inquiry_cmd)
 * 2. Packet buffer is allocated via pkb_alloc()
 * 3. Command structure is filled using prep_hss_cmd_* functions
 * 4. Command is enqueued using hss_command_to_lpw()
 * 5. This function sends notification via RAIL_USER_SendMbox()
 * 6. Controller receives notification in rail_btc_mbox (btc_rail_intf.c)
 * 7. Message is decoded and handled in message_decode.c
 * 8. Appropriate handler function initiates the procedure autonomously
 *
 * @param args CLI command arguments for error/success reporting
 * @param cmd Pointer to the command identifier/address to send
 *
 * @retval None
 *
 * @note This function prints success/failure status via responsePrint/responsePrintError
 *       and is a critical part of the testing framework communication path
 *
 * @see start_inquiry_cmd, hss_command_to_lpw, RAIL_USER_SendMbox
 */
void send_command(sl_cli_command_arg_t *args, uint32_t *cmd)
{
  RAIL_Status_t status = RAIL_USER_SendMbox(btc_rail_handle, *cmd);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xA, "Could not send command %d.", status);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "request:%u",
                  *cmd
                  );
  }
}

/**
 * @brief Register event callbacks for Bluetooth controller testing
 *
 * This function registers all necessary event callback handlers for the
 * Bluetooth Classic testing framework. These callbacks handle asynchronous
 * events from the controller's state machines and provide feedback to the
 * testing application.
 *
 * The registered callbacks include:
 * - Packet reception handling for analyzing received data
 * - Inquiry/inquiry scan completion events
 * - Page/page scan completion events
 * - General activity completion events
 * - Role switch completion events
 * - Transmit packet acknowledgment events
 *
 * @note This function must be called during device initialization to ensure
 *       proper event handling throughout the testing session. The callbacks
 *       are implemented in btc_ut_hss_event_callbacks.h/c
 */
void btc_app_init_register_cbs(void)
{
  // Register callback for processing the received packet
  register_pkt_rx_cb(parse_rx_pkt_handler);

  // Register callback for inquiry complete event
  register_hss_event_cb(BTC_INQUIRY_COMPLETE, hss_event_inquiry_complete_handler);

  // Register callback for inquiry scan complete event
  register_hss_event_cb(BTC_INQUIRY_SCAN_COMPLETE, hss_event_inquiry_scan_complete_handler);

  // Register callback for page complete event
  register_hss_event_cb(BTC_PAGE_COMPLETE, hss_event_page_complete_handler);

  // Register callback for page scan complete event
  register_hss_event_cb(BTC_PAGE_SCAN_COMPLETE, hss_event_page_scan_complete_handler);

  // Register callback for procedure complete event
  register_hss_event_cb(BTC_ACTIVITY_COMPLETE, hss_event_activity_complete_handler);

  // Register callback for role-switch complete event
  register_hss_event_cb(BTC_ROLE_SWITCH_COMPLETE, hss_event_role_switch_complete_handler);

  // Register callback for tx pkt ack received event
  register_hss_event_cb(BTC_TX_PKT_ACK_RECEIVED, hss_event_tx_pkt_ack_received_handler);

  register_hss_event_cb(BTC_CLK_SLOT_OFFSET_NOTIFICATION, hss_event_clk_notification_handler);
}

/**
 * @brief Initialize the Bluetooth Classic device for testing
 *
 * This function performs the initial device setup required before any Bluetooth
 * operations can be performed. It configures the device's BD_ADDR, initializes
 * the shared memory queue system, and registers all necessary event callbacks.
 *
 * The initialization process includes:
 * - Setting the device's Bluetooth address (default or custom)
 * - Configuring packet buffer metadata and default EIR data
 * - Registering event callbacks for handling controller responses
 * - Sending the initialization command to the controller
 *
 * This is typically the first command issued in any testing sequence and must
 * complete successfully before other Bluetooth procedures can be started.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default BD_ADDR (99) or custom (0)
 *             [0-5]: bd_address bytes (when not using defaults)
 *
 * @note This function directly sends the command via RAIL_USER_SendMbox rather
 *       than using send_command() to handle any initialization-specific requirements
 *
 * @note Default command: init_device 99
 *       Non-default command: init_device [bd_addr0] [bd_addr1] [bd_addr2] [bd_addr3] [bd_addr4] [bd_addr5]
 */
void cli_init_device_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_default_bd_address(init_btc_device.bd_address);
  } else {
    for (int i = 0; i < 6; i++) {
      init_btc_device.bd_address[i] = sl_cli_get_argument_uint8(args, i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, init_btc_device.bd_address[0], init_btc_device.bd_address[1],
                init_btc_device.bd_address[2], init_btc_device.bd_address[3], init_btc_device.bd_address[4], init_btc_device.bd_address[5]);

  queue.metadata_size = PKB_DATA_OFFSET;
  queue.tx_pkt = (uint32_t)&default_eir_pkt;
  queue.tx_pkt_len = 0;
  init_device_cmd(&init_btc_device, (shared_mem_t *)&queue);

  btc_app_init_register_cbs();

  uint32_t cmd = (uint32_t)&queue;
  RAIL_Status_t status = RAIL_USER_SendMbox(btc_rail_handle, cmd);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xA, "Could not send command %d.", status);
    return;
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "request:%u",
                  cmd);
  }
}

// NON-CONNECTED STATES

/**
 * @brief Configure parameters for Bluetooth inquiry procedure
 *
 * This function configures the parameters for the Bluetooth inquiry procedure,
 * which is used to discover other Bluetooth devices in the vicinity. The inquiry
 * process transmits ID packets at regular intervals across different frequency
 * channels to find discoverable devices.
 *
 * The function supports both default parameters (per Bluetooth specification)
 * and custom user-defined parameters for specialized testing scenarios.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (99) or custom (0)
 *             For custom: [0]: length, [1]: hci_trigger, [2]: EIR_value,
 *                        [3]: tx_pwr_index, [4-6]: lap_address[3]
 *
 * @note The LAP (Lower Address Part) typically uses 0x9E8B00 for general inquiry.
 *       After configuration, the command is queued and sent to the controller.
 *
 * @note Default command: set_inquiry 99
 *       Non-default command: set_inquiry [length] [hci_trigger] [EIR_value] [tx_pwr_index] [lap_addr0] [lap_addr1] [lap_addr2]
 */
void cli_set_inquiry_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_inquiry.length = 1;
    set_inquiry.hci_trigger = 0;
    set_inquiry.EIR_value = 1;
    set_inquiry.tx_pwr_index = 100;
    set_inquiry.lap_address[0] = 0x00;
    set_inquiry.lap_address[1] = 0x8B;
    set_inquiry.lap_address[2] = 0x9E;
  } else {
    set_inquiry.length = sl_cli_get_argument_uint32(args, 0);
    set_inquiry.hci_trigger = sl_cli_get_argument_uint8(args, 1);
    set_inquiry.EIR_value = sl_cli_get_argument_uint8(args, 2);
    set_inquiry.tx_pwr_index = sl_cli_get_argument_uint8(args, 3);
    for (int i = 0; i < 3; i++) {
      set_inquiry.lap_address[i] = sl_cli_get_argument_uint8(args, 4 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,length:%d,hci_trigger:%d,EIR_value:%d,tx_pwr_index:%d,lap_address:0x%02X%02X%02X",
                is_default, set_inquiry.length, set_inquiry.hci_trigger, set_inquiry.EIR_value,
                set_inquiry.tx_pwr_index, set_inquiry.lap_address[0], set_inquiry.lap_address[1], set_inquiry.lap_address[2]);

  set_inquiry_cmd(&set_inquiry, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start the Bluetooth inquiry procedure
 *
 * This function initiates the actual inquiry process after parameters have been
 * configured via cli_set_inquiry_handler. The inquiry procedure will run
 * autonomously using the controller's state machines until completion or
 * until manually stopped.
 *
 * ## Prerequisites:
 * - Device must be initialized via cli_init_device_handler()
 * - Inquiry parameters must be set via cli_set_inquiry_handler()
 * - Notification system should be enabled via cli_start_notification_handler()
 *
 * ## Command Processing:
 * The function configures scheduling parameters (priority, step size, end TSF)
 * and sends the start command to the controller. Once started, the controller
 * will handle the frequency hopping, timing, and packet transmission according
 * to the Bluetooth specification.
 *
 * ## State Machine Operation:
 * After successful initiation, the inquiry runs autonomously in the controller
 * using state machines until one of the following occurs:
 * - Natural completion (inquiry length expires)
 * - Manual termination via cli_stop_inquiry_handler()
 * - Error condition encountered
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default scheduling values (99) or custom (0)
 *             For custom: [0]: priority, [1]: step_size, [2]: end_tsf
 *
 * @retval None - Status reported via responsePrint/responsePrintError
 *
 * @note The inquiry will generate BTC_INQUIRY_COMPLETE events when finished,
 *       which are handled by the registered callback functions
 *
 * @note Default command: start_inquiry 99
 *       Non-default command: start_inquiry [priority] [step_size] [end_tsf]
 *
 * @see cli_set_inquiry_handler, cli_stop_inquiry_handler, hss_event_inquiry_complete_handler
 */
void cli_start_inquiry_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_command_parameters(&start_inquiry, 1, 1, 16384);
  } else {
    start_inquiry.priority = sl_cli_get_argument_uint8(args, 0);
    start_inquiry.step_size = sl_cli_get_argument_uint8(args, 1);
    start_inquiry.end_tsf = sl_cli_get_argument_uint32(args, 2);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,priority:%d,step_size:%d,end_tsf:%d",
                is_default, start_inquiry.priority, start_inquiry.step_size, start_inquiry.end_tsf);

  start_inquiry_cmd((hss_cmd_start_inquiry_t *)&start_inquiry, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start notification system for controller events
 *
 * This function enables the notification system that allows the controller to
 * send asynchronous event notifications back to the test application. This is
 * essential for receiving completion events, error notifications, and status
 * updates from ongoing Bluetooth procedures.
 *
 * The function sends a simple notification enable command (ID: 4) to the
 * controller via the RAIL mailbox interface.
 *
 * @param args CLI command arguments (no parameters required)
 *
 * @note This should typically be called early in the testing sequence to ensure
 *       that event notifications are properly received and processed
 */
void cli_start_notification_handler(sl_cli_command_arg_t *args)
{
  uint32_t cmd = 4;
  RAIL_Status_t status = RAIL_USER_SendMbox(btc_rail_handle, cmd);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xA, "Could not send command %d", status);
    return;
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "request:%u",
                  cmd
                  );
  }
}

/**
 * @brief Configure parameters for Bluetooth inquiry scan procedure
 *
 * This function configures the inquiry scan procedure, which allows the device
 * to listen for inquiry packets from other devices attempting discovery. The
 * device will respond with inquiry response packets containing its identity
 * and Extended Inquiry Response (EIR) data.
 *
 * The function supports configuration of scan windows, intervals, EIR data
 * content, and scan types. It can generate custom EIR data based on packet
 * type and length specifications.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (99) or custom (0)
 *             For custom: [0]: window, [1]: interval, [2]: EIR_value,
 *                        [3]: pkt_type, [4]: pkt_len, [5]: curr_scan_type,
 *                        [6-8]: lap_address[3]
 *
 * @note The scan window defines how long to listen, while interval defines
 *       the period between scan windows. EIR data is automatically generated
 *       based on packet type and length when custom parameters are used.
 *
 * @note Default command: set_inquiry_scan 99
 *       Non-default command: set_inquiry_scan [window] [interval] [EIR_value] [pkt_type] [pkt_len] [curr_scan_type] [lap_addr0] [lap_addr1] [lap_addr2]
 */
void cli_set_inquiry_scan_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  uint8_t pkt_type = 0;
  uint16_t pkt_len = 0;
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_inquiry_scan.window = 2048;
    set_inquiry_scan.interval = 4096;
    memcpy(set_inquiry_scan.eir_data, default_eir_pkt, sizeof(default_eir_pkt));
    set_inquiry_scan.eir_data_length = sizeof(default_eir_pkt);
    set_inquiry_scan.EIR_value = 1;
    set_inquiry_scan.curr_scan_type = 0;
    set_inquiry_scan.lap_address[0] = 0x00;
    set_inquiry_scan.lap_address[1] = 0x8B;
    set_inquiry_scan.lap_address[2] = 0x9E;
  } else {
    set_inquiry_scan.window = sl_cli_get_argument_uint32(args, 0);
    set_inquiry_scan.interval = sl_cli_get_argument_uint32(args, 1);
    set_inquiry_scan.EIR_value = sl_cli_get_argument_uint8(args, 2);
    pkt_type = sl_cli_get_argument_uint8(args, 3);
    pkt_len = sl_cli_get_argument_uint16(args, 4);
    form_eir_data(pkt_type, pkt_len, &set_inquiry_scan);
    set_inquiry_scan.curr_scan_type = sl_cli_get_argument_uint8(args, 5);
    for (int i = 0; i < 3; i++) {
      set_inquiry_scan.lap_address[i] = sl_cli_get_argument_uint8(args, 6 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,window:%d,interval:%d,EIR_value:%d,pkt_type:%d,pkt_len:%d,curr_scan_type:%d,lap_address:0x%02X%02X%02X",
                is_default, set_inquiry_scan.window, set_inquiry_scan.interval,
                set_inquiry_scan.EIR_value, pkt_type, pkt_len, set_inquiry_scan.curr_scan_type,
                set_inquiry_scan.lap_address[0], set_inquiry_scan.lap_address[1], set_inquiry_scan.lap_address[2]);

  set_inquiry_scan_cmd(&set_inquiry_scan, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start the Bluetooth inquiry scan procedure
 *
 * This function initiates the inquiry scan process after parameters have been
 * configured via cli_set_inquiry_scan_handler. The device will enter inquiry
 * scan mode, listening for inquiry packets from other devices and responding
 * with inquiry response packets containing its EIR data.
 *
 * The inquiry scan runs autonomously using the controller's state machines,
 * following the configured scan window and interval parameters until completion
 * or manual termination.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default scheduling values (99) or custom (0)
 *             For custom: [0]: priority, [1]: step_size, [2]: end_tsf
 *
 * @note The inquiry scan will generate BTC_INQUIRY_SCAN_COMPLETE events when
 *       finished, handled by the registered callback functions
 *
 * @note Default command: start_inquiry_scan 99
 *       Non-default command: start_inquiry_scan [priority] [step_size] [end_tsf]
 */
void cli_start_inquiry_scan_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_command_parameters(&start_inquiry_scan, 1, 1, 16384);
  } else {
    start_inquiry_scan.priority = sl_cli_get_argument_uint8(args, 0);
    start_inquiry_scan.step_size = sl_cli_get_argument_uint8(args, 1);
    start_inquiry_scan.end_tsf = sl_cli_get_argument_uint32(args, 2);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,priority:%d,step_size:%d,end_tsf:%d",
                is_default, start_inquiry_scan.priority, start_inquiry_scan.step_size, start_inquiry_scan.end_tsf);

  start_inquiry_scan_cmd((hss_cmd_start_inquiry_scan_t *)&start_inquiry_scan, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure parameters for Bluetooth page procedure
 *
 * This function configures the parameters for the Bluetooth page procedure,
 * which is used to establish a connection with a specific remote device whose
 * BD_ADDR is known. The page process involves transmitting ID packets at
 * regular intervals to connect to the target device.
 *
 * The function supports both default parameters (per Bluetooth specification)
 * and custom user-defined parameters for specialized testing scenarios.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (99) or custom (0)
 *             For custom: [0]: clk_e_offset, [1]: hci_trigger, [2]: tx_pwr_index,
 *                        [3]: remote_scan_rep_mode, [4]: lt_addr, [5-10]: bd_address[6]
 *
 * @note After configuration, the command is queued and sent to the controller.
 *       The page procedure must be started separately using cli_start_page_handler.
 *
 * @note Default command: set_page 99
 *       Non-default command: set_page [clk_e_offset] [hci_trigger] [tx_pwr_index] [remote_scan_rep_mode] [lt_addr] [bd_addr0] [bd_addr1] [bd_addr2] [bd_addr3] [bd_addr4] [bd_addr5]
 */
void cli_set_page_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_page.clk_e_offset = 0;   // Default value, change as needed
    set_page.hci_trigger = 0;
    set_page.tx_pwr_index = 100;
    set_page.remote_scan_rep_mode = 0;
    set_page.lt_addr = 5;
    set_default_bd_address(set_page.bd_address);
  } else {
    set_page.clk_e_offset = sl_cli_get_argument_uint32(args, 0);
    set_page.hci_trigger = sl_cli_get_argument_uint32(args, 1);
    set_page.tx_pwr_index = sl_cli_get_argument_uint8(args, 2);
    set_page.remote_scan_rep_mode = sl_cli_get_argument_uint8(args, 3);
    set_page.lt_addr = sl_cli_get_argument_uint8(args, 4);
    for (int i = 0; i < 6; i++) {
      set_page.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,clk_e_offset:%d,hci_trigger:%d,tx_pwr_index:%d,remote_scan_rep_mode:%d,lt_addr:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, set_page.clk_e_offset, set_page.hci_trigger, set_page.tx_pwr_index,
                set_page.remote_scan_rep_mode, set_page.lt_addr, set_page.bd_address[0], set_page.bd_address[1],
                set_page.bd_address[2], set_page.bd_address[3], set_page.bd_address[4], set_page.bd_address[5]);

  set_page_cmd(&set_page, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue
  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start the Bluetooth page procedure
 *
 * This function initiates the actual page process after parameters have been
 * configured via cli_set_page_handler. The page procedure will attempt to
 * establish a connection with the target device specified in the configuration.
 *
 * The function configures scheduling parameters (priority, step size, end TSF)
 * and sends the start command to the controller. Once started, the controller
 * will handle the frequency hopping, timing, and packet transmission according
 * to the Bluetooth specification.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default scheduling values (99) or custom (0)
 *             For custom: [0]: priority, [1]: step_size, [2]: end_tsf
 *
 * @note The page will generate BTC_PAGE_COMPLETE events when finished,
 *       which are handled by the registered callback functions
 *
 * @note Default command: start_page 99
 *       Non-default command: start_page [priority] [step_size] [end_tsf]
 */
void cli_start_page_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_command_parameters(&start_page, 1, 1, 32768);
  } else {
    start_page.priority = sl_cli_get_argument_uint8(args, 0);
    start_page.step_size = sl_cli_get_argument_uint8(args, 1);
    start_page.end_tsf = sl_cli_get_argument_uint32(args, 2);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,priority:%d,step_size:%d,end_tsf:%d",
                is_default, start_page.priority, start_page.step_size, start_page.end_tsf);

  start_page_cmd((hss_cmd_start_page_t *)&start_page, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure parameters for Bluetooth page scan procedure
 *
 * This function configures the page scan procedure, which allows the device
 * to listen for page packets from other devices attempting to establish a
 * connection. The device will respond with page response packets when its
 * BD_ADDR matches the target address in the page packet.
 *
 * The function supports configuration of scan windows, intervals, scan type,
 * and transmit power for the page scan responses.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (99) or custom (0)
 *             For custom: [0]: window, [1]: interval, [2]: curr_scan_type,
 *                        [3]: tx_pwr_index, [4-9]: bd_address[6]
 *
 * @note The scan window defines how long to listen, while interval defines
 *       the period between scan windows. After configuration, the command
 *       is queued and sent to the controller.
 *
 * @note Default command: set_page_scan 99
 *       Non-default command: set_page_scan [window] [interval] [curr_scan_type] [tx_pwr_index] [bd_addr0] [bd_addr1] [bd_addr2] [bd_addr3] [bd_addr4] [bd_addr5]
 */
void cli_set_page_scan_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_page_scan.window = 16;
    set_page_scan.interval = 2048;
    set_page_scan.curr_scan_type = 0;
    set_page_scan.tx_pwr_index = 0;
    set_default_bd_address(set_page_scan.bd_address);
  } else {
    set_page_scan.window = sl_cli_get_argument_uint32(args, 0);
    set_page_scan.interval = sl_cli_get_argument_uint32(args, 1);
    set_page_scan.curr_scan_type = sl_cli_get_argument_uint8(args, 2);
    set_page_scan.tx_pwr_index = sl_cli_get_argument_uint8(args, 3);
    for (int i = 0; i < 6; i++) {
      set_page_scan.bd_address[i] = sl_cli_get_argument_uint8(args, 4 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,window:%d,interval:%d,curr_scan_type:%d,tx_pwr_index:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, set_page_scan.window, set_page_scan.interval, set_page_scan.curr_scan_type,
                set_page_scan.tx_pwr_index, set_page_scan.bd_address[0], set_page_scan.bd_address[1],
                set_page_scan.bd_address[2], set_page_scan.bd_address[3], set_page_scan.bd_address[4], set_page_scan.bd_address[5]);

  set_page_scan_cmd(&set_page_scan, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start the Bluetooth page scan procedure
 *
 * This function initiates the page scan process after parameters have been
 * configured via cli_set_page_scan_handler. The device will enter page scan
 * mode, listening for page packets from other devices and responding with
 * page response packets when its BD_ADDR is targeted.
 *
 * The page scan runs autonomously using the controller's state machines,
 * following the configured scan window and interval parameters until completion
 * or manual termination.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default scheduling values (99) or custom (0)
 *             For custom: [0]: priority, [1]: step_size, [2]: end_tsf
 *
 * @note The page scan will generate BTC_PAGE_SCAN_COMPLETE events when
 *       finished, handled by the registered callback functions
 *
 * @note Default command: start_page_scan 99
 *       Non-default command: start_page_scan [priority] [step_size] [end_tsf]
 */
void cli_start_page_scan_handler(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_command_parameters(&start_page_scan, 1, 1, 32768);
  } else {
    start_page_scan.priority = sl_cli_get_argument_uint8(args, 0);
    start_page_scan.step_size = sl_cli_get_argument_uint8(args, 1);
    start_page_scan.end_tsf = sl_cli_get_argument_uint32(args, 2);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,priority:%d,step_size:%d,end_tsf:%d",
                is_default, start_page_scan.priority, start_page_scan.step_size, start_page_scan.end_tsf);

  start_page_scan_cmd((hss_cmd_start_page_scan_t *)&start_page_scan, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

// CONNECTED STATES

/**
 * @brief Configure device parameters for ACL (Asynchronous Connection-Less) connections
 *
 * This function configures the device parameters required for establishing and
 * maintaining ACL connections, which carry user data in Bluetooth Classic.
 * ACL connections support various data rates (BR/EDR) and flow control mechanisms.
 *
 * Parameters include connection device index, peer role (central/peripheral),
 * transmit power, flow control settings, BR/EDR mode selection, and clock offset
 * compensation for accurate timing synchronization.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (99) or custom (0)
 *             For custom: [0]: tx_pwr_index, [1]: flow, [2]: br_edr_mode, [3]: end_tsf
 *
 * @note These parameters must be configured before starting ACL connections.
 *       The device index allows for multiple concurrent connections.
 */
void cli_set_device_parameters_acl(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_acl_parameters.tx_pwr_index = 29;
    set_acl_parameters.flow = 1;
    set_acl_parameters.br_edr_mode = 1;
    end_tsf_acl = 8192;
  } else {
    set_acl_parameters.tx_pwr_index = sl_cli_get_argument_uint8(args, 0);
    set_acl_parameters.flow = sl_cli_get_argument_uint8(args, 1);
    set_acl_parameters.br_edr_mode = sl_cli_get_argument_uint8(args, 2);
    end_tsf_acl = sl_cli_get_argument_uint32(args, 3);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,tx_pwr_index:%d,flow:%d,br_edr_mode:%d,end_tsf_acl:%d",
                is_default, set_acl_parameters.tx_pwr_index, set_acl_parameters.flow,
                set_acl_parameters.br_edr_mode, end_tsf_acl);
}

/**
 * @brief Configure Adaptive Frequency Hopping (AFH) parameters for ACL connections
 *
 * This function configures AFH parameters for ACL connections, which allows
 * the Bluetooth system to avoid interference by excluding certain frequency
 * channels from the hopping sequence. AFH improves coexistence with other
 * wireless technologies operating in the 2.4 GHz band.
 *
 * The function sets up channel maps, piconet configuration, and timing
 * parameters for AFH operation on the specified ACL connection.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: no_of_valid_afh_channels,
 *                        [2]: piconet_type, [3]: afh_new_channel_map_instant,
 *                        [4-13]: afh_new_channel_map[10], [14-23]: channel_map_in_use[10]
 *
 * @note AFH parameters must be coordinated between central and peripheral devices.
 *       The channel maps define which of the 79 Bluetooth channels are available.
 */
void cli_set_afh_parameters_acl(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_afh_parameters_acl.connection_device_index = 0;
    set_afh_parameters_acl.no_of_valid_afh_channels = 10;
    set_afh_parameters_acl.piconet_type = 0;
    set_afh_parameters_acl.afh_new_channel_map_instant = 0;
    for (int i = 0; i < 10; i++) {
      set_afh_parameters_acl.afh_new_channel_map[i] = 0x11 + i * (0x11) > 100 ? (0x11 + i * (0x11)) / 0x11 : 0x11 + i * (0x11);
      set_afh_parameters_acl.channel_map_in_use[i] = 0x11 + i * (0x11) > 100 ? (0x11 + i * (0x11)) / 0x11 : 0x11 + i * (0x11);
    }
  } else {
    set_afh_parameters_acl.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    set_afh_parameters_acl.no_of_valid_afh_channels = sl_cli_get_argument_uint8(args, 1);
    set_afh_parameters_acl.piconet_type = sl_cli_get_argument_uint8(args, 2);
    set_afh_parameters_acl.afh_new_channel_map_instant = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 10; i++) {
      set_afh_parameters_acl.afh_new_channel_map[i] = sl_cli_get_argument_uint8(args, 4 + i);
      set_afh_parameters_acl.channel_map_in_use[i] = sl_cli_get_argument_uint8(args, 14 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,no_of_valid_afh_channels:%d,piconet_type:%d,afh_new_channel_map_instant:%d",
                is_default, set_afh_parameters_acl.connection_device_index, set_afh_parameters_acl.no_of_valid_afh_channels,
                set_afh_parameters_acl.piconet_type, set_afh_parameters_acl.afh_new_channel_map_instant);

  set_afh_parameters_acl_cmd(&set_afh_parameters_acl, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure sniff mode parameters for ACL connections
 *
 * This function configures sniff mode parameters for ACL connections. Sniff mode
 * is a power-saving mechanism where the peripheral device only listens for
 * packets at specified intervals, reducing power consumption while maintaining
 * the connection.
 *
 * The function sets up sniff timing parameters including interval, attempt,
 * timeout, and the instant when sniff mode becomes active.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: sniff_instant,
 *                        [2]: sniff_interval, [3]: sniff_attempt, [4]: sniff_timeout
 *
 * @note Sniff parameters must be negotiated between central and peripheral devices.
 *       The sniff interval defines how often the peripheral listens for packets.
 */
void cli_set_sniff_parameters_acl(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_sniff_parameters_acl.connection_device_index = 0;
    set_sniff_parameters_acl.sniff_instant = 0;
    set_sniff_parameters_acl.sniff_interval = 0;
    set_sniff_parameters_acl.sniff_attempt = 0;
    set_sniff_parameters_acl.sniff_timeout = 0;
  } else {
    set_sniff_parameters_acl.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    set_sniff_parameters_acl.sniff_instant = sl_cli_get_argument_uint32(args, 1);
    set_sniff_parameters_acl.sniff_interval = sl_cli_get_argument_uint32(args, 2);
    set_sniff_parameters_acl.sniff_attempt = sl_cli_get_argument_uint32(args, 3);
    set_sniff_parameters_acl.sniff_timeout = sl_cli_get_argument_uint32(args, 4);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,sniff_instant:%d,sniff_interval:%d,sniff_attempt:%d,sniff_timeout:%d",
                is_default, set_sniff_parameters_acl.connection_device_index, set_sniff_parameters_acl.sniff_instant,
                set_sniff_parameters_acl.sniff_interval, set_sniff_parameters_acl.sniff_attempt, set_sniff_parameters_acl.sniff_timeout);

  set_sniff_parameters_acl_cmd(&set_sniff_parameters_acl, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start ACL connection as central device
 *
 * This function initiates an ACL connection with the device acting as the central
 * (formerly master) role. The central device is responsible for initiating the
 * connection, managing the frequency hopping sequence, and controlling data flow.
 *
 * Before calling this function, ACL device parameters should be configured using
 * cli_set_device_parameters_acl. The function sets up scheduling parameters and
 * target device address, then starts the ACL connection procedure.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: priority,
 *                        [2]: step_size, [3]: end_tsf, [5-10]: bd_address[6]
 *
 * @note The connection will generate BTC_ACTIVITY_COMPLETE events when
 *       established or if connection fails. Multiple ACL connections can
 *       be managed using different device indices.
 */
void cli_start_acl_central(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    start_acl_central.connection_device_index = 0;
    set_command_parameters(&start_acl_central, 1, 1, 8192);
    set_default_bd_address(start_acl_central.bd_address);
  } else {
    start_acl_central.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    start_acl_central.priority = sl_cli_get_argument_uint8(args, 1);
    start_acl_central.step_size = sl_cli_get_argument_uint8(args, 2);
    start_acl_central.end_tsf = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 6; i++) {
      start_acl_central.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,priority:%d,step_size:%d,end_tsf:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, start_acl_central.connection_device_index, start_acl_central.priority,
                start_acl_central.step_size, start_acl_central.end_tsf, start_acl_central.bd_address[0],
                start_acl_central.bd_address[1], start_acl_central.bd_address[2], start_acl_central.bd_address[3],
                start_acl_central.bd_address[4], start_acl_central.bd_address[5]);

  start_acl_central_cmd(&start_acl_central, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start ACL connection as peripheral device
 *
 * This function initiates an ACL connection with the device acting as the peripheral
 * (formerly slave) role. The peripheral device responds to connection requests
 * from the central device and follows the central's frequency hopping sequence.
 *
 * Before calling this function, ACL device parameters should be configured using
 * cli_set_device_parameters_acl. The function sets up scheduling parameters and
 * target device address, then starts the ACL connection procedure.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: priority,
 *                        [2]: step_size, [3]: end_tsf, [5-10]: bd_address[6]
 *
 * @note The connection will generate BTC_ACTIVITY_COMPLETE events when
 *       established or if connection fails. Multiple ACL connections can
 *       be managed using different device indices.
 */
void cli_start_acl_peripheral(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    start_acl_peripheral.connection_device_index = 0;
    set_command_parameters(&start_acl_peripheral, 1, 1, 8192);
    set_default_bd_address(start_acl_peripheral.bd_address);
  } else {
    start_acl_peripheral.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    start_acl_peripheral.priority = sl_cli_get_argument_uint8(args, 1);
    start_acl_peripheral.step_size = sl_cli_get_argument_uint8(args, 2);
    start_acl_peripheral.end_tsf = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 6; i++) {
      start_acl_peripheral.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,priority:%d,step_size:%d,end_tsf:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, start_acl_peripheral.connection_device_index, start_acl_peripheral.priority,
                start_acl_peripheral.step_size, start_acl_peripheral.end_tsf, start_acl_peripheral.bd_address[0],
                start_acl_peripheral.bd_address[1], start_acl_peripheral.bd_address[2], start_acl_peripheral.bd_address[3],
                start_acl_peripheral.bd_address[4], start_acl_peripheral.bd_address[5]);

  start_acl_peripheral_cmd(&start_acl_peripheral, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure device parameters for eSCO (enhanced Synchronous Connection-Oriented) connections
 *
 * This function configures the device parameters required for establishing and
 * maintaining eSCO connections, which carry real-time audio data in Bluetooth Classic.
 * eSCO connections provide guaranteed bandwidth and timing for voice applications.
 *
 * Parameters include connection device index, logical transport address, flow control,
 * peer role, and eSCO-specific timing parameters (T_esco, D_esco, W_esco) that define
 * the synchronous slots and retransmission windows.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: lt_addr, [2]: flow,
 *                        [3]: peer_role, [4]: t_esco, [5]: d_esco, [6]: w_esco
 *
 * @note These parameters must be configured before starting eSCO connections.
 *       The timing parameters determine bandwidth and latency characteristics.
 */
void cli_set_device_parameters_esco(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_device_parameters_esco.connection_device_index = 0;
    set_device_parameters_esco.lt_addr = 2;
    set_device_parameters_esco.flow = 1;
    set_device_parameters_esco.peer_role = 1;
    set_device_parameters_esco.t_esco = 6;
    set_device_parameters_esco.d_esco = 0;
    set_device_parameters_esco.w_esco = 2;
  } else {
    set_device_parameters_esco.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    set_device_parameters_esco.lt_addr = sl_cli_get_argument_uint8(args, 1);
    set_device_parameters_esco.flow = sl_cli_get_argument_uint8(args, 2);
    set_device_parameters_esco.peer_role = sl_cli_get_argument_uint8(args, 3);
    set_device_parameters_esco.t_esco = sl_cli_get_argument_uint8(args, 4);
    set_device_parameters_esco.d_esco = sl_cli_get_argument_uint8(args, 5);
    set_device_parameters_esco.w_esco = sl_cli_get_argument_uint8(args, 6);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,lt_addr:%d,flow:%d,peer_role:%d,t_esco:%d,d_esco:%d,w_esco:%d",
                is_default, set_device_parameters_esco.connection_device_index, set_device_parameters_esco.lt_addr,
                set_device_parameters_esco.flow, set_device_parameters_esco.peer_role, set_device_parameters_esco.t_esco,
                set_device_parameters_esco.d_esco, set_device_parameters_esco.w_esco);

  set_device_parameters_esco_cmd(&set_device_parameters_esco, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start eSCO connection as central device
 *
 * This function initiates an eSCO connection with the device acting as the central
 * role. The central device is responsible for establishing the synchronous connection
 * and managing the eSCO link parameters including timing slots and bandwidth allocation.
 *
 * Before calling this function, eSCO device parameters should be configured using
 * cli_set_device_parameters_esco. The function sets up scheduling parameters and
 * target device address, then starts the eSCO connection procedure.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: priority,
 *                        [2]: step_size, [3]: end_tsf, [5-10]: bd_address[6]
 *
 * @note The eSCO connection will generate BTC_ACTIVITY_COMPLETE events when
 *       established or if connection fails. eSCO provides guaranteed timing
 *       for real-time audio applications.
 */
void cli_start_esco_central(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    start_esco_central.connection_device_index = 0;
    set_command_parameters(&start_esco_central, 1, 1, 8192);
    set_default_bd_address(start_esco_central.bd_address);
  } else {
    start_esco_central.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    start_esco_central.priority = sl_cli_get_argument_uint8(args, 1);
    start_esco_central.step_size = sl_cli_get_argument_uint8(args, 2);
    start_esco_central.end_tsf = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 6; i++) {
      start_esco_central.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,priority:%d,step_size:%d,end_tsf:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, start_esco_central.connection_device_index, start_esco_central.priority,
                start_esco_central.step_size, start_esco_central.end_tsf, start_esco_central.bd_address[0],
                start_esco_central.bd_address[1], start_esco_central.bd_address[2], start_esco_central.bd_address[3],
                start_esco_central.bd_address[4], start_esco_central.bd_address[5]);

  start_esco_central_cmd(&start_esco_central, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start eSCO connection as peripheral device
 *
 * This function initiates an eSCO connection with the device acting as the peripheral
 * role. The peripheral device responds to eSCO connection requests from the central
 * device and follows the central's synchronous slot timing and bandwidth allocation.
 *
 * Before calling this function, eSCO device parameters should be configured using
 * cli_set_device_parameters_esco. The function sets up scheduling parameters and
 * target device address, then starts the eSCO connection procedure.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: priority,
 *                        [2]: step_size, [3]: end_tsf, [5-10]: bd_address[6]
 *
 * @note The eSCO connection will generate BTC_ACTIVITY_COMPLETE events when
 *       established or if connection fails. The peripheral must synchronize
 *       with the central's timing requirements.
 */
void cli_start_esco_peripheral(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    start_esco_peripheral.connection_device_index = 0;
    set_command_parameters(&start_esco_peripheral, 1, 1, 8192);
    set_default_bd_address(start_esco_peripheral.bd_address);
  } else {
    start_esco_peripheral.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    start_esco_peripheral.priority = sl_cli_get_argument_uint8(args, 1);
    start_esco_peripheral.step_size = sl_cli_get_argument_uint8(args, 2);
    start_esco_peripheral.end_tsf = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 6; i++) {
      start_esco_peripheral.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,priority:%d,step_size:%d,end_tsf:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, start_esco_peripheral.connection_device_index, start_esco_peripheral.priority,
                start_esco_peripheral.step_size, start_esco_peripheral.end_tsf, start_esco_peripheral.bd_address[0],
                start_esco_peripheral.bd_address[1], start_esco_peripheral.bd_address[2], start_esco_peripheral.bd_address[3],
                start_esco_peripheral.bd_address[4], start_esco_peripheral.bd_address[5]);

  start_esco_peripheral_cmd(&start_esco_peripheral, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure device parameters for role switch operations
 *
 * This function configures the device parameters required for performing
 * role switch operations in Bluetooth Classic connections. Role switch allows
 * the central and peripheral devices to exchange roles during an active connection,
 * which can be useful for power management and connection optimization.
 *
 * Parameters include connection device index, peer role configuration, logical
 * transport address, transmit power, flow control, and clock offset compensation
 * for accurate timing synchronization during the role switch procedure.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: peer_role, [2]: lt_addr,
 *                        [3]: tx_pwr_index, [4]: flow, [5]: clk_e_offset
 *
 * @note These parameters must be configured before initiating role switch operations.
 *       The role switch procedure requires coordination between both devices.
 */
void cli_set_device_parameters_roleswitch(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_device_parameters_roleswitch.connection_device_index = 1;
    set_device_parameters_roleswitch.peer_role = 1;
    set_device_parameters_roleswitch.lt_addr = 1;
    set_device_parameters_roleswitch.tx_pwr_index = 23;
    set_device_parameters_roleswitch.flow = 0;
    set_device_parameters_roleswitch.clk_e_offset = 0;
  } else {
    set_device_parameters_roleswitch.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    set_device_parameters_roleswitch.peer_role = sl_cli_get_argument_uint8(args, 1);
    set_device_parameters_roleswitch.lt_addr = sl_cli_get_argument_uint8(args, 2);
    set_device_parameters_roleswitch.tx_pwr_index = sl_cli_get_argument_uint8(args, 3);
    set_device_parameters_roleswitch.flow = sl_cli_get_argument_uint8(args, 4);
    set_device_parameters_roleswitch.clk_e_offset = sl_cli_get_argument_uint32(args, 5);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,peer_role:%d,lt_addr:%d,tx_pwr_index:%d,flow:%d,clk_e_offset:%d",
                is_default, set_device_parameters_roleswitch.connection_device_index, set_device_parameters_roleswitch.peer_role,
                set_device_parameters_roleswitch.lt_addr, set_device_parameters_roleswitch.tx_pwr_index,
                set_device_parameters_roleswitch.flow, set_device_parameters_roleswitch.clk_e_offset);

  set_device_parameters_roleswitch_cmd(&set_device_parameters_roleswitch, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Configure role switch timing parameters
 *
 * This function configures the specific timing parameters for role switch
 * operations including the new logical transport address, slot offset, and
 * the instant when the role switch becomes effective. These parameters ensure
 * proper coordination between devices during the role exchange.
 *
 * The role switch instant defines the exact slot when both devices will
 * exchange their roles, ensuring synchronized transition without connection loss.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: new_lt_addr,
 *                        [2]: roleswitch_slot_offset, [3]: roleswitch_instant
 *
 * @note The role switch instant must be far enough in the future to allow
 *       both devices to prepare for the role exchange. This function should
 *       be called after configuring device parameters for role switch.
 */
void cli_set_roleswitch_parameters(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    set_roleswitch_parameters.connection_device_index = 0;
    set_roleswitch_parameters.new_lt_addr = 2;
    set_roleswitch_parameters.roleswitch_slot_offset = 0;
    set_roleswitch_parameters.roleswitch_instant = 16;
  } else {
    set_roleswitch_parameters.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    set_roleswitch_parameters.new_lt_addr = sl_cli_get_argument_uint8(args, 1);
    set_roleswitch_parameters.roleswitch_slot_offset = sl_cli_get_argument_uint16(args, 2);
    set_roleswitch_parameters.roleswitch_instant = sl_cli_get_argument_uint32(args, 3);
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,new_lt_addr:%d,roleswitch_slot_offset:%d,roleswitch_instant:%d",
                is_default, set_roleswitch_parameters.connection_device_index, set_roleswitch_parameters.new_lt_addr,
                set_roleswitch_parameters.roleswitch_slot_offset, set_roleswitch_parameters.roleswitch_instant);

  set_roleswitch_parameters_cmd(&set_roleswitch_parameters, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start role switch operation as central device
 *
 * This function initiates a role switch operation with the device currently
 * acting as the central. During role switch, the central and peripheral
 * exchange roles, allowing the current central to become the peripheral
 * and vice versa. This can be useful for power management and optimization.
 *
 * Before calling this function, role switch parameters should be configured
 * using cli_set_device_parameters_roleswitch and cli_set_roleswitch_parameters.
 * The function sets up scheduling parameters and sends the start command.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: priority,
 *                        [2]: step_size, [3]: end_tsf, [5-10]: bd_address[6]
 *
 * @note The role switch will generate BTC_ROLE_SWITCH_COMPLETE events when
 *       finished. Both devices must coordinate for successful role exchange.
 */
void cli_start_roleswitch_central(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    start_roleswitch_central.connection_device_index = 0;
    start_roleswitch_central.priority = 1;
    start_roleswitch_central.step_size = 1;
    start_roleswitch_central.end_tsf = 18192;
    set_default_bd_address(start_roleswitch_central.bd_address);
  } else {
    start_roleswitch_central.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    start_roleswitch_central.priority = sl_cli_get_argument_uint8(args, 1);
    start_roleswitch_central.step_size = sl_cli_get_argument_uint8(args, 2);
    start_roleswitch_central.end_tsf = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 6; i++) {
      start_roleswitch_central.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,priority:%d,step_size:%d,end_tsf:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, start_roleswitch_central.connection_device_index, start_roleswitch_central.priority,
                start_roleswitch_central.step_size, start_roleswitch_central.end_tsf, start_roleswitch_central.bd_address[0],
                start_roleswitch_central.bd_address[1], start_roleswitch_central.bd_address[2], start_roleswitch_central.bd_address[3],
                start_roleswitch_central.bd_address[4], start_roleswitch_central.bd_address[5]);

  start_roleswitch_central_cmd(&start_roleswitch_central, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief Start role switch operation as peripheral device
 *
 * This function initiates a role switch operation with the device currently
 * acting as the peripheral. During role switch, the peripheral and central
 * exchange roles, allowing the current peripheral to become the central
 * and vice versa. The peripheral responds to role switch requests from central.
 *
 * Before calling this function, role switch parameters should be configured
 * using cli_set_device_parameters_roleswitch and cli_set_roleswitch_parameters.
 * The function sets up scheduling parameters and sends the start command.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default values (1) or custom (0)
 *             For custom: [0]: connection_device_index, [1]: priority,
 *                        [2]: step_size, [3]: end_tsf, [5-10]: bd_address[6]
 *
 * @note The role switch will generate BTC_ROLE_SWITCH_COMPLETE events when
 *       finished. The peripheral must coordinate with central for successful
 *       role exchange at the predetermined instant.
 */
void cli_start_roleswitch_peripheral(sl_cli_command_arg_t *args)
{
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);
  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    start_roleswitch_peripheral.connection_device_index = 0;
    start_roleswitch_peripheral.priority = 1;
    start_roleswitch_peripheral.step_size = 1;
    start_roleswitch_peripheral.end_tsf = 18192;
    set_default_bd_address(start_roleswitch_peripheral.bd_address);
  } else {
    start_roleswitch_peripheral.connection_device_index = sl_cli_get_argument_uint8(args, 0);
    start_roleswitch_peripheral.priority = sl_cli_get_argument_uint8(args, 1);
    start_roleswitch_peripheral.step_size = sl_cli_get_argument_uint8(args, 2);
    start_roleswitch_peripheral.end_tsf = sl_cli_get_argument_uint32(args, 3);
    for (int i = 0; i < 6; i++) {
      start_roleswitch_peripheral.bd_address[i] = sl_cli_get_argument_uint8(args, 5 + i);
    }
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "is_default:%d,connection_device_index:%d,priority:%d,step_size:%d,end_tsf:%d,bd_address:0x%02X%02X%02X%02X%02X%02X",
                is_default, start_roleswitch_peripheral.connection_device_index, start_roleswitch_peripheral.priority,
                start_roleswitch_peripheral.step_size, start_roleswitch_peripheral.end_tsf, start_roleswitch_peripheral.bd_address[0],
                start_roleswitch_peripheral.bd_address[1], start_roleswitch_peripheral.bd_address[2], start_roleswitch_peripheral.bd_address[3],
                start_roleswitch_peripheral.bd_address[4], start_roleswitch_peripheral.bd_address[5]);

  start_roleswitch_peripheral_cmd(&start_roleswitch_peripheral, (shared_mem_t *)&queue);   // pkb_alloc, fill the macros and enqueue

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

/**
 * @brief CLI handler for stopping an inquiry procedure
 *
 * This function stops an ongoing inquiry procedure on the specified device.
 * When an inquiry is running autonomously via the controller's state machines,
 * this command provides a way to terminate it before natural completion.
 *
 * The function prepares a stop inquiry command with the target device index
 * and sends it to the controller. The controller will immediately halt the
 * inquiry process and may generate a completion event.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default device index (99) or specify (0)
 *             [0]: current_device_index (when not using defaults)
 *
 * @note This function assumes that an inquiry procedure is currently active.
 *       Calling this when no inquiry is running may result in an error or
 *       be silently ignored by the controller.
 *
 * @note Default command: stop_inquiry 99
 *       Non-default command: stop_inquiry 0 [device_index]
 */
void cli_stop_inquiry_handler(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index;

  // Check if using default parameters
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    // Use default device index
    current_device_index = 0;  // Assuming 0 is the default device index
  } else {
    // Get device index from command line
    current_device_index = sl_cli_get_argument_uint8(args, 0);
  }

  // Initialize stop_inquiry structure with device index
  stop_inquiry.current_device_index = current_device_index;

  // Call command preparation function (similar to other commands)
  stop_inquiry_cmd(&stop_inquiry, (shared_mem_t *)&queue);

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Stopping inquiry on device: %d",
                current_device_index);
}

/**
 * @brief Stop the Bluetooth inquiry scan procedure
 *
 * This function stops an ongoing inquiry scan procedure on the specified device.
 * When an inquiry scan is running autonomously via the controller's state machines,
 * this command provides a way to terminate it before natural completion.
 *
 * The function prepares a stop inquiry scan command with the target device index
 * and sends it to the controller. The controller will immediately halt the
 * inquiry scan process and may generate a completion event.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default device index (99) or specify (0)
 *             [0]: current_device_index (when not using defaults)
 *
 * @note This function assumes that an inquiry scan procedure is currently active.
 *       Calling this when no inquiry scan is running may result in an error or
 *       be silently ignored by the controller.
 *
 * @note Default command: stop_inquiry_scan 99
 *       Non-default command: stop_inquiry_scan 0 [device_index]
 */
void cli_stop_inquiry_scan_handler(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index;

  // Check if using default parameters
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    // Use default device index
    current_device_index = 0;  // Assuming 0 is the default device index
  } else {
    // Get device index from command line
    current_device_index = sl_cli_get_argument_uint8(args, 0);
  }

  // Initialize stop_inquiry_scan structure with device index
  stop_inquiry_scan.current_device_index = current_device_index;

  // Call command preparation function (similar to other commands)
  stop_inquiry_scan_cmd(&stop_inquiry_scan, (shared_mem_t *)&queue);

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Stopping inquiry scan on device: %d",
                current_device_index);
}

/**
 * @brief Stop the Bluetooth page procedure
 *
 * This function stops an ongoing page procedure on the specified device.
 * When a page is running autonomously via the controller's state machines,
 * this command provides a way to terminate it before connection establishment
 * or natural timeout.
 *
 * The function prepares a stop page command with the target device index
 * and sends it to the controller. The controller will immediately halt the
 * page process and may generate a completion event.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default device index (99) or specify (0)
 *             [0]: current_device_index (when not using defaults)
 *
 * @note This function assumes that a page procedure is currently active.
 *       Calling this when no page is running may result in an error or
 *       be silently ignored by the controller.
 *
 * @note Default command: stop_page 99
 *       Non-default command: stop_page 0 [device_index]
 */
void cli_stop_page_handler(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index = sl_cli_get_argument_uint8(args, 0) == TRIGGER_CMD_WITH_DEFAULTS ? 0 : sl_cli_get_argument_uint8(args, 0);
  stop_page.current_device_index = current_device_index;
  stop_page_cmd(&stop_page, (shared_mem_t *)&queue);

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Stopping page on device: %d",
                current_device_index);
}

/**
 * @brief Stop the Bluetooth page scan procedure
 *
 * This function stops an ongoing page scan procedure on the specified device.
 * When a page scan is running autonomously via the controller's state machines,
 * this command provides a way to terminate it before natural completion.
 *
 * The function prepares a stop page scan command with the target device index
 * and sends it to the controller. The controller will immediately halt the
 * page scan process and may generate a completion event.
 *
 * @param args CLI command arguments containing:
 *             [0]: is_default - Use default device index (99) or specify (0)
 *             [0]: current_device_index (when not using defaults)
 *
 * @note This function assumes that a page scan procedure is currently active.
 *       Calling this when no page scan is running may result in an error or
 *       be silently ignored by the controller.
 *
 * @note Default command: stop_page_scan 99
 *       Non-default command: stop_page_scan 0 [device_index]
 */
void cli_stop_page_scan_handler(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index;
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    current_device_index = 0;  // Assuming 0 is the default device index
  } else {
    current_device_index = sl_cli_get_argument_uint8(args, 0);
  }

  stop_page_scan.current_device_index = current_device_index;
  stop_page_scan_cmd(&stop_page_scan, (shared_mem_t *)&queue);
  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Stopping page scan on device: %d",
                current_device_index);
}

/**
 * @brief Set the fixed BTC channel
 *
 * This function sets the fixed BTC channel for the specified device for
 * performing tx and rx in the same channel.
 *
 * @param args CLI command arguments containing:
 *             [0]: channel - The channel to set
 */
void set_fixed_btc_channel_handler(sl_cli_command_arg_t *args)
{
  uint8_t channel = sl_cli_get_argument_uint8(args, 0);
  set_fixed_btc_channel.channel = channel;
  set_fixed_btc_channel_cmd(&set_fixed_btc_channel, (shared_mem_t *)&queue);
  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
  responsePrint(sl_cli_get_command_string(args, 0),
                "Set fixed BTC channel: %d",
                channel);
}

/**
 * @brief Generate Extended Inquiry Response (EIR) data packet
 *
 * This function creates properly formatted EIR data for inquiry scan responses.
 * It uses the form_data_pkt utility to generate a complete packet with headers
 * and payload according to the specified packet type and length. The rate is
 * automatically determined based on the packet type.
 *
 * The generated EIR data includes proper packet headers, payload headers based
 * on packet type, and payload data that will be transmitted in response to
 * inquiry packets from discovering devices.
 *
 * @param pkt_type Bluetooth packet type (DM1, DH1, DM3, DH3, DM5, DH5)
 * @param pkt_len Length of payload data in bytes
 * @param set_inquiry_scan Pointer to inquiry scan command structure to update
 *
 * @note The total EIR data length includes packet header, payload header,
 *       and actual payload data. This is automatically calculated and stored.
 *       The rate (BR/EDR mode) is determined based on packet type:
 *       - DM1/DH1: Can be BASIC_RATE or ENHANCED_RATE (defaulting to ENHANCED_RATE)
 *       - Multi-slot packets (DM3, DH3, DM5, DH5, etc.): ENHANCED_RATE
 *       - Other packets: BASIC_RATE
 */
void cli_stop_acl_central_handler(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index;
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    current_device_index = 0;  // Assuming 0 is the default device index
  } else {
    current_device_index = sl_cli_get_argument_uint8(args, 0);
  }
  stop_acl_central.connection_device_index = current_device_index;
  stop_acl_central_cmd(&stop_acl_central, (shared_mem_t *)&queue);

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Stopping ACL on device: %d",
                current_device_index);
}

void cli_stop_acl_peripheral_handler(sl_cli_command_arg_t *args)
{
  uint8_t current_device_index;
  uint8_t is_default = sl_cli_get_argument_uint8(args, 0);

  if (is_default == TRIGGER_CMD_WITH_DEFAULTS) {
    current_device_index = 0;  // Assuming 0 is the default device index
  } else {
    current_device_index = sl_cli_get_argument_uint8(args, 0);
  }
  stop_acl_peripheral.connection_device_index = current_device_index;
  stop_acl_peripheral_cmd(&stop_acl_peripheral, (shared_mem_t *)&queue);

  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);

  responsePrint(sl_cli_get_command_string(args, 0),
                "Stopping ACL on device: %d",
                current_device_index);
}

void cli_trigger_roleswitch(sl_cli_command_arg_t *args)
{
  uint32_t flags = sl_cli_get_argument_uint32(args, 0);
  if (flags & 0x1) {
    g_trigger_roleswitch = 1;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "enable_roleswitch: %d", flags);
}

void cli_compute_roleswitch_slot_offset(sl_cli_command_arg_t *args)
{
  compute_slot_offset_event_cmd(&hss_event_roleswitch_compute_offset, (shared_mem_t *)&queue);     // pkb_alloc, fill the macros and enqueue
  uint32_t cmd = (uint32_t)&queue;
  send_command(args, &cmd);
}

void form_eir_data(uint8_t pkt_type, uint16_t pkt_len, hss_cmd_set_inquiry_scan_t* set_inquiry_scan)
{
  uint8_t *eir_pkt = set_inquiry_scan->eir_data;
  uint16_t len = form_data_pkt(pkt_type, pkt_len, eir_pkt, BASIC_RATE);
  set_inquiry_scan->eir_data_length = len; // Set the length of the EIR data + payload header len + packet header len
}

/**
 * @brief Request statistics from the Bluetooth controller
 *
 * This function requests the controller to provide statistics about its
 * current operation, including packet counts, error rates, timing information,
 * and other diagnostic data useful for analyzing test results.
 *
 * The function sets a global flag to indicate that statistics are being
 * requested and sends a statistics command to the controller. The controller
 * will respond asynchronously with the requested data.
 *
 * @param args CLI command arguments (no parameters required)
 *
 * @note The stats_check flag is used by other parts of the system to know
 *       that statistics data is expected. The actual statistics will be
 *       received via the notification/event system.
 */
void cli_show_stats_handler(sl_cli_command_arg_t *args)
{
  stats_check = true;
  uint32_t cmd = STATS_CLI_VALUE;
  RAIL_Status_t status = RAIL_USER_SendMbox(btc_rail_handle, cmd);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xA, "Could not send command %d", status);
    return;
  }

  responsePrint(sl_cli_get_command_string(args, 0), "status:Statistics request sent successfully");
}

/**
 * @brief Configure PTI (Packet Trace Interface) auxiliary data output
 *
 * This function configures the PTI system for debugging and analysis of
 * Bluetooth traffic. PTI provides real-time packet capture and timing
 * information that can be used with external analysis tools.
 *
 * The function enables PTI auxiliary data output and configures PRS (Peripheral
 * Reflex System) signals for various Bluetooth controller events including:
 * - TX/RX start/end signals
 * - Packet detection and data valid signals
 * - MAC layer activity indicators
 * - PHY layer status signals
 *
 * @param args CLI command arguments containing:
 *             [0]: ptiValue - Enable (1) or disable (0) PTI auxiliary data
 *
 * @note This function is primarily used for debugging and development.
 *       The PRS configuration provides GPIO outputs for oscilloscope
 *       or logic analyzer monitoring of internal controller signals.
 */
void set_pti_auxdata(sl_cli_command_arg_t * args)
{
  int ptiValue = !!sl_cli_get_argument_uint8(args, 0);
  PTI_ConfigOutput(0x18);
  responsePrint(sl_cli_get_command_string(args, 0), "PTI: %d", ptiValue);
  PTI_EnableAuxdata(ptiValue);
  PTI_AuxdataOutput('A');
  PTI_AuxdataOutput('D');
}

/**
 * @brief Enable PRS signals for either Tx(0)/Rx(1)
 *
 * This function enables the PRS signals for either Tx(0)/Rx(1) on the specified GPIO pins.
 * It also configures the PRS system to output the signals on the specified GPIO pins.
 * PA5: TX_START/RX_START - mapped to PA4 on connector P2 - 12
 * PA6: TX_DATA/RX_DATA - mapped to PA5 on connector P2 - 11
 * PA0: TX_END/RX_END - mapped to PA0 on connector P2 - 14
 * PB1: TX_PHY_CCA/RX_PHY_CCA - mapped to PB1 on connector P2 - 13
 * PB0: BBP_SOFT_RESET_N - mapped to PB0 on connector P2 - 10
 * @param args CLI command arguments containing:
 *             [0]: prs_mode - PRS mode (0: Tx, 1: Rx)
 */
void enable_prs_handler(sl_cli_command_arg_t *args)
{
  uint8_t prs_mode = sl_cli_get_argument_uint8(args, 0);
  // Enable PRS for Tx
  if (prs_mode == 0) {
    BTCLMAC->PRS_CTRL = BTCLMAC_PRS_CTRL_PRSMODE_TX;
  } else {
    // Enable PRS for Rx
    BTCLMAC->PRS_CTRL = BTCLMAC_PRS_CTRL_PRSMODE_RX;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "PRS: %d", prs_mode);
  sl_gpio_t gpio;

  // PA5 available on connector P2 - 12 - TX_START/RX_START
  gpio.port = 0;
  gpio.pin = 5;
  halEnablePrs(0, 0, gpio, _PRS_ASYNC_CH_CTRL_SOURCESEL_BTCLMACL, 0x00);

  // PA6 available on connector P2 - 11 - TX_DATA/RX_DATA
  gpio.port = 0;
  gpio.pin = 6;
  halEnablePrs(1, 0, gpio, _PRS_ASYNC_CH_CTRL_SOURCESEL_BTCLMACL, 0x01);

  // PA0 available on connector P2 - 14 - TX_END/RX_END
  gpio.port = 0;
  gpio.pin = 0;
  halEnablePrs(2, 0, gpio, _PRS_ASYNC_CH_CTRL_SOURCESEL_BTCLMACL, 0x06);

  // PB1 available on connector P2 - 13 - TX_PHY_CCA/RX_PHY_CCA
  gpio.port = 1;
  gpio.pin = 1;
  halEnablePrs(3, 0, gpio, _PRS_ASYNC_CH_CTRL_SOURCESEL_BTCLMACL, 0x04);

  // PB0 available on connector P2 - 10 - BBP_SOFT_RESET_N
  gpio.port = 1;
  gpio.pin = 0;
  halEnablePrs(4, 0, gpio, _PRS_ASYNC_CH_CTRL_SOURCESEL_BTCLMAC, 0x0D);
}

/**
 * @brief Enable PRS (Peripheral Reflex System) output on specified GPIO
 *
 * This function configures a PRS channel to output internal controller signals
 * on a GPIO pin for debugging and analysis purposes. It connects a specific
 * signal source to a PRS channel and routes that channel to a GPIO output.
 *
 * The PRS system allows real-time monitoring of internal controller events
 * such as TX/RX activity, packet detection, data flow, and timing signals
 * without affecting controller performance.
 *
 * @param channel PRS channel number (0-6)
 * @param loc Location parameter (unused in current implementation)
 * @param portPin GPIO port and pin structure for output
 * @param source Signal source selector (controller peripheral)
 * @param signal Signal selector within the source peripheral
 *
 * @note This function handles both legacy CMU and new clock manager APIs
 *       for maximum compatibility across different SDK versions
 */
void halEnablePrs(uint8_t channel,
                  uint8_t loc,
                  sl_gpio_t portPin,
                  uint8_t source,
                  uint8_t signal)
{
#ifndef SL_CATALOG_CLOCK_MANAGER_PRESENT
  (void)loc;
  // Make sure the PRS is on and clocked
  CMU_ClockEnable(cmuClock_PRS, true);

  PRS_SourceAsyncSignalSet(channel,
                           ( ( uint32_t ) source << _PRS_ASYNC_CH_CTRL_SOURCESEL_SHIFT),
                           ( ( uint32_t ) signal << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT) );
#else
  (void)loc;
  // Make sure the PRS is on and clocked
  sl_clock_manager_enable_bus_clock(SL_BUS_CLOCK_PRS);

  sl_hal_prs_async_connect_channel_producer(channel,
                                            (sl_hal_prs_sync_producer_signal_t)(((uint32_t)source << _PRS_ASYNC_CH_CTRL_SOURCESEL_SHIFT)
                                                                                | ((uint32_t)signal << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT)));
#endif //SL_CATALOG_CLOCK_MANAGER_PRESENT
  sl_hal_prs_pin_output(channel, SL_HAL_PRS_TYPE_ASYNC, portPin.port, portPin.pin);
  // Configure this GPIO as an output low to finish enabling this signal
  sl_gpio_set_pin_mode(&portPin, SL_GPIO_MODE_PUSH_PULL, 0);
}

/**
 * @brief Configure Bluetooth dual-mode logging system
 *
 * This function enables or configures the Bluetooth dual-mode fast logging
 * system for capturing detailed controller operation data. The logging system
 * provides high-speed capture of internal events, state transitions, and
 * debugging information from the Bluetooth controller.
 *
 * When enabled (flags & 0x1), the function initializes the fast log consumer
 * application which can capture and format log data for analysis.
 *
 * @param args CLI command arguments containing:
 *             [0]: flags - Logging configuration flags
 *                  Bit 0: Enable/disable fast log consumer
 *
 * @note This logging system is separate from PTI and provides software-level
 *       debugging information rather than hardware signal monitoring
 */
void set_btdm_logger(sl_cli_command_arg_t *args)
{
  uint32_t flags = sl_cli_get_argument_uint32(args, 0);
  if (flags & 0x1) {
    sl_btdm_fast_log_consumer_app_init();
  }
  responsePrint(sl_cli_get_command_string(args, 0), "setBtdmLogger: %d", flags);
}

/**
 * @brief Start the ACL traffic pattern
 *
 * This function initiates the ACL traffic pattern for testing purposes.
 *
 * @param args CLI command arguments containing:
 *             [0]: packet_type - Type of packet to send
 *             [1]: payload_length - Length of the payload
 *             [2]: packet_interval - packet_interval in slots (1 slot = 625 microseconds)
 */
void start_acl_traffic_pattern(sl_cli_command_arg_t *args)
{
  uint32_t timestamp = (uint32_t)(PROTIMER_GetPTicks() >> TSF_SCALE_FACTOR);
  acl_traffic_pattern.packet_type = sl_cli_get_argument_uint32(args, 0);
  acl_traffic_pattern.payload_length = sl_cli_get_argument_uint32(args, 1);
  acl_traffic_pattern.packet_interval = sl_cli_get_argument_uint32(args, 2);
  acl_traffic_pattern.packet_interval_tsf =  acl_traffic_pattern.packet_interval * BT_SLOT_WIDTH_HSS;
  RAIL_SetTimer(btc_rail_handle, acl_traffic_pattern.packet_interval_tsf + timestamp, RAIL_TIME_ABSOLUTE, &acl_traffic_pattern_cb);
  responsePrint(sl_cli_get_command_string(args, 0),
                "status:Set ACL traffic pattern, packet_type:%d, payload_length:%d, interval:%d",
                acl_traffic_pattern.packet_type, acl_traffic_pattern.payload_length, acl_traffic_pattern.packet_interval);
}

/**
 * @brief Stop the ACL traffic pattern
 *
 * This function stops the ACL traffic pattern that was previously started.
 *
 * @param args CLI command arguments (not used)
 */
void stop_acl_traffic_pattern(sl_cli_command_arg_t *args)
{
  RAIL_CancelTimer(btc_rail_handle);
  responsePrint(sl_cli_get_command_string(args, 0), "status:Stopped ACL traffic pattern");
}

/**
 * @brief Enable LPW to HSS event logger
 *
 * This function enables the LPW to HSS event logger which is used to log events from LPW to HSS.
 *
 * @param args CLI command arguments containing:
 *             [0]: flags - Logging configuration flags
 *                  Bit 0: Enable/disable LPW to HSS event logger
 *
 * @note This function is used to log events from LPW to HSS.
 */
void set_lpw_hss_event_logger(sl_cli_command_arg_t *args)
{
  uint32_t flags = sl_cli_get_argument_uint32(args, 0);
  if (flags & 0x1) {
    lpw_hss_event_rtt_init();
    lpw_hss_event_logger_enabled = 1;
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "set_lpw_hss_event_logger: flags=%d, enabled=%d",
                flags, lpw_hss_event_logger_enabled);
}
/**
 * @brief Initialize the Bluetooth Classic testing application
 *
 * This function performs comprehensive initialization of the Bluetooth testing
 * framework including packet buffer pools, semaphore locks, and receive buffer
 * pre-allocation. It sets up the infrastructure required for efficient packet
 * handling and inter-process communication.
 *
 * Initialization includes:
 * - Semaphore locks for thread-safe queue operations
 * - Small packet buffer pool (8 buffers) for control packets
 * - Medium packet buffer pool (48 buffers) for data packets
 * - Pre-allocation of 20 receive buffers for incoming packets
 *
 * The buffer pools use different sizes optimized for different packet types:
 * - Small buffers: Control and short data packets
 * - Medium buffers: Longer data packets and complex structures
 *
 * @note This function must be called before any Bluetooth operations can be
 *       performed. It establishes the memory management and synchronization
 *       infrastructure used throughout the testing framework.
 */
void btc_app_init()
{
  btc_semaphore_locks_init();
  init_btc_pool(&pkb_pool_g[pkb_chunk_type_small], pkb_chunk_size_small, pkb_chunk_type_small, 8, 3);
  init_btc_pool(&pkb_pool_g[pkb_chunk_type_medium], pkb_chunk_size_medium, pkb_chunk_type_medium, 48, 5);

  for (uint32_t i = 0; i < pkb_pool_g[pkb_chunk_type_small].pkb_num_max; i++) {
    add_buffer_to_btc_pool(&pkb_pool_g[pkb_chunk_type_small], (pkb_t *)short_pkt_heap[i]);
  }

  for (uint32_t i = 0; i < pkb_pool_g[pkb_chunk_type_medium].pkb_num_max; i++) {
    add_buffer_to_btc_pool(&pkb_pool_g[pkb_chunk_type_medium], (pkb_t *)medium_pkt_heap[i]);
  }
  for (int i = 0; i < 20; i++) {
    pkb_t *pkb = pkb_alloc(&pkb_pool_g[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
    enqueue(&queue.rx_precommit_buffers_queue, &pkb->queue_node);
  }
}

/**
 * @brief Initialize semaphore locks for thread-safe queue operations
 *
 * This function initializes all semaphore locks used throughout the Bluetooth
 * testing framework to ensure thread-safe access to shared queue structures.
 * It creates RAIL semaphores for each queue and assigns the appropriate
 * acquire/release function pointers.
 *
 * Initialized semaphores include:
 * - Pending command queue lock
 * - Processed command queue lock
 * - RX event buffers queue lock
 * - RX precommit buffers queue lock
 * - Small packet buffer pool lock
 * - Medium packet buffer pool lock
 *
 * Each semaphore is configured with RAIL_USER_TryLockSemaphore for acquisition
 * and RAIL_USER_ReleaseSemaphore for release, providing consistent locking
 * behavior across all queue operations.
 *
 * @note This function must complete successfully before any queue operations
 *       can be performed safely in a multi-threaded environment
 */
void btc_semaphore_locks_init()
{
  RAIL_Status_t status = RAIL_USER_InitSemaphore(btc_rail_handle, &queue.pending_queue.plt_lock.lock);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint("RAIL_USER_InitSemaphore pending_queue", "Success:True");
  } else {
    responsePrintError("RAIL_USER_InitSemaphore pending_queue", status, "Unexpected error in RAIL_USER_InitSemaphore()");
    //error logs will be added and responseprinterror will be removed
  }

  status = RAIL_USER_InitSemaphore(btc_rail_handle, &queue.processed_queue.plt_lock.lock);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint("RAIL_USER_InitSemaphore processed_queue", "Success:True");
  } else {
    responsePrintError("RAIL_USER_InitSemaphore processed_queue", status, "Unexpected error in RAIL_USER_InitSemaphore()");
    //error logs will be added and responseprinterror will be removed
  }

  status = RAIL_USER_InitSemaphore(btc_rail_handle, &queue.rx_event_buffers_queue.plt_lock.lock);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint("RAIL_USER_InitSemaphore rx_event_buffers_queue", "Success:True");
  } else {
    responsePrintError("RAIL_USER_InitSemaphore rx_event_buffers_queue", status, "Unexpected error in RAIL_USER_InitSemaphore()");
    //error logs will be added and responseprinterror will be removed
  }

  status = RAIL_USER_InitSemaphore(btc_rail_handle, &queue.rx_precommit_buffers_queue.plt_lock.lock);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint("RAIL_USER_InitSemaphore rx_precommit_buffers_queue", "Success:True");
  } else {
    responsePrintError("RAIL_USER_InitSemaphore rx_precommit_buffers_queue", status, "Unexpected error in RAIL_USER_InitSemaphore()");
    //error logs will be added and responseprinterror will be removed
  }

  status = RAIL_USER_InitSemaphore(btc_rail_handle, &pkb_pool_g[pkb_chunk_type_small].pkb_queue.plt_lock.lock);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint("RAIL_USER_InitSemaphore pkb_pool_g[pkb_chunk_type_small].pkb_queue", "Success:True");
  } else {
    responsePrintError("RAIL_USER_InitSemaphore pkb_pool_g[pkb_chunk_type_small].pkb_queue", status, "Unexpected error in RAIL_USER_InitSemaphore()");
    //error logs will be added and responseprinterror will be removed
  }

  status = RAIL_USER_InitSemaphore(btc_rail_handle, &pkb_pool_g[pkb_chunk_type_medium].pkb_queue.plt_lock.lock);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint("RAIL_USER_InitSemaphore pkb_pool_g[pkb_chunk_type_medium].pkb_queue", "Success:True");
  } else {
    responsePrintError("RAIL_USER_InitSemaphore pkb_pool_g[pkb_chunk_type_medium].pkb_queue", status, "Unexpected error in RAIL_USER_InitSemaphore()");
    //error logs will be added and responseprinterror will be removed
  }

  pkb_pool_g[pkb_chunk_type_small].pkb_queue.plt_lock.acquire_lock = RAIL_USER_TryLockSemaphore;
  pkb_pool_g[pkb_chunk_type_small].pkb_queue.plt_lock.release_lock = RAIL_USER_ReleaseSemaphore;

  pkb_pool_g[pkb_chunk_type_medium].pkb_queue.plt_lock.acquire_lock = RAIL_USER_TryLockSemaphore;
  pkb_pool_g[pkb_chunk_type_medium].pkb_queue.plt_lock.release_lock = RAIL_USER_ReleaseSemaphore;

  queue.pending_queue.plt_lock.acquire_lock = RAIL_USER_TryLockSemaphore;
  queue.pending_queue.plt_lock.release_lock = RAIL_USER_ReleaseSemaphore;

  queue.processed_queue.plt_lock.acquire_lock = RAIL_USER_TryLockSemaphore;
  queue.processed_queue.plt_lock.release_lock = RAIL_USER_ReleaseSemaphore;

  queue.rx_event_buffers_queue.plt_lock.acquire_lock = RAIL_USER_TryLockSemaphore;
  queue.rx_event_buffers_queue.plt_lock.release_lock = RAIL_USER_ReleaseSemaphore;

  queue.rx_precommit_buffers_queue.plt_lock.acquire_lock = RAIL_USER_TryLockSemaphore;
  queue.rx_precommit_buffers_queue.plt_lock.release_lock = RAIL_USER_ReleaseSemaphore;
}
