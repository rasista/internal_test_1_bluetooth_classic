#ifndef LL_HCI_EVNT_DEF_H
#define LL_HCI_EVNT_DEF_H
#include <stdint.h>

/*INQUIRY_COMPLETE PARAMETERS*/
#define SET_INQUIRY_COMPLETE_STATUS(buf ,value)  buf[3] = value
 

/*INQUIRY_RESULT PARAMETERS*/
#define SET_INQUIRY_RESULT_NUM_RESP(buf ,value)  buf[3] = value
 
#define SET_INQUIRY_RESULT_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 
#define SET_INQUIRY_RESULT_PAGE_SCAN_REP_MODE(buf ,value)  buf[10] = value
 
#define SET_INQUIRY_RESULT_RESERVED(buf ,value)  *(uint16_t *)&buf[11] = value
 
#define SET_INQUIRY_RESULT_CLASS_OF_DEVICE(buf ,value)  memcpy((uint8_t *)&buf[13], &value, 3)
 
#define SET_INQUIRY_RESULT_CLOCK_OFFSET(buf ,value)  *(uint16_t *)&buf[16] = value
 

/*CONN_COMPLETE PARAMETERS*/
#define SET_CONN_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_CONN_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_CONN_COMPLETE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[6], &value, 6)
 
#define SET_CONN_COMPLETE_LINK_TYPE(buf ,value)  buf[12] = value
 
#define SET_CONN_COMPLETE_ENCRYPTION_ENABLE(buf ,value)  buf[13] = value
 

/*CONN_REQUEST PARAMETERS*/
#define SET_CONN_REQUEST_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_CONN_REQUEST_CLASS_OF_DEVICE(buf ,value)  memcpy((uint8_t *)&buf[9], &value, 3)
 
#define SET_CONN_REQUEST_LINK_TYPE(buf ,value)  buf[12] = value
 

/*DISCONNECTION_COMPLETE PARAMETERS*/
#define SET_DISCONNECTION_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_DISCONNECTION_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_DISCONNECTION_COMPLETE_REASON(buf ,value)  buf[6] = value
 

/*AUTHENTICATION_COMPLETE PARAMETERS*/
#define SET_AUTHENTICATION_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_AUTHENTICATION_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 

/*REMOTE_NAME_REQUEST_COMPLETE PARAMETERS*/
#define SET_REMOTE_NAME_REQUEST_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_REMOTE_NAME_REQUEST_COMPLETE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 
#define SET_REMOTE_NAME_REQUEST_COMPLETE_REMOTE_NAME(buf ,value)   memcpy((uint8_t *)&buf[10], &value, 248)
 

/*ENCRYTPION_CHANGE_V1 PARAMETERS*/
#define SET_ENCRYTPION_CHANGE_V1_STATUS(buf ,value)  buf[3] = value
 
#define SET_ENCRYTPION_CHANGE_V1_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_ENCRYTPION_CHANGE_V1_ENCRYPTION_ENABLE(buf ,value)  buf[6] = value
 

/*ENCRYTPION_CHANGE_V2 PARAMETERS*/
#define SET_ENCRYTPION_CHANGE_V2_STATUS(buf ,value)  buf[3] = value
 
#define SET_ENCRYTPION_CHANGE_V2_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_ENCRYTPION_CHANGE_V2_ENCRYPTION_ENABLE(buf ,value)  buf[6] = value
 
#define SET_ENCRYTPION_CHANGE_V2_ENCRYPTION_KEY_SIZE(buf ,value)  buf[7] = value
 

/*CHANGE_CONNECTION_LINK_KEY_COMPLETE PARAMETERS*/
#define SET_CHANGE_CONNECTION_LINK_KEY_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_CHANGE_CONNECTION_LINK_KEY_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 

/*LINK_KEY_TYPE_CHANGED PARAMETERS*/
#define SET_LINK_KEY_TYPE_CHANGED_STATUS(buf ,value)  buf[3] = value
 
