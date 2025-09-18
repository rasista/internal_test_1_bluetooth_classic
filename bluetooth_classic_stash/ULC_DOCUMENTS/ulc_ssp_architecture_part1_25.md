# Simple Secure Pairing (SSP) Architecture in ULC - Part 1

## Executive Summary

The Simple Secure Pairing (SSP) architecture in ULC provides a comprehensive framework for implementing secure Bluetooth pairing using modern cryptographic methods. This architecture supports multiple association models, including Numeric Comparison, Passkey Entry, Just Works, and Out of Band (OOB), with support for both P-192 and P-256 elliptic curve cryptography.

## Table of Contents - Part 1

1. [Architecture Overview](#architecture-overview)
2. [SSP Core Components](#ssp-core-components)
3. [SSP Protocol Management](#ssp-protocol-management)
4. [Association Models](#association-models)
5. [Cryptographic Operations](#cryptographic-operations)
6. [Key Management](#key-management)

## Architecture Overview

### High-Level SSP Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    SSP Architecture                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ IO          │  │ Protocol    │  │ Key         │            │
│  │ Capability  │  │ Management  │  │ Management  │            │
│  │ Exchange    │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Public Key  │  │ Association │  │ Link Key    │            │
│  │ Exchange    │  │ Models      │  │ Generation  │            │
│  │             │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              SSP Core Engine                              │ │
│  │  • IO Capability Management                               │ │
│  │  • Cryptographic Operations                               │ │
│  │  • Association Model Selection                            │ │
│  │  • Key Generation and Validation                          │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Core Design Principles

1. **Security**: Strong cryptographic security using elliptic curve cryptography
2. **Flexibility**: Support for multiple association models
3. **Compatibility**: Support for both P-192 and P-256 curves
4. **User Experience**: Appropriate pairing methods based on device capabilities
5. **Protocol Compliance**: Full compliance with Bluetooth SSP specifications

## SSP Core Components

### 1. SSP Parameters Structure

```c
// SSP Key Types and Lengths
#define PUBLIC_KEY_LEN_192      48
#define PUBLIC_KEY_LEN_256      64
#define PRIVATE_KEY_LEN_192     24
#define PRIVATE_KEY_LEN_256     32
#define DHKEY_LEN_192          24
#define DHKEY_LEN_256          32

// SSP Public Key Types
#define PUBLIC_KEY_MAJOR_TYPE_192  0x01
#define PUBLIC_KEY_MINOR_TYPE_192  0x01
#define PUBLIC_KEY_MAJOR_TYPE_256  0x02
#define PUBLIC_KEY_MINOR_TYPE_256  0x02

// SSP Authentication Types
#define NUMERIC_COMPARISON_AUTH  0x01
#define PASSKEY_ENTRY_AUTH      0x02
#define OUT_OF_BAND_AUTH        0x03
```

### 2. SSP Core Functions

```
┌─────────────────────────────────────────────────────────────────┐
│              SSP Core Functions                               │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ IO          │  │ Public Key  │  │ DHKey       │            │
│  │ Capability  │  │ Exchange    │  │ Generation  │            │
│  │ Exchange    │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Core Functionality                           │ │
│  │  • IO Capability Exchange                                 │ │
│  │  • Public Key Exchange                                    │ │
│  │  • DHKey Generation                                       │ │
│  │  • Link Key Generation                                    │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. SSP State Definitions

```c
// SSP States
#define SSP_IDLE_STATE                  0x00
#define HCI_IO_CAP_REQ_SENT_STATE      0x01
#define LMP_IO_CAP_REQ_SENT_STATE      0x02
#define LMP_ENCAP_HEADER_SENT_STATE    0x03
#define LMP_ENCAP_PAYLOAD_SENT_STATE   0x04
#define PUBLIC_KEY_SENT_STATE          0x05
#define HCI_PASSKEY_REQ_SENT_STATE     0x06
#define LMP_SIMPLE_PAIRING_CONFIRM_SENT_STATE 0x07
#define LMP_SIMPLE_PAIRING_NUMBER_SENT_STATE  0x08
#define LMP_DHKEY_CHECK_SENT_STATE     0x09

// SSP Events
#define IO_CAP_RES_RCVD               0x01
#define PUBLIC_KEY_RECEIVED           0x02
#define ENCAP_HEADER_ACCEPTED         0x03
#define ENCAP_HEADER_REJECTED         0x04
#define ENCAP_PAYLOAD_ACCEPTED        0x05
#define ENCAP_PAYLOAD_REJECTED        0x06
#define PASSKEY_REPLY_RCVD           0x07
#define LMP_DHKEY_CHECK_PASS         0x08
#define LMP_DHKEY_CHECK_FAILED       0x09
```

## SSP Protocol Management

### 1. LMP SSP Commands

```yaml
# IO Capability Exchange
lmp_io_capability_req:
  OPCODE: 127
  EXTENDED_OPCODE: 25
  parameters:
    io_capabilities: 1
    oob_auth_data: 1
    authentication_requirements: 1

# Public Key Exchange
lmp_encapsulated_header:
  OPCODE: 61
  parameters:
    encap_major_type: 1
    encap_minor_type: 1
    encap_payload_length: 1

lmp_encapsulated_payload:
  OPCODE: 62
  parameters:
    encap_data: 16

# Authentication
lmp_simple_pairing_confirm:
  OPCODE: 63
  parameters:
    commitment_value: 16

lmp_simple_pairing_number:
  OPCODE: 64
  parameters:
    noncvalue: 16

lmp_dhkey_check:
  OPCODE: 65
  parameters:
    confirmation_value: 16
```

### 2. SSP Protocol Handlers

```c
// IO Capability Exchange Handlers
void lmp_io_capability_req_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_io_capability_req_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_io_capability_res_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_io_capability_res_rx_handler(ulc_t *ulc, pkb_t *pkb);

// Public Key Exchange Handlers
void lmp_encapsulated_header_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_encapsulated_header_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_encapsulated_payload_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_encapsulated_payload_rx_handler(ulc_t *ulc, pkb_t *pkb);

// Authentication Handlers
void lmp_simple_pairing_confirm_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_simple_pairing_confirm_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_simple_pairing_number_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_simple_pairing_number_rx_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_dhkey_check_tx_handler(ulc_t *ulc, uint32_t lmp_control_word, void *lmp_input_params);
void lmp_dhkey_check_rx_handler(ulc_t *ulc, pkb_t *pkb);
```

### 3. SSP Protocol Flow

```
┌─────────────────────────────────────────────────────────────────┐
│              SSP Protocol Flow                                │
├─────────────────────────────────────────────────────────────────┤
│  1. IO Capability Exchange                                    │
│     • Exchange IO capabilities                                │
│     • Determine association model                             │
│                                                              │
│  2. Public Key Exchange                                      │
│     • Exchange public keys                                    │
│     • Validate public keys                                    │
│                                                              │
│  3. Authentication Stage 1                                   │
│     • Generate/exchange random numbers                        │
│     • Calculate/verify commitments                            │
│                                                              │
│  4. Authentication Stage 2                                   │
│     • User confirmation (if required)                         │
│     • Calculate/verify DHKey                                  │
│                                                              │
│  5. Link Key Calculation                                     │
│     • Generate link key                                       │
│     • Complete pairing                                        │
└─────────────────────────────────────────────────────────────────┘
```

## Association Models

### 1. Numeric Comparison

```
┌─────────────────────────────────────────────────────────────────┐
│              Numeric Comparison Model                         │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Generate    │  │ Calculate   │  │ Display     │            │
│  │ Random      │  │ Commitment  │  │ Value       │            │
│  │ Numbers     │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Model Features                               │ │
│  │  • 6-digit number display                                 │ │
│  │  • User confirmation                                      │ │
│  │  • MITM protection                                        │ │
│  │  • Commitment scheme                                      │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Passkey Entry

```
┌─────────────────────────────────────────────────────────────────┐
│              Passkey Entry Model                              │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Generate    │  │ Bit-by-bit  │  │ Verify      │            │
│  │ Passkey     │  │ Commitment  │  │ Commitments │            │
│  │             │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Model Features                               │ │
│  │  • 6-digit passkey                                        │ │
│  │  • 20 rounds of verification                              │ │
│  │  • MITM protection                                        │ │
│  │  • Bit-by-bit verification                               │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 3. Out of Band

```
┌─────────────────────────────────────────────────────────────────┐
│              Out of Band Model                                │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Exchange    │  │ Verify      │  │ Generate    │            │
│  │ OOB Data    │  │ Commitment  │  │ Link Key    │            │
│  │             │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Model Features                               │ │
│  │  • External data exchange                                 │ │
│  │  • MITM protection                                        │ │
│  │  • Commitment verification                                │ │
│  │  • Optional authentication                                │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

## Cryptographic Operations

### 1. Key Generation

```c
// Public/Private Key Management
#define GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? \
     btc_dev_mgmt_info->public_key_256 : btc_dev_mgmt_info->public_key)

#define GET_DEVICE_PRIVATE_KEY(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? \
     btc_dev_mgmt_info->private_key_256 : btc_dev_mgmt_info->private_key)

// DHKey Generation
void dm_derive_dhkey(ulc_t *ulc, uint8_t peer_id);
void lmp_dhkey_calculation_done(ulc_t *ulc, uint8_t peer_id);
```

### 2. Cryptographic Functions

```c
// Cryptographic Algorithms
void f1_algo(uint8_t *U, uint8_t *V, uint8_t *X, uint8_t Z, uint8_t *output);
void f2_algo(uint8_t *W, uint8_t *N1, uint8_t *N2, uint8_t *keyID, 
             uint8_t *A1, uint8_t *A2, uint8_t *output);
void f3_algo(uint8_t *W, uint8_t *N1, uint8_t *N2, uint8_t *R, uint8_t *IOcap,
             uint8_t *A1, uint8_t *A2, uint8_t *output);
void g_algo(uint8_t *U, uint8_t *V, uint8_t *X, uint8_t *Y, uint8_t *output);
```

### 3. Key Validation

```c
// Key Validation Functions
uint8_t compare_public_key(uint32_t *public_key, uint32_t key_len);
void validate_public_key(uint8_t *public_key, uint8_t key_type);

// Key Length Management
#define GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? \
     PUBLIC_KEY_LEN_256 : PUBLIC_KEY_LEN_192)

#define GET_DHKEY_LEN(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? \
     DHKEY_LEN_256 : DHKEY_LEN_192)
```

## Key Management

### 1. Key Types

```
┌─────────────────────────────────────────────────────────────────┐
│              SSP Key Types                                    │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐            │
│  │ Public/     │  │ DHKey       │  │ Link        │            │
│  │ Private     │  │             │  │ Key         │            │
│  │ Keys        │  │             │  │             │            │
│  └─────────────┘  └─────────────┘  └─────────────┘            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │              Key Features                                 │ │
│  │  • P-192/P-256 support                                   │ │
│  │  • Secure key generation                                 │ │
│  │  • Key validation                                        │ │
│  │  • Key storage                                           │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Key Storage

```c
// Key Storage Structures
typedef struct {
    uint8_t public_key[PUBLIC_KEY_LEN_MAX];    // Public key
    uint8_t private_key[PRIVATE_KEY_LEN_MAX];  // Private key
    uint8_t dh_key[DHKEY_LEN_MAX];            // DHKey
    uint8_t link_key[LINK_KEY_LEN];           // Link key
} ssp_keys_t;

// Key Type Flags
#define SSP_COMB_KEY              0x04  // Combination key
#define SSP_DEBUG_LINK_KEY        0x05  // Debug link key
#define SSP_UNAUTHENTICATED_KEY   0x06  // Unauthenticated key
#define SSP_AUTHENTICATED_KEY     0x07  // Authenticated key
```

### 3. Key Protection

```c
// Key Protection Mechanisms
#define SECURE_CONNECTIONS_HOST_SUPPORT  0x01  // Secure Connections support
#define SSP_DEBUG_MODE_ENABLED          0x02  // Debug mode enabled
#define PUBLIC_KEY_VALIDATION_ENABLED   0x04  // Public key validation enabled

// Key Protection Functions
void protect_keys(ssp_keys_t *keys);
void clear_keys(ssp_keys_t *keys);
void validate_keys(ssp_keys_t *keys);
```
