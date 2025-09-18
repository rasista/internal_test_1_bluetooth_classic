# LMP Connection Management Protocol Functions

## Overview

The Link Manager Protocol (LMP) Connection Management is a critical component of the Bluetooth Classic stack that handles the establishment, maintenance, and teardown of Bluetooth connections. This document provides a comprehensive overview of the connection management functions implemented in the LMP layer.

## Table of Contents

1. [Connection Management Overview](#connection-management-overview)
2. [Connection States and Lifecycle](#connection-states-and-lifecycle)
3. [Connection Establishment](#connection-establishment)
4. [Connection Parameter Negotiation](#connection-parameter-negotiation)
5. [Link Supervision and Monitoring](#link-supervision-and-monitoring)
6. [Connection Teardown](#connection-teardown)
7. [State Machine Management](#state-machine-management)
8. [Error Handling and Recovery](#error-handling-and-recovery)
9. [API Functions](#api-functions)
10. [Configuration and Parameters](#configuration-and-parameters)

## Connection Management Overview

### Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                LMP Connection Management                    │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ Connection  │  │ Parameter   │  │ Link        │         │
│  │ Establishment│  │ Negotiation │  │ Supervision │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              State Machine Engine                      │ │
│  │  • Connection State Tracking                          │ │
│  │  • Event Processing                                   │ │
│  │  • State Transitions                                  │ │
│  └─────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              LMP Protocol Handler                      │ │
│  │  • Packet Processing                                   │ │
│  │  • Command/Response Handling                           │ │
│  │  • Error Management                                    │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Key Components

- **Connection State Machine**: Manages connection lifecycle states
- **Parameter Negotiation**: Handles connection parameter exchange
- **Link Supervision**: Monitors connection health and quality
- **Error Recovery**: Manages connection failures and recovery

## Connection States and Lifecycle

### Connection State Definitions

```c
// Connection states in LMP
typedef enum {
    HCI_IO_CAP_REQ_SENT_STATE,           // IO Capability request sent
    LMP_IO_CAP_REQ_SENT_STATE,           // LMP IO Capability request sent
    LMP_ENCAP_HEADER_SENT_STATE,         // Encapsulated header sent
    NAME_REQ_SENT_STATE,                 // Name request sent
    VERSION_REQ_SENT_STATE,              // Version request sent
    FEATURES_REQ_SENT_STATE,             // Features request sent
    CLKOFFSET_REQ_SENT_STATE,            // Clock offset request sent
    SETUP_COMPLETE_STATE,                // Setup complete
    CONNECTION_ESTABLISHED_STATE,        // Connection established
    CONNECTION_TERMINATED_STATE          // Connection terminated
} lmp_connection_state_t;
```

### Connection Lifecycle Flow

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Idle      │───▶│ Establishing│───▶│ Established │───▶│ Terminating │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
       ▲                   │                   │                   │
       │                   ▼                   ▼                   ▼
       └─────────────────────────────────────────────────────────────┘
                              │
                              ▼
                       ┌─────────────┐
                       │   Failed    │
                       └─────────────┘
```

## Connection Establishment

### 1. Initial Connection Setup

The connection establishment process involves several key steps:

#### **Step 1: IO Capability Exchange**
```c
// IO Capability request handling
void lmp_io_capabilities_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
    uint8_t peer_id = GET_PEER_ID(pkb);
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
    
    // Process IO capabilities
    if (acl_link_mgmt_info->lmp_connection_state == HCI_IO_CAP_REQ_SENT_STATE) {
        acl_link_mgmt_info->lmp_connection_state = LMP_IO_CAP_REQ_SENT_STATE;
        // Send IO capability response
        lmp_send_io_capabilities_res(ulc, peer_id);
    }
}
```

#### **Step 2: Device Name Exchange**
```c
// Device name request handling
void lmp_name_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
    uint8_t peer_id = GET_PEER_ID(pkb);
    uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
    uint8_t name_offset = GET_LMP_NAME_REQ_PARAMETER_NAME_OFFSET(bt_pkt_start);
    uint8_t tid = GET_LMP_NAME_REQ_TID(bt_pkt_start);
    
    // Validate name offset and send response
    if (name_offset < device_name_len) {
        lmp_send_name_res(ulc, peer_id, tid, name_offset);
    } else {
        lmp_send_not_accepted(ulc, peer_id, tid, LMP_NAME_RES);
    }
}
```

#### **Step 3: Version Information Exchange**
```c
// Version request handling
void lmp_version_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
    uint8_t peer_id = GET_PEER_ID(pkb);
    uint8_t tid = GET_LMP_VERSION_REQ_TID(pkb->data);
    
    // Send version response with device information
    lmp_send_version_res(ulc, peer_id, tid);
}
```

#### **Step 4: Feature Exchange**
```c
// Features request handling
void lmp_features_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
    uint8_t peer_id = GET_PEER_ID(pkb);
    uint8_t tid = GET_LMP_FEATURES_REQ_TID(pkb->data);
    
    // Send supported features
    lmp_send_features_res(ulc, peer_id, tid);
}
```

### 2. Connection State Processing

```c
// Main connection state processing function
uint8_t lmp_process_ssp_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
    
    switch (acl_link_mgmt_info->lmp_connection_state) {
        case HCI_IO_CAP_REQ_SENT_STATE:
            // Handle IO capability exchange
            if (lmp_recvd_pkt_flag == IO_CAPABILITIES_REPLY) {
                acl_link_mgmt_info->lmp_connection_state = LMP_IO_CAP_REQ_SENT_STATE;
                // Continue with next step
            }
            break;
            
        case LMP_IO_CAP_REQ_SENT_STATE:
            // Handle IO capability response
            if (lmp_recvd_pkt_flag == IO_CAP_RES_RCVD) {
                // Send event to host
                hci_io_cap_response_event(ulc, peer_id);
                // Continue with authentication
            }
            break;
            
        case LMP_ENCAP_HEADER_SENT_STATE:
            // Handle encapsulated header
            if (lmp_recvd_pkt_flag == ENCAP_HEADER_ACCEPTED) {
                // Continue with key exchange
            }
            break;
            
        // Additional states...
    }
    
    return 0;
}
```

## Connection Parameter Negotiation

### 1. Clock Offset Negotiation

```c
// Clock offset request handling
void lmp_clkoffset_req_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
    uint8_t peer_id = GET_PEER_ID(pkb);
    uint8_t tid = GET_LMP_CLKOFFSET_REQ_TID(pkb->data);
    
    // Calculate clock offset
    uint16_t clock_offset = calculate_clock_offset(ulc, peer_id);
    
    // Send clock offset response
    lmp_send_clkoffset_res(ulc, peer_id, tid, clock_offset);
}
```

### 2. Connection Parameter Structure

```c
// Connection parameters structure
typedef struct {
    uint32_t connection_handle;        // Connection handle
    uint16_t clock_offset;            // Clock offset
    uint8_t role;                     // Central/Peripheral role
    uint8_t mode;                     // Connection mode
    uint32_t connection_timeout;      // Connection timeout
    uint8_t encryption_mode;          // Encryption mode
    uint8_t key_size;                 // Encryption key size
} connection_params_t;
```

### 3. Parameter Validation

```c
// Validate connection parameters
bool validate_connection_params(connection_params_t *params)
{
    // Validate clock offset
    if (params->clock_offset > MAX_CLOCK_OFFSET) {
        return false;
    }
    
    // Validate role
    if (params->role != BT_CENTRAL_MODE && params->role != BT_PERIPHERAL_MODE) {
        return false;
    }
    
    // Validate encryption parameters
    if (params->encryption_mode > MAX_ENCRYPTION_MODE) {
        return false;
    }
    
    return true;
}
```

## Link Supervision and Monitoring

### 1. Link Supervision Timer

```c
// Link supervision timer management
typedef struct {
    uint32_t supervision_timeout;     // Supervision timeout value
    uint32_t last_activity_time;      // Last activity timestamp
    uint32_t supervision_counter;     // Supervision counter
    bool supervision_active;          // Supervision active flag
} link_supervision_t;

// Start link supervision
void start_link_supervision(ulc_t *ulc, uint8_t peer_id)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    link_supervision_t *supervision = &acl_peer_info->link_supervision;
    
    supervision->supervision_timeout = DEFAULT_SUPERVISION_TIMEOUT;
    supervision->last_activity_time = get_current_time();
    supervision->supervision_active = true;
    supervision->supervision_counter = 0;
}
```

### 2. Connection Quality Monitoring

```c
// Connection quality monitoring
typedef struct {
    uint32_t packet_loss_count;       // Packet loss counter
    uint32_t total_packets;           // Total packet counter
    uint32_t retransmission_count;    // Retransmission counter
    uint32_t error_count;             // Error counter
    float connection_quality;         // Connection quality score
} connection_quality_t;

// Update connection quality
void update_connection_quality(ulc_t *ulc, uint8_t peer_id, bool packet_success)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    connection_quality_t *quality = &acl_peer_info->connection_quality;
    
    quality->total_packets++;
    
    if (!packet_success) {
        quality->packet_loss_count++;
    }
    
    // Calculate quality score
    quality->connection_quality = (float)(quality->total_packets - quality->packet_loss_count) / quality->total_packets;
    
    // Trigger quality-based actions
    if (quality->connection_quality < QUALITY_THRESHOLD) {
        trigger_connection_optimization(ulc, peer_id);
    }
}
```

### 3. Link Supervision Timeout Handling

```c
// Handle link supervision timeout
void handle_link_supervision_timeout(ulc_t *ulc, uint8_t peer_id)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    link_supervision_t *supervision = &acl_peer_info->link_supervision;
    
    supervision->supervision_counter++;
    
    if (supervision->supervision_counter >= MAX_SUPERVISION_RETRIES) {
        // Connection lost - initiate detach
        lmp_send_detach(ulc, peer_id, LINK_LOSS_ERROR);
        acl_peer_info->connection_state = CONNECTION_TERMINATED_STATE;
    } else {
        // Retry supervision
        restart_link_supervision(ulc, peer_id);
    }
}
```

## Connection Teardown

### 1. Detach Procedure

```c
// LMP Detach handling
void lmp_detach_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
    uint8_t peer_id = GET_PEER_ID(pkb);
    uint8_t *bt_pkt_start = &GET_RX_BT_PKT(pkb);
    uint8_t error_code = GET_LMP_DETACH_PARAMETER_ERROR_CODE(bt_pkt_start);
    
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    // Update connection state
    acl_peer_info->connection_state = CONNECTION_TERMINATED_STATE;
    
    // Clean up resources
    cleanup_connection_resources(ulc, peer_id);
    
    // Notify host
    hci_disconnection_complete_event(ulc, peer_id, error_code);
}
```

### 2. Connection Cleanup

```c
// Clean up connection resources
void cleanup_connection_resources(ulc_t *ulc, uint8_t peer_id)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    // Stop link supervision
    stop_link_supervision(ulc, peer_id);
    
    // Clear pending LMP transactions
    clear_pending_lmp_transactions(ulc, peer_id);
    
    // Reset connection parameters
    reset_connection_parameters(acl_peer_info);
    
    // Free allocated resources
    free_connection_buffers(acl_peer_info);
    
    // Update device management
    update_device_management_state(ulc, peer_id, DEVICE_DISCONNECTED);
}
```

### 3. Graceful Disconnection

```c
// Initiate graceful disconnection
void initiate_graceful_disconnection(ulc_t *ulc, uint8_t peer_id, uint8_t reason)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    // Set disconnection state
    acl_peer_info->connection_state = CONNECTION_TERMINATING_STATE;
    
    // Send LMP detach
    lmp_send_detach(ulc, peer_id, reason);
    
    // Start disconnection timer
    start_disconnection_timer(ulc, peer_id);
}
```

## State Machine Management

### 1. State Machine Structure

```c
// Connection state machine
typedef struct {
    lmp_connection_state_t current_state;    // Current state
    lmp_connection_state_t previous_state;   // Previous state
    uint32_t state_entry_time;              // State entry time
    uint32_t state_timeout;                 // State timeout
    bool state_timeout_active;              // State timeout active
} connection_state_machine_t;
```

### 2. State Transition Management

```c
// State transition function
void transition_connection_state(ulc_t *ulc, uint8_t peer_id, lmp_connection_state_t new_state)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    connection_state_machine_t *state_machine = &acl_peer_info->state_machine;
    
    // Validate state transition
    if (!is_valid_state_transition(state_machine->current_state, new_state)) {
        // Handle invalid transition
        handle_invalid_state_transition(ulc, peer_id, new_state);
        return;
    }
    
    // Update state machine
    state_machine->previous_state = state_machine->current_state;
    state_machine->current_state = new_state;
    state_machine->state_entry_time = get_current_time();
    
    // Start state timeout
    start_state_timeout(ulc, peer_id, new_state);
    
    // Execute state entry actions
    execute_state_entry_actions(ulc, peer_id, new_state);
}
```

### 3. State Timeout Handling

```c
// Handle state timeout
void handle_state_timeout(ulc_t *ulc, uint8_t peer_id)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    connection_state_machine_t *state_machine = &acl_peer_info->state_machine;
    
    // Handle timeout based on current state
    switch (state_machine->current_state) {
        case HCI_IO_CAP_REQ_SENT_STATE:
            // IO capability request timeout
            handle_io_cap_timeout(ulc, peer_id);
            break;
            
        case LMP_IO_CAP_REQ_SENT_STATE:
            // LMP IO capability request timeout
            handle_lmp_io_cap_timeout(ulc, peer_id);
            break;
            
        case NAME_REQ_SENT_STATE:
            // Name request timeout
            handle_name_req_timeout(ulc, peer_id);
            break;
            
        default:
            // Generic timeout handling
            handle_generic_timeout(ulc, peer_id);
            break;
    }
}
```

## Error Handling and Recovery

### 1. Error Types and Handling

```c
// LMP error codes
typedef enum {
    LMP_ERROR_SUCCESS = 0,
    LMP_ERROR_INVALID_PARAMETERS,
    LMP_ERROR_NOT_ACCEPTED,
    LMP_ERROR_AUTHENTICATION_FAILURE,
    LMP_ERROR_ENCRYPTION_FAILURE,
    LMP_ERROR_LINK_LOSS,
    LMP_ERROR_TIMEOUT,
    LMP_ERROR_RESOURCE_UNAVAILABLE
} lmp_error_code_t;