#define SET_LINK_KEY_TYPE_CHANGED_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_LINK_KEY_TYPE_CHANGED_KEY_FLAG(buf ,value)  buf[6] = value
 

/*READ_REMOTE_SUPPORTED_FEATURES_COMPLETE PARAMETERS*/
#define SET_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_LMP_FEATURES(buf ,value)   memcpy((uint8_t *)&buf[6], &value, 8)
 

/*READ_REMOTE_VERSION_INFO_COMPLETE PARAMETERS*/
#define SET_READ_REMOTE_VERSION_INFO_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_READ_REMOTE_VERSION_INFO_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_READ_REMOTE_VERSION_INFO_COMPLETE_VERSION(buf ,value)  buf[6] = value
 
#define SET_READ_REMOTE_VERSION_INFO_COMPLETE_COMPANY_IDENTIFIER(buf ,value)  *(uint16_t *)&buf[7] = value
 
#define SET_READ_REMOTE_VERSION_INFO_COMPLETE_SUB_VERSION(buf ,value)  *(uint16_t *)&buf[9] = value
 

/*QOS_SETUP_COMPLETE PARAMETERS*/
#define SET_QOS_SETUP_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_QOS_SETUP_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_QOS_SETUP_COMPLETE_UNUSED(buf ,value)  buf[6] = value
 
#define SET_QOS_SETUP_COMPLETE_SERVICE_TYPE(buf ,value)  buf[7] = value
 
#define SET_QOS_SETUP_COMPLETE_TOKEN_RATE(buf ,value)  (*(uint32_t *)&buf[8]) = value
 
#define SET_QOS_SETUP_COMPLETE_PEAK_BANDWIDTH(buf ,value)  (*(uint32_t *)&buf[12]) = value
 
#define SET_QOS_SETUP_COMPLETE_LATENCY(buf ,value)  (*(uint32_t *)&buf[16]) = value
 
#define SET_QOS_SETUP_COMPLETE_DELAY_VARIATION(buf ,value)  (*(uint32_t *)&buf[20]) = value
 

/*CMD_COMPLETE PARAMETERS*/
#define SET_CMD_COMPLETE_NUM_HCI_CMD_PKTS(buf ,value)  buf[3] = value
 
#define SET_CMD_COMPLETE_CMD_OPCODE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_CMD_COMPLETE_RETURN_PARAMS(buf ,value)   memcpy((uint8_t *)&buf[6], &value, 0)
 

/*CMD_STATUS PARAMETERS*/
#define SET_CMD_STATUS_STATUS(buf ,value)  buf[3] = value
 
#define SET_CMD_STATUS_NUM_HCI_CMD_PKTS(buf ,value)  buf[4] = value
 
#define SET_CMD_STATUS_CMD_OPCODE(buf ,value)  *(uint16_t *)&buf[5] = value
 

/*HARDWARE_ERROR PARAMETERS*/
#define SET_HARDWARE_ERROR_HW_CODE(buf ,value)  buf[3] = value
 

/*FLUSH_OCCURRED PARAMETERS*/
#define SET_FLUSH_OCCURRED_HANDLE(buf ,value)  *(uint16_t *)&buf[3] = value
 

/*ROLE_CHANGE PARAMETERS*/
#define SET_ROLE_CHANGE_STATUS(buf ,value)  buf[3] = value
 
#define SET_ROLE_CHANGE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 
#define SET_ROLE_CHANGE_NEW_ROLE(buf ,value)  buf[10] = value
 

/*NUM_OF_COMPLETED_PKTS PARAMETERS*/
#define SET_NUM_OF_COMPLETED_PKTS_NUM_HANDLES(buf ,value)  buf[3] = value
 
#define SET_NUM_OF_COMPLETED_PKTS_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_NUM_OF_COMPLETED_PKTS_NUM_COMPLETED_PKTS(buf ,value)  *(uint16_t *)&buf[6] = value
 

