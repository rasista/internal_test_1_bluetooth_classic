#ifndef LL_HCI_CMD_SET_DEF_STATUS_EVENT_H
#define LL_HCI_CMD_SET_DEF_STATUS_EVENT_H

/* Note - These macros are to be used while preparing a status event packet*/
/* SE in the macro name stands for Status Event */

/* LINK_CONTROL */
#define SET_SE_INQUIRY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_INQUIRY_CANCEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_PERIODIC_INQUIRY_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_EXIT_PERIODIC_INQUIRY_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_CREATE_CONNECTION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_DISCONNECT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_CREATE_CONNECTION_CANCEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_ACCEPT_CONNECTION_REQUEST_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REJECT_CONNECTION_REQUEST_STATUS(buf, value)  buf[3] = value 

#define SET_SE_LINK_KEY_REQUEST_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_LINK_KEY_REQUEST_NEGATIVE_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_PIN_CODE_REQUEST_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_PIN_CODE_REQUEST_NEGATIVE_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_CHANGE_CONNECTION_PACKET_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_AUTHENTICATION_REQUESTED_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SET_CONNECTION_ENCRYPTION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_CHANGE_CONNECTION_LINK_KEY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REMOTE_NAME_REQUEST_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REMOTE_NAME_REQUEST_CANCEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_REMOTE_SUPPORTED_FEATURES_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_REMOTE_EXTENDED_FEATURES_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_REMOTE_VERSION_INFORMATION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_CLOCK_OFFSET_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SETUP_SYNCHRONOUS_CONNECTION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REJECT_SYNCHRONOUS_CONNECTION_REQUEST_STATUS(buf, value)  buf[3] = value 

#define SET_SE_IO_CAPABILITY_REQUEST_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_USER_CONFIRMATION_REQUEST_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_USER_PASSKEY_REQUEST_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REMOTE_OOB_DATA_REQUEST_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_STATUS(buf, value)  buf[3] = value 

/* LINK_POLICY */
#define SET_SE_SNIFF_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_EXIT_SNIFF_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_QOS_SETUP_STATUS(buf, value)  buf[3] = value 

#define SET_SE_ROLE_DISCOVERY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SWITCH_ROLE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LINK_POLICY_SETTINGS_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_LINK_POLICY_SETTINGS_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_DEFAULT_LINK_POLICY_SETTINGS_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_DEFAULT_LINK_POLICY_SETTINGS_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SNIFF_SUBRATING_STATUS(buf, value)  buf[3] = value 

/* CONTROLLER_AND_BASEBAND */
#define SET_SE_SET_EVENT_MASK_STATUS(buf, value)  buf[3] = value 

#define SET_SE_RESET_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SET_EVENT_FILTER_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_PIN_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_PIN_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_STORED_LINK_KEY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_STORED_LINK_KEY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_DELETE_STORED_LINK_KEY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_LOCAL_NAME_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LOCAL_NAME_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_CONNECTION_ACCEPT_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_CONNECTION_ACCEPT_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_PAGE_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_PAGE_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_SCAN_ENABLE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_SCAN_ENABLE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_PAGE_SCAN_ACTIVITY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_PAGE_SCAN_ACTIVITY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_INQUIRY_SCAN_ACTIVITY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_INQUIRY_SCAN_ACTIVITY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_AUTHENTICATION_ENABLE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_AUTHENTICATION_ENABLE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_CLASS_OF_DEVICE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_CLASS_OF_DEVICE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_VOICE_SETTING_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_VOICE_SETTING_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_AUTOMATIC_FLUSH_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_AUTOMATIC_FLUSH_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_TRANSMIT_POWER_LEVEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LINK_SUPERVISION_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_LINK_SUPERVISION_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_NUMBER_OF_SUPPORTED_IAC_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_CURRENT_IAC_LAP_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_CURRENT_IAC_LAP_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SET_AFH_HOST_CHANNEL_CLASSIFICATION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_INQUIRY_SCAN_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_INQUIRY_SCAN_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_INQUIRY_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_INQUIRY_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_PAGE_SCAN_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_PAGE_SCAN_TYPE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_AFH_CHANNEL_ASSESSMENT_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_EXTENDED_INQUIRY_RESPONSE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_EXTENDED_INQUIRY_RESPONSE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_REFRESH_ENCRYPTION_KEY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_SIMPLE_PAIRING_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_SIMPLE_PAIRING_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LOCAL_OOB_DATA_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SEND_KEYPRESS_NOTIFICATION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_ENHANCED_FLUSH_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_ENHANCED_TRANSMIT_POWER_LEVEL_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_SECURE_CONNECTIONS_HOST_SUPPORT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_EXTENDED_PAGE_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_EXTENDED_PAGE_TIMEOUT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_EXTENDED_INQUIRY_LENGTH_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_EXTENDED_INQUIRY_LENGTH_STATUS(buf, value)  buf[3] = value 

/* INFORMATIONAL_PARAMETERS */
#define SET_SE_READ_LOCAL_VERSION_INFORMATION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LOCAL_SUPPORTED_COMMANDS_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LOCAL_SUPPORTED_FEATURES_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_LOCAL_EXTENDED_FEATURES_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_BUFFER_SIZE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_BD_ADDR_STATUS(buf, value)  buf[3] = value 

/* STATUS_PARAMETERS */
#define SET_SE_READ_LINK_QUALITY_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_RSSI_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_AFH_CHANNEL_MAP_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_CLOCK_STATUS(buf, value)  buf[3] = value 

#define SET_SE_READ_ENCRYPTION_KEY_SIZE_STATUS(buf, value)  buf[3] = value 

/* TEST_COMMANDS */
#define SET_SE_ENABLE_DEVICE_UNDER_TEST_MODE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_WRITE_SIMPLE_PAIRING_DEBUG_MODE_STATUS(buf, value)  buf[3] = value 

/* VENDOR_SPECIFIC */
#define SET_SE_ECHO_STATUS(buf, value)  buf[3] = value 

#define SET_SE_VIRTUAL_LISTEN_CONNECTION_STATUS(buf, value)  buf[3] = value 

#define SET_SE_VIRTUAL_CANCEL_LISTEN_STATUS(buf, value)  buf[3] = value 

#define SET_SE_VIRTUAL_CONNECT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_VIRTUAL_CANCEL_CONNECT_STATUS(buf, value)  buf[3] = value 

#define SET_SE_INTERNAL_EVENT_TRIGGER_FROM_LPW_STATUS(buf, value)  buf[3] = value 

#define SET_SE_KILL_FIRMWARE_STATUS(buf, value)  buf[3] = value 

#define SET_SE_SET_LOCAL_LMP_FEATURES_STATUS(buf, value)  buf[3] = value 

#endif