// Handle LMP error
void handle_lmp_error(ulc_t *ulc, uint8_t peer_id, lmp_error_code_t error_code)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    switch (error_code) {
        case LMP_ERROR_AUTHENTICATION_FAILURE:
            // Handle authentication failure
            handle_auth_failure(ulc, peer_id);
            break;
            
        case LMP_ERROR_ENCRYPTION_FAILURE:
            // Handle encryption failure
            handle_encryption_failure(ulc, peer_id);
            break;
            
        case LMP_ERROR_LINK_LOSS:
            // Handle link loss
            handle_link_loss(ulc, peer_id);
            break;
            
        case LMP_ERROR_TIMEOUT:
            // Handle timeout
            handle_connection_timeout(ulc, peer_id);
            break;
            
        default:
            // Handle generic error
            handle_generic_error(ulc, peer_id, error_code);
            break;
    }
}
```

### 2. Recovery Mechanisms

```c
// Connection recovery
void initiate_connection_recovery(ulc_t *ulc, uint8_t peer_id)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    // Reset connection state
    acl_peer_info->connection_state = CONNECTION_ESTABLISHING_STATE;
    
    // Clear error conditions
    clear_error_conditions(ulc, peer_id);
    
    // Restart connection process
    restart_connection_process(ulc, peer_id);
}
```

## API Functions

### 1. Connection Management API

```c
// Start connection
int lmp_start_connection(ulc_t *ulc, uint8_t peer_id, connection_params_t *params);