/*MODE_CHANGE PARAMETERS*/
#define SET_MODE_CHANGE_STATUS(buf ,value)  buf[3] = value
 
#define SET_MODE_CHANGE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_MODE_CHANGE_CURRENT_MODE(buf ,value)  buf[6] = value
 
#define SET_MODE_CHANGE_INTERVAL(buf ,value)  buf[7] = value
 

/*RETURN_LINK_KEYS PARAMETERS*/
#define SET_RETURN_LINK_KEYS_NUM_KEYS(buf ,value)  buf[3] = value
 
#define SET_RETURN_LINK_KEYS_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 
#define SET_RETURN_LINK_KEYS_LINK_KEY(buf ,value)   memcpy((uint8_t *)&buf[10], &value, 16)
 

/*PIN_CODE_REQUEST PARAMETERS*/
#define SET_PIN_CODE_REQUEST_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 

/*LINK_KEY_REQUEST PARAMETERS*/
#define SET_LINK_KEY_REQUEST_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 

/*LINK_KEY_NOTIFICATION PARAMETERS*/
#define SET_LINK_KEY_NOTIFICATION_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_LINK_KEY_NOTIFICATION_LINK_KEY(buf ,value)   memcpy((uint8_t *)&buf[9], &value, 16)
 
#define SET_LINK_KEY_NOTIFICATION_KEY_TYPE(buf ,value)  buf[25] = value
 

/*LOOPBACK_CMD PARAMETERS*/
#define SET_LOOPBACK_CMD_HCI_CMD_PKTS(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 0)
 

/*DATA_BUFFER_OVERFLOW PARAMETERS*/
#define SET_DATA_BUFFER_OVERFLOW_LINK_TYPE(buf ,value)  buf[3] = value
 

/*MAX_SLOTS_CHANGE PARAMETERS*/
#define SET_MAX_SLOTS_CHANGE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[3] = value
 
#define SET_MAX_SLOTS_CHANGE_LMPMAX_SLOTS(buf ,value)  buf[5] = value
 

/*READ_CLOCK_OFFSET_COMPLETE PARAMETERS*/
#define SET_READ_CLOCK_OFFSET_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_READ_CLOCK_OFFSET_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_READ_CLOCK_OFFSET_COMPLETE_CLOCK_OFFSET(buf ,value)  *(uint16_t *)&buf[6] = value
 

/*CONN_PKT_TYPE_CHANGED PARAMETERS*/
#define SET_CONN_PKT_TYPE_CHANGED_STATUS(buf ,value)  buf[3] = value
 
#define SET_CONN_PKT_TYPE_CHANGED_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_CONN_PKT_TYPE_CHANGED_PKT_TYPE(buf ,value)  *(uint16_t *)&buf[6] = value
 

/*QOS_VIOLATION PARAMETERS*/
#define SET_QOS_VIOLATION_HANDLE(buf ,value)  *(uint16_t *)&buf[3] = value
 

/*PAGE_SCAN_REP_MODE_CHANGE PARAMETERS*/
#define SET_PAGE_SCAN_REP_MODE_CHANGE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_PAGE_SCAN_REP_MODE_CHANGE_PAGE_SCAN_REP_MODE(buf ,value)  buf[9] = value
 

/*FLOW_SPECIFICATION_COMPLETE PARAMETERS*/
#define SET_FLOW_SPECIFICATION_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_UNUSED(buf ,value)  buf[6] = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_FLOW_DIRECTION(buf ,value)  buf[7] = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_SERVICE_TYPE(buf ,value)  buf[8] = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_TOKEN_RATE(buf ,value)  (*(uint32_t *)&buf[9]) = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_TOKEN_BUCKET_SIZE(buf ,value)  (*(uint32_t *)&buf[13]) = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_PEAK_BANDWIDTH(buf ,value)  (*(uint32_t *)&buf[17]) = value
 
