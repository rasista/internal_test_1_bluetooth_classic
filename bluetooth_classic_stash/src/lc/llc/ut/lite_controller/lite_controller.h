#ifndef LITE_CONTROLLER_CLI_H
#define LITE_CONTROLLER_CLI_H

#include <string.h>
#include <stdio.h>

#include "sl_cli_handles.h"
#include "sl_cli_instances.h"
#include "sl_cli_storage_ram_instances.h"
#include "sl_component_catalog.h"
#include "sl_event_handler.h"
#include "sl_iostream_handles.h"
#include "sl_iostream_init_eusart_instances.h"
#include "sl_iostream_init_instances.h"
#include "sl_rail_util_init.h"
#include "response_print.h"
#include "btc_riscvseq_address_map.h"
#include "dmadrv.h"
#include "sl_host_to_lpw_interface.h"
#include "sl_rail_util_pti.h"
#include "rail.h"
#include "rail_seq.h"
#include "rail_seq_user_app_info.h"
#include "sl_hal_prs.h"
#include "sl_gpio.h"
#include "sl_clock_manager.h"
#include "pkb_mgmt.h"
#include "btc.h"
// #include "protocol_btc.h"
#include "sl_structs_btc_seq_interface.h"
#include "host_to_lpw_interface.h"
#include "stats.h"
#include "data_path.h"

extern uint8_t test_mode_buffer[256];
extern bool stats_check;
extern const RAIL_SeqUserAppInfo_t riscvseq_user_btc_info;

// Constants
#define STATS_CLI_VALUE 100
#define TRIGGER_CMD_WITH_DEFAULTS 99
#define TEST_ENCRYPTION_MODE_OFF 0
#define TEST_ENCRYPTION_MODE_E0  1
#define TEST_ENCRYPTION_MODE_AES 2
#define RX_FIFO_SIZE 2048
extern uint32_t BT_SLOT_WIDTH_HSS;

void PTI_EnableAuxdata(uint8_t enable);
void PTI_AuxdataOutput(uint8_t value);
void PTI_ConfigOutput(uint8_t sniffConfigs);

extern pkb_pool_t pkb_pool_g[num_supported_pkb_chunk_type];
extern hss_cmd_set_device_parameters_acl_t set_acl_parameters;
extern uint32_t end_tsf_acl;
extern uint8_t connection_device_index_central;
extern uint8_t connection_device_index_peripheral;

hss_cmd_stop_inquiry_t stop_inquiry;
hss_cmd_stop_inquiry_scan_t stop_inquiry_scan;

hss_cmd_stop_page_t stop_page;
hss_cmd_stop_page_scan_t stop_page_scan;

hss_cmd_stop_acl_central_t stop_acl_central;
hss_cmd_stop_acl_peripheral_t stop_acl_peripheral;

hss_cmd_set_E0_encryption_t e0EncConfig;
hss_cmd_set_AES_encryption_t aesEncConfig;

// Function declarations
void PTI_EnableAuxdata(uint8_t enable);
void PTI_AuxdataOutput(uint8_t value);
void PTI_ConfigOutput(uint8_t sniffConfigs);
void form_eir_data(uint8_t pkt_type, uint16_t pkt_len, hss_cmd_set_inquiry_scan_t* set_inquiry_scan);
void halEnablePrs(uint8_t channel, uint8_t loc, sl_gpio_t portPin, uint8_t source, uint8_t signal);

void btc_hello(sl_cli_command_arg_t *args);

/**
 * @brief Sends a command to the BTC rail handle via the hardware mailbox.
 *
 * This function sends a command to the BTC rail handle using the hardware mailbox.
 * It processes the command and writes it to the shared command queue for the Link Layer to process.
 * The function also prints the status of the command execution.
 *
 * @param[in] *args Pointer to the CLI command arguments
 * @param[in] *cmd Pointer to the command to be sent
 *
 * @note Uses the responsePrint system to report the status of the command execution
 */
void send_command(sl_cli_command_arg_t *args, uint32_t *cmd);

/**
 * @brief Initializes the BTC device and configures the necessary parameters.
 *
 * This function initializes the BTC device by setting up the BTC pools and configuring
 * the device parameters. It processes CLI inputs to determine whether to use default
 * parameters or user-provided parameters for the initialization. The function then
 * prepares and sends the initialization command to the LPW subsystem.
 *
 * @param[in] *args CLI command arguments:
 *                  - defaults_flag (uint32)
 *                  - bd_address (6 uint32, custom mode)
 *
 * Command Flow:
 * 1. BTC Pool Initialization
 *    - Initializes the BTC pools for small and medium packet buffers
 *
 * 2. Parameter Setup
 *    - Uses default BD address if defaults_flag is set
 *    - Otherwise uses user-provided BD address
 *
 * 3. Command Preparation
 *    - Prepares the initialization command with the configured parameters
 *
 * 4. Command Sending
 *    - Sends the initialization command to the LPW subsystem via RAIL
 *
 * @note Asynchronous processing via hardware mailbox
 * @note Status reported through responsePrint system
 *
 * Example:
 * Default: "init_device 99"
 * Custom: "init_device 11 22 33 44 55 66"
 */
