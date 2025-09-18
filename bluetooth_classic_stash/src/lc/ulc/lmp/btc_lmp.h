#ifndef _BTC_LMP_INTF_H
#define _BTC_LMP_INTF_H

#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"

#define MAX_SIZE_OF_INPUT_PARAMS 16
#define LMP_MAX_NAME_FRAGMENT_LENGTH 14
#define LMP_PKT_START_OFFSET         4

#define PUBLIC_KEY_MAJOR_TYPE        1
#define PUBLIC_KEY_MINOR_TYPE        1

#define PUBLIC_KEY_MAJOR_TYPE_192        1
#define PUBLIC_KEY_MINOR_TYPE_192        1
#define PUBLIC_KEY_LEN_192               48

#define PUBLIC_KEY_MAJOR_TYPE_256        1
#define PUBLIC_KEY_MINOR_TYPE_256        2
#define PUBLIC_KEY_LEN_256               64

#define LMP_TRUE  1
#define LMP_FALSE  0

#define LMP_HV1_PKT_TYPE 0
#define LMP_HV2_PKT_TYPE 1
#define LMP_HV3_PKT_TYPE 2

#define FEATURES_RESP_RCVD               0
#define EXT_FEATURES_RESP_RCVD           1
#define HOST_CONNECTION_REQ_REJECTED     2
#define HOST_CONNECTION_REQ_ACCEPTED     3
#define SETUP_COMPLETE_RCVD              4
#define TIMING_ACCURACY_RES_RECVD        5
#define SCO_LINK_REQ_ACCEPTED            6
#define REMOVE_SCO_LINK_RCVD             7
#define VERSION_RESP_RCVD                8
#define NAME_RESP_RCVD                   9
#define LINK_KEY_NOT_PRESENT             10
#define LINK_KEY_PRESENT                 11
#define PIN_CODE_REPLY                   12
#define PIN_CODE_NEG_REPLY               13
#define IN_RAND_ACCEPTED                 14
#define IN_RAND_NOT_ACCEPTED             15
#define COMB_KEY_RCVD                    16
#define EXPECTED_SRES_RECVD              17
#define WRONG_SRES_RECVD                 18
#define PTT_ACCEPTED                     19
#define PTT_REJECTED                     20
#define ENCRYPTION_MODE_ACCEPTED         21
#define ENCRYPTION_MODE_NOT_ACCEPTED     22
#define ENC_KEY_SIZE_ACCEPTED            23
#define ENC_KEY_SIZE_NOT_ACCEPTED        24
#define ENCRYPTION_ACCEPTED              25
#define ENCRYPTION_NOT_ACCEPTED          26
#define IO_CAPABILITIES_REPLY            27
#define IO_CAP_NEG_REPLY                 28
#define IO_CAP_RES_RCVD                  29
#define ENCAP_HEADER_ACCEPTED            30
#define PUBLIC_KEY_PAYLOAD_ACCEPTED      31
#define COMMITMENT_VAL_RCVD              32
#define LMP_SIMPLE_PAIRING_NUM_ACCEPTED  33
#define LMP_SIMPLE_PAIRING_NUM_REJECTED  34
#define USER_CONFIRM_REQ_REPLY_RECVD     35
#define USER_CONFIRM_REQ_NEG_REPLY_RECVD 36
#define LMP_DHKEY_CHECK_PASS             37
#define AU_RAND_REJECTED                 38
#define PASSKEY_REPLY_RCVD               39
#define PASSKEY_NEG_REPLY_RCVD           40
#define LMP_SIMPLE_PAIRING_NUM_RCVD      41
#define IO_CAP_REQ_REJECTED              42
#define LMP_DHKEY_CHECK_FAILED           43
#define ENCAP_HEADER_REJECTED            44
#define ENCAP_PAYLOAD_REJECTED           45
#define PUBLIC_KEY_RECEIVED              46
#define SCO_LINK_REQ_REJECTED            47
#define ESCO_LINK_REQ_ACCEPTED           48
#define REMOVE_ESCO_LINK_RCVD            49
#define ESCO_LINK_REQ_REJECTED           50

/* BIT(0) is for enabling roleswitch from host*/
#define ENABLE_ROLE_SWITCH_FROM_HOST BIT(0)
#define ENABLE_NOISE_FIGURE_MEASUREMENT BIT(1)
#define BT_POWER_SAVE_ENABLE BIT(2)
#define DEBUG_HCI_LOGS_UART2 BIT(3)
#define DISABLE_AE_FEATURE BIT(4)
#define ENABLE_BT_SPOOF_MAC_ADDR BIT(5)

