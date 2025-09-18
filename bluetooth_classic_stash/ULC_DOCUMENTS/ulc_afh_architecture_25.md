# AFH (Adaptive Frequency Hopping) Architecture in ULC

## Executive Summary

The AFH (Adaptive Frequency Hopping) architecture in ULC provides a comprehensive framework for managing adaptive frequency hopping in Bluetooth Classic systems. This architecture enables dynamic channel selection based on channel quality assessment, interference avoidance, and regulatory compliance, ensuring optimal performance in various RF environments.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [AFH Core Components](#afh-core-components)
3. [Channel Classification System](#channel-classification-system)
4. [AFH Protocol Management](#afh-protocol-management)
5. [Channel Map Operations](#channel-map-operations)
6. [AFH State Management](#afh-state-management)
7. [Timing and Synchronization](#timing-and-synchronization)
8. [Integration Architecture](#integration-architecture)
9. [Performance Architecture](#performance-architecture)
10. [Error Handling Architecture](#error-handling-architecture)
11. [Future Extensions](#future-extensions)

## Architecture Overview

### High-Level AFH Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    AFH Architecture                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ AFH         │  │ Channel     │            │
│  │ Assessment  │  │ Protocol    │  │ Map         │            │
│  │ Engine      │  │ Manager     │  │ Manager     │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Timing      │  │ State       │  │ LMP         │            │
│  │ Manager     │  │ Manager     │  │ Interface   │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              AFH Core Engine                              │ │
│  │  • Channel Quality Assessment                             │ │
│  │  • Interference Detection                                 │ │
│  │  • Dynamic Channel Selection                              │ │
│  │  • Regulatory Compliance                                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Core Design Principles

1. **Adaptive Channel Selection**: Dynamic selection of channels based on quality assessment
2. **Interference Avoidance**: Automatic avoidance of channels with high interference
3. **Regulatory Compliance**: Compliance with local regulatory requirements
4. **Performance Optimization**: Optimization of throughput and reliability
5. **Real-time Adaptation**: Real-time adaptation to changing RF conditions

## AFH Core Components

### 1. AFH Classification Engine

```c
// Channel Classification Definitions
#define GOOD_CHANNEL_PAIR 0x03
#define BAD_GOOD_CHANNEL_PAIR 0x02
#define GOOD_BAD_CHANNEL_PAIR 0x01
#define GOOD_CHANNEL 0x03
#define UNKNOWN_CHANNEL 0x02
#define BAD_CHANNEL 0x01

// AFH Configuration
#define AFH_DEFAULT_MIN_INTERVAL 0xbb78
#define AFH_DEFAULT_MAX_INTERVAL 0xbb80
#define AFH_CHANNEL_MAP_DEFAULT_LOW 0xFFFFFFFF
#define AFH_CHANNEL_MAP_DEFAULT_HIGH 0xFFFFFFFF
#define AFH_CHANNEL_MAP_DEFAULT_UPPER 0x7FFF
```

### 2. AFH Core Functions

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Core Functions                               │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ AFH         │  │ Channel     │            │
│  │ Classification│  │ Management │  │ Map         │            │
│  │             │  │             │  │ Operations  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Core Functionality                           │ │
│  │  • Channel Quality Assessment                             │ │
│  │  • AFH Protocol Management                                │ │
│  │  • Channel Map Operations                                 │ │
│  │  • Timing and Synchronization                             │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. AFH Data Structures

```c
// AFH Channel Map Operations
typedef enum {
  CHANNEL_MAP_OP_AND,           // dst[i] &= src[i]
  CHANNEL_MAP_OP_OR,            // dst[i] |= src[i]
  CHANNEL_MAP_OP_COPY,          // dst[i] = src[i]
  CHANNEL_MAP_OP_AND_COMBINE,   // dst[i] = (src1[i] & src2[i])
  CHANNEL_MAP_OP_SET_DEFAULT    // Set to default AFH values
} channel_map_operation_t;

// AFH Channel Classification Map (10 bytes)
uint8_t afh_channel_classification[CHANNEL_MAP_BYTES];
uint8_t afh_channel_map_new[CHANNEL_MAP_BYTES];
```

## Channel Classification System

### 1. Channel Assessment Engine

```
┌─────────────────────────────────────────────────────────────────┐
│              Channel Assessment Engine                        │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ Quality     │  │ Classification│            │
│  │ Monitoring  │  │ Assessment  │  │ Engine       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Assessment Features                          │ │
│  │  • Real-time Channel Monitoring                           │ │
│  │  • Quality Metrics Collection                             │ │
│  │  • Interference Detection                                 │ │
│  │  • Channel Classification                                 │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Channel Classification Process

```
┌─────────────────────────────────────────────────────────────────┐
│              Channel Classification Process                   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ Quality     │  │ Classification│            │
│  │ Monitoring  │  │ Analysis    │  │ Decision     │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Classification Steps                         │ │
│  │  1. Monitor Channel Quality                               │ │
│  │  2. Analyze Interference Levels                           │ │
│  │  3. Classify Channels (Good/Bad/Unknown)                  │ │
│  │  4. Update Channel Map                                    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Channel Classification Functions

```c
// Channel Classification Functions
void lmp_start_channel_classification(ulc_t *ulc, pkb_t *pkb);
void channel_classification_timeout_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_central_send_classification(ulc_t *ulc, pkb_t *pkb);
void lmp_update_afh_channel_map(acl_dev_mgmt_info_t *acl_dev_mgmt_info, 
                                uint8_t *temp_map, uint8_t update_classification);
void bt_calculate_classification_channel_map(uint8_t *temp_map);
```

## AFH Protocol Management

### 1. LMP AFH Commands

```
┌─────────────────────────────────────────────────────────────────┐
│              LMP AFH Commands                                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ SET_AFH     │  │ CHANNEL_    │  │ CHANNEL_    │            │
│  │             │  │ CLASSIFICATION│  │ CLASSIFICATION│            │
│  │             │  │ _REQ        │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              LMP AFH Command Features                     │ │
│  │  • SET_AFH: Set AFH mode and channel map                  │ │
│  │  • CHANNEL_CLASSIFICATION_REQ: Request channel classification│ │
│  │  • CHANNEL_CLASSIFICATION: Send channel classification    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. LMP AFH Command Parameters

```yaml
# LMP SET_AFH Command
lmp_set_afh:
  OPCODE: 60
  parameters:
    afh_instant: 4        # AFH instant (4 bytes)
    afh_mode: 1          # AFH mode (1 byte)
    afh_channel_map: 10  # AFH channel map (10 bytes)

# LMP CHANNEL_CLASSIFICATION_REQ Command
lmp_channel_classification_req:
  OPCODE: 127
  EXTENDED_OPCODE: 16
  parameters:
    afh_reporting_mode: 1    # AFH reporting mode (1 byte)
    afh_min_interval: 2      # AFH minimum interval (2 bytes)
    afh_max_interval: 2      # AFH maximum interval (2 bytes)

# LMP CHANNEL_CLASSIFICATION Command
lmp_channel_classification:
  OPCODE: 127
  EXTENDED_OPCODE: 17
  parameters:
    afh_channel_classification: 10  # AFH channel classification (10 bytes)
```

### 3. AFH Protocol Handlers

```c
// AFH Protocol Handlers
void lmp_set_afh_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_set_afh_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_channel_classification_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_channel_classification_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_channel_classification_req_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_channel_classification_rx_handler(ulc_t *ulc, pkb_t *pkb);
```

## Channel Map Operations

### 1. Channel Map Management

```
┌─────────────────────────────────────────────────────────────────┐
│              Channel Map Management                           │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ Channel     │  │ Channel     │            │
│  │ Map         │  │ Map         │  │ Map         │            │
│  │ Operations  │  │ Validation  │  │ Updates     │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Channel Map Features                         │ │
│  │  • Bit-wise Operations (AND, OR, COPY)                    │ │
│  │  • Channel Map Validation                                 │ │
│  │  • Dynamic Channel Map Updates                            │ │
│  │  • Default Channel Map Management                         │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Channel Map Operations

```c
// Channel Map Operation Macros
#define CHANNEL_MAP_AND(dst, src) \
  perform_channel_map_operation(dst, src, NULL, CHANNEL_MAP_OP_AND)

#define CHANNEL_MAP_OR(dst, src) \
  perform_channel_map_operation(dst, src, NULL, CHANNEL_MAP_OP_OR)

#define CHANNEL_MAP_COPY(dst, src) \
  perform_channel_map_operation(dst, src, NULL, CHANNEL_MAP_OP_COPY)

#define CHANNEL_MAP_AND_COMBINE(dst, src1, src2) \
  perform_channel_map_operation(dst, src1, src2, CHANNEL_MAP_OP_AND_COMBINE)

#define CHANNEL_MAP_SET_DEFAULT(dst) \
  perform_channel_map_operation(dst, NULL, NULL, CHANNEL_MAP_OP_SET_DEFAULT)

// Channel Map Operation Function
void perform_channel_map_operation(uint8_t *dst_map, const uint8_t *src1_map, 
                                  const uint8_t *src2_map, channel_map_operation_t operation);
```

### 3. Channel Pair Operations

```c
// Channel Pair Macros
#define SET_CHANNEL_PAIR_GOOD(map, byte_pos, bit_pos) \
  ((map)[byte_pos] |= (BIT(bit_pos) | BIT((bit_pos + 1))))

#define SET_CHANNEL_PAIR_BAD(map, byte_pos, bit_pos) \
  ((map)[byte_pos] &= ~(BIT(bit_pos) | BIT((bit_pos + 1))))

#define GET_CHANNEL_PAIR_CLASSIFICATION(map, byte_pos, bit_pos) \
  (((map)[byte_pos] >> (bit_pos)) & 0x03)

#define IS_GOOD_CHANNEL_PAIR(channel_pair) \
  (channel_pair == GOOD_CHANNEL_PAIR)

#define IS_UNKNOWN_CHANNEL(channel_pair) ((channel_pair) == UNKNOWN_CHANNEL)
#define IS_BAD_CHANNEL(channel_pair) ((channel_pair) == BAD_CHANNEL)
```

## AFH State Management

### 1. AFH State Machine

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH State Machine                                │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ AFH         │  │ Channel     │  │ AFH         │            │
│  │ Disabled    │  │ Assessment  │  │ Enabled     │            │
│  │ State       │  │ State       │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
│         │                │                │                   │
│         │                │                │                   │
│         ▼                ▼                ▼                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ AFH         │  │ Channel     │            │
│  │ Map         │  │ Update      │  │ Map         │            │
│  │ Update      │  │ State       │  │ Validation  │            │
│  │ State       │  │             │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────────┘
```

### 2. AFH Status Flags

```c
// AFH Status Flags
#define AFH_ENABLED                    // AFH is enabled
#define CHANNEL_CLASSIFICATION_REQ_SENT // Channel classification request sent
#define CHANNEL_CLASSIFICATION_RECVD   // Channel classification received
#define UPDATE_AFH_PENDING            // AFH update pending
#define UPDATE_AFH_CLASSIFICATION_PENDING // AFH classification update pending

// AFH Status Checking Macros
#define IS_AFH_ENABLED_IN_RX_PKT(bt_pkt_start) \
  (GET_LMP_SET_AFH_PARAMETER_AFH_MODE(bt_pkt_start) == AFH_ENABLE)

#define IS_AFH_REPORTING_ENABLED(acl_dev_mgmt_info) \
  (acl_dev_mgmt_info->afh_reporting_mode == AFH_REPORTING_ENABLED)
```

### 3. AFH State Transitions

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH State Transitions                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ AFH         │  │ Channel     │  │ AFH         │            │
│  │ Disable     │  │ Assessment  │  │ Enable      │            │
│  │ Request     │  │ Request     │  │ Request     │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              State Transition Triggers                    │ │
│  │  • LMP SET_AFH Command                                    │ │
│  │  • Channel Classification Request                         │ │
│  │  • Channel Assessment Timeout                             │ │
│  │  • AFH Instant Reached                                    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Timing and Synchronization

### 1. AFH Timing Management

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Timing Management                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ AFH         │  │ AFH         │  │ Clock       │            │
│  │ Instant     │  │ Interval    │  │ Management  │            │
│  │ Calculation │  │ Management  │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Timing Features                              │ │
│  │  • AFH Instant Calculation                                │ │
│  │  • AFH Interval Management                                │ │
│  │  • Clock Synchronization                                  │ │
│  │  • Timing Accuracy                                        │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. AFH Timing Functions

```c
// AFH Timing Macros
#define AFH_INSTANT_CLOCK_MASK 0x0FFFFFFF
#define CALCULATE_AFH_INSTANT(master_clock) \
  (((master_clock) & AFH_INSTANT_CLOCK_MASK) + 1 + AFH_INTERVAL_500_MS) & ~BIT(0)

#define CALCULATE_AFH_CLASSIFICATION_INSTANT(afh_interval) \
  (get_clock_from_lpw() + (afh_interval))

// AFH Timing Functions
uint32_t get_clock_from_lpw();  // Get current clock from LPW
```

### 3. AFH Interval Management

```c
// AFH Interval Configuration
#define AFH_DEFAULT_MIN_INTERVAL 0xbb78  // Default minimum interval
#define AFH_DEFAULT_MAX_INTERVAL 0xbb80  // Default maximum interval

// AFH Interval Calculation
if (acl_dev_mgmt_info->afh_max_interval - acl_dev_mgmt_info->afh_min_interval) {
  GENERATE_AFH_RANDOM_NUM(&random_num);
  acl_dev_mgmt_info->afh_interval = 
    acl_dev_mgmt_info->afh_min_interval + 
    ((random_num & 0xFFFF) % ((acl_dev_mgmt_info->afh_max_interval - 
                               acl_dev_mgmt_info->afh_min_interval)));
} else {
  acl_dev_mgmt_info->afh_interval = acl_dev_mgmt_info->afh_min_interval;
}
```

## Integration Architecture

### 1. LMP Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              LMP Integration Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ LMP         │  │ AFH         │  │ Channel     │            │
│  │ Command     │  │ Protocol    │  │ Management  │            │
│  │ Processing  │  │ Handlers    │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              LMP Integration Features                     │ │
│  │  • LMP Command Processing                                 │ │
│  │  • AFH Protocol Handlers                                  │ │
│  │  • Channel Management Integration                         │ │
│  │  • State Synchronization                                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. ULC Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              ULC Integration Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ ULC         │  │ Event       │  │ Scheduler   │            │
│  │ Context     │  │ Loop        │  │ Integration │            │
│  │ Management  │  │ Integration │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              ULC Integration Features                     │ │
│  │  • ULC Context Access                                     │ │
│  │  • Event Loop Integration                                 │ │
│  │  • Scheduler Coordination                                 │ │
│  │  • Resource Management                                    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Device Management Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              Device Management Integration                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ ACL         │  │ Device      │  │ Host        │            │
│  │ Peer        │  │ Management  │  │ Interface   │            │
│  │ Management  │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Device Management Features                   │ │
│  │  • ACL Peer Information Management                        │ │
│  │  • Device State Management                                │ │
│  │  • Host Interface Integration                             │ │
│  │  • Feature Capability Management                          │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Performance Architecture

### 1. Performance Optimization

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Performance Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ Memory      │  │ CPU         │            │
│  │ Assessment  │  │ Management  │  │ Optimization│            │
│  │ Optimization│  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Performance Features                         │ │
│  │  • Efficient Channel Assessment                           │ │
│  │  • Optimized Memory Usage                                 │ │
│  │  • CPU Resource Management                                │ │
│  │  • Real-time Performance                                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Channel Assessment Performance

```c
// Channel Assessment Performance Functions
void bt_calculate_classification_channel_map(uint8_t *temp_map)
{
  // For now, just set all channels as good
  for (int i = 0; i < CHANNEL_MAP_BYTES; i++) {
    temp_map[i] = 0xFF;
  }
}

uint8_t get_no_of_ones(uint8_t *map, uint8_t max_channels)
{
  uint8_t count = 0;
  for (uint8_t i = 0; i < max_channels; i++) {
    if (IS_BIT_SET_IN_MAP(map, i)) {
      count++;
    }
  }
  return count;
}
```

### 3. Memory Management

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Memory Management                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Channel     │  │ Channel     │  │ Memory      │            │
│  │ Map         │  │ Classification│  │ Pools       │            │
│  │ Storage     │  │ Storage     │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Memory Features                              │ │
│  │  • Efficient Channel Map Storage                          │ │
│  │  • Channel Classification Storage                         │ │
│  │  • Memory Pool Management                                 │ │
│  │  • Memory Usage Optimization                              │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Error Handling Architecture

### 1. Error Detection and Recovery

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Error Handling Architecture                  │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Error       │  │ Error       │  │ Error       │            │
│  │ Detection   │  │ Recovery    │  │ Reporting   │            │
│  │ Engine      │  │ Engine      │  │ Engine      │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Error Types                                  │ │
│  │  • Channel Classification Errors                          │ │
│  │  • AFH Protocol Errors                                    │ │
│  │  • Channel Map Errors                                     │ │
│  │  • Timing Synchronization Errors                          │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Error Recovery Strategies

```c
// Error Recovery in Channel Classification
if (IS_CHANNEL_ASSESSMENT_ENABLED(btc_dev_mgmt_info)) {
  // Channel assessment enabled - use calculated channel map
  bt_calculate_classification_channel_map(temp_map);
} else {
  // Channel assessment disabled - use default or host classification
  if (btc_dev_mgmt_info->host_ch_classification_cmd == 1) {
    // Use host classification
    for (uint8_t i = 0; i <= NUM_OF_MAX_CHANNELS; i += 2) {
      // Process host classification
    }
  } else {
    // Use default classification
    memcpy(acl_dev_mgmt_info->afh_channel_classification, 
           acl_dev_mgmt_info->afh_channel_map_new, CHANNEL_MAP_BYTES);
  }
}
```

### 3. Fallback Mechanisms

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Fallback Mechanisms                          │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Default     │  │ Host        │  │ Minimum     │            │
│  │ Channel     │  │ Classification│  │ Channel     │            │
│  │ Map         │  │ Fallback    │  │ Count       │            │
│  │ Fallback    │  │             │  │ Fallback    │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Fallback Features                            │ │
│  │  • Default Channel Map Usage                              │ │
│  │  • Host Classification Fallback                           │ │
│  │  • Minimum Channel Count Enforcement                      │ │
│  │  • Graceful Degradation                                   │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Future Extensions

### 1. Planned Enhancements

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Future Extensions                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Enhanced    │  │ Advanced    │  │ Extended    │            │
│  │ Channel     │  │ Interference│  │ Regulatory  │            │
│  │ Assessment  │  │ Detection   │  │ Compliance  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Extension Features                           │ │
│  │  • Enhanced Channel Assessment Algorithms                 │ │
│  │  • Advanced Interference Detection                        │ │
│  │  • Extended Regulatory Compliance                         │ │
│  │  • Machine Learning-based Channel Selection               │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Implementation Roadmap

```
┌─────────────────────────────────────────────────────────────────┐
│              AFH Implementation Roadmap                       │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Phase 1     │  │ Phase 2     │  │ Phase 3     │            │
│  │ Enhanced    │  │ Advanced    │  │ Extended    │            │
│  │ Assessment  │  │ Detection   │  │ Features    │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Development Phases                           │ │
│  │  • Phase 1: Enhanced Channel Assessment                   │ │
│  │  • Phase 2: Advanced Interference Detection               │ │
│  │  • Phase 3: Extended Regulatory Features                  │ │
│  │  • Phase 4: Machine Learning Integration                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Conclusion

The AFH (Adaptive Frequency Hopping) architecture in ULC provides a comprehensive framework for managing adaptive frequency hopping in Bluetooth Classic systems. The architecture is designed with the following key principles:

### Key Architectural Strengths

1. **Adaptive Channel Selection**: Dynamic selection of channels based on quality assessment
2. **Interference Avoidance**: Automatic avoidance of channels with high interference
3. **Regulatory Compliance**: Compliance with local regulatory requirements
4. **Performance Optimization**: Optimization of throughput and reliability
5. **Real-time Adaptation**: Real-time adaptation to changing RF conditions

### Architectural Benefits

- **Interference Mitigation**: Effective mitigation of interference through channel avoidance
- **Performance Optimization**: Optimized performance through intelligent channel selection
- **Regulatory Compliance**: Automatic compliance with regulatory requirements
- **Real-time Adaptation**: Real-time adaptation to changing RF conditions
- **Extensibility**: Modular design supporting future enhancements

### Current Implementation Status

- **Channel Classification**: Complete channel classification system with quality assessment
- **AFH Protocol**: Full AFH protocol support with LMP command handling
- **Channel Map Management**: Comprehensive channel map operations and management
- **Timing Management**: Complete AFH timing and synchronization support
- **Error Handling**: Robust error handling and fallback mechanisms

### Key Components

- **Channel Assessment Engine**: Real-time channel quality assessment and classification
- **AFH Protocol Manager**: Complete AFH protocol management with LMP integration
- **Channel Map Manager**: Efficient channel map operations and updates
- **Timing Manager**: AFH timing calculation and synchronization
- **State Manager**: AFH state management and transitions
- **Error Handler**: Comprehensive error handling and recovery

### Future Development

- **Enhanced Assessment**: Advanced channel assessment algorithms
- **Interference Detection**: Sophisticated interference detection and mitigation
- **Regulatory Features**: Extended regulatory compliance features
- **Machine Learning**: Machine learning-based channel selection

This architecture provides a solid foundation for implementing comprehensive AFH functionality in Bluetooth Classic systems, ensuring optimal performance through intelligent channel selection and interference avoidance while maintaining regulatory compliance.
