#ifndef _BTC_LMP_SIMPLE_SECURE_PAIRING_H
#define _BTC_LMP_SIMPLE_SECURE_PAIRING_H

#define PASSKEY_ENTRY_COMPLETED 0x04

// Macro to check if received public key type matches P-192
#define IS_PUBLIC_KEY_TYPE_P192(major, minor) \
    ((major == PUBLIC_KEY_MAJOR_TYPE_192) && (minor == PUBLIC_KEY_MINOR_TYPE_192))

// Macro to check if received public key type matches P-256  
#define IS_PUBLIC_KEY_TYPE_P256(major, minor) \
    ((major == PUBLIC_KEY_MAJOR_TYPE_256) && (minor == PUBLIC_KEY_MINOR_TYPE_256))

// Generic macro to check if received public key type is supported
#define IS_SUPPORTED_PUBLIC_KEY_TYPE(major, minor) \
    (IS_PUBLIC_KEY_TYPE_P192(major, minor) || IS_PUBLIC_KEY_TYPE_P256(major, minor))

#define GET_DEVICE_PUBLIC_KEY(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? btc_dev_mgmt_info->public_key_256 : btc_dev_mgmt_info->public_key)


#define GET_DEVICE_PUBLIC_KEY_MID(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? &btc_dev_mgmt_info->public_key_256[32] : &btc_dev_mgmt_info->public_key[24])

#define GET_PERIPHERAL_PUBLIC_KEY_MID(btc_dev_mgmt_info, acl_enc_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? &acl_enc_info->public_key[32] : &acl_enc_info->public_key[24])

#define GET_DEVICE_CURVE_TYPE(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? 1 : 0)

    #define GET_DEVICE_PRIVATE_KEY(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? btc_dev_mgmt_info->private_key_256 : btc_dev_mgmt_info->private_key)

#define GET_DEVICE_DEBUG_PUBLIC_KEY(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? btc_dev_mgmt_info->debug_public_key_256 : btc_dev_mgmt_info->debug_public_key)

#define GET_DEVICE_PUBLIC_KEY_LEN(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? PUBLIC_KEY_LEN_256 : PUBLIC_KEY_LEN_192)

#define GET_DHKEY_LEN(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? DHKEY_LEN_256 : DHKEY_LEN)

#define GET_DEVICE_PRIVATE_KEY_LEN(btc_dev_mgmt_info) \
    ((btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT) ? PRIVATE_KEY_LEN_256 : PRIVATE_KEY_LEN)

#define IS_SECURE_CONNECTIONS_ENABLED(btc_dev_mgmt_info)    (btc_dev_mgmt_info->dev_status_flags & SECURE_CONNECTIONS_HOST_SUPPORT)

#define GET_ENCAPSULATED_HEADER(lmp_input_params, btc_dev_mgmt_info) \
    do { \
        if (IS_SECURE_CONNECTIONS_ENABLED(btc_dev_mgmt_info)) { \
            lmp_input_params[0] = PUBLIC_KEY_MAJOR_TYPE_256; \
            lmp_input_params[1] = PUBLIC_KEY_MINOR_TYPE_256; \
            lmp_input_params[2] = PUBLIC_KEY_LEN_256; \
        } else { \
            lmp_input_params[0] = PUBLIC_KEY_MAJOR_TYPE_192; \
            lmp_input_params[1] = PUBLIC_KEY_MINOR_TYPE_192; \
            lmp_input_params[2] = PUBLIC_KEY_LEN_192; \
        } \
    } while(0)

#endif