// Stop connection
int lmp_stop_connection(ulc_t *ulc, uint8_t peer_id, uint8_t reason);

// Update connection parameters
int lmp_update_connection_params(ulc_t *ulc, uint8_t peer_id, connection_params_t *params);

// Get connection status
connection_status_t lmp_get_connection_status(ulc_t *ulc, uint8_t peer_id);
```

### 2. Event Handling API

```c
// Register connection event callback
void lmp_register_connection_callback(connection_event_callback_t callback);

// Unregister connection event callback
void lmp_unregister_connection_callback(connection_event_callback_t callback);

// Send connection event
void lmp_send_connection_event(ulc_t *ulc, uint8_t peer_id, connection_event_t event);
```

### 3. Utility Functions

```c
// Check if connection is active
bool lmp_is_connection_active(ulc_t *ulc, uint8_t peer_id);

// Get connection quality
float lmp_get_connection_quality(ulc_t *ulc, uint8_t peer_id);

// Get connection statistics
connection_stats_t lmp_get_connection_stats(ulc_t *ulc, uint8_t peer_id);
```

## Configuration and Parameters

### 1. Connection Parameters

```c
// Default connection parameters
#define DEFAULT_SUPERVISION_TIMEOUT     (0x7D00)  // 32 seconds
#define DEFAULT_CONNECTION_TIMEOUT      (0x0BB8)  // 3 seconds
#define DEFAULT_RETRANSMISSION_TIMEOUT  (0x03E8)  // 1 second
#define MAX_CONNECTION_RETRIES          3
#define MAX_SUPERVISION_RETRIES         3
```

### 2. State Machine Configuration

```c
// State timeout values
#define IO_CAP_REQ_TIMEOUT              (0x03E8)  // 1 second
#define NAME_REQ_TIMEOUT                (0x07D0)  // 2 seconds
#define VERSION_REQ_TIMEOUT             (0x03E8)  // 1 second
#define FEATURES_REQ_TIMEOUT            (0x03E8)  // 1 second
#define CLKOFFSET_REQ_TIMEOUT           (0x03E8)  // 1 second
```

### 3. Quality Thresholds

```c
// Connection quality thresholds
#define QUALITY_THRESHOLD_HIGH          0.95
#define QUALITY_THRESHOLD_MEDIUM        0.80
#define QUALITY_THRESHOLD_LOW           0.60
#define QUALITY_THRESHOLD_CRITICAL      0.40
```

## Performance Considerations

### 1. Memory Management

- **Connection State Storage**: Efficient storage of connection states
- **Buffer Management**: Proper allocation and deallocation of buffers
- **Resource Cleanup**: Timely cleanup of connection resources

### 2. Timing Considerations

- **State Timeouts**: Appropriate timeout values for different states
- **Retry Mechanisms**: Balanced retry strategies
- **Performance Monitoring**: Continuous monitoring of connection performance

### 3. Error Recovery

- **Fast Recovery**: Quick recovery from transient errors
- **Graceful Degradation**: Graceful handling of persistent errors
- **Resource Protection**: Protection against resource exhaustion

## Conclusion

The LMP Connection Management provides a robust and efficient framework for managing Bluetooth Classic connections. It handles the complete connection lifecycle from establishment to teardown, with comprehensive error handling and recovery mechanisms.

Key features include:

- **State Machine Management**: Robust state machine for connection lifecycle
- **Parameter Negotiation**: Comprehensive parameter exchange and validation
- **Link Supervision**: Continuous monitoring of connection health
- **Error Recovery**: Multiple recovery mechanisms for different error types
- **Performance Optimization**: Efficient resource management and timing control

The implementation provides a solid foundation for reliable Bluetooth Classic connections with excellent performance characteristics and comprehensive error handling capabilities.
