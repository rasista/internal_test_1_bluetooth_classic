#ifndef acl_common_H_
#define acl_common_H_
#include "state_machine.h"
#include "btc_common_defines.h"
#include "btc_rail.h"
#include "data_path.h"
#include "esco.h"

/*BT Classic mode packet types*/

#define BT_NULL_PKT_TYPE 0x0

#define BT_POLL_PKT_TYPE 0x1

#define BT_FHS_PKT_TYPE 0x02

#define BT_DM1_PKT_TYPE 0x3

#define BT_DH1_PKT_TYPE 0x4
#define BT_2DH1_PKT_TYPE 0x4

#define BT_HV1_PKT_TYPE 0x05

#define BT_HV2_PKT_TYPE 0x06
#define BT_2EV3_PKT_TYPE 0x06

#define BT_HV3_PKT_TYPE 0x07
#define BT_EV3_PKT_TYPE 0x07
#define BT_3EV3_PKT_TYPE 0x07

#define BT_DV_PKT_TYPE 0x8
#define BT_3DH1_PKT_TYPE 0x8

#define BT_AUX_PKT_TYPE 0x09

#define BT_DM3_PKT_TYPE 0x0A
#define BT_2DH3_PKT_TYPE 0x0A

#define BT_DH3_PKT_TYPE 0x0B
#define BT_3DH3_PKT_TYPE 0x0B

#define BT_EV4_PKT_TYPE 0x0C
#define BT_2EV5_PKT_TYPE 0x0C

#define BT_EV5_PKT_TYPE 0x0D
#define BT_3EV5_PKT_TYPE 0x0D

#define BT_DM5_PKT_TYPE 0x0E
#define BT_2DH5_PKT_TYPE 0x0E

#define BT_DH5_PKT_TYPE 0x0F
#define BT_3DH5_PKT_TYPE 0x0F

#define AFH_INSTANT_PENDING_BIT   0    // Bit position

// Connection state frequency hop type (Basic or Adapted)
#define BASIC_PICONET 0
#define ADAPTED_PICONET 1

#define ACL_LLID_CONTINUATION_FRAGMENT 0x01

/*
 *  This structure contains members w.r.t to ARQN_SEQN
 */
typedef struct arqn_seqn_scheme_status_s {
  uint8_t arqn_status;  //  Local's arqn status
  uint8_t seqn_status;  //  Local's seqn status
  uint8_t remote_seqn; //  Last Successfully Recieved Seqn of Remote device
  uint8_t remote_flow; // Last Successfully Recieved Flow of Remote device
} arqn_seqn_scheme_status_t;

/*
 * This structure contains members w.r.t to acl
 */

typedef struct acl_info_s {
  uint8_t is_encryption_enabled; //  Is encryption enabled
  uint32_t acl_end_tsf;          //  variable to store procedure complete timing of acl procedure
  sm_t acl_sm;
} acl_info_t;

/*
 * This structure contains members w.r.t to sniff
 */

typedef struct sniff_info_s {
  uint32_t sniff_instant;
  uint32_t sniff_interval;
  uint32_t sniff_attempt;
  uint32_t sniff_timeout;
} sniff_info_t;

/*
 * This structure contains remote device connection information
 */

typedef struct connection_info_s {
  uint8_t bd_address[BD_ADDR_LEN]; //  BD Address
  uint8_t lap_address[LAP_ADDR_LEN];
  uint8_t lt_addr;                 //  Logical Transport(LT) address
  /* peer role is used to differentiate between central and
   * Peripheral(holds value 1 if central, 0 if peripheral) w.r.t remote device.
   */
  uint8_t peer_role;
  uint8_t flow;                //  flow bit field of Packet header
  uint8_t tx_pwr_index;        //  Transmit Power variable for the connected states
  uint8_t procedure_instant_bitmap;  //   bit field of pending procedure instants
  uint8_t piconet_type;       //  BASIC_PICONET or ADAPTED_PICONET
  uint8_t priority;
  uint8_t step_size;
  uint8_t br_edr_mode;        // used to define BR/EDR Mode
  uint8_t no_of_valid_afh_channels;
  uint8_t encryption_mode; // 0: OFF, 1: E0, 2: AES
  uint8_t zero_len_acl_packet; // 0: For non-zero length ACL packets, 1: For zero length ACL packets
  uint8_t channel_map_in_use[CHANNEL_MAP_SIZE];      // current channelmap in use
  uint8_t afh_new_channel_map[CHANNEL_MAP_SIZE];      // AFH channelmap for use from AFH instant
  uint8_t acl_enc_tx_pkt_count[5]; // Encrypted ACL tx packet count
  uint8_t acl_enc_rx_pkt_count[5]; // Encrypted ACL rx packet count
  uint32_t day_counter;
  uint32_t afh_new_channel_map_instant;      // Master clock value from which new AFH map needs to be updated
  uint32_t end_tsf;
  uint32_t sync_word[2];
  acl_info_t acl_info;                    //  acl structure
  esco_info_t esco_info;
  sniff_info_t sniff_info;
  sm_t *connection_info_sm;
  sm_t *esco_sm;
  arqn_seqn_scheme_status_t acl_arqn_seqn_info;  // arqn_seqn structure for ACL packets
  arqn_seqn_scheme_status_t esco_arqn_seqn_info; // arqn_seqn structure for ESCO Packets
  btc_trx_configs_t btc_trx_configs_esco; // This structure instance will be updated while adding periodic activities to periodic queue
  btc_trx_configs_t trx_config;
  RAIL_SEQ_BtcFhConfig_t btc_fh_data;
  RAIL_SEQ_E0EncryptionConfig_t e0EncConfig; // E0 encryption configuration
  RAIL_SEQ_AESEncryptionConfig_t aesEncConfig; // AES encryption configuration
} connection_info_t;

void packetization_acl(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs);
void trigger_slot_procedures(connection_info_t *connection_info_p, btc_trx_configs_t *btc_trx_configs);
void update_channel_info_connected_state(sm_t *connection_info_sm_p);
void reset_acl_enc_pkt_count(connection_info_t *connection_info);
#endif
