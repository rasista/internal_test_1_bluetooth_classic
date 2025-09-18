#ifndef _BTC_DM_SIMPLE_SECURE_PAIRING_H
#define _BTC_DM_SIMPLE_SECURE_PAIRING_H

#define ECDH_CURVE_P 0
#define ECDH_CURVE_K 1
#define ECDH_CURVE_B 2

//! ECDH modes
#define ECDH_163 0 //! Valid only for K/B curves
#define ECDH_192 1 //! Valid for P curve
#define ECDH_224 2 //! Valid for P curve
#define ECDH_233 3 //! Valid only for K/B curves
#define ECDH_256 4 //! Valid for P curve

#define IS_OOB_AUTH_DATA_AVAILABLE(btc_dev_mgmt_info, acl_dev_mgmt_info) \
    (((btc_dev_mgmt_info)->oob_authentication_data == OOB_AUTH_DATA_AVAILABLE) || \
     ((acl_dev_mgmt_info)->oob_authentication_data == OOB_AUTH_DATA_AVAILABLE))


#define IS_LOCAL_DISPLAYS_REMOTE_KEYBOARDS_IO_CAPABILITY(btc_dev_mgmt_info, acl_dev_mgmt_info) \
    (((btc_dev_mgmt_info)->io_capabilities == DISPLAY_YES_NO || \
      (btc_dev_mgmt_info)->io_capabilities == DISPLAY_ONLY) && \
     ((acl_dev_mgmt_info)->io_capabilities == KEYBOARD_ONLY))


#define COMPARE_16_BYTE_ARRAYS(arr1, arr2) \
    ((*(uint32_t *)&(arr1)[0] == *(uint32_t *)&(arr2)[0]) && \
     (*(uint32_t *)&(arr1)[4] == *(uint32_t *)&(arr2)[4]) && \
     (*(uint32_t *)&(arr1)[8] == *(uint32_t *)&(arr2)[8]) && \
     (*(uint32_t *)&(arr1)[12] == *(uint32_t *)&(arr2)[12]))

#define IS_KEYBOARD_ONLY_CAPABILITY(device_info)    ((device_info)->io_capabilities == KEYBOARD_ONLY)
#define IS_NUMERIC_COMPARISON_AUTH_TYPE(acl_enc_info)     ((acl_enc_info)->auth_protocol == NUMERIC_COMPARISON)
#define IS_PASSKEY_ENTRY_AUTH_TYPE(acl_enc_info)           ((acl_enc_info)->auth_protocol == PASSKEY_ENTRY)
#define IS_OUT_OF_BAND_AUTH_TYPE(acl_enc_info)             ((acl_enc_info)->auth_protocol == OUT_OF_BAND)

#define GET_AUTH_PROTOCOL(acl_enc_info, btc_dev_mgmt_info, acl_dev_mgmt_info) \
    ((IS_OOB_AUTH_DATA_AVAILABLE(btc_dev_mgmt_info, acl_dev_mgmt_info)) ? OUT_OF_BAND : \
     ((IS_KEYBOARD_ONLY_CAPABILITY(btc_dev_mgmt_info) || IS_KEYBOARD_ONLY_CAPABILITY(acl_dev_mgmt_info)) ? \
      PASSKEY_ENTRY : NUMERIC_COMPARISON))

void lmp_process_ssp_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code);
void lmp_user_passkey_req_neg_reply_rcvd(ulc_t *ulc, uint8_t peer_id);
void lmp_io_cap_req_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_io_cap_neg_reply_rcvd_from_hci(ulc_t *ulc, uint8_t peer_id, uint8_t reason_code);
void lmp_user_confirm_req_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_user_confirm_req_neg_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_user_passkey_req_reply_rcvd(ulc_t *ulc, uint8_t peer_id);
void lmp_remote_oob_data_req_reply_rcvd(ulc_t *ulc, uint8_t peer_id, uint8_t *hash_value);
void lmp_remote_oob_data_req_neg_reply_rcvd(ulc_t *ulc, uint8_t peer_id);
uint8_t compare_public_key(uint32_t *key, uint32_t key_length);
void lmp_dhkey_calculation_done(ulc_t *ulc, uint8_t peer_id);
void lmp_send_key_press_notification(ulc_t *ulc, uint8_t peer_id, uint8_t notification_type);
void dm_generate_public_key(ulc_t *ulc);
void dm_derive_dhkey(ulc_t *ulc, uint8_t peer_id);
#endif
