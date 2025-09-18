#ifndef _BTC_LMP_AUTHENTICATION_H
#define _BTC_LMP_AUTHENTICATION_H

#define GET_LINK_KEY_TYPE(pairing_proc, ssp_key_type) \
    ((pairing_proc == SECURE_SIMPLE_PAIRING && ssp_key_type == SSP_DEBUG_LINK_KEY) ? \
     DEBUG_COMB_KEY : AUTHENTICATED_COMB_KEY)

#define GET_LINK_KEY_TYPE_FROM_PAIRING_PROCEDURE(pairing_proc) \
    ((pairing_proc == SECURE_SIMPLE_PAIRING) ? \
     CHANGED_COMB_KEY : COMB_KEY)


#define IS_BOTH_DEVICES_SUPPORT_PAUSE_ENC(local_features, remote_features) \
    (((local_features[5] & LMP_PAUSE_ENC) == LMP_PAUSE_ENC) && \
     ((remote_features[5] & LMP_PAUSE_ENC) == LMP_PAUSE_ENC))


#define IS_AUTH_REQ_WITHOUT_PAIRING(acl_link_mgmt_info, acl_enc_info) \
    (((acl_link_mgmt_info)->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) && \
     ((acl_enc_info)->pairing_requirement == PAIRING_NOT_REQUIRED))

#define IS_AUTH_REQ_WITH_PAIRING(acl_link_mgmt_info, acl_enc_info) \
    (((acl_link_mgmt_info)->check_pending_req_from_hci & AUTH_REQ_FROM_HCI) && \
     ((acl_enc_info)->pairing_requirement == PAIRING_REQUIRED))

#define IS_FIXED_PIN_ENABLED(dev_mgmt_info) ((dev_mgmt_info)->fixed_pin)
#define IS_AUTH_RESPONDER(enc_info) ((enc_info)->auth_mode == AUTH_RESPONDER)
#define IS_AUTH_INITIATOR(enc_info) ((enc_info)->auth_mode == AUTH_INITIATOR)

#define LINK_KEY_UINT32_BLOCKS    (uint32_t) (LINK_KEY_LEN / sizeof(uint32_t)) 
#define XOR_BYTE_ARRAYS(dest, src, len)\
    do { \
        for (uint32_t ii = 0; ii < len; ii++) { \
            *(uint32_t *)&(dest)[(4 * ii)] ^= *(uint32_t *)&(src)[4 * ii]; \
        } \
    } while(0)

#endif
