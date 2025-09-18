import copy 
import ctypes
import json 
import numpy as np
from enum import Enum
import logging
logger = logging.getLogger()



class hci_event_code(Enum):
    HCI_EVENT_CODE_INQUIRY_COMPLETE = 1
    HCI_EVENT_CODE_INQUIRY_RESULT = 2
    HCI_EVENT_CODE_CONN_COMPLETE = 3
    HCI_EVENT_CODE_CONN_REQUEST = 4
    HCI_EVENT_CODE_DISCONNECTION_COMPLETE = 5
    HCI_EVENT_CODE_AUTHENTICATION_COMPLETE = 6
    HCI_EVENT_CODE_REMOTE_NAME_REQUEST_COMPLETE = 7
    HCI_EVENT_CODE_ENCRYTPION_CHANGE_V1 = 8
    HCI_EVENT_CODE_ENCRYTPION_CHANGE_V2 = 89
    HCI_EVENT_CODE_CHANGE_CONNECTION_LINK_KEY_COMPLETE = 9
    HCI_EVENT_CODE_LINK_KEY_TYPE_CHANGED = 10
    HCI_EVENT_CODE_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE = 11
    HCI_EVENT_CODE_READ_REMOTE_VERSION_INFO_COMPLETE = 12
    HCI_EVENT_CODE_QOS_SETUP_COMPLETE = 13
    HCI_EVENT_CODE_CMD_COMPLETE = 14
    HCI_EVENT_CODE_CMD_STATUS = 15
    HCI_EVENT_CODE_HARDWARE_ERROR = 16
    HCI_EVENT_CODE_FLUSH_OCCURRED = 17
    HCI_EVENT_CODE_ROLE_CHANGE = 18
    HCI_EVENT_CODE_NUM_OF_COMPLETED_PKTS = 19
    HCI_EVENT_CODE_MODE_CHANGE = 20
    HCI_EVENT_CODE_RETURN_LINK_KEYS = 21
    HCI_EVENT_CODE_PIN_CODE_REQUEST = 22
    HCI_EVENT_CODE_LINK_KEY_REQUEST = 23
    HCI_EVENT_CODE_LINK_KEY_NOTIFICATION = 24
    HCI_EVENT_CODE_LOOPBACK_CMD = 25
    HCI_EVENT_CODE_DATA_BUFFER_OVERFLOW = 26
    HCI_EVENT_CODE_MAX_SLOTS_CHANGE = 27
    HCI_EVENT_CODE_READ_CLOCK_OFFSET_COMPLETE = 28
    HCI_EVENT_CODE_CONN_PKT_TYPE_CHANGED = 29
    HCI_EVENT_CODE_QOS_VIOLATION = 30
    HCI_EVENT_CODE_PAGE_SCAN_REP_MODE_CHANGE = 32
    HCI_EVENT_CODE_FLOW_SPECIFICATION_COMPLETE = 33
    HCI_EVENT_CODE_INQUIRY_RESULT_RSSI = 34
    HCI_EVENT_CODE_READ_REMOTE_EXTENDED_FEATURES_COMPLETE = 35
    HCI_EVENT_CODE_SYNC_CONN_COMPLETE = 44
    HCI_EVENT_CODE_SYNC_CONN_CHANGED = 45
    HCI_EVENT_CODE_SNIFF_SUBRATING = 46
    HCI_EVENT_CODE_EXTENDED_INQUIRY_RESULT = 47
    HCI_EVENT_CODE_ENCRYPTION_KEY_REFRESH_COMPLETE = 48
    HCI_EVENT_CODE_IO_CAPABILITY_REQ = 49
    HCI_EVENT_CODE_IO_CAPABILITY_RESP = 50
    HCI_EVENT_CODE_USER_CONFIRMATION_REQUEST = 51
    HCI_EVENT_CODE_USER_PASSKEY_REQUEST = 52
    HCI_EVENT_CODE_REMOTE_OOB_DATA_REQUEST = 53
    HCI_EVENT_CODE_SIMPLE_PAIRING_COMPLETE = 54
    HCI_EVENT_CODE_LINK_SUPERVISION_TIMEOUT_CHANGED = 56
    HCI_EVENT_CODE_ENHANCED_FLUSH_COMPLETE = 57
    HCI_EVENT_CODE_USER_PASSKEY_NOTIFICATION = 59
    HCI_EVENT_CODE_KEYPRESS_NOTIFICATION = 60
    HCI_EVENT_CODE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION = 61
    HCI_EVENT_CODE_NUM_OF_COMPLETED_DATA_BLOCKS = 72
    HCI_EVENT_CODE_VENDOR_EVENT = 255
    HCI_EVENT_CODE_VIRTUAL_CONNECT_COMPLETE = 252
    HCI_EVENT_CODE_VIRTUAL_DISCONNECT_COMPLETE = 253