#define NEGOTIATE_ESCO_PARAMS 1
#define RENEGOTIATE_REQ 2
#define HOST_ACCEPT_ESCO_LINK 4

#define CENTERAL_INITIATED_TID 0
#define PERIPHERAL_INITIATED_TID 1

#define GET_INITIATOR_TID_OF_MODE(mode) (mode == BT_PERIPHERAL_MODE ? PERIPHERAL_INITIATED_TID : CENTERAL_INITIATED_TID)
#define GET_TID_OF_MODE(mode) (mode == BT_PERIPHERAL_MODE ? CENTERAL_INITIATED_TID : PERIPHERAL_INITIATED_TID)
#define BT_SLOT_TIME_US                625     // Bluetooth slot time in microseconds
#define BT_POLL_INTERVAL_THRESHOLD_US  1250    // Poll interval threshold in microseconds (2 slots worth)
#define BT_MIN_POLL_INTERVAL_SLOTS     2       // Minimum poll interval in slots
#define BT_POLL_INTERVAL_SHIFT         1       // Shift value for poll interval calculation

// Macro to calculate poll interval based on latency
#define CALCULATE_POLL_INTERVAL(latency) \
    ((latency) > BT_POLL_INTERVAL_THRESHOLD_US) ? \
    (((latency) / BT_POLL_INTERVAL_THRESHOLD_US) << BT_POLL_INTERVAL_SHIFT) : \
    BT_MIN_POLL_INTERVAL_SLOTS

/* eSCO pkt type param in LM PDU */
#define LMP_EV3_PKT_TYPE  0x07
#define LMP_2EV3_PKT_TYPE 0x26
#define LMP_3EV3_PKT_TYPE 0x37
#define LMP_EV4_PKT_TYPE  0x0C
#define LMP_EV5_PKT_TYPE  0x0D
#define LMP_2EV5_PKT_TYPE 0x2C
#define LMP_3EV5_PKT_TYPE 0x3D

#define LMP_NAME_REQ 1
#define LMP_NAME_RES 2
#define LMP_ACCEPTED 3
#define LMP_NOT_ACCEPTED 4
#define LMP_CLKOFFSET_REQ 5
#define LMP_CLKOFFSET_RES 6
#define LMP_DETACH 7
#define LMP_IN_RAND 8
#define LMP_COMB_KEY 9
#define LMP_UNIT_KEY 10
#define LMP_AU_RAND 11
#define LMP_SRES 12
#define LMP_TEMP_RAND 13
#define LMP_TEMP_KEY 14
#define LMP_ENCRYPTION_MODE_REQ 15
#define LMP_ENCRYPTION_KEY_SIZE_REQ 16
#define LMP_START_ENCRYPTION_REQ 17
#define LMP_STOP_ENCRYPTION_REQ 18
#define LMP_SWITCH_REQ 19
#define LMP_HOLD 20
#define LMP_HOLD_REQ 21
#define LMP_SNIFF_REQ 23
#define LMP_UNSNIFF_REQ 24
#define LMP_PARK_REQ 25
#define LMP_SET_BROADCAST_SCAN_WINDOW 27
#define LMP_MODIFY_BEACON 28
#define LMP_UNPARK_BD_ADDR_REQ 29
#define LMP_UNPARK_PM_ADDR_REQ 30
#define LMP_INCR_POWER_REQ 31
#define LMP_DECR_POWER_REQ 32
#define LMP_MAX_POWER 33
#define LMP_MIN_POWER 34
#define LMP_AUTO_RATE 35
#define LMP_PREFERRED_RATE 36
#define LMP_VERSION_REQ 37
#define LMP_VERSION_RES 38
#define LMP_FEATURES_REQ 39
#define LMP_FEATURES_RES 40
#define LMP_QUALITY_OF_SERVICE 41
#define LMP_QUALITY_OF_SERVICE_REQ 42
#define LMP_SCO_LINK_REQ 43
#define LMP_REMOVE_SCO_LINK_REQ 44
#define LMP_MAX_SLOT 45
#define LMP_MAX_SLOT_REQ 46
#define LMP_TIMING_ACCURACY_REQ 47
#define LMP_TIMING_ACCURACY_RES 48
#define LMP_SETUP_COMPLETE 49
#define LMP_USE_SEMI_PERMANENT_KEY 50
#define LMP_HOST_CONNECTION_REQ 51
#define LMP_SLOT_OFFSET 52
#define LMP_PAGE_MODE_REQ 53
#define LMP_PAGE_SCAN_MODE_REQ 54
#define LMP_SUPERVISION_TIMEOUT 55
#define LMP_TEST_ACTIVATE 56
#define LMP_TEST_CONTROL 57
#define LMP_ENCRYPTION_KEYSIZE_MASK_REQ 58
#define LMP_ENCRYPTION_KEYSIZE_MASK_RES 59
#define LMP_SET_AFH 60
#define LMP_ENCAPSULATED_HEADER 61
#define LMP_ENCAPSULATED_PAYLOAD 62
#define LMP_SIMPLE_PAIRING_CONFIRM 63
#define LMP_SIMPLE_PAIRING_NUMBER 64
#define LMP_DHKEY_CHECK 65
#define LMP_ESCAPE_OPCODE 127
/* extended op codes, LS byte(escape opcode) = 127 */
#define LMP_ACCEPTED_EXT 1
#define LMP_NOT_ACCEPTED_EXT 2
#define LMP_FEATURES_REQ_EXT 3
#define LMP_FEATURES_RES_EXT 4
#define LMP_PACKET_TYPE_TABLE_REQ 11
#define LMP_ESCO_LINK_REQ 12
#define LMP_REMOVE_ESCO_LINK_REQ 13
#define LMP_CHANNEL_CLASSIFICATION_REQ 16
#define LMP_CHANNEL_CLASSIFICATION 17
#define LMP_SNIFF_SUBRATING_REQ 21
#define LMP_SNIFF_SUBRATING_RES 22
#define LMP_PAUSE_ENCRYPTION_REQ 23
#define LMP_RESUME_ENCRYPTION_REQ 24
#define LMP_IO_CAPABILITIES_REQ 25
#define LMP_IO_CAPABILITIES_RES 26
#define LMP_NUMERIC_COMPARISON_FAILED 27
#define LMP_PASSKEY_FAILED 28
#define LMP_OOB_FAILED 29
#define LMP_KEYPRESS_NOTIFICATION 30
#define LMP_POWER_CONTROL_REQ 31
#define LMP_POWER_CONTROL_RES 32

