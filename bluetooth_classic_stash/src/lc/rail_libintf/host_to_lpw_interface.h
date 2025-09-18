#ifndef HOST_TO_LPW_INTF_H_
#define HOST_TO_LPW_INTF_H_

#include "sl_btc_seq_interface.h"
#include "stdint.h"
#include "stddef.h"
#include "sl_host_to_lpw_interface.h"
#include "sl_structs_btc_seq_interface.h"
#include "pkb_mgmt_struct.h"

#define BTC_LPW_RX_METADATA_SIZE 4

#define ENCRYPTION_MODE_OFF 0
#define ENCRYPTION_MODE_E0  1
#define ENCRYPTION_MODE_AES 2

typedef union btc_rx_meta_data_u {
  struct {
    uint8_t event_type;
    uint8_t rssi;
    uint8_t conn_id;
    uint8_t rfu;
  } rx_pkt_meta_data;
  uint32_t data;
} btc_rx_meta_data_t;

typedef enum btc_llc_to_ulc_event_id_s {
  BTC_LLC_TO_ULC_DMA_TRANSFER_DONE = 0,
  BTC_LLC_TO_ULC_CMD_PROCESSED_EVENT,
  BTC_LLC_TO_ULC_INTERNAL_EVENT
}btc_llc_to_ulc_event_id_t;

typedef enum btc_internal_event_type_s {
  BTC_INQUIRY_COMPLETE = 0,
  BTC_INQUIRY_SCAN_COMPLETE,
  BTC_PAGE_COMPLETE,
  BTC_PAGE_SCAN_COMPLETE,
  BTC_ACTIVITY_COMPLETE,
  BTC_ROLE_SWITCH_COMPLETE,
  BTC_TX_PKT_ACK_RECEIVED,
  BTC_DATA_PKT_RECEIVED,
  BTC_CLK_SLOT_OFFSET_NOTIFICATION,
  BTC_MAX_INTERNAL_EVENT_TYPES
}btc_internal_event_type_t;

typedef enum btc_internal_event_status_s {
  BTC_ACTIVITY_DURATION_COMPLETED = 0,
  BTC_INQUIRY_EIR_TIMEOUT,    // EIR timeout
  BTC_CONNECTION_CREATED,    // FHS and ID packet exchange is completed
  BTC_CONNECTION_RESP_TOUT,  // FHS and ID packet exchange is timed out
  BTC_NEW_CONNECTION_TOUT,   // New connection timed out for POLL and NULL packet exchange
  BTC_CONNECTION_COMPLETED,  // Connection completed, POLL and NULL packet exchange is completed
  BTC_DATA_PKT_RECEIVED_SUCCESS
}btc_internal_event_status_t;

/**
 * @brief Command Processing Flow in Bluetooth Stack
 *
 * This describes the generic flow of commands from Host Stack Software (HSS) to Link Layer Processing Wrapper (LPW):
 *
 * 1. Command Initiation
 *    - High-level command functions (e.g., set_*_cmd, start_*_cmd, stop_*_cmd) are called with procedure-specific parameters
 *    - These functions invoke execute_command() with appropriate command preparation function
 *
 * 2. Command Execution (execute_command)
 *    - Allocates a packet buffer (pkb) for the command
 *    - Calls procedure-specific preparation function (prep_hss_cmd_*) to format command
 *    - Forwards prepared command to LPW via hss_command_to_lpw()
 *
 * 3. Command Preparation (prep_hss_cmd_*)
 *    - Sets common command fields (command type, procedure ID)
 *    - Populates procedure-specific parameters in command buffer
 *    - Ensures proper formatting of command for LPW processing
 *
 * 4. Command Queuing (hss_command_to_lpw)
 *    - Enqueues formatted command to pending queue
 *    - Signals LPW task about pending command (via semaphore)
 *    - Transfers command to LPW via RAIL mailbox interface
 *
 * 5. Command Completion (lpw_command_processed_handler)
 *    - Processes commands completed by LPW
 *    - Dequeues from processed queue
 *    - Signals command completion to host
 *    - Frees associated packet buffer
 *
 * Flow Example:
 * HSS Command → execute_command → Command Preparation → Command Queuing → LPW Processing → Completion Handling
 *
 * @note This flow is consistent across all Bluetooth procedures (inquiry, page, scans, etc.)
 *       with only the specific parameters and preparation functions varying by procedure type.
 */
