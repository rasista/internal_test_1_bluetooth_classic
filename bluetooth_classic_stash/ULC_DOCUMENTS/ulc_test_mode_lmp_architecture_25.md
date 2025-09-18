# Test Mode LMP Procedures Architecture in ULC

## Executive Summary

The Test Mode LMP Procedures architecture in ULC provides a comprehensive framework for managing Bluetooth test mode operations through Link Manager Protocol (LMP) commands. This architecture enables comprehensive testing of Bluetooth functionality including transmitter tests, receiver tests, loopback tests, and various test scenarios for validation and certification purposes.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Test Mode Core Components](#test-mode-core-components)
3. [Test Mode Protocol Management](#test-mode-protocol-management)
4. [Test Mode Types and Scenarios](#test-mode-types-and-scenarios)
5. [Test Mode State Management](#test-mode-state-management)
6. [Test Mode Packet Processing](#test-mode-packet-processing)
7. [Test Mode Data Generation](#test-mode-data-generation)
8. [Integration Architecture](#integration-architecture)
9. [Performance Architecture](#performance-architecture)
10. [Error Handling Architecture](#error-handling-architecture)
11. [Future Extensions](#future-extensions)

## Architecture Overview

### High-Level Test Mode Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    Test Mode Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Test        │  │ Test        │  │ Test        │            │
│  │ Activation  │  │ Control     │  │ Processing  │            │
│  │ Manager     │  │ Manager     │  │ Engine      │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Transmitter │  │ Receiver    │  │ Loopback    │            │
│  │ Test        │  │ Test        │  │ Test        │            │
│  │ Engine      │  │ Engine      │  │ Engine      │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Test Mode Core Engine                        │ │
│  │  • Test Scenario Management                               │ │
│  │  • Packet Type Configuration                              │ │
│  │  • Data Pattern Generation                                │ │
│  │  • Test Result Validation                                 │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Core Design Principles

1. **Comprehensive Testing**: Support for all Bluetooth test scenarios and modes
2. **Protocol Compliance**: Full compliance with Bluetooth test mode specifications
3. **Flexible Configuration**: Configurable test parameters and scenarios
4. **Data Integrity**: Reliable test data generation and validation
5. **Performance Validation**: Comprehensive performance and functionality testing

## Test Mode Core Components

### 1. Test Mode Parameters Structure

```c
typedef struct test_mode_params_s {
  uint8_t test_scenario;           // Test scenario type
  uint8_t hopping_mode;            // Frequency hopping mode
  uint8_t tx_freq;                 // Transmit frequency
  uint8_t rx_freq;                 // Receive frequency
  uint8_t pwr_cntrl_mode;          // Power control mode
  uint8_t poll_period;             // Poll period
  uint8_t pkt_type;                // Packet type
  uint8_t test_lt_type;            // Test link type
  uint8_t link_type;               // Link type
  uint16_t len_of_seq;             // Length of test sequence
  uint8_t test_mode_type;          // Test mode type
  uint8_t whitening_enable;        // Whitening enable/disable
  uint8_t test_started;            // Test started flag
  pkb_t *tx_pkt;                   // Transmit test packet
} test_mode_params_t;
```

### 2. Test Mode Core Functions

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode Core Functions                         │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Test        │  │ Test        │  │ Test        │            │
│  │ Activation  │  │ Control     │  │ Processing  │            │
│  │ Handlers    │  │ Handlers    │  │ Functions   │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Core Functionality                           │ │
│  │  • Test Mode Activation/Deactivation                      │ │
│  │  • Test Control and Configuration                         │ │
│  │  • Test Packet Processing                                 │ │
│  │  • Test Data Generation                                   │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Test Mode State Definitions

```c
// Test Mode State Definitions
#define TEST_MODE_ENABLED           1
#define TEST_MODE_DISABLED          0
#define TEST_IN_PROGRESS            1
#define TEST_NOT_IN_PROGRESS        0
#define TEST_STARTED                1
#define TEST_NOT_STARTED            0
#define TEST_IN_PAUSE               2

// Test Mode Types
#define ACL_TEST_MODE               0
#define SCO_TEST_MODE               1
#define ESCO_TEST_MODE              2

// Test Link Types
#define TEST_ACL                    0x02
#define TEST_SCO                    0x00
#define TEST_ESCO                   0x01
```

## Test Mode Protocol Management

### 1. LMP Test Mode Commands

```
┌─────────────────────────────────────────────────────────────────┐
│              LMP Test Mode Commands                           │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ TEST_       │  │ TEST_       │  │ Test        │            │
│  │ ACTIVATE    │  │ CONTROL     │  │ Mode        │            │
│  │             │  │             │  │ Management  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              LMP Test Mode Command Features               │ │
│  │  • TEST_ACTIVATE: Activate test mode                      │ │
│  │  • TEST_CONTROL: Configure test parameters                │ │
│  │  • Test Mode State Management                             │ │
│  │  • Test Configuration Processing                          │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. LMP Test Mode Command Parameters

```yaml
# LMP TEST_ACTIVATE Command
lmp_test_activate:
  OPCODE: 56
  parameters: 0                  # No parameters

# LMP TEST_CONTROL Command
lmp_test_control:
  OPCODE: 57
  parameters:
    test_senario: 1              # Test scenario (1 byte)
    hopping_mode: 1              # Hopping mode (1 byte)
    tx_frequency: 1              # TX frequency (1 byte)
    rx_frequency: 1              # RX frequency (1 byte)
    power_mode: 1                # Power mode (1 byte)
    poll_period: 1               # Poll period (1 byte)
    packet_type: 1               # Packet type (1 byte)
    test_data_length: 2          # Test data length (2 bytes)
```

### 3. Test Mode Protocol Handlers

```c
// Test Mode Protocol Handlers
void lmp_test_activate_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_test_activate_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_test_control_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_test_control_rx_handler(ulc_t *ulc, pkb_t *pkb);

// Test Mode Processing Functions
void lmp_test_mode_rx_packet_handler(ulc_t *ulc, pkb_t *rx_pkb);
void btc_process_test_mode_packets(void *ctx, pkb_t *pkb);
void lmp_process_test_config(ulc_t *ulc, uint8_t peer_id);
```

## Test Mode Types and Scenarios

### 1. Test Scenarios

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Scenarios                                   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Transmitter │  │ Receiver    │  │ Loopback    │            │
│  │ Tests       │  │ Tests       │  │ Tests       │            │
│  │             │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Test Scenario Types                          │ │
│  │  • Transmitter Test 0, 1, 2, PRBS, F0                    │ │
│  │  • Receiver Test with various patterns                    │ │
│  │  • Loopback ACL/SCO with/without whitening               │ │
│  │  • Pause Test Mode                                        │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Test Scenario Definitions

```c
// Test Scenario Types
#define PAUSE_TEST_MODE            0x00
#define TRANSMITTER_TEST_0         0x01
#define TRANSMITTER_TEST_1         0x02
#define TRANSMITTER_TEST_2         0x03
#define TRANSMITTER_TEST_PRBS      0x04
#define TRANSMITTER_TEST_F0        0x09
#define LOOPBACK_ACL               0x05
#define LOOPBACK_SCO               0x06
#define LOOPBACK_ACL_NO_WHITENING  0x07
#define LOOPBACK_SCO_NO_WHITENING  0x08
#define EXIT_TEST_MODE             0x0A

// Test Types
#define TRANSMITTER_TEST           1
#define RECEIVER_TEST              2
#define LOOPBACK_TEST              3
```

### 3. Test Scenario Processing

```c
// Test Scenario Processing
void lmp_process_test_config(ulc_t *ulc, uint8_t peer_id)
{
  switch (test_mode_params->test_scenario) {
    case PAUSE_TEST_MODE: {
      flush_queue(&acl_peer_info->tx_acld_q);
      test_mode_params->test_started = TEST_NOT_STARTED;
      acl_link_mgmt_info->test_type = TEST_IN_PAUSE;
      break;
    }
    case TRANSMITTER_TEST_0:
    case TRANSMITTER_TEST_1: 
    case TRANSMITTER_TEST_2: 
    case TRANSMITTER_TEST_PRBS: 
    case TRANSMITTER_TEST_F0: {
      payload = transmitter_payload_types[test_mode_params->test_scenario];
      test_mode_params->whitening_enable = WHITENING_DISABLED;
      dm_start_transmitter_test(ulc, peer_id, payload);
      break;
    }
    case LOOPBACK_ACL:
    case LOOPBACK_SCO: {
      test_mode_params->whitening_enable = WHITENING_ENABLED;
      acl_link_mgmt_info->test_type = LOOPBACK_TEST;
      break;
    }
    case LOOPBACK_ACL_NO_WHITENING:
    case LOOPBACK_SCO_NO_WHITENING: {
      test_mode_params->whitening_enable = WHITENING_DISABLED;
      acl_link_mgmt_info->test_type = LOOPBACK_TEST;
      break;
    }
    case EXIT_TEST_MODE: {
      acl_link_mgmt_info->test_in_progress = TEST_NOT_IN_PROGRESS;
      acl_link_mgmt_info->dev_in_test_mode = TEST_MODE_DISABLED;
      btc_dev_info->btc_dev_mgmt_info.testmode_block_host_pkts = 0;
      break;
    }
  }
}
```

## Test Mode State Management

### 1. Test Mode State Machine

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode State Machine                          │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Normal      │  │ Test        │  │ Test        │            │
│  │ Mode        │  │ Activated   │  │ In Progress │            │
│  │ State       │  │ State       │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
│         │                │                │                   │
│         │                │                │                   │
│         ▼                ▼                ▼                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Test        │  │ Test        │  │ Test        │            │
│  │ Paused      │  │ Completed   │  │ Error       │            │
│  │ State       │  │ State       │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Test Mode State Management

```c
// Test Mode State Management Macros
#define IS_HOST_DEV_TEST_MODE_ENABLED(dev_mgmt_info) \
  ((dev_mgmt_info)->host_dev_test_mode_enable & DEVICE_TEST_MODE_ENABLED)

#define SET_TEST_MODE_ENABLED(acl_link_mgmt_info) \
  ((acl_link_mgmt_info)->dev_in_test_mode = TEST_MODE_ENABLED)

#define SET_TEST_MODE_DISABLED(acl_link_mgmt_info) \
  ((acl_link_mgmt_info)->dev_in_test_mode = TEST_MODE_DISABLED)

#define IS_TEST_MODE_ENABLED(ulc, pkb) \
  (((btc_dev_info_t *)&((ulc)->btc_dev_info))->acl_peer_info[GET_PEER_ID(pkb)]->acl_link_mgmt_info.dev_in_test_mode == TEST_MODE_ENABLED)

#define IS_TEST_MODE_BLOCKING_HOST_PKTS(ulc) \
  ((ulc)->btc_dev_info.btc_dev_mgmt_info.testmode_block_host_pkts == 1)
```

### 3. Test Mode State Transitions

```c
// Test Mode State Transition Functions
void start_test_mode(ulc_t *ulc, uint8_t peer_id)
{
  acl_link_mgmt_info->test_in_progress = TEST_IN_PROGRESS;
  test_mode_params->test_started = TEST_STARTED;
  set_test_mode_params_cmd((hss_cmd_set_test_mode_params_t *)&test_mode_params, 
                          (shared_mem_t *)&ulc->tx_mbox_queue);
}

// Test Mode Activation
void lmp_test_activate_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  if (IS_HOST_DEV_TEST_MODE_ENABLED(btc_dev_mgmt_info)) {
    SET_TEST_MODE_ENABLED(acl_link_mgmt_info);
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
    acl_link_mgmt_info->lmp_connection_state = LMP_STANDBY_STATE;
    conn_pwr_ctrl_info->enable_power_control = DISABLE_POWER_CNTRL;
  } else {
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  }
}
```

## Test Mode Packet Processing

### 1. Test Mode Packet Handler

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode Packet Processing                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ LMP         │  │ Test        │  │ Test        │            │
│  │ Packet      │  │ Mode        │  │ Packet      │            │
│  │ Dispatcher  │  │ Handler     │  │ Processing  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Packet Processing Features                   │ │
│  │  • LMP Packet Dispatching                                 │ │
│  │  • Test Mode Specific Handling                            │ │
│  │  • Test Packet Processing                                 │ │
│  │  • Test Result Validation                                 │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Test Mode Packet Processing Functions

```c
// Test Mode Packet Processing
void lmp_test_mode_rx_packet_handler(ulc_t *ulc, pkb_t *rx_pkb)
{
  uint8_t lmp_opcode = GET_OPCODE(rx_pkb);
  uint8_t lmp_ext_opcode = GET_EXT_OPCODE(rx_pkb);
  
  switch (lmp_opcode) {
    case LMP_TEST_ACTIVATE: {
      lmp_input_params[0] = lmp_opcode;
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), lmp_input_params);
      break;
    }
    case LMP_TEST_CONTROL: {
      // Process test control parameters
      test_mode_params->test_scenario = (GET_LMP_TEST_CONTROL_PARAMETER_TEST_SENARIO(bt_pkt_start) ^ 0x55);
      // ... process other parameters
      btc_dev_info->btc_dev_mgmt_info.testmode_block_host_pkts = 1;
      lmp_process_test_config(ulc, peer_id);
      break;
    }
    // ... other LMP commands
  }
}

void btc_process_test_mode_packets(void *ctx, pkb_t *pkb)
{
  uint8_t llid_val = GET_LLID(pkb);
  uint8_t rx_pkt_type = GET_PKT_TYPE(pkb);
  if (rx_pkt_type == BT_DM1_PKT_TYPE && llid_val == ACLC_LLID) {
    lmp_test_mode_rx_packet_handler(ulc, pkb);
  }
}
```

### 3. Test Mode Packet Types

```c
// Test Mode Packet Type Management
void test_mode_set_pkt_type(uint16_t *acl_pkt_type, uint8_t ptt, uint8_t pkt_type)
{
  // Always enable DM1
  *acl_pkt_type = PTYPE_DM1_MAY_BE_USED;
  
  if (ptt == BT_EDR_MODE) {
    for (size_t i = 0; i < ARRAY_SIZE(edr_mappings); i++) {
      if (edr_mappings[i].pkt_type == pkt_type) {
        *acl_pkt_type |= edr_mappings[i].mask;
        return;
      }
    }
  } else if (ptt == BT_BR_MODE) {
    for (size_t i = 0; i < ARRAY_SIZE(br_mappings); i++) {
      if (br_mappings[i].pkt_type == pkt_type) {
        *acl_pkt_type |= br_mappings[i].mask;
        return;
      }
    }
  }
}
```

## Test Mode Data Generation

### 1. Test Data Generation Engine

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Data Generation Engine                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Pattern     │  │ Payload     │  │ Packet      │            │
│  │ Generation  │  │ Generation  │  │ Formation   │            │
│  │             │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Data Generation Features                     │ │
│  │  • Test Pattern Generation (0x00, 0xFF, 0x55, 0xF0)      │ │
│  │  • PRBS Sequence Generation                               │ │
│  │  • Test Packet Formation                                  │ │
│  │  • Payload Data Management                                │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Test Data Generation Functions

```c
// Test Data Generation
const uint8_t pn9_data[] = {
  0xFF, 0xC1, 0xFB, 0xE8, 0x4C, 0x90, 0x72, 0x8B, 0xE7, 0xB3, 0x51, 0x89, 0x63, 0xAB, 0x23, 0x23,
  // ... PRBS sequence data
};

static const uint8_t transmitter_payload_types[] = {
    INVALID_SEQUENCE, // TRANSMITTER_TEST_INVALID
    SEQUENCE_0,      // TRANSMITTER_TEST_0
    SEQUENCE_1,      // TRANSMITTER_TEST_1
    SEQUENCE_2,      // TRANSMITTER_TEST_2
    SEQUENCE_PRBS,   // TRANSMITTER_TEST_PRBS
    INVALID_SEQUENCE, // TRANSMITTER_TEST_INVALID
    INVALID_SEQUENCE, // TRANSMITTER_TEST_INVALID
    INVALID_SEQUENCE, // TRANSMITTER_TEST_INVALID
    INVALID_SEQUENCE, // TRANSMITTER_TEST_INVALID
    SEQUENCE_F0      // TRANSMITTER_TEST_F0
};

// Payload Generation
static void fill_payload(uint8_t *data, uint32_t len, uint8_t payload_byte, uint8_t payload_type, uint8_t *payload_data)
{
    for (uint32_t i = 0; i < len; i++) {
        if (payload_type == SEQUENCE_PRBS) {
            data[i] = payload_data[i]; // PRBS sequence
        } else {
            data[i] = payload_byte;
        }
    }
}
```

### 3. Test Packet Formation

```c
// Test Packet Formation
void form_tx_test_pkt(test_mode_params_t *test_mode_params, uint8_t payload_type)
{
    uint8_t payload_byte = 0;
    uint32_t pkt_data_len;
    pkb_t *test_pkb = NULL;
    uint8_t *payload_data = NULL;
    
    test_pkb = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
    pkt_data_len = MIN((uint32_t)test_mode_params->len_of_seq, (uint32_t)(pkb_chunk_type_medium - 4));
    
    if (test_mode_params->test_mode_type == ACL_TEST_MODE) {
        test_pkb->pkt_len = test_mode_params->len_of_seq + 1;
        test_pkb->data[0] = 2;
    } else {
        test_pkb->pkt_len = test_mode_params->len_of_seq;
    }

    switch (payload_type) {
        case SEQUENCE_0:  payload_byte = 0x00; break;
        case SEQUENCE_1:  payload_byte = 0xFF; break;
        case SEQUENCE_2:  payload_byte = 0x55; break;
        case SEQUENCE_F0: payload_byte = 0xF0; break;
        case SEQUENCE_PRBS:
        default:
            payload_data = (uint8_t *)pn9_data;
            break;
    }

    // Fill payload
    fill_payload(&test_pkb->data[(test_mode_params->test_mode_type == ACL_TEST_MODE) ? 1 : 0],
                 pkt_data_len, payload_byte, payload_type, payload_data);
    
    test_mode_params->tx_pkt = test_pkb;
}
```

## Integration Architecture

### 1. LMP Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              LMP Integration Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ LMP         │  │ Test        │  │ Test        │            │
│  │ Command     │  │ Mode        │  │ Mode        │            │
│  │ Processing  │  │ Handlers    │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              LMP Integration Features                     │ │
│  │  • LMP Command Processing                                 │ │
│  │  • Test Mode Protocol Handlers                            │ │
│  │  • Test Mode State Management                             │ │
│  │  • Test Configuration Processing                          │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. HCI Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              HCI Integration Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ HCI         │  │ Test        │  │ Test        │            │
│  │ Command     │  │ Mode        │  │ Mode        │            │
│  │ Processing  │  │ Activation  │  │ Events      │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              HCI Integration Features                     │ │
│  │  • HCI Command Processing                                 │ │
│  │  • Test Mode Activation                                   │ │
│  │  • Test Mode Event Generation                             │ │
│  │  • Host Interface Management                              │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Device Management Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              Device Management Integration                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ ACL         │  │ Test        │  │ Power       │            │
│  │ Peer        │  │ Mode        │  │ Control     │            │
│  │ Management  │  │ Management  │  │ Integration │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Device Management Features                   │ │
│  │  • ACL Peer Information Management                        │ │
│  │  • Test Mode Parameter Management                         │ │
│  │  • Power Control Integration                              │ │
│  │  • Link Management Integration                            │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Performance Architecture

### 1. Performance Optimization

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode Performance Architecture               │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Test        │  │ Memory      │  │ CPU         │            │
│  │ Execution   │  │ Management  │  │ Optimization│            │
│  │ Optimization│  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Performance Features                         │ │
│  │  • Efficient Test Execution                               │ │
│  │  • Optimized Memory Usage                                 │ │
│  │  • CPU Resource Management                                │ │
│  │  • Real-time Test Performance                             │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Test Execution Optimization

```c
// Test Execution Optimization
void dm_start_transmitter_test(ulc_t *ulc, uint8_t peer_id, uint8_t payload_type)
{
    // Set ptt based on link_type
    acl_dev_mgmt_info->ptt = (test_mode_params->link_type == 2) ? BT_EDR_MODE : BT_BR_MODE;
    
    test_mode_set_pkt_type(&acl_link_mgmt_info->acl_pkt_type, acl_dev_mgmt_info->ptt, test_mode_params->pkt_type);
    form_tx_test_pkt(test_mode_params, payload_type);
    acl_link_mgmt_info->test_type = TRANSMITTER_TEST;
    
    // Flush the data queue
    flush_queue(&acl_peer_info->tx_acld_q);
    
    add_pkt_to_acld_q(ulc, test_mode_params->tx_pkt, peer_id);
}
```

### 3. Memory Management

```c
// Memory Management for Test Packets
void form_tx_test_pkt(test_mode_params_t *test_mode_params, uint8_t payload_type)
{
    test_pkb = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
    
    // Handle extra data if len_of_seq > chunk size
    uint32_t remaining = test_mode_params->len_of_seq - pkt_data_len;
    while (remaining > 0) {
        test_pkb = pkb_alloc(&btc.pkb_pool[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
        uint32_t fill_len = MIN(remaining, (uint32_t)(pkb_chunk_type_medium - 4));
        fill_payload(test_pkb->data, fill_len, payload_byte, payload_type, payload_data);
        test_pkb->pkt_len = fill_len;
        remaining -= fill_len;
    }
}
```

## Error Handling Architecture

### 1. Error Detection and Recovery

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode Error Handling Architecture            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Error       │  │ Error       │  │ Error       │            │
│  │ Detection   │  │ Recovery    │  │ Reporting   │            │
│  │ Engine      │  │ Engine      │  │ Engine      │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Error Types                                  │ │
│  │  • Test Mode Activation Errors                            │ │
│  │  • Test Configuration Errors                              │ │
│  │  • Test Execution Errors                                  │ │
│  │  • Test Data Generation Errors                            │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Error Recovery Strategies

```c
// Error Recovery in Test Mode
void lmp_test_activate_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  if (IS_HOST_DEV_TEST_MODE_ENABLED(btc_dev_mgmt_info)) {
    SET_TEST_MODE_ENABLED(acl_link_mgmt_info);
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  } else {
    lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
  }
}

void lmp_test_control_rx_handler(ulc_t *ulc, pkb_t *pkb)
{
  // Send NOT_ACCEPTED as per spec - test_control not processed in normal operation
  lmp_input_params[1] = LMP_PDU_NOT_ALLOWED;
  call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params);
}
```

### 3. Fallback Mechanisms

```c
// Fallback Mechanisms
void lmp_process_test_config(ulc_t *ulc, uint8_t peer_id)
{
  switch (test_mode_params->test_scenario) {
    case PAUSE_TEST_MODE: {
      flush_queue(&acl_peer_info->tx_acld_q);
      test_mode_params->test_started = TEST_NOT_STARTED;
      acl_link_mgmt_info->test_type = TEST_IN_PAUSE;
      break;
    }
    case EXIT_TEST_MODE: {
      acl_link_mgmt_info->test_in_progress = TEST_NOT_IN_PROGRESS;
      acl_link_mgmt_info->dev_in_test_mode = TEST_MODE_DISABLED;
      btc_dev_info->btc_dev_mgmt_info.testmode_block_host_pkts = 0;
      break;
    }
    default: {
      // Handle unknown test scenarios
      break;
    }
  }
}
```

## Future Extensions

### 1. Planned Enhancements

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode Future Extensions                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Enhanced    │  │ Advanced    │  │ Extended    │            │
│  │ Test        │  │ Test        │  │ Test        │            │
│  │ Scenarios   │  │ Automation  │  │ Validation  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Extension Features                           │ │
│  │  • Enhanced Test Scenarios                                │ │
│  │  • Advanced Test Automation                               │ │
│  │  • Extended Test Validation                               │ │
│  │  • Machine Learning-based Test Optimization               │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Implementation Roadmap

```
┌─────────────────────────────────────────────────────────────────┐
│              Test Mode Implementation Roadmap                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Phase 1     │  │ Phase 2     │  │ Phase 3     │            │
│  │ Enhanced    │  │ Advanced    │  │ Extended    │            │
│  │ Scenarios   │  │ Automation  │  │ Features    │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Development Phases                           │ │
│  │  • Phase 1: Enhanced Test Scenarios                       │ │
│  │  • Phase 2: Advanced Test Automation                      │ │
│  │  • Phase 3: Extended Test Validation                      │ │
│  │  • Phase 4: Machine Learning Integration                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Conclusion

The Test Mode LMP Procedures architecture in ULC provides a comprehensive framework for managing Bluetooth test mode operations through Link Manager Protocol commands. The architecture is designed with the following key principles:

### Key Architectural Strengths

1. **Comprehensive Testing**: Support for all Bluetooth test scenarios and modes
2. **Protocol Compliance**: Full compliance with Bluetooth test mode specifications
3. **Flexible Configuration**: Configurable test parameters and scenarios
4. **Data Integrity**: Reliable test data generation and validation
5. **Performance Validation**: Comprehensive performance and functionality testing

### Architectural Benefits

- **Comprehensive Testing**: Complete coverage of Bluetooth test scenarios
- **Protocol Compliance**: Full compliance with Bluetooth specifications
- **Flexible Configuration**: Configurable test parameters and scenarios
- **Data Integrity**: Reliable test data generation and validation
- **Extensibility**: Modular design supporting future enhancements

### Current Implementation Status

- **Test Mode Protocol**: Complete test mode protocol support with all LMP commands
- **Test Scenarios**: Full support for transmitter, receiver, and loopback tests
- **State Management**: Complete test mode state management and transitions
- **Data Generation**: Full test data generation with various patterns
- **Packet Processing**: Complete test mode packet processing and handling
- **Error Handling**: Robust error handling and recovery mechanisms

### Key Components

- **Test Mode Protocol Manager**: Complete test mode protocol management with LMP integration
- **Test Scenario Manager**: Comprehensive test scenario management and processing
- **State Manager**: Test mode state management and transitions
- **Data Generator**: Test data generation with various patterns and sequences
- **Packet Processor**: Test mode packet processing and handling
- **Error Handler**: Comprehensive error handling and recovery

### Future Development

- **Enhanced Scenarios**: Advanced test scenarios and validation
- **Test Automation**: Automated test execution and validation
- **Extended Validation**: Extended test validation and reporting
- **Machine Learning**: Machine learning-based test optimization

This architecture provides a solid foundation for implementing comprehensive test mode functionality in Bluetooth Classic systems, ensuring complete testing coverage and validation through intelligent test scenario management and reliable data generation.