/* LMP features */
#define LMP_3SLOT 0x01
#define LMP_5SLOT 0x02
#define LMP_ENCRYPT 0x04
#define LMP_SOFFSET 0x08
#define LMP_TACCURACY 0x10
#define LMP_RSWITCH 0x20
#define LMP_HOLD_FLAG 0x40
#define LMP_SNIFF 0x80

#define LMP_PARK 0x01
#define LMP_PCONTROL_REQ 0x02
#define LMP_CQDDR 0x04
#define LMP_SCO 0x08
#define LMP_HV2 0x10
#define LMP_HV3 0x20
#define LMP_ULAW 0x40
#define LMP_ALAW 0x80

#define LMP_CVSD 0x01
#define LMP_PSCHEME 0x02
#define LMP_PCONTROL 0x04
#define LMP_SYNCH_DATA 0x08
#define LMP_FCL_LSB 0x10
#define LMP_FCL_MB 0x20
#define LMP_FCL_MSB 0x40
#define LMP_BCAST_ENC 0x80

#define LMP_EDR_ACL_2M 0x02
#define LMP_EDR_ACL_3M 0x04
#define LMP_ENHNCD_INQ_SCN 0x08
#define LMP_INTLCD_INQ_SCN 0x10
#define LMP_INTLCD_PG_SCN 0x20
#define LMP_RSSI_INQ 0x40
#define LMP_ESCO 0x80

#define LMP_EV4 0x01
#define LMP_EV5 0x02
#define LMP_AFH_CAP_SLAVE 0x08
#define LMP_AFH_CLS_SLAVE 0x10
#define LMP_NO_BREDR 0x20
#define LMP_LE 0x40
#define LMP_3S_EDR_ACL 0x80

#define LMP_5S_EDR_ACL 0x01
#define LMP_SNIFF_SUBR 0x02
#define LMP_PAUSE_ENC 0x04
#define LMP_AFH_CAP_MASTER 0x08
#define LMP_AFH_CLS_MASTER 0x10
#define LMP_EDR_ESCO_2M 0x20
#define LMP_EDR_ESCO_3M 0x40
#define LMP_EDR_3S_ESCO 0x80

#define LMP_EXT_INQ 0x01
#define LMP_SIMUL_LE_BR 0x02
#define LMP_SIMPLE_PAIR 0x08
#define LMP_ENCAP_PDU 0x10
#define LMP_ERR_DATA_REP 0x20
#define LMP_NO_FLUSH 0x40

#define LMP_LSTO_CHANGED 0x01
#define LMP_INQ_TX_PWR 0x02
#define LMP_ENHCED_PWR 0x04
#define LMP_EXTFEATURES 0x80

#define IS_LMP_ENHANCED_PWR_SUPPORTED(a) \
  ((a)->features_bitmap[7] & LMP_ENHCED_PWR)