class callback_handlers:
    def __init__(self):
        self.command_handlers = {
                hci_event_code.HCI_EVENT_CODE_INQUIRY_COMPLETE.value: self.inquiry_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_INQUIRY_RESULT.value: self.inquiry_result_callback_handler,
                hci_event_code.HCI_EVENT_CODE_CONN_COMPLETE.value: self.conn_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_CONN_REQUEST.value: self.conn_request_callback_handler,
                hci_event_code.HCI_EVENT_CODE_DISCONNECTION_COMPLETE.value: self.disconnection_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_AUTHENTICATION_COMPLETE.value: self.authentication_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_REMOTE_NAME_REQUEST_COMPLETE.value: self.remote_name_request_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_ENCRYTPION_CHANGE_V1.value: self.encrytpion_change_v1_callback_handler,
                hci_event_code.HCI_EVENT_CODE_ENCRYTPION_CHANGE_V2.value: self.encrytpion_change_v2_callback_handler,
                hci_event_code.HCI_EVENT_CODE_CHANGE_CONNECTION_LINK_KEY_COMPLETE.value: self.change_connection_link_key_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_LINK_KEY_TYPE_CHANGED.value: self.link_key_type_changed_callback_handler,
                hci_event_code.HCI_EVENT_CODE_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE.value: self.read_remote_supported_features_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_READ_REMOTE_VERSION_INFO_COMPLETE.value: self.read_remote_version_info_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_QOS_SETUP_COMPLETE.value: self.qos_setup_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_CMD_COMPLETE.value: self.cmd_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_CMD_STATUS.value: self.cmd_status_callback_handler,
                hci_event_code.HCI_EVENT_CODE_HARDWARE_ERROR.value: self.hardware_error_callback_handler,
                hci_event_code.HCI_EVENT_CODE_FLUSH_OCCURRED.value: self.flush_occurred_callback_handler,
                hci_event_code.HCI_EVENT_CODE_ROLE_CHANGE.value: self.role_change_callback_handler,
                hci_event_code.HCI_EVENT_CODE_NUM_OF_COMPLETED_PKTS.value: self.num_of_completed_pkts_callback_handler,
                hci_event_code.HCI_EVENT_CODE_MODE_CHANGE.value: self.mode_change_callback_handler,
                hci_event_code.HCI_EVENT_CODE_RETURN_LINK_KEYS.value: self.return_link_keys_callback_handler,
                hci_event_code.HCI_EVENT_CODE_PIN_CODE_REQUEST.value: self.pin_code_request_callback_handler,
                hci_event_code.HCI_EVENT_CODE_LINK_KEY_REQUEST.value: self.link_key_request_callback_handler,
                hci_event_code.HCI_EVENT_CODE_LINK_KEY_NOTIFICATION.value: self.link_key_notification_callback_handler,
                hci_event_code.HCI_EVENT_CODE_LOOPBACK_CMD.value: self.loopback_cmd_callback_handler,
                hci_event_code.HCI_EVENT_CODE_DATA_BUFFER_OVERFLOW.value: self.data_buffer_overflow_callback_handler,
                hci_event_code.HCI_EVENT_CODE_MAX_SLOTS_CHANGE.value: self.max_slots_change_callback_handler,
                hci_event_code.HCI_EVENT_CODE_READ_CLOCK_OFFSET_COMPLETE.value: self.read_clock_offset_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_CONN_PKT_TYPE_CHANGED.value: self.conn_pkt_type_changed_callback_handler,
                hci_event_code.HCI_EVENT_CODE_QOS_VIOLATION.value: self.qos_violation_callback_handler,
                hci_event_code.HCI_EVENT_CODE_PAGE_SCAN_REP_MODE_CHANGE.value: self.page_scan_rep_mode_change_callback_handler,
                hci_event_code.HCI_EVENT_CODE_FLOW_SPECIFICATION_COMPLETE.value: self.flow_specification_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_INQUIRY_RESULT_RSSI.value: self.inquiry_result_rssi_callback_handler,
                hci_event_code.HCI_EVENT_CODE_READ_REMOTE_EXTENDED_FEATURES_COMPLETE.value: self.read_remote_extended_features_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_SYNC_CONN_COMPLETE.value: self.sync_conn_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_SYNC_CONN_CHANGED.value: self.sync_conn_changed_callback_handler,
                hci_event_code.HCI_EVENT_CODE_SNIFF_SUBRATING.value: self.sniff_subrating_callback_handler,
                hci_event_code.HCI_EVENT_CODE_EXTENDED_INQUIRY_RESULT.value: self.extended_inquiry_result_callback_handler,
                hci_event_code.HCI_EVENT_CODE_ENCRYPTION_KEY_REFRESH_COMPLETE.value: self.encryption_key_refresh_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_IO_CAPABILITY_REQ.value: self.io_capability_req_callback_handler,
                hci_event_code.HCI_EVENT_CODE_IO_CAPABILITY_RESP.value: self.io_capability_resp_callback_handler,
                hci_event_code.HCI_EVENT_CODE_USER_CONFIRMATION_REQUEST.value: self.user_confirmation_request_callback_handler,
                hci_event_code.HCI_EVENT_CODE_USER_PASSKEY_REQUEST.value: self.user_passkey_request_callback_handler,
                hci_event_code.HCI_EVENT_CODE_REMOTE_OOB_DATA_REQUEST.value: self.remote_oob_data_request_callback_handler,
                hci_event_code.HCI_EVENT_CODE_SIMPLE_PAIRING_COMPLETE.value: self.simple_pairing_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_LINK_SUPERVISION_TIMEOUT_CHANGED.value: self.link_supervision_timeout_changed_callback_handler,
                hci_event_code.HCI_EVENT_CODE_ENHANCED_FLUSH_COMPLETE.value: self.enhanced_flush_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_USER_PASSKEY_NOTIFICATION.value: self.user_passkey_notification_callback_handler,
                hci_event_code.HCI_EVENT_CODE_KEYPRESS_NOTIFICATION.value: self.keypress_notification_callback_handler,
                hci_event_code.HCI_EVENT_CODE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION.value: self.remote_host_supported_features_notification_callback_handler,
                hci_event_code.HCI_EVENT_CODE_NUM_OF_COMPLETED_DATA_BLOCKS.value: self.num_of_completed_data_blocks_callback_handler,
                hci_event_code.HCI_EVENT_CODE_VENDOR_EVENT.value: self.vendor_event_callback_handler,
                hci_event_code.HCI_EVENT_CODE_VIRTUAL_CONNECT_COMPLETE.value: self.virtual_connect_complete_callback_handler,
                hci_event_code.HCI_EVENT_CODE_VIRTUAL_DISCONNECT_COMPLETE.value: self.virtual_disconnect_complete_callback_handler,
        }

    def inquiry_complete_callback_handler(self, packet, opcode):
        pass
    def inquiry_result_callback_handler(self, packet, opcode):
        pass
    def conn_complete_callback_handler(self, packet, opcode):
        pass
    def conn_request_callback_handler(self, packet, opcode):
        pass
    def disconnection_complete_callback_handler(self, packet, opcode):
        pass
    def authentication_complete_callback_handler(self, packet, opcode):
        pass
    def remote_name_request_complete_callback_handler(self, packet, opcode):
        pass
    def encrytpion_change_v1_callback_handler(self, packet, opcode):
        pass
    def encrytpion_change_v2_callback_handler(self, packet, opcode):
        pass
    def change_connection_link_key_complete_callback_handler(self, packet, opcode):
        pass
    def link_key_type_changed_callback_handler(self, packet, opcode):
        pass
    def read_remote_supported_features_complete_callback_handler(self, packet, opcode):
        pass
    def read_remote_version_info_complete_callback_handler(self, packet, opcode):
        pass
    def qos_setup_complete_callback_handler(self, packet, opcode):
        pass
    def cmd_complete_callback_handler(self, packet, opcode):
        pass
    def cmd_status_callback_handler(self, packet, opcode):
        pass
    def hardware_error_callback_handler(self, packet, opcode):
        pass
    def flush_occurred_callback_handler(self, packet, opcode):
        pass
    def role_change_callback_handler(self, packet, opcode):
        pass
    def num_of_completed_pkts_callback_handler(self, packet, opcode):
        pass
    def mode_change_callback_handler(self, packet, opcode):
        pass
    def return_link_keys_callback_handler(self, packet, opcode):
        pass
    def pin_code_request_callback_handler(self, packet, opcode):
        pass
    def link_key_request_callback_handler(self, packet, opcode):
        pass
    def link_key_notification_callback_handler(self, packet, opcode):
        pass
    def loopback_cmd_callback_handler(self, packet, opcode):
        pass
    def data_buffer_overflow_callback_handler(self, packet, opcode):
        pass
    def max_slots_change_callback_handler(self, packet, opcode):
        pass
    def read_clock_offset_complete_callback_handler(self, packet, opcode):
        pass
    def conn_pkt_type_changed_callback_handler(self, packet, opcode):
        pass
    def qos_violation_callback_handler(self, packet, opcode):
        pass
    def page_scan_rep_mode_change_callback_handler(self, packet, opcode):
        pass
    def flow_specification_complete_callback_handler(self, packet, opcode):
        pass
    def inquiry_result_rssi_callback_handler(self, packet, opcode):
        pass
    def read_remote_extended_features_complete_callback_handler(self, packet, opcode):
        pass
    def sync_conn_complete_callback_handler(self, packet, opcode):
        pass
    def sync_conn_changed_callback_handler(self, packet, opcode):
        pass
    def sniff_subrating_callback_handler(self, packet, opcode):
        pass
    def extended_inquiry_result_callback_handler(self, packet, opcode):
        pass
    def encryption_key_refresh_complete_callback_handler(self, packet, opcode):
        pass
    def io_capability_req_callback_handler(self, packet, opcode):
        pass
    def io_capability_resp_callback_handler(self, packet, opcode):
        pass
    def user_confirmation_request_callback_handler(self, packet, opcode):
        pass
    def user_passkey_request_callback_handler(self, packet, opcode):
        pass
    def remote_oob_data_request_callback_handler(self, packet, opcode):
        pass
    def simple_pairing_complete_callback_handler(self, packet, opcode):
        pass
    def link_supervision_timeout_changed_callback_handler(self, packet, opcode):
        pass
    def enhanced_flush_complete_callback_handler(self, packet, opcode):
        pass
    def user_passkey_notification_callback_handler(self, packet, opcode):
        pass
    def keypress_notification_callback_handler(self, packet, opcode):
        pass
    def remote_host_supported_features_notification_callback_handler(self, packet, opcode):
        pass
    def num_of_completed_data_blocks_callback_handler(self, packet, opcode):
        pass
    def vendor_event_callback_handler(self, packet, opcode):
        pass
    def virtual_connect_complete_callback_handler(self, packet, opcode):
        pass
    def virtual_disconnect_complete_callback_handler(self, packet, opcode):
        pass

    def handle_unknown_command(self, packet,opcode):
        pass

 