extern shared_mem_t host_to_lpw_command_queue;

/**
 * @brief Helper function to set common fields in HSS command buffer.
 *
 * Sets the command type and procedure ID fields that are common to all
 * HSS commands. This ensures consistent command formatting across all
 * procedure types.
 *
 * @param[out] *hss_cmd Pointer to command buffer where fields will be written
 * @param[in] cmd_type Type of command (set/start/stop procedure)
 * @param[in] procedure_id Identifier for the specific procedure
 */
void set_common_hss_cmd_fields(uint8_t *hss_cmd, uint8_t cmd_type, uint8_t procedure_id);

/**
 * @brief Command preparation functions for setting Bluetooth procedure parameters.
 *
 * These functions prepare command buffers with specific parameters for various
 * Bluetooth procedures. Each set command configures the necessary parameters
 * before the procedure can be started.
 *
 * @param[out] *hss_cmd Pointer to command buffer where parameters will be written
 * @param[in] *_args Pointer to structure containing procedure-specific parameters
 *
 * @note Uses common field setting for command type and procedure ID
 */
void prep_hss_cmd_init_btc_device(uint8_t *hss_cmd, hss_cmd_init_btc_device_t *init_btc_device_args);
void prep_hss_cmd_set_page(uint8_t *hss_cmd, hss_cmd_set_page_t *set_page_args);
void prep_hss_cmd_set_page_scan(uint8_t *hss_cmd, hss_cmd_set_page_scan_t *set_page_scan_args);
void prep_hss_cmd_set_inquiry(uint8_t *hss_cmd, hss_cmd_set_inquiry_t *set_inquiry_args);
void prep_hss_cmd_set_inquiry_scan(uint8_t *hss_cmd, hss_cmd_set_inquiry_scan_t *set_inquiry_scan_args);
void prep_hss_cmd_set_afh_parameters_acl(uint8_t *hss_cmd, hss_cmd_set_afh_parameters_acl_t *set_afh_parameters_acl_args);
void prep_hss_cmd_set_sniff_parameters_acl(uint8_t *hss_cmd, hss_cmd_set_sniff_parameters_acl_t *set_sniff_parameters_acl_args);
void prep_hss_cmd_set_device_parameters_acl(uint8_t *hss_cmd, hss_cmd_set_device_parameters_acl_t *set_device_parameters_acl_args);
void prep_hss_cmd_set_device_parameters_esco(uint8_t *hss_cmd, hss_cmd_set_device_parameters_esco_t *set_device_parameters_esco);
void prep_hss_cmd_set_device_parameters_roleswitch(uint8_t *hss_cmd, hss_cmd_set_device_parameters_roleswitch_t *set_device_parameters_roleswitch);
void prep_hss_cmd_set_roleswitch_parameters(uint8_t *hss_cmd, hss_cmd_set_roleswitch_parameters_t *set_roleswitch_parameters);

/**
 * @brief Command preparation functions for starting Bluetooth procedures.
 *
 * These functions prepare command buffers to initiate Bluetooth procedures.
 * Start commands include parameters that control how the procedure will operate
 * once started.
 *
 * @param[out] *hss_cmd Pointer to command buffer where parameters will be written
 * @param[in] *_args Pointer to structure containing procedure-specific parameters
 *
 * @note Uses common field setting for command type and procedure ID
 */
void prep_hss_cmd_start_page(uint8_t *hss_cmd, hss_cmd_start_page_t *start_page_args);
void prep_hss_cmd_start_page_scan(uint8_t *hss_cmd, hss_cmd_start_page_scan_t *start_page_scan_args);
void prep_hss_cmd_start_inquiry(uint8_t *hss_cmd, hss_cmd_start_inquiry_t *start_inquiry_args);
void prep_hss_cmd_start_inquiry_scan(uint8_t *hss_cmd, hss_cmd_start_inquiry_scan_t *start_inquiry_scan_args);
void prep_hss_cmd_start_acl_central(uint8_t *hss_cmd, hss_cmd_start_acl_central_t *start_acl_args);
void prep_hss_cmd_start_acl_peripheral(uint8_t *hss_cmd, hss_cmd_start_acl_peripheral_t *start_acl_args);
void prep_hss_cmd_start_esco_central(uint8_t *hss_cmd, hss_cmd_start_esco_central_t *start_esco_args);
void prep_hss_cmd_start_esco_peripheral(uint8_t *hss_cmd, hss_cmd_start_esco_peripheral_t *start_esco_args);
void prep_hss_cmd_start_roleswitch_central(uint8_t *hss_cmd, hss_cmd_start_roleswitch_central_t *start_roleswitch_args);
void prep_hss_cmd_start_roleswitch_peripheral(uint8_t *hss_cmd, hss_cmd_start_roleswitch_peripheral_t *start_roleswitch_args);
void prep_hss_cmd_start_test_mode(uint8_t *hss_cmd, hss_cmd_start_test_mode_t *start_test_mode_args);
void prep_hss_cmd_compute_slot_offset_event(uint8_t *hss_cmd, hss_event_btc_slot_offset_t *btc_slot_offset_event_args);

