# Sniff and Sniff Subrating Architecture in ULC

## Executive Summary

The Sniff and Sniff Subrating architecture in ULC provides a comprehensive framework for managing power-saving modes in Bluetooth Classic systems. This architecture enables devices to enter low-power states while maintaining connectivity, with advanced subrating capabilities for dynamic adjustment of sniff intervals based on traffic patterns and latency requirements.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Sniff Core Components](#sniff-core-components)
3. [Sniff Protocol Management](#sniff-protocol-management)
4. [Sniff Subrating System](#sniff-subrating-system)
5. [Sniff State Management](#sniff-state-management)
6. [Timing and Synchronization](#timing-and-synchronization)
7. [Anchor Point Management](#anchor-point-management)
8. [Integration Architecture](#integration-architecture)
9. [Performance Architecture](#performance-architecture)
10. [Error Handling Architecture](#error-handling-architecture)
11. [Future Extensions](#future-extensions)

## Architecture Overview

### High-Level Sniff Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    Sniff Architecture                          │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Sniff       │  │ Sniff       │  │ Anchor      │            │
│  │ Protocol    │  │ Subrating   │  │ Point       │            │
│  │ Manager     │  │ Manager     │  │ Manager     │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Timing      │  │ State       │  │ Power       │            │
│  │ Manager     │  │ Manager     │  │ Management  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Sniff Core Engine                            │ │
│  │  • Power Saving Management                                │ │
│  │  • Traffic Pattern Analysis                               │ │
│  │  • Dynamic Interval Adjustment                            │ │
│  │  • Multi-Connection Coordination                          │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Core Design Principles

1. **Power Efficiency**: Maximize power savings while maintaining connectivity
2. **Traffic Adaptation**: Dynamic adjustment based on traffic patterns
3. **Multi-Connection Support**: Coordination across multiple ACL connections
4. **Timing Precision**: Precise timing for sniff anchor points and intervals
5. **Latency Optimization**: Balance between power savings and latency requirements

## Sniff Core Components

### 1. Sniff Parameters Structure

```c
typedef struct sniff_params_s {
  uint8_t init_flag;                    // Initialization flag
  uint16_t d_sniff;                     // Sniff delay (slots)
  uint16_t t_sniff;                     // Sniff interval (slots)
  uint16_t sniff_attempt;               // Sniff attempt duration (slots)
  uint16_t sniff_tout;                  // Sniff timeout (slots)
  
  // Sniff Subrating Parameters
  uint8_t max_remote_sniff_subrate;     // Maximum remote sniff subrate
  uint8_t max_local_sniff_subrate;      // Maximum local sniff subrate
  uint16_t min_remote_sniff_subrating_tout; // Minimum remote sniff subrating timeout
  uint16_t min_local_sniff_subrating_tout;  // Minimum local sniff subrating timeout
  uint16_t remote_max_sniff_latency;    // Remote maximum sniff latency
  uint16_t subrating_tout_rcvd;         // Subrating timeout received
  uint32_t sniff_subrating_instant;     // Sniff subrating instant
  uint8_t sniff_subrate_params_set;     // Sniff subrate parameters set flag
  uint8_t sniff_subrating_enable;       // Sniff subrating enable flag
} sniff_params_t;
```

### 2. Sniff Core Functions

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Core Functions                             │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Sniff       │  │ Sniff       │  │ Sniff       │            │
│  │ Request     │  │ Response    │  │ Management  │            │
│  │ Handlers    │  │ Handlers    │  │ Functions   │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Core Functionality                           │ │
│  │  • Sniff Request/Response Processing                      │ │
│  │  • Sniff Mode Entry/Exit                                 │ │
│  │  • Sniff Parameter Management                             │ │
│  │  • Sniff State Transitions                                │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Sniff State Definitions

```c
// Sniff State Definitions
#define CONNECTED_ACTIVE_MODE          0x00
#define CONNECTED_SNIFF_MODE           0x02
#define CONNECTED_SNIFF_TRANSITION_MODE 0x03

// Sniff Mode Control
#define SNIFF_MODE_ENABLE              1
#define SNIFF_MODE_DISABLE             0
#define ENABLE_SNIFF_MODE              BIT(0)

// Sniff Subrating Control
#define SNIFF_SUBRATE_PARAMS_SET       1
#define SNIFF_SUBR_ENABLE              1
#define MAX_SNIFF_SUB_RATE             0xFF
```

## Sniff Protocol Management

### 1. LMP Sniff Commands

```
┌─────────────────────────────────────────────────────────────────┐
│              LMP Sniff Commands                               │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ SNIFF_REQ   │  │ UNSNIFF_REQ │  │ SUPERVISION │            │
│  │             │  │             │  │ _TIMEOUT    │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              LMP Sniff Command Features                   │ │
│  │  • SNIFF_REQ: Request sniff mode entry                   │ │
│  │  • UNSNIFF_REQ: Request sniff mode exit                  │ │
│  │  • SUPERVISION_TIMEOUT: Set link supervision timeout     │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. LMP Sniff Command Parameters

```yaml
# LMP SNIFF_REQ Command
lmp_sniff_req:
  OPCODE: 23
  parameters:
    timing_control_flags_1: 1    # Timing control flags 1 (1 byte)
    D_sniff: 2                   # Sniff delay (2 bytes)
    T_sniff: 2                   # Sniff interval (2 bytes)
    timing_control_flags_2: 2    # Timing control flags 2 (2 bytes)
    sniff_timeout: 2             # Sniff timeout (2 bytes)

# LMP UNSNIFF_REQ Command
lmp_unsniff_req:
  OPCODE: 24
  parameters: 0                  # No parameters

# LMP SUPERVISION_TIMEOUT Command
lmp_supervision_timeout:
  OPCODE: 55
  parameters:
    supervision_timeout: 2       # Supervision timeout (2 bytes)
```

### 3. Sniff Protocol Handlers

```c
// Sniff Protocol Handlers
void lmp_sniff_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_sniff_req_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_unsniff_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_unsniff_req_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_supervision_timeout_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_supervision_timeout_rx_handler(ulc_t *ulc, pkb_t *pkb);

// Sniff Response Handlers
void lmp_sniff_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_unsniff_req_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_sniff_req_not_accepted_rx_handler(ulc_t *ulc, pkb_t *pkb);
```

## Sniff Subrating System

### 1. Sniff Subrating Commands

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Subrating Commands                         │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ SNIFF_      │  │ SNIFF_      │  │ SNIFF_      │            │
│  │ SUBRATING_  │  │ SUBRATING_  │  │ SUBRATING_  │            │
│  │ REQ         │  │ RES         │  │ Management  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Sniff Subrating Features                     │ │
│  │  • Dynamic Subrate Adjustment                             │ │
│  │  • Latency-based Subrate Calculation                      │ │
│  │  • Instant-based Subrate Activation                       │ │
│  │  • Multi-Connection Subrate Coordination                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Sniff Subrating Command Parameters

```yaml
# LMP SNIFF_SUBRATING_REQ Command
lmp_sniff_subrating_req:
  OPCODE: 127
  EXTENDED_OPCODE: 21
  parameters:
    max_sniff_subrate: 1         # Maximum sniff subrate (1 byte)
    min_sniff_mode_tieout: 2     # Minimum sniff mode timeout (2 bytes)
    sniff_subrating_instant: 4   # Sniff subrating instant (4 bytes)

# LMP SNIFF_SUBRATING_RES Command
lmp_sniff_subrating_res:
  OPCODE: 127
  EXTENDED_OPCODE: 22
  parameters:
    max_sniff_subrate: 1         # Maximum sniff subrate (1 byte)
    min_sniff_mode_timeout: 2    # Minimum sniff mode timeout (2 bytes)
    sniff_subrating_instant: 4   # Sniff subrating instant (4 bytes)
```

### 3. Sniff Subrating Handlers

```c
// Sniff Subrating Protocol Handlers
void lmp_sniff_subrating_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_sniff_subrating_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_sniff_subrating_req_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_sniff_subrating_res_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_sniff_subrating_req_not_accepted_ext_rx_handler(ulc_t *ulc, pkb_t *pkb);

// Sniff Subrating Management Functions
uint8_t lmp_change_sniff_subrating_params_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb);
uint16_t dm_calc_remote_max_sniff_subrate(ulc_t *ulc, uint8_t peer_id);
void calc_sniff_subrating_instant(ulc_t *ulc, uint8_t peer_id);
```

## Sniff State Management

### 1. Sniff State Machine

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff State Machine                              │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Active      │  │ Sniff       │  │ Sniff       │            │
│  │ Mode        │  │ Transition  │  │ Mode        │            │
│  │ State       │  │ State       │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
│         │                │                │                   │
│         │                │                │                   │
│         ▼                ▼                ▼                   │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Sniff       │  │ Sniff       │  │ Sniff       │            │
│  │ Entry       │  │ Exit        │  │ Subrating   │            │
│  │ State       │  │ State       │  │ State       │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Sniff State Transitions

```c
// Sniff State Transition Functions
void lmp_enable_or_disable_sniff_mode(ulc_t *ulc, uint8_t peer_id, uint8_t enable);
void exit_sniff_mode(ulc_t *ulc, uint8_t peer_id);
void sniff_role_change_from_lpw(void *ctx, pkb_t *pkb);

// Sniff State Checking Macros
#define IS_IN_SNIFF_TRANSITION_MODE(acl_link_mgmt_info) \
  ((acl_link_mgmt_info)->connected_sub_state == CONNECTED_SNIFF_TRANSITION_MODE)

#define IS_IN_SNIFF_MODE(acl_link_mgmt_info) \
  ((acl_link_mgmt_info)->connected_sub_state == CONNECTED_SNIFF_MODE)

#define IS_SNIFF_SUBRATE_PARAMS_SET(sniff_params) \
  ((sniff_params)->sniff_subrate_params_set == SNIFF_SUBRATE_PARAMS_SET)
```

### 3. Sniff Parameter Validation

```c
// Sniff Parameter Validation Macros
#define SNIFF_SUPERVISION_TIMEOUT_MULTIPLIER 3
#define IS_SNIFF_SUPERVISION_TIMEOUT_INVALID(supervision_timeout, sniff_interval) \
  ((supervision_timeout) < (SNIFF_SUPERVISION_TIMEOUT_MULTIPLIER * (sniff_interval)))

#define SNIFF_PARAMS_INVALID(sniff_params, t_sniff_orig, d_sniff_orig) \
  (((sniff_params)->t_sniff != (t_sniff_orig)) || ((sniff_params)->d_sniff != (d_sniff_orig)))

// Sniff Parameter Validation Functions
uint8_t dm_enable_sniff_mode_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb);
```

## Timing and Synchronization

### 1. Sniff Timing Management

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Timing Management                          │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Sniff       │  │ Sniff       │  │ Sniff       │            │
│  │ Interval    │  │ Delay       │  │ Timeout     │            │
│  │ Management  │  │ Management  │  │ Management  │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Timing Features                              │ │
│  │  • Random Interval Generation                             │ │
│  │  • Even Interval Alignment                                │ │
│  │  • Timeout Management                                     │ │
│  │  • Instant Calculation                                    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Sniff Timing Functions

```c
// Sniff Timing Functions
uint32_t get_current_tsf(ulc_t *ulc);
uint32_t get_next_anchor_pt_tsf(ulc_t *ulc, uint8_t peer_id);
uint32_t get_priority_slot_start_tsf(ulc_t *ulc, uint8_t peer_id);
uint32_t get_next_sniff_anchor_point(ulc_t *ulc, uint8_t peer_id);

// Sniff Parameter Extraction
void extract_sniff_parameters_from_packet(uint8_t *bt_pkt_start, sniff_params_t *sniff_params);
```

### 3. Sniff Interval Calculation

```c
// Sniff Interval Calculation
if (max_interval != min_interval) {
  uint16_t rand_num;
  generate_rand_num(2, (uint8_t *)&rand_num, PSEUDO_RAND_NUM);
  // Ensure t_sniff is even by masking the LSB
  uint16_t range = max_interval - min_interval;
  sniff_params->t_sniff = CALCULATE_SNIFF_RANDOM_EVEN_INTERVAL(min_interval, rand_num, range);
} else {
  sniff_params->t_sniff = min_interval;
}
```

## Anchor Point Management

### 1. Anchor Point Coordination

```
┌─────────────────────────────────────────────────────────────────┐
│              Anchor Point Coordination                        │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Anchor      │  │ Priority    │  │ Conflict    │            │
│  │ Point       │  │ Role        │  │ Resolution  │            │
│  │ Calculation │  │ Management  │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Anchor Point Features                        │ │
│  │  • Multi-Connection Coordination                          │ │
│  │  • Priority-based Scheduling                              │ │
│  │  • Conflict Detection and Resolution                      │ │
│  │  • Dynamic Anchor Point Adjustment                        │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Anchor Point Functions

```c
// Anchor Point Management Functions
uint16_t align_sniff_anchor_point(ulc_t *ulc, uint8_t peer_id);
uint16_t get_priority_role_running(ulc_t *ulc, uint8_t peer_id);

// Anchor Point Validation Macros
#define IS_ANCHOR_ALIGNMENT_VALID(d_sniff, t_sniff) ((d_sniff) <= (t_sniff))

// Anchor Point Conflict Resolution
#define HANDLE_SNIFF_ANCHOR_ALIGNMENT(ulc, priority_roles, peer_id, tid, sniff_params, lmp_input_params, d_sniff_temp) \
  do { \
    if (HAS_OTHER_ACTIVE_PEERS(priority_roles, peer_id)) { \
      d_sniff_temp = align_sniff_anchor_point(ulc, peer_id); \
      if (!IS_ANCHOR_ALIGNMENT_VALID(d_sniff_temp, sniff_params->t_sniff)) { \
        REJECT_SNIFF_ALIGNMENT_FAILED(ulc, peer_id, tid, sniff_params, lmp_input_params); \
      } \
    } \
  } while(0)
```

### 3. Multi-Connection Coordination

```c
// Multi-Connection Coordination
uint16_t priority_role_running = get_priority_role_running(ulc, peer_id);
if (HAS_OTHER_ACTIVE_PEERS(priority_role_running, peer_id)) {
  sniff_params->d_sniff = align_sniff_anchor_point(ulc, peer_id);
}

// Conflict Detection and Resolution
for (tmp_peer_id = 0; (tmp_peer_id < MAX_NUM_ACL_CONNS); tmp_peer_id++) {
  if (tmp_peer_id != peer_id && (priority_role_running & (BIT(tmp_peer_id)))) {
    // Check for anchor point conflicts
    if (IS_NO_SNIFF_CONFLICT(tmp_priority_slot_start_tsf, d_sniff_temp_tsf, 
                            sniff_params->sniff_attempt * BT_SLOT_TIME_US)) {
      d_sniff = sniff_params->d_sniff;
    } else {
      // Adjust anchor point to avoid conflict
      d_sniff = CALCULATE_D_SNIFF(next_anchor_pt_tsf, curr_tsf, 
                                 temp_sniff_params->sniff_attempt, 
                                 temp_sniff_params->sniff_tout);
    }
  }
}
```

## Integration Architecture

### 1. LMP Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              LMP Integration Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ LMP         │  │ Sniff       │  │ Sniff       │            │
│  │ Command     │  │ Protocol    │  │ Subrating   │            │
│  │ Processing  │  │ Handlers    │  │ Handlers    │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              LMP Integration Features                     │ │
│  │  • LMP Command Processing                                 │ │
│  │  • Sniff Protocol Handlers                                │ │
│  │  • Sniff Subrating Handlers                               │ │
│  │  • State Synchronization                                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. HCI Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              HCI Integration Architecture                      │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ HCI         │  │ Mode        │  │ Sniff       │            │
│  │ Command     │  │ Change      │  │ Subrating   │            │
│  │ Processing  │  │ Events      │  │ Events      │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              HCI Integration Features                     │ │
│  │  • HCI Command Processing                                 │ │
│  │  • Mode Change Event Generation                           │ │
│  │  • Sniff Subrating Event Generation                       │ │
│  │  • Host Notification                                      │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Device Management Integration

```
┌─────────────────────────────────────────────────────────────────┐
│              Device Management Integration                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ ACL         │  │ Sniff       │  │ Power       │            │
│  │ Peer        │  │ Parameter   │  │ Management  │            │
│  │ Management  │  │ Management  │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Device Management Features                   │ │
│  │  • ACL Peer Information Management                        │ │
│  │  • Sniff Parameter Management                             │ │
│  │  • Power Management Integration                           │ │
│  │  • Link Policy Management                                 │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Performance Architecture

### 1. Performance Optimization

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Performance Architecture                   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Power       │  │ Latency     │  │ Throughput  │            │
│  │ Optimization│  │ Optimization│  │ Optimization│            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Performance Features                         │ │
│  │  • Dynamic Power Management                               │ │
│  │  • Latency-based Subrate Adjustment                       │ │
│  │  • Throughput Optimization                                │ │
│  │  • Multi-Connection Efficiency                            │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Power Management

```c
// Power Management Functions
void lmp_enable_or_disable_sniff_mode(ulc_t *ulc, uint8_t peer_id, uint8_t enable);
void exit_sniff_mode(ulc_t *ulc, uint8_t peer_id);

// Power Management Macros
#define PAUSE_ACLD_TRAFFIC                    // Pause ACL data traffic
#define SAFE_SNIFF_TRANSITION_SLOTS          // Safe sniff transition slots
```

### 3. Latency Optimization

```c
// Latency Optimization Functions
uint16_t dm_calc_remote_max_sniff_subrate(ulc_t *ulc, uint8_t peer_id);
void calc_sniff_subrating_instant(ulc_t *ulc, uint8_t peer_id);

// Latency Calculation
max_sniff_subrate = CALCULATE_MAX_SNIFF_SUBRATE(sniff_params->remote_max_sniff_latency, 
                                               sniff_params->t_sniff);
```

## Error Handling Architecture

### 1. Error Detection and Recovery

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Error Handling Architecture                │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Parameter   │  │ Timing      │  │ State       │            │
│  │ Validation  │  │ Validation  │  │ Recovery    │            │
│  │             │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Error Types                                  │ │
│  │  • Parameter Validation Errors                            │ │
│  │  • Timing Synchronization Errors                          │ │
│  │  • State Transition Errors                                │ │
│  │  • Multi-Connection Conflicts                             │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Error Recovery Strategies

```c
// Error Recovery Macros
#define REJECT_SNIFF_IF_SUPERVISION_TIMEOUT_INVALID(ulc, peer_id, tid, acl_link_mgmt_info, sniff_params, lmp_input_params) \
  do { \
    if (IS_SNIFF_SUPERVISION_TIMEOUT_INVALID(acl_link_mgmt_info->link_supervision_timeout, sniff_params->t_sniff)) { \
      memset((uint8_t *)sniff_params, 0, sizeof(sniff_params_t)); \
      lmp_input_params[1] = UNSUPPORTED_LMP_PARAMETER_VALUE; \
      call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params); \
      return; \
    } \
  } while(0)

#define REJECT_SNIFF_ALIGNMENT_FAILED(ulc, peer_id, tid, sniff_params, lmp_input_params) \
  do { \
    memset((uint8_t *)sniff_params, 0, sizeof(sniff_params_t)); \
    lmp_input_params[1] = UNSUPPORTED_LMP_PARAMETER_VALUE; \
    call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params); \
    return; \
  } while(0)
```

### 3. Fallback Mechanisms

```c
// Fallback Mechanisms
void exit_sniff_mode(ulc_t *ulc, uint8_t peer_id)
{
  btc_dev_info_t *btc_dev_info = &ulc->btc_dev_info;
  acl_peer_info_t *acl_peer_info = btc_dev_info->acl_peer_info[peer_id];
  sniff_params_t *sniff_params = &acl_peer_info->sniff_params;
  
  // Reset to active mode
  acl_peer_info->acl_link_mgmt_info.connected_sub_state = CONNECTED_ACTIVE_MODE;
  memset((uint8_t *)sniff_params, 0, sizeof(sniff_params_t));
  acl_peer_info->acl_link_mgmt_info.sniff_poll_transition = SAFE_SNIFF_TRANSITION_SLOTS;
}
```

## Future Extensions

### 1. Planned Enhancements

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Future Extensions                          │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Enhanced    │  │ Advanced    │  │ Extended    │            │
│  │ Subrating   │  │ Multi-      │  │ Power       │            │
│  │ Algorithms  │  │ Connection  │  │ Management  │            │
│  │             │  │ Support     │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Extension Features                           │ │
│  │  • Enhanced Subrating Algorithms                          │ │
│  │  • Advanced Multi-Connection Support                      │ │
│  │  • Extended Power Management                              │ │
│  │  • Machine Learning-based Optimization                    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Implementation Roadmap

```
┌─────────────────────────────────────────────────────────────────┐
│              Sniff Implementation Roadmap                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Phase 1     │  │ Phase 2     │  │ Phase 3     │            │
│  │ Enhanced    │  │ Advanced    │  │ Extended    │            │
│  │ Subrating   │  │ Multi-      │  │ Features    │            │
│  │             │  │ Connection  │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Development Phases                           │ │
│  │  • Phase 1: Enhanced Subrating Algorithms                 │ │
│  │  • Phase 2: Advanced Multi-Connection Support             │ │
│  │  • Phase 3: Extended Power Management                     │ │
│  │  • Phase 4: Machine Learning Integration                  │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Conclusion

The Sniff and Sniff Subrating architecture in ULC provides a comprehensive framework for managing power-saving modes in Bluetooth Classic systems. The architecture is designed with the following key principles:

### Key Architectural Strengths

1. **Power Efficiency**: Maximize power savings while maintaining connectivity
2. **Traffic Adaptation**: Dynamic adjustment based on traffic patterns
3. **Multi-Connection Support**: Coordination across multiple ACL connections
4. **Timing Precision**: Precise timing for sniff anchor points and intervals
5. **Latency Optimization**: Balance between power savings and latency requirements

### Architectural Benefits

- **Power Savings**: Significant power savings through intelligent sniff mode management
- **Traffic Adaptation**: Dynamic adjustment of sniff intervals based on traffic patterns
- **Multi-Connection Efficiency**: Efficient coordination across multiple ACL connections
- **Latency Optimization**: Optimal balance between power savings and latency
- **Extensibility**: Modular design supporting future enhancements

### Current Implementation Status

- **Sniff Protocol**: Complete sniff protocol support with all LMP commands
- **Sniff Subrating**: Full sniff subrating support with dynamic adjustment
- **State Management**: Complete sniff state management and transitions
- **Timing Management**: Full timing and synchronization support
- **Anchor Point Management**: Complete anchor point coordination and conflict resolution
- **Error Handling**: Robust error handling and recovery mechanisms

### Key Components

- **Sniff Protocol Manager**: Complete sniff protocol management with LMP integration
- **Sniff Subrating Manager**: Dynamic subrate adjustment and management
- **State Manager**: Sniff state management and transitions
- **Timing Manager**: Sniff timing calculation and synchronization
- **Anchor Point Manager**: Multi-connection anchor point coordination
- **Error Handler**: Comprehensive error handling and recovery

### Future Development

- **Enhanced Subrating**: Advanced subrating algorithms and optimization
- **Multi-Connection**: Enhanced multi-connection support and coordination
- **Power Management**: Extended power management capabilities
- **Machine Learning**: Machine learning-based optimization

This architecture provides a solid foundation for implementing comprehensive sniff functionality in Bluetooth Classic systems, ensuring optimal power efficiency while maintaining connectivity and performance through intelligent traffic adaptation and multi-connection coordination.