#define IS_LMP_PWR_CONTROL_SUPPORTED(a) \
  ((a)->features_bitmap[2] & LMP_PCONTROL)

#define IS_PERIPHERAL_MODE(dev_mgmt_info) ((dev_mgmt_info)->mode == BT_PERIPHERAL_MODE)
#define IS_CENTRAL_MODE(dev_mgmt_info) ((dev_mgmt_info)->mode == BT_CENTRAL_MODE)


/* Extended LMP features */
#define LMP_HOST_SSP 0x01
#define LMP_HOST_LE 0x02
#define LMP_HOST_SIMUL_LE_BREDR 0x04

#define LMP_HOST_SECURE_CONNECTIONS 0x08

/* Extended LMP features  Page Feature Page 2*/

#define LMP_CSB_MASTER                  0x01
#define LMP_CSB_SLAVE                   0x02
#define LMP_SYNC_TRAIN                  0x04
#define LMP_SYNC_SCAN                   0x08
#define LMP_INQUIRY_NOTIFICATION_EVENT  0x10
#define LMP_GENERALIZED_INTERLACED_SCAN 0x20
#define LMP_COARSE_CLOCK_ADJUSTMENT     0x40

#define LMP_SECURE_CONNECTIONS     0x01
#define LMP_PING                   0x02
#define LMP_SLOT_AVAILABILITY_MASK 0x04
#define LMP_TRAIN_NUDGING          0x08

#define BLUETOOTH_DEVICE_KEY 0x6274646b
#define BLUETOOTH_AES_KEY    0x6274616b

#define BT_ENCRYPTION_DISABLE 0
#define BT_ENCRYPTION_ENABLE  1
#define SEND_SYNC_CONN_CHANGED 1
#define GET_RSSI(pkb) pkb->data[1]
#define GET_PEER_ID(pkb) pkb->data[2]
#define GET_LPW_RX_PKT_BD_ADDR(pkb) pkb->data[5]
#define GET_RX_BT_PKT(pkb) pkb->data[META_DATA_LENGTH]

#define LMP_PKT_HDR_LEN 3
#define BIT_POSITION_MASK       0x7     // Extracts lower 3 bits (0-7) for bit position
#define BYTE_POSITION_SHIFT     3       // Right shift by 3 (divide by 8) for byte position

#define BT_ESCO_LINK_PARAMS_SIZE sizeof(esco_link_params_t)
void lmp_remote_dev_name_req_from_hci(ulc_t *ulc, uint8_t peer_id);
uint8_t lmp_accept_connection_req_from_dm(ulc_t *ulc, uint8_t peer_id, uint8_t role);
void lmp_change_conn_pkt_type(ulc_t *ulc, uint8_t peer_id, uint16_t pkt_type);
void lmp_start_enc_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_stop_enc_from_hci(ulc_t *ulc, uint8_t peer_id);
uint8_t lmp_change_conn_link_key_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_remote_dev_features_req_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_remote_ext_features_req(ulc_t *ulc, uint8_t peer_id, uint8_t page_num);
void lmp_read_remote_version_req_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_read_clk_offset_req_from_hci(ulc_t *ulc, uint8_t peer_id);
uint8_t lmp_refresh_enc_key_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_read_local_oob_data(ulc_t *ulc, uint8_t *oob_data);
void lmp_switch_to_central(ulc_t *ulc, uint8_t peer_id);
void lmp_switch_to_peripheral(ulc_t *ulc, uint8_t peer_id);
void lmp_start_connection(ulc_t *ulc, uint8_t peer_id);
uint8_t lmp_process_central_conn_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code);
uint8_t lmp_process_peripheral_conn_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code);
uint8_t lmp_form_tx_pkt(ulc_t *ulc, uint8_t opcode, uint8_t tid, uint8_t slave_id, uint8_t *input_params, uint8_t reason_code);
void  bt_start_role_switch(ulc_t *ulc, uint8_t peer_id);
void update_feature_map(acl_peer_info_t *acl_peer_info, uint8_t *rx_pkt_buff, ulc_t *ulc_ptr);
void dm_acl_conn_indication_from_lm(ulc_t *ulc, uint8_t peer_id, uint8_t status);
void lmp_send_detach_frame(ulc_t *ulc, uint8_t peer_id, uint8_t reason);
void update_new_acl_pkt_type(acl_link_mgmt_info_t *acl_link_mgmt_info);
uint8_t bt_get_free_lt_addr(btc_dev_mgmt_info_t *btc_dev_mgmt_info);
uint8_t get_no_of_good_channels(uint8_t *addr);
uint16_t get_slot_offset(ulc_t *ulc, uint8_t peer_id);
#endif