/**
 * @brief Command preparation functions for stopping Bluetooth procedures.
 *
 * These functions prepare command buffers to stop ongoing Bluetooth procedures.
 * Stop commands typically include minimal parameters needed to identify and
 * terminate the specific procedure instance.
 *
 * @param[out] *hss_cmd Pointer to command buffer where parameters will be written
 * @param[in] *_args Pointer to structure containing procedure-specific parameters
 *
 * @note Uses common field setting for command type and procedure ID
 */
void prep_hss_cmd_stop_page(uint8_t *hss_cmd, hss_cmd_stop_page_t *stop_page_args);
void prep_hss_cmd_stop_inquiry(uint8_t *hss_cmd, hss_cmd_stop_inquiry_t *stop_inquiry_args);
void prep_hss_cmd_stop_page_scan(uint8_t *hss_cmd, hss_cmd_stop_page_scan_t *stop_page_scan_args);
void prep_hss_cmd_stop_inquiry_scan(uint8_t *hss_cmd, hss_cmd_stop_inquiry_scan_t *stop_inquiry_scan_args);
void prep_hss_cmd_stop_acl_central(uint8_t *hss_cmd, hss_cmd_stop_acl_central_t *stop_acl_args);
void prep_hss_cmd_stop_acl_peripheral(uint8_t *hss_cmd, hss_cmd_stop_acl_peripheral_t *stop_acl_args);
void prep_hss_cmd_stop_esco_central(uint8_t *hss_cmd, hss_cmd_stop_esco_central_t *stop_esco_args);
void prep_hss_cmd_stop_esco_peripheral(uint8_t *hss_cmd, hss_cmd_stop_esco_peripheral_t *stop_esco_args);
void prep_hss_cmd_stop_roleswitch_central(uint8_t *hss_cmd, hss_cmd_stop_roleswitch_central_t *stop_roleswitch_args);
void prep_hss_cmd_stop_roleswitch_peripheral(uint8_t *hss_cmd, hss_cmd_stop_roleswitch_peripheral_t *stop_roleswitch_args);

/**
 * @brief Enqueues a command to the LPW pending queue for processing.
 *
 * This function adds a command to the pending queue and signals LPW to process it.
 * The command is sent via RAIL mailbox interface for LPW processing.
 *
 * @param[in] pkb Pointer to the packet buffer containing the command
 * @param[in] hss_to_lpw_command_queue Pointer to shared memory structure for command queuing
 */
void hss_command_to_lpw(pkb_t *pkb, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Handles processed commands from LPW and frees associated resources.
 *
 * Dequeues all processed commands from the LPW processed queue and frees their
 * packet buffers. Implements cleanup of completed command resources.
 *
 * @param[in] lpw_cdq Pointer to shared memory structure containing processed commands
 */

void lpw_command_processed_handler(shared_mem_t *lpw_cdq);

/**
 * @brief Generic command execution function for preparing and sending commands to LPW.
 *
 * This function serves as a common path for command execution:
 * 1. Allocates a packet buffer
 * 2. Prepares the command using the provided preparation function
 * 3. Sends the command to LPW for processing
 *
 * @param[in] prep_cmd_func Function pointer to command-specific preparation function
 * @param[in] cmd_handler Pointer to command-specific handler structure
 * @param[in] hss_to_lpw_command_queue Pointer to shared memory structure for command queuing
 */
void execute_command(void (*prep_cmd_func)(uint8_t *, void *), void *cmd_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Initializes the Bluetooth Classic device with specified parameters.
 *
 * Prepares and sends an initialization command to LPW using the common command
 * execution path. This function specifically handles device initialization parameters.
 *
 * @param[in] init_btc_device Pointer to structure containing device initialization parameters
 * @param[in] hss_to_lpw_command_queue Pointer to shared memory structure for command queuing
 */
void init_device_cmd(hss_cmd_init_btc_device_t *init_btc_device, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting inquiry parameters.
 *
 * This function processes the command to configure inquiry parameters
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_inquiry_handler Pointer to the command structure containing inquiry parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_inquiry_cmd(hss_cmd_set_inquiry_t *set_inquiry_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting device parameters for ACL.
 *
 * This function processes the command to configure device parameters for ACL
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_device_parameters_acl_handler Pointer to the command structure containing ACL device parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_device_parameters_acl_cmd(hss_cmd_set_device_parameters_acl_t *set_device_parameters_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting AFH parameters for ACL.
 *
 * This function processes the command to configure AFH parameters for ACL
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_afh_parameters_acl_handler Pointer to the command structure containing AFH parameters for ACL
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_afh_parameters_acl_cmd(hss_cmd_set_afh_parameters_acl_t *set_afh_parameters_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting sniff parameters for ACL.
 *
 * This function processes the command to configure sniff parameters for ACL
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_sniff_parameters_acl_handler Pointer to the command structure containing sniff parameters for ACL
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_sniff_parameters_acl_cmd(hss_cmd_set_sniff_parameters_acl_t *set_sniff_parameters_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting device parameters for role switch.
 *
 * This function processes the command to configure device parameters for role switch
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_device_parameters_roleswitch_handler Pointer to the command structure containing role switch parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_device_parameters_roleswitch_cmd(hss_cmd_set_device_parameters_roleswitch_t *set_device_parameters_roleswitch_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting role switch parameters.
 *
 * This function processes the command to configure role switch parameters
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_roleswitch_parameters_handler Pointer to the command structure containing role switch parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_roleswitch_parameters_cmd(hss_cmd_set_roleswitch_parameters_t *set_roleswitch_parameters_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting device parameters for eSCO.
 *
 * This function processes the command to configure device parameters for eSCO
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_device_parameters_esco_handler Pointer to the command structure containing eSCO device parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_device_parameters_esco_cmd(hss_cmd_set_device_parameters_esco_t *set_device_parameters_esco_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting inquiry scan parameters.
 *
 * This function processes the command to configure inquiry scan parameters
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_inquiry_scan_handler Pointer to the command structure containing inquiry scan parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_inquiry_scan_cmd(hss_cmd_set_inquiry_scan_t *set_inquiry_scan_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting page parameters.
 *
 * This function processes the command to configure page parameters
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_page_handler Pointer to the command structure containing page parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_page_cmd(hss_cmd_set_page_t *set_page_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for setting page scan parameters.
 *
 * This function processes the command to configure page scan parameters
 * and writes them to the shared command queue for the Link Layer to process.
 *
 * @param[in] *set_page_scan_handler Pointer to the command structure containing page scan parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void set_page_scan_cmd(hss_cmd_set_page_scan_t *set_page_scan_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the inquiry procedure.
 *
 * This function processes the command to start the inquiry procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_inquiry_handler Pointer to the command structure containing inquiry parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_inquiry_cmd(hss_cmd_start_inquiry_t *start_inquiry_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the eSCO central procedure.
 *
 * This function processes the command to start the eSCO central procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_esco_handler Pointer to the command structure containing eSCO parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_esco_central_cmd(hss_cmd_start_esco_central_t *start_esco_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the eSCO peripheral procedure.
 *
 * This function processes the command to start the eSCO peripheral procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_esco_handler Pointer to the command structure containing eSCO parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_esco_peripheral_cmd(hss_cmd_start_esco_peripheral_t *start_esco_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the role switch central procedure.
 *
 * This function processes the command to start the role switch central procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_roleswitch_handler Pointer to the command structure containing role switch parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_roleswitch_central_cmd(hss_cmd_start_roleswitch_central_t *start_roleswitch_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the role switch peripheral procedure.
 *
 * This function processes the command to start the role switch peripheral procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_roleswitch_handler Pointer to the command structure containing role switch parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_roleswitch_peripheral_cmd(hss_cmd_start_roleswitch_peripheral_t *start_roleswitch_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the ACL central procedure.
 *
 * This function processes the command to start the ACL central procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_acl_handler Pointer to the command structure containing ACL parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_acl_central_cmd(hss_cmd_start_acl_central_t *start_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the ACL peripheral procedure.
 *
 * This function processes the command to start the ACL peripheral procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_acl_handler Pointer to the command structure containing ACL parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_acl_peripheral_cmd(hss_cmd_start_acl_peripheral_t *start_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the page procedure.
 *
 * This function processes the command to start the page procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_page_handler Pointer to the command structure containing page parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_page_cmd(hss_cmd_start_page_t *start_page_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the inquiry scan procedure.
 *
 * This function processes the command to start the inquiry scan procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_inquiry_scan_handler Pointer to the command structure containing inquiry scan parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_inquiry_scan_cmd(hss_cmd_start_inquiry_scan_t *start_inquiry_scan_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the page scan procedure.
 *
 * This function processes the command to start the page scan procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_page_scan_handler Pointer to the command structure containing page scan parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_page_scan_cmd(hss_cmd_start_page_scan_t *start_page_scan_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for stopping the ACL central procedure.
 *
 * This function processes the command to stop the ACL central procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *stop_acl_handler Pointer to the command structure containing ACL parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void stop_acl_central_cmd(hss_cmd_stop_acl_central_t *stop_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for stopping the ACL peripheral procedure.
 *
 * This function processes the command to stop the ACL peripheral procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *stop_acl_handler Pointer to the command structure containing ACL parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void stop_acl_peripheral_cmd(hss_cmd_stop_acl_peripheral_t *stop_acl_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for stopping the inquiry procedure.
 *
 * This function processes the command to stop the inquiry procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *stop_inquiry_handler Pointer to the command structure containing inquiry parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void stop_inquiry_cmd(hss_cmd_stop_inquiry_t *stop_inquiry_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for stopping the page procedure.
 *
 * This function processes the command to stop the page procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *stop_page_handler Pointer to the command structure containing page parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void stop_page_cmd(hss_cmd_stop_page_t *stop_page_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for stopping the inquiry scan procedure.
 *
 * This function processes the command to stop the inquiry scan procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *stop_inquiry_scan_handler Pointer to the command structure containing inquiry scan parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void stop_inquiry_scan_cmd(hss_cmd_stop_inquiry_scan_t *stop_inquiry_scan_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for stopping the page scan procedure.
 *
 * This function processes the command to stop the page scan procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *stop_page_scan_handler Pointer to the command structure containing page scan parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void stop_page_scan_cmd(hss_cmd_stop_page_scan_t *stop_page_scan_handler, shared_mem_t *hss_to_lpw_command_queue);

/**
 * @brief Command handler for starting the test mode procedure.
 *
 * This function processes the command to start the test mode procedure
 * and writes it to the shared command queue for the Link Layer to process.
 *
 * @param[in] *start_test_mode_handler Pointer to the command structure containing test mode parameters
 * @param[in] *hss_to_lpw_command_queue Pointer to the shared command queue between HSS and LPW
 */
void start_test_mode_cmd(hss_cmd_start_test_mode_t *start_test_mode_handler, shared_mem_t *hss_to_lpw_command_queue);
void set_test_mode_params_cmd(hss_cmd_set_test_mode_params_t *set_test_mode_handler, shared_mem_t *hss_to_lpw_command_queue);

void prep_hss_cmd_set_e0_encryption(uint8_t *hss_cmd, hss_cmd_set_E0_encryption_t *set_e0_encryption_args);
void prep_hss_cmd_set_aes_encryption(uint8_t *hss_cmd, hss_cmd_set_AES_encryption_t *set_aes_encryption_args);
void set_e0_encryption_cmd(hss_cmd_set_E0_encryption_t *set_e0_encryption_handler, shared_mem_t *hss_to_lpw_command_queue);
void set_aes_encryption_cmd(hss_cmd_set_AES_encryption_t *set_aes_encryption_handler, shared_mem_t *hss_to_lpw_command_queue);
void compute_slot_offset_event_cmd(hss_event_btc_slot_offset_t *btc_slot_offset_event_handler, shared_mem_t *hss_to_lpw_command_queue);
#endif