#define SET_FLOW_SPECIFICATION_COMPLETE_ACCESS_LATENCY(buf ,value)  (*(uint32_t *)&buf[21]) = value
 

/*INQUIRY_RESULT_RSSI PARAMETERS*/
#define SET_INQUIRY_RESULT_RSSI_NUM_RESP(buf ,value)  buf[3] = value
 
#define SET_INQUIRY_RESULT_RSSI_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 
#define SET_INQUIRY_RESULT_RSSI_PAGE_SCAN_REPITION_MODE(buf ,value)  buf[10] = value
 
#define SET_INQUIRY_RESULT_RSSI_RESERVED(buf ,value)  buf[11] = value
 
#define SET_INQUIRY_RESULT_RSSI_CLASS_OF_DEVICE(buf ,value)  memcpy((uint8_t *)&buf[12], &value, 3)
 
#define SET_INQUIRY_RESULT_RSSI_CLOCK_OFFSET(buf ,value)  *(uint16_t *)&buf[15] = value
 
#define SET_INQUIRY_RESULT_RSSI_RSSI(buf ,value)  buf[17] = value
 

/*READ_REMOTE_EXTENDED_FEATURES_COMPLETE PARAMETERS*/
#define SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_PG_NUM(buf ,value)  buf[6] = value
 
#define SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_MAX_PAGE_NUM(buf ,value)  buf[7] = value
 
#define SET_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EXTENDED_LMP_FEATURES(buf ,value)   memcpy((uint8_t *)&buf[8], &value, 8)
 

/*SYNC_CONN_COMPLETE PARAMETERS*/
#define SET_SYNC_CONN_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_SYNC_CONN_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_SYNC_CONN_COMPLETE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[6], &value, 6)
 
#define SET_SYNC_CONN_COMPLETE_LINK_TYPE(buf ,value)  buf[12] = value
 
#define SET_SYNC_CONN_COMPLETE_TRANSMISSION_INTERVAL(buf ,value)  buf[13] = value
 
#define SET_SYNC_CONN_COMPLETE_RETRANSMISSIONWINDOW(buf ,value)  buf[14] = value
 
#define SET_SYNC_CONN_COMPLETE_RX_PKT_LENGTH(buf ,value)  *(uint16_t *)&buf[15] = value
 
#define SET_SYNC_CONN_COMPLETE_TX_PKT_LENGTH(buf ,value)  *(uint16_t *)&buf[17] = value
 
#define SET_SYNC_CONN_COMPLETE_AIR_MODE(buf ,value)  buf[19] = value
 

/*SYNC_CONN_CHANGED PARAMETERS*/
#define SET_SYNC_CONN_CHANGED_STATUS(buf ,value)  buf[3] = value
 
#define SET_SYNC_CONN_CHANGED_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_SYNC_CONN_CHANGED_TRANSMISSION_INTERVAL(buf ,value)  buf[6] = value
 
#define SET_SYNC_CONN_CHANGED_RETRANSMISSIONWINDOW(buf ,value)  buf[7] = value
 
#define SET_SYNC_CONN_CHANGED_RX_PKT_LENGTH(buf ,value)  *(uint16_t *)&buf[8] = value
 
#define SET_SYNC_CONN_CHANGED_TX_PKT_LENGTH(buf ,value)  *(uint16_t *)&buf[10] = value
 

/*SNIFF_SUBRATING PARAMETERS*/
#define SET_SNIFF_SUBRATING_STATUS(buf ,value)  buf[3] = value
 
#define SET_SNIFF_SUBRATING_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 
#define SET_SNIFF_SUBRATING_MAX_TXLATENCY(buf ,value)  *(uint16_t *)&buf[6] = value
 
#define SET_SNIFF_SUBRATING_MAX_RX_LATENCY(buf ,value)  *(uint16_t *)&buf[8] = value
 
