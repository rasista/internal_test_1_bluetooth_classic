# LMP Synchronous Connections Protocol Functions

## Overview

The Link Manager Protocol (LMP) Synchronous Connections management provides comprehensive support for SCO (Synchronous Connection-Oriented) and eSCO (Extended SCO) connections in Bluetooth Classic. This document provides a detailed overview of the synchronous connection management functions implemented in the LMP layer.

## Table of Contents

1. [Synchronous Connections Overview](#synchronous-connections-overview)
2. [SCO Connection Management](#sco-connection-management)
3. [eSCO Connection Management](#esco-connection-management)
4. [Connection Parameters](#connection-parameters)
5. [Parameter Negotiation](#parameter-negotiation)
6. [Timing and Scheduling](#timing-and-scheduling)
7. [Air Mode Management](#air-mode-management)
8. [Error Handling and Recovery](#error-handling-and-recovery)
9. [API Functions](#api-functions)
10. [Configuration and Parameters](#configuration-and-parameters)

## Synchronous Connections Overview

### Architecture

```
┌─────────────────────────────────────────────────────────────┐
│            LMP Synchronous Connections                     │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ SCO         │  │ eSCO        │  │ Parameter   │         │
│  │ Management  │  │ Management  │  │ Negotiation │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              Connection State Machine                  │ │
│  │  • SCO Connection States                               │ │
│  │  • eSCO Connection States                              │ │
│  │  • Negotiation States                                  │ │
│  └─────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐ │
│  │              Protocol Handler                          │ │
│  │  • SCO/eSCO Request/Response Handling                  │ │
│  │  │ Parameter Validation                                │ │
│  │  • Connection Management                               │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Key Components

- **SCO Management**: Basic synchronous voice connections
- **eSCO Management**: Enhanced synchronous connections with retransmission
- **Parameter Negotiation**: Dynamic parameter negotiation and validation
- **Timing Control**: Precise timing and scheduling management
- **Air Mode Support**: Multiple audio coding schemes

## SCO Connection Management

### 1. SCO Connection Structure

```c
// SCO link parameters structure
typedef struct {
    uint8_t sco_handle;                  // SCO connection handle
    uint8_t master_given_sco_handle;     // Master-assigned handle
    uint16_t sco_offset;                 // SCO offset (DSCO)
    uint16_t sco_interval;               // SCO interval (TSCO)
    uint8_t sco_pkt_type;                // SCO packet type
    uint8_t air_mode;                    // Air mode (CVSD, A-law, μ-law)
    uint8_t timing_control_flags;        // Timing control flags
    bool sco_handle_occupied;            // Handle occupation flag
} sco_link_params_t;

// SCO packet types
#define LMP_HV1_PKT_TYPE                 0       // HV1 packet
#define LMP_HV2_PKT_TYPE                 1       // HV2 packet  
#define LMP_HV3_PKT_TYPE                 2       // HV3 packet

// Air modes
#define CVSD_MODE                        0       // CVSD audio coding
#define A_LAW_MODE                       1       // A-law audio coding
#define MU_LAW_MODE                      2       // μ-law audio coding
#define TRANSPARENT_MODE                 3       // Transparent mode
```

### 2. SCO Connection Establishment

```c
// SCO link request transmission handler
void lmp_SCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
    uint8_t tid = lmp_input_params->tid;
    uint8_t peer_id = lmp_input_params->peer_id;
    btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
    acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    
    // Prepare SCO link request packet
    pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_SCO_LINK_REQ_CMD);
    SET_LMP_SCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
    SET_LMP_SCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
    
    // Get SCO link parameters
    sco_link_params_t *sco_link_params = &acl_peer_info->sco_link_params[lmp_input_params->input_params[0]];
    
    // Set SCO handle
    SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(lmp_tx_pkt->data, sco_link_params->master_given_sco_handle);
    
    // Set timing control flags
    SET_LMP_SCO_LINK_REQ_PARAMETER_TIMING_CONTROL_FLAGS(lmp_tx_pkt->data, CLK_INIT_1_MODE);
    
    // Set SCO offset (DSCO)
    SET_LMP_SCO_LINK_REQ_PARAMETER_D_SCO(lmp_tx_pkt->data, sco_link_params->sco_offset);
    
    // Set SCO interval (TSCO)
    SET_LMP_SCO_LINK_REQ_PARAMETER_T_SCO(lmp_tx_pkt->data, sco_link_params->sco_interval);
    
    // Set SCO packet type
    if (sco_link_params->sco_pkt_type == BT_HV1_PKT_TYPE) {
        SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV1_PKT_TYPE);
    } else if (sco_link_params->sco_pkt_type == BT_HV2_PKT_TYPE) {
        SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV2_PKT_TYPE);
    } else if (sco_link_params->sco_pkt_type == BT_HV3_PKT_TYPE) {
        SET_LMP_SCO_LINK_REQ_PARAMETER_SCO_PACKET(lmp_tx_pkt->data, LMP_HV3_PKT_TYPE);
    }
    
    // Set air mode
    SET_LMP_SCO_LINK_REQ_PARAMETER_AIR_MODE(lmp_tx_pkt->data, sco_link_params->air_mode);
    
    // Set pending flag for peripheral mode
    if (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE) {
        acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
        acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_SCO_LINK_REQ;
    }
    
    // Add packet to ACL queue
    add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}
```

### 3. SCO Connection Removal

```c
// Remove SCO link request handler
void lmp_remove_SCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
    uint8_t tid = lmp_input_params->tid;
    uint8_t peer_id = lmp_input_params->peer_id;
    btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
    acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    
    // Prepare remove SCO link request packet
    pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_REMOVE_SCO_LINK_REQ_CMD);
    SET_LMP_REMOVE_SCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
    SET_LMP_REMOVE_SCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
    
    // Set SCO handle and error code
    SET_LMP_REMOVE_SCO_LINK_REQ_PARAMETER_SCO_HANDLE(lmp_tx_pkt->data, lmp_input_params->input_params[0]);
    SET_LMP_REMOVE_SCO_LINK_REQ_PARAMETER_ERROR_CODE(lmp_tx_pkt->data, lmp_input_params->reason_code);
    
    // Set pending flags
    acl_peer_info->acl_link_mgmt_info.lmp_tx_pending = 1;
    acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_opcode = LMP_REMOVE_SCO_LINK_REQ;
    
    // Add packet to ACL queue
    add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}
```

## eSCO Connection Management

### 1. eSCO Connection Structure

```c
// eSCO link parameters structure
typedef struct {
    uint8_t esco_handle;                 // eSCO connection handle
    uint8_t master_given_esco_handle;    // Master-assigned handle
    uint8_t esco_lt_addr;                // eSCO logical transport address
    uint8_t initialisation_flag;         // Initialization flag
    uint16_t esco_offset;                // eSCO offset (DeSCO)
    uint16_t esco_interval;              // eSCO interval (TeSCO)
    uint16_t esco_window;                // eSCO window (WeSCO)
    uint8_t central_to_peripheral_pkt_type;    // C→P packet type
    uint8_t peripheral_to_central_pkt_type;    // P→C packet type
    uint16_t central_to_peripheral_pkt_len;    // C→P packet length
    uint16_t peripheral_to_central_pkt_len;    // P→C packet length
    uint8_t air_mode;                    // Air mode
    uint8_t negotiation_state;           // Negotiation state
    uint32_t tx_bandwidth;               // TX bandwidth
    uint32_t rx_bandwidth;               // RX bandwidth
    uint32_t max_latency;                // Maximum latency
    uint8_t retry_effort;                // Retry effort
    uint8_t esco_reserved_slot_num;      // Reserved slot number
    bool esco_handle_occupied;           // Handle occupation flag
} esco_link_params_t;

// eSCO packet types
#define LMP_EV3_PKT_TYPE                 0x07    // EV3 packet
#define LMP_EV4_PKT_TYPE                 0x0C    // EV4 packet
#define LMP_EV5_PKT_TYPE                 0x0D    // EV5 packet
#define LMP_2EV3_PKT_TYPE                0x07    // 2EV3 packet
#define LMP_2EV5_PKT_TYPE                0x0C    // 2EV5 packet
#define LMP_3EV3_PKT_TYPE                0x07    // 3EV3 packet
#define LMP_3EV5_PKT_TYPE                0x0D    // 3EV5 packet

// Negotiation states
#define ESCO_UNDEFINED_STATE             0       // Undefined state
#define ESCO_NEGOTIATION_STATE_1         1       // Negotiation state 1
#define ESCO_NEGOTIATION_STATE_2         2       // Negotiation state 2
#define ESCO_NEGOTIATION_STATE_3         3       // Negotiation state 3
#define ESCO_NEGOTIATION_STATE_4         4       // Negotiation state 4
```

### 2. eSCO Connection Establishment

```c
// eSCO link request transmission handler
void lmp_eSCO_link_req_tx_handler(ulc_t *ulc, lmp_input_params_t *lmp_input_params)
{
    uint8_t tid = lmp_input_params->tid;
    uint8_t peer_id = lmp_input_params->peer_id;
    btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
    acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    
    // Get eSCO link parameters
    esco_link_params_t *esco_link_temp_params = &acl_peer_info->esco_link_params[0];
    
    // Prepare eSCO link request packet
    pkb_t *lmp_tx_pkt = prepare_lmp_command_tx_pkt(SET_PTL_OF_LMP_ESCO_LINK_REQ_CMD);
    SET_LMP_ESCO_LINK_REQ_TID(lmp_tx_pkt->data, tid);
    SET_LMP_ESCO_LINK_REQ_OPCODE(lmp_tx_pkt->data);
    
    // Set eSCO handle
    if ((acl_peer_info->acl_dev_mgmt_info.mode == BT_CENTRAL_MODE) && 
        (acl_peer_info->acl_link_mgmt_info.received_req_flag |= ESCO_CONNECTION_REQ)) {
        SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(lmp_tx_pkt->data, esco_link_temp_params->master_given_esco_handle);
    } else {
        SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_HANDLE(lmp_tx_pkt->data, lmp_input_params->input_params[0]);
    }
    
    // Set eSCO parameters
    SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_LT_ADDR(lmp_tx_pkt->data, esco_link_temp_params->esco_lt_addr);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_TIMING_CONTROL_FLAGS(lmp_tx_pkt->data, esco_link_temp_params->initialisation_flag);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_D_ESCO(lmp_tx_pkt->data, esco_link_temp_params->esco_offset);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_T_ESCO(lmp_tx_pkt->data, esco_link_temp_params->esco_interval);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_W_ESCO(lmp_tx_pkt->data, esco_link_temp_params->esco_window);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_PACKET_TYPE_C_P(lmp_tx_pkt->data, esco_link_temp_params->central_to_peripheral_pkt_type);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_ESCO_PACKET_TYPE_P_C(lmp_tx_pkt->data, esco_link_temp_params->peripheral_to_central_pkt_type);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_PACKET_LENGTH_C_P(lmp_tx_pkt->data, esco_link_temp_params->central_to_peripheral_pkt_len);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_PACKET_LENGTH_P_C(lmp_tx_pkt->data, esco_link_temp_params->peripheral_to_central_pkt_len);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_AIR_MODE(lmp_tx_pkt->data, esco_link_temp_params->air_mode);
    SET_LMP_ESCO_LINK_REQ_PARAMETER_NEGOTIATION_STATE(lmp_tx_pkt->data, esco_link_temp_params->negotiation_state);
    
    // Set pending flags
    acl_peer_info->acl_link_mgmt_info.lmp_ext_tx_pending = 1;
    acl_peer_info->acl_link_mgmt_info.lmp_tx_pending_ext_opcode = LMP_ESCO_LINK_REQ;
    
    // Add packet to ACL queue
    add_pkt_to_aclc_q(ulc, lmp_tx_pkt, peer_id);
}
```

### 3. eSCO Parameter Validation

```c
// eSCO parameter acceptance check
uint8_t lmp_esco_params_accept_check(ulc_t *ulc, uint8_t peer_id, esco_link_params_t *esco_link_update_params)
{
    uint8_t status = 0;
    btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
    uint16_t update_tx_bandwidth, update_rx_bandwidth;
    acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    
    // RULE 3: Multi-slot packets (EV4, EV5, 2EV5, 3EV5) support only transparent air mode
    if ((esco_link_update_params->air_mode != TRANSPARENT_MODE) && 
        ((esco_link_update_params->central_to_peripheral_pkt_type & 0x0C) == 0x0C)) {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    // RULE 2: Check air mode support
    if (((esco_link_update_params->air_mode == TRANSPARENT_MODE) && 
         (btc_dev_info->btc_dev_mgmt_info.features_bitmap[2] & LMP_SYNCH_DATA)) ||
        ((esco_link_update_params->air_mode == MU_LAW_MODE) && 
         (btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_ULAW)) ||
        ((esco_link_update_params->air_mode == A_LAW_MODE) && 
         (btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_ALAW)) ||
        ((esco_link_update_params->air_mode == CVSD_MODE) && 
         (btc_dev_info->btc_dev_mgmt_info.features_bitmap[2] & LMP_CVSD))) {
        status |= ACCEPT_PARAMS_SET;
    } else {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    // Check eSCO offset (must be 0)
    if (esco_link_update_params->esco_offset != 0) {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    // Calculate bandwidth requirements
    if (acl_peer_info->acl_dev_mgmt_info.mode == BT_CENTRAL_MODE) {
        update_rx_bandwidth = 1600 * ((esco_link_update_params->peripheral_to_central_pkt_len) / 
                                     (esco_link_update_params->esco_interval));
        update_tx_bandwidth = 1600 * ((esco_link_update_params->central_to_peripheral_pkt_len) / 
                                     (esco_link_update_params->esco_interval));
    } else {
        update_rx_bandwidth = 1600 * ((esco_link_update_params->peripheral_to_central_pkt_len) / 
                                     (esco_link_update_params->esco_interval));
        update_tx_bandwidth = 1600 * ((esco_link_update_params->central_to_peripheral_pkt_len) / 
                                     (esco_link_update_params->esco_interval));
    }
    
    // Check bandwidth constraints
    if ((update_rx_bandwidth <= esco_link_update_params->rx_bandwidth) && 
        (update_tx_bandwidth <= esco_link_update_params->tx_bandwidth)) {
        status |= ACCEPT_PARAMS_SET;
    } else {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    // Check packet type support
    if (esco_link_update_params->peripheral_to_central_pkt_type != 
        esco_link_update_params->central_to_peripheral_pkt_type) {
        esco_link_update_params->peripheral_to_central_pkt_type = 
            esco_link_update_params->central_to_peripheral_pkt_len;
        status |= SEND_ANOTHER_PARAMS_SET;
    }
    
    // Check EDR eSCO support
    if ((((esco_link_update_params->central_to_peripheral_pkt_type & 0x30) == 0x30) && 
         (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_EDR_ESCO_3M))) ||
        (((esco_link_update_params->central_to_peripheral_pkt_type & 0x30) == 0x20) && 
         (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[5] & LMP_EDR_ESCO_2M)))) {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    // Check EV4/EV5 packet type support
    if (((esco_link_update_params->central_to_peripheral_pkt_type == LMP_EV4_PKT_TYPE) && 
         (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[4] & LMP_EV4))) ||
        ((esco_link_update_params->central_to_peripheral_pkt_type >= LMP_EV5_PKT_TYPE) && 
         (!(esco_link_update_params->central_to_peripheral_pkt_type & BIT(1))) && 
         (!(btc_dev_info->btc_dev_mgmt_info.features_bitmap[4] & LMP_EV5)))) {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    // Set reserved slot number based on packet type
    if (esco_link_update_params->central_to_peripheral_pkt_type & BIT(1)) {
        // Single slot packets (EV3, 2EV3, 3EV3)
        esco_link_update_params->esco_reserved_slot_num = 2;
    } else {
        // Multi-slot packets (EV4, EV5, 2EV5, 3EV5)
        esco_link_update_params->esco_reserved_slot_num = 6;
    }
    
    // Check retry window configuration
    if ((esco_link_update_params->esco_window == 0) && 
        ((esco_link_update_params->retry_effort == 0) || (esco_link_update_params->retry_effort == 0xFF))) {
        esco_link_update_params->esco_window = 0;
        status |= ACCEPT_PARAMS_SET;
    } else if ((esco_link_update_params->esco_window != 0) && 
               (esco_link_update_params->retry_effort & 0x03)) {
        if (esco_link_update_params->esco_window == esco_link_update_params->esco_reserved_slot_num) {
            status |= ACCEPT_PARAMS_SET;
        } else {
            esco_link_update_params->esco_window = esco_link_update_params->esco_reserved_slot_num;
            status |= SEND_ANOTHER_PARAMS_SET;
        }
    } else {
        esco_link_update_params->esco_window = 0;
        status |= SEND_ANOTHER_PARAMS_SET;
    }
    
    esco_link_update_params->esco_reserved_slot_num += esco_link_update_params->esco_window;
    
    // RULE 3: Check latency violation
    if (((esco_link_update_params->esco_interval + esco_link_update_params->esco_reserved_slot_num) * 625) > 
        ((esco_link_update_params->max_latency) * 1000)) {
        status |= REJECT_PARAMS_SET;
        return status;
    }
    
    return status;
}
```

## Parameter Negotiation

### 1. eSCO Parameter Negotiation

```c
// eSCO parameter negotiation
uint8_t lmp_negotiate_esco_params(ulc_t *ulc, uint8_t peer_id, uint8_t tid, uint8_t esco_nego_status)
{
    uint8_t status;
    esco_link_params_t *esco_link_params, *esco_link_temp_params;
    lmp_input_params_t lmp_input_params;
    btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
    acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
    acl_link_mgmt_info_t *acl_link_mgmt_info = &acl_peer_info->acl_link_mgmt_info;
    
    esco_link_params = &acl_peer_info->esco_link_params[acl_peer_info->esco_info.esco_handle];
    esco_link_temp_params = &acl_peer_info->esco_link_params[0];
    
    // Check parameter acceptance
    status = lmp_esco_params_accept_check(ulc, peer_id, esco_link_temp_params);
    lmp_input_params.tid = tid;
    lmp_input_params.peer_id = peer_id;
    
    // Handle negotiation results
    if (status & REJECT_PARAMS_SET) {
        // Send LMP_NOT_ACCEPTED_EXT
        lmp_input_params.exopcode = LMP_NOT_ACCEPTED_EXT;
        lmp_input_params.input_params[0] = LMP_ESCAPE_OPCODE;
        lmp_input_params.input_params[1] = LMP_ESCO_LINK_REQ;
        lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
        lmp_input_params.reason_code = UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
        
        copy_esco_params(esco_link_params, esco_link_temp_params, RESET_ESCO_PARAMS);
        
        if (esco_nego_status != RENEGOTIATE_REQ) {
            dm_esco_link_formation_indication_from_lm(ulc, peer_id, UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE);
            if (acl_link_mgmt_info->check_pending_msg_flag & SYNC_LINK_REQ_SENT) {
                acl_link_mgmt_info->check_pending_msg_flag &= ~SYNC_LINK_REQ_SENT;
            }
            if (esco_link_params->esco_handle_occupied != ESCO_HANDLE_OCCUPIED) {
                memset((uint8_t *)&acl_peer_info->esco_info, 0, sizeof(esco_info_t));
            }
        }
    } else if (status & SEND_ANOTHER_PARAMS_SET) {
        // Send another parameter set
        esco_link_temp_params->negotiation_state = ESCO_UNDEFINED_STATE;
        lmp_input_params.exopcode = LMP_ESCO_LINK_REQ;
        lmp_input_params.input_params[0] = acl_peer_info->esco_info.esco_handle;
        lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
        lmp_input_params.reason_code = 0;
        
        if (esco_nego_status == RENEGOTIATE_REQ) {
            acl_link_mgmt_info->check_pending_msg_flag |= ESCO_SCO_CHANGE_PKT_REQ_SENT;
        } else if (esco_nego_status == HOST_ACCEPT_ESCO_LINK) {
            acl_link_mgmt_info->check_pending_msg_flag |= SYNC_LINK_REQ_SENT;
        }
    } else if (status & ACCEPT_PARAMS_SET) {
        // Accept parameters
        if ((esco_nego_status == HOST_ACCEPT_ESCO_LINK) && 
            (acl_peer_info->acl_dev_mgmt_info.mode == BT_PERIPHERAL_MODE)) {
            // Send link request again for peripheral mode
            lmp_input_params.exopcode = LMP_ESCO_LINK_REQ;
            lmp_input_params.input_params[0] = acl_peer_info->esco_info.esco_handle;
            esco_link_temp_params->esco_lt_addr = bt_get_free_lt_addr(&btc_dev_info->btc_dev_mgmt_info);
            lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
            lmp_input_params.reason_code = 0;
            acl_link_mgmt_info->check_pending_msg_flag |= SYNC_LINK_REQ_SENT;
        } else {
            // Send LMP_ACCEPTED_EXT
            lmp_input_params.exopcode = LMP_ACCEPTED_EXT;
            lmp_input_params.input_params[0] = LMP_ESCAPE_OPCODE;
            lmp_input_params.input_params[1] = LMP_ESCO_LINK_REQ;
            lmp_input_params.opcode = LMP_ESCAPE_OPCODE;
            lmp_input_params.reason_code = 0;
            
            copy_esco_params(esco_link_params, esco_link_temp_params, COPY_N_RESET_ESCO_PARAMS);
            acl_peer_info->esco_info.esco_lt_addr = esco_link_params->esco_lt_addr;
            dm_esco_link_formation_indication_from_lm(ulc, peer_id, BT_STATUS_SUCCESS);
            
            if (acl_link_mgmt_info->check_pending_msg_flag & SYNC_LINK_REQ_SENT) {
                acl_link_mgmt_info->check_pending_msg_flag &= ~SYNC_LINK_REQ_SENT;
            }
            return SEND_SYNC_CONN_CHANGED;
        }
    }
    
    return 0;
}
```

### 2. Parameter Copy and Reset

```c
// Copy eSCO parameters
void copy_esco_params(esco_link_params_t *dest_params, esco_link_params_t *src_params, uint8_t reset)
{
    if (reset == RESET_ESCO_PARAMS) {
        // Reset eSCO link parameters
        memset((uint8_t *)src_params, 0, BT_ESCO_LINK_PARAMS_SIZE);
    } else {
        // Copy eSCO link parameters
        memcpy((uint8_t *)dest_params, (uint8_t *)src_params, BT_ESCO_LINK_PARAMS_SIZE);
        
        if (reset == COPY_N_RESET_ESCO_PARAMS) {
            // Reset source parameters after copy
            memset((uint8_t *)src_params, 0, BT_ESCO_LINK_PARAMS_SIZE);
        }
    }
}
```

## Timing and Scheduling

### 1. Timing Control Flags

```c
// Timing control flags
#define CLK_INIT_1_MODE                  0       // Clock initialization mode 1
#define CLK_INIT_2_MODE                  1       // Clock initialization mode 2

// Timing parameters
#define BT_SLOT_TIME_US                  625     // Bluetooth slot time in microseconds
#define SCO_DEFAULT_INTERVAL             6       // Default SCO interval (6 slots)
#define ESCO_DEFAULT_INTERVAL            6       // Default eSCO interval (6 slots)
#define ESCO_DEFAULT_WINDOW              1       // Default eSCO window (1 slot)
```

### 2. Timing Calculation

```c
// Calculate timing parameters
void calculate_timing_parameters(sco_link_params_t *sco_params, esco_link_params_t *esco_params)
{
    // Calculate SCO timing
    if (sco_params) {
        sco_params->sco_interval = SCO_DEFAULT_INTERVAL;
        sco_params->sco_offset = 0;  // DSCO = 0 for basic SCO
    }
    
    // Calculate eSCO timing
    if (esco_params) {
        esco_params->esco_interval = ESCO_DEFAULT_INTERVAL;
        esco_params->esco_window = ESCO_DEFAULT_WINDOW;
        esco_params->esco_offset = 0;  // DeSCO = 0 for basic eSCO
        
        // Calculate reserved slot number based on packet type
        if (esco_params->central_to_peripheral_pkt_type & BIT(1)) {
            esco_params->esco_reserved_slot_num = 2;  // Single slot
        } else {
            esco_params->esco_reserved_slot_num = 6;  // Multi-slot
        }
    }
}
```

## Air Mode Management

### 1. Air Mode Support

```c
// Air mode support check
bool is_air_mode_supported(ulc_t *ulc, uint8_t air_mode)
{
    btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
    
    switch (air_mode) {
        case TRANSPARENT_MODE:
            return (btc_dev_info->btc_dev_mgmt_info.features_bitmap[2] & LMP_SYNCH_DATA) != 0;
            
        case MU_LAW_MODE:
            return (btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_ULAW) != 0;
            
        case A_LAW_MODE:
            return (btc_dev_info->btc_dev_mgmt_info.features_bitmap[1] & LMP_ALAW) != 0;
            
        case CVSD_MODE:
            return (btc_dev_info->btc_dev_mgmt_info.features_bitmap[2] & LMP_CVSD) != 0;
            
        default:
            return false;
    }
}
```

### 2. Air Mode Configuration

```c
// Configure air mode
void configure_air_mode(ulc_t *ulc, uint8_t peer_id, uint8_t air_mode)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    // Validate air mode support
    if (!is_air_mode_supported(ulc, air_mode)) {
        // Handle unsupported air mode
        handle_unsupported_air_mode(ulc, peer_id, air_mode);
        return;
    }
    
    // Configure SCO air mode
    if (acl_peer_info->sco_link_params[0].sco_handle_occupied) {
        acl_peer_info->sco_link_params[0].air_mode = air_mode;
    }
    
    // Configure eSCO air mode
    if (acl_peer_info->esco_link_params[0].esco_handle_occupied) {
        acl_peer_info->esco_link_params[0].air_mode = air_mode;
    }
}
```

## Error Handling and Recovery

### 1. Error Types

```c
// Synchronous connection error codes
typedef enum {
    SYNC_CONN_ERROR_SUCCESS = 0,
    SYNC_CONN_ERROR_INVALID_PARAMETERS,
    SYNC_CONN_ERROR_UNSUPPORTED_FEATURE,
    SYNC_CONN_ERROR_INSUFFICIENT_RESOURCES,
    SYNC_CONN_ERROR_NEGOTIATION_FAILED,
    SYNC_CONN_ERROR_TIMEOUT,
    SYNC_CONN_ERROR_CONNECTION_LOST
} sync_conn_error_code_t;
```

### 2. Error Handling

```c
// Handle synchronous connection error
void handle_sync_conn_error(ulc_t *ulc, uint8_t peer_id, sync_conn_error_code_t error_code)
{
    acl_peer_info_t *acl_peer_info = ulc->btc_dev_info.acl_peer_info[peer_id];
    
    switch (error_code) {
        case SYNC_CONN_ERROR_INVALID_PARAMETERS:
            // Handle invalid parameters
            handle_invalid_sync_params(ulc, peer_id);
            break;
            
        case SYNC_CONN_ERROR_UNSUPPORTED_FEATURE:
            // Handle unsupported feature
            handle_unsupported_feature(ulc, peer_id);
            break;
            
        case SYNC_CONN_ERROR_INSUFFICIENT_RESOURCES:
            // Handle insufficient resources
            handle_insufficient_resources(ulc, peer_id);
            break;
            
        case SYNC_CONN_ERROR_NEGOTIATION_FAILED:
            // Handle negotiation failure
            handle_negotiation_failure(ulc, peer_id);
            break;
            
        case SYNC_CONN_ERROR_TIMEOUT:
            // Handle timeout
            handle_sync_conn_timeout(ulc, peer_id);
            break;
            
        case SYNC_CONN_ERROR_CONNECTION_LOST:
            // Handle connection loss
            handle_sync_conn_loss(ulc, peer_id);
            break;
    }
}
```

## API Functions

### 1. Synchronous Connection Management API

```c
// Setup SCO connection
int lmp_setup_sco_connection(ulc_t *ulc, uint8_t peer_id, sco_link_params_t *params);

// Setup eSCO connection
int lmp_setup_esco_connection(ulc_t *ulc, uint8_t peer_id, esco_link_params_t *params);

// Remove SCO connection
int lmp_remove_sco_connection(ulc_t *ulc, uint8_t peer_id, uint8_t sco_handle);

// Remove eSCO connection
int lmp_remove_esco_connection(ulc_t *ulc, uint8_t peer_id, uint8_t esco_handle);

// Update eSCO parameters
int lmp_update_esco_parameters(ulc_t *ulc, uint8_t peer_id, esco_link_params_t *params);
```

### 2. Connection Status API

```c
// Get SCO connection status
sco_status_t lmp_get_sco_status(ulc_t *ulc, uint8_t peer_id, uint8_t sco_handle);

// Get eSCO connection status
esco_status_t lmp_get_esco_status(ulc_t *ulc, uint8_t peer_id, uint8_t esco_handle);

// Get connection statistics
sync_conn_stats_t lmp_get_sync_conn_stats(ulc_t *ulc, uint8_t peer_id);
```

## Configuration and Parameters

### 1. Default Parameters

```c
// Default SCO parameters
#define DEFAULT_SCO_INTERVAL             6       // 6 slots
#define DEFAULT_SCO_OFFSET               0       // No offset
#define DEFAULT_SCO_PACKET_TYPE          LMP_HV3_PKT_TYPE
#define DEFAULT_SCO_AIR_MODE             CVSD_MODE

// Default eSCO parameters
#define DEFAULT_ESCO_INTERVAL            6       // 6 slots
#define DEFAULT_ESCO_WINDOW              1       // 1 slot
#define DEFAULT_ESCO_OFFSET              0       // No offset
#define DEFAULT_ESCO_PACKET_TYPE         LMP_EV3_PKT_TYPE
#define DEFAULT_ESCO_AIR_MODE            CVSD_MODE
#define DEFAULT_ESCO_RETRY_EFFORT        0       // No retry
```

### 2. Feature Support

```c
// Feature support flags
#define LMP_SYNCH_DATA                   BIT(0)  // Synchronous data support
#define LMP_CVSD                         BIT(1)  // CVSD support
#define LMP_ULAW                         BIT(2)  // μ-law support
#define LMP_ALAW                         BIT(3)  // A-law support
#define LMP_EV4                          BIT(4)  // EV4 packet support
#define LMP_EV5                          BIT(5)  // EV5 packet support
#define LMP_EDR_ESCO_2M                  BIT(6)  // EDR eSCO 2M support
#define LMP_EDR_ESCO_3M                  BIT(7)  // EDR eSCO 3M support
```

## Performance Considerations

### 1. Bandwidth Management

- **Bandwidth Calculation**: Precise bandwidth calculation for eSCO connections
- **Resource Allocation**: Efficient allocation of synchronous connection resources
- **Latency Optimization**: Minimization of connection latency

### 2. Parameter Optimization

- **Packet Type Selection**: Optimal packet type selection based on requirements
- **Timing Optimization**: Efficient timing parameter configuration
- **Retry Configuration**: Balanced retry effort configuration

## Conclusion

The LMP Synchronous Connections management provides comprehensive support for both SCO and eSCO connections in Bluetooth Classic. It offers flexible parameter negotiation, robust error handling, and efficient resource management.

Key features include:

- **Dual Connection Support**: Both SCO and eSCO connection management
- **Parameter Negotiation**: Dynamic parameter negotiation and validation
- **Air Mode Support**: Multiple audio coding schemes (CVSD, A-law, μ-law, Transparent)
- **Timing Control**: Precise timing and scheduling management
- **Error Recovery**: Comprehensive error handling and recovery mechanisms

The implementation provides a solid foundation for voice and audio connections in Bluetooth Classic with excellent performance characteristics and comprehensive connection management capabilities.