void cli_init_device_handler(sl_cli_command_arg_t *args);

/**
 * For all set handlers:
 * @brief Configures Bluetooth parameters through the command processing pipeline.
 *
 * Processes CLI inputs (commands coming from host) to set Bluetooth parameters (inquiry, page, etc.)
 * with default or custom values. Part of the command execution chain that
 * prepares and transmits commands to the LPW subsystem.
 *
 * @param[in] *args CLI command arguments:
 *                  - defaults_flag or parameter values (uint32)
 *
 * Command Flow:
 * 1. Parameter Setup
 *    - Uses defaults if defaults_flag set
 *    - Otherwise uses user provided parameters
 *
 * 2. Command Processing
 *    - Calls set_*_cmd to initiate command execution
 *    - Command is prepared, queued, and sent to LPW via RAIL
 *
 * @note Asynchronous processing via hardware mailbox
 * @note Status reported through responsePrint system
 *
 * Example:
 * Default: "set_inquiry 99"
 * Custom: "set_inquiry 1 0 0 1 100 0x00 0x8B 0x9E"
 */
void set_command_parameters(void *cmd_struct, uint8_t priority, uint8_t step_size, uint32_t end_tsf);
void set_default_bd_address(uint8_t *bd_address);
void cli_set_inquiry_handler(sl_cli_command_arg_t *args);
void cli_set_inquiry_scan_handler(sl_cli_command_arg_t *args);
void cli_set_page_handler(sl_cli_command_arg_t *args);
void cli_set_page_scan_handler(sl_cli_command_arg_t *args);
void cli_set_device_parameters_acl(sl_cli_command_arg_t *args);
void cli_set_afh_parameters_acl(sl_cli_command_arg_t *args);
void cli_set_sniff_parameters_acl(sl_cli_command_arg_t *args);
void cli_set_device_parameters_esco(sl_cli_command_arg_t *args);
void cli_set_device_parameters_roleswitch(sl_cli_command_arg_t *args);
void cli_set_roleswitch_parameters(sl_cli_command_arg_t *args);
void cli_compute_roleswitch_slot_offset(sl_cli_command_arg_t *args);
void cli_trigger_roleswitch(sl_cli_command_arg_t *args);

/**
 * For all start handlers:
 * @brief Initiates a Bluetooth procedure through the command processing pipeline.
 *
 * Processes CLI inputs (commands coming from host) to start Bluetooth procedures (inquiry scan, page scan, etc.)
 * with default or custom parameters. Part of the command execution chain that
 * prepares and transmits commands to the LPW subsystem.
 *
 * @param[in] *args CLI command arguments:
 *                  - defaults_flag or parameter values (uint32)
 *
 * Command Flow:
 * 1. Parameter Setup
 *    - Uses defaults if defaults_flag set
 *    - Otherwise uses user provided parameters
 *
 * 2. Command Processing
 *    - Calls start_*_cmd to initiate command execution
 *    - Command is prepared, queued, and sent to LPW via RAIL
 *
 * @note Asynchronous processing via hardware mailbox
 * @note Status reported through responsePrint system
 *
 * Example:
 * Default: "start_inquiry_scan 99"
 * Custom: "start_inquiry_scan 1 1 8192"
 */
void cli_start_test_mode_handler(sl_cli_command_arg_t *args);
void cli_start_inquiry_handler(sl_cli_command_arg_t *args);
void cli_start_inquiry_scan_handler(sl_cli_command_arg_t *args);
void cli_start_page_handler(sl_cli_command_arg_t *args);
void cli_start_page_scan_handler(sl_cli_command_arg_t *args);
void cli_start_acl_central(sl_cli_command_arg_t *args);
void cli_start_acl_peripheral(sl_cli_command_arg_t *args);
void cli_start_esco_central(sl_cli_command_arg_t *args);
void cli_start_esco_peripheral(sl_cli_command_arg_t *args);
void cli_start_roleswitch_central(sl_cli_command_arg_t *args);
void cli_start_roleswitch_peripheral(sl_cli_command_arg_t *args);
void cli_stop_acl_central_handler(sl_cli_command_arg_t *args);
void cli_stop_acl_peripheral_handler(sl_cli_command_arg_t *args);
void set_btdm_logger(sl_cli_command_arg_t *args);
void set_lpw_hss_event_logger(sl_cli_command_arg_t *args);
void start_acl_traffic_pattern(sl_cli_command_arg_t *args);
void stop_acl_traffic_pattern(sl_cli_command_arg_t *args);
void set_fixed_btc_channel_handler(sl_cli_command_arg_t *args);

void set_pti_auxdata(sl_cli_command_arg_t *args);
void btc_app_init(void);
void btc_app_init_register_cbs(void);
void btc_semaphore_locks_init();
#endif // LITE_CONTROLLER_CLI_H