#define SET_SNIFF_SUBRATING_MIN_REMOTE_TIMEOUT(buf ,value)  *(uint16_t *)&buf[10] = value
 
#define SET_SNIFF_SUBRATING_MIN_LOCAL_TIMEOUT(buf ,value)  *(uint16_t *)&buf[12] = value
 

/*EXTENDED_INQUIRY_RESULT PARAMETERS*/
#define SET_EXTENDED_INQUIRY_RESULT_NUM_RESP(buf ,value)  buf[3] = value
 
#define SET_EXTENDED_INQUIRY_RESULT_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 
#define SET_EXTENDED_INQUIRY_RESULT_PAGE_SCAN_REPITION_MODE(buf ,value)  buf[10] = value
 
#define SET_EXTENDED_INQUIRY_RESULT_RESERVED(buf ,value)  buf[11] = value
 
#define SET_EXTENDED_INQUIRY_RESULT_CLASS_OF_DEVICE(buf ,value)  memcpy((uint8_t *)&buf[12], &value, 3)
 
#define SET_EXTENDED_INQUIRY_RESULT_CLOCK_OFFSET(buf ,value)  *(uint16_t *)&buf[15] = value
 
#define SET_EXTENDED_INQUIRY_RESULT_RSSI(buf ,value)  buf[17] = value
 
#define SET_EXTENDED_INQUIRY_RESULT_EXTENDED_INQUIRY_RESP(buf ,value)   memcpy((uint8_t *)&buf[18], &value, 240)
 

/*ENCRYPTION_KEY_REFRESH_COMPLETE PARAMETERS*/
#define SET_ENCRYPTION_KEY_REFRESH_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_ENCRYPTION_KEY_REFRESH_COMPLETE_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[4] = value
 

/*IO_CAPABILITY_REQ PARAMETERS*/
#define SET_IO_CAPABILITY_REQ_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 

/*IO_CAPABILITY_RESP PARAMETERS*/
#define SET_IO_CAPABILITY_RESP_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_IO_CAPABILITY_RESP_IO_CAPABILITY(buf ,value)  buf[9] = value
 
#define SET_IO_CAPABILITY_RESP_OOB_DATA_PRESENT(buf ,value)  buf[10] = value
 
#define SET_IO_CAPABILITY_RESP_AUTHENTICATION_REQUIREMENTS(buf ,value)  buf[11] = value
 

/*USER_CONFIRMATION_REQUEST PARAMETERS*/
#define SET_USER_CONFIRMATION_REQUEST_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_USER_CONFIRMATION_REQUEST_NUMERIC_VALUE(buf ,value)  (*(uint32_t *)&buf[9]) = value
 

/*USER_PASSKEY_REQUEST PARAMETERS*/
#define SET_USER_PASSKEY_REQUEST_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 

/*REMOTE_OOB_DATA_REQUEST PARAMETERS*/
#define SET_REMOTE_OOB_DATA_REQUEST_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 

/*SIMPLE_PAIRING_COMPLETE PARAMETERS*/
#define SET_SIMPLE_PAIRING_COMPLETE_STATUS(buf ,value)  buf[3] = value
 
#define SET_SIMPLE_PAIRING_COMPLETE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 6)
 

/*LINK_SUPERVISION_TIMEOUT_CHANGED PARAMETERS*/
#define SET_LINK_SUPERVISION_TIMEOUT_CHANGED_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[3] = value
 
#define SET_LINK_SUPERVISION_TIMEOUT_CHANGED_LINK_SUPERVISION_TIMEOUT(buf ,value)  *(uint16_t *)&buf[5] = value
 

/*ENHANCED_FLUSH_COMPLETE PARAMETERS*/
#define SET_ENHANCED_FLUSH_COMPLETE_HANDLE(buf ,value)  *(uint16_t *)&buf[3] = value
 

/*USER_PASSKEY_NOTIFICATION PARAMETERS*/
#define SET_USER_PASSKEY_NOTIFICATION_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_USER_PASSKEY_NOTIFICATION_PASSKEY(buf ,value)  (*(uint32_t *)&buf[9]) = value
 

/*KEYPRESS_NOTIFICATION PARAMETERS*/
#define SET_KEYPRESS_NOTIFICATION_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_KEYPRESS_NOTIFICATION_NOTIFICATION_TYPE(buf ,value)  buf[9] = value
 

/*REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION PARAMETERS*/
#define SET_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[3], &value, 6)
 
#define SET_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_HOST_SUPPORTED_FEATURES(buf ,value)   memcpy((uint8_t *)&buf[9], &value, 8)
 

/*NUM_OF_COMPLETED_DATA_BLOCKS PARAMETERS*/
#define SET_NUM_OF_COMPLETED_DATA_BLOCKS_TOTAL_NUM_DATA_BLOCKS(buf ,value)  *(uint16_t *)&buf[3] = value
 
#define SET_NUM_OF_COMPLETED_DATA_BLOCKS_NUM_HANDLES(buf ,value)  buf[5] = value
 
#define SET_NUM_OF_COMPLETED_DATA_BLOCKS_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[6] = value
 
#define SET_NUM_OF_COMPLETED_DATA_BLOCKS_NUM_COMPLETED_PKTS(buf ,value)  *(uint16_t *)&buf[8] = value
 
#define SET_NUM_OF_COMPLETED_DATA_BLOCKS_NUM_COMPLETED_BLOCKS(buf ,value)  *(uint16_t *)&buf[10] = value
 

/*VENDOR_EVENT PARAMETERS*/
#define SET_VENDOR_EVENT_SUBEVENT_CODE(buf ,value)  buf[3] = value
 
#define SET_VENDOR_EVENT_VENDOR_SPECIFIC_PARAMS(buf ,value)   memcpy((uint8_t *)&buf[4], &value, 9)
 

/*VIRTUAL_CONNECT_COMPLETE PARAMETERS*/
#define SET_VIRTUAL_CONNECT_COMPLETE_VENDOR_ID(buf ,value)  *(uint16_t *)&buf[3] = value
 
#define SET_VIRTUAL_CONNECT_COMPLETE_SUB_EVENT_CODE(buf ,value)  buf[5] = value
 
#define SET_VIRTUAL_CONNECT_COMPLETE_VIRTUAL_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[6] = value
 
#define SET_VIRTUAL_CONNECT_COMPLETE_BD_ADDR(buf ,value)   memcpy((uint8_t *)&buf[8], &value, 6)
 
#define SET_VIRTUAL_CONNECT_COMPLETE_VIRTUAL_LINK_TYPE(buf ,value)  buf[14] = value
 
#define SET_VIRTUAL_CONNECT_COMPLETE_STATUS(buf ,value)  buf[15] = value
 
#define SET_VIRTUAL_CONNECT_COMPLETE_RESERVED(buf ,value)  memcpy((uint8_t *)&buf[16], &value, 3)
 

/*VIRTUAL_DISCONNECT_COMPLETE PARAMETERS*/
#define SET_VIRTUAL_DISCONNECT_COMPLETE_VENDOR_ID(buf ,value)  *(uint16_t *)&buf[3] = value
 
#define SET_VIRTUAL_DISCONNECT_COMPLETE_SUB_EVENT_CODE(buf ,value)  *(uint16_t *)&buf[5] = value
 
#define SET_VIRTUAL_DISCONNECT_COMPLETE_VIRTUAL_CONN_HANDLE(buf ,value)  *(uint16_t *)&buf[7] = value
 
#define SET_VIRTUAL_DISCONNECT_COMPLETE_REASON(buf ,value)  buf[9] = value
 
#define SET_VIRTUAL_DISCONNECT_COMPLETE_RESERVED(buf ,value)  memcpy((uint8_t *)&buf[10], &value, 3)
 



#endif