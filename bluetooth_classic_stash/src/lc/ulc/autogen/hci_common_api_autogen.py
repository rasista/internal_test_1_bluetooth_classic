import threading
import os
import sys
import time
from typing import Optional
import numpy as np
from utf.devices import Device
from utf_lib_btdm.interfaces.hci import HCI
BUILD_PATH = os.getenv("BUILD_PATH")
sys.path.append(f"{BUILD_PATH}/../src/lc/ulc/autogen")
from hci_intf_autogen import *

class HciCommonApi(HCI):
    def __init__(self, device: Device, config: Optional[dict], cli_transport: str):
        super().__init__(device, config, cli_transport)

    def setFileName(self, fileName):
        self.fileName = fileName
        with open(self.fileName, "w") as f:
            f.write("")

    def convert_mac_to_bytes(self, mac_address):
        """Convert MAC address string to list of bytes"""
        if isinstance(mac_address, str):
            # Remove colons and convert hex string to bytes, then to list
            mac_hex = mac_address.replace(':', '')
            mac_bytes = bytes.fromhex(mac_hex)
            return list(mac_bytes)
        elif isinstance(mac_address, bytes):
            return list(mac_address)
        elif isinstance(mac_address, list):
            return mac_address
        else:
            raise ValueError(f"Unsupported MAC address format: {type(mac_address)}")

    def swap_bytes_16(self, value):
        """Swap bytes for 16-bit values (little-endian to big-endian or vice versa)"""
        return (((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8))

    def swap_bytes_32(self, value):
        """Swap bytes for 32-bit values (little-endian to big-endian or vice versa)"""
        return (((value & 0xFF000000) >> 24) | 
                ((value & 0x00FF0000) >> 8) | 
                ((value & 0x0000FF00) << 8) | 
                ((value & 0x000000FF) << 24))

    def inquiry(self, lap, inquiry_length, num_responses):
        """
        INQUIRY HCI Command
        Parameters:
        - lap: 3 bytes
        - inquiry_length: 1 bytes
        - num_responses: 1 bytes
        """
        self.logger.info(f'inquiry - {threading.get_ident()}')
        test_data_flow = inquiry_t()
        # Handle parameter assignments
        test_data_flow.lap = lap
        test_data_flow.inquiry_length = inquiry_length
        test_data_flow.num_responses = num_responses
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"inquiry sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def inquiry_cancel(self):
        """
        INQUIRY_CANCEL HCI Command
        """
        self.logger.info(f'inquiry_cancel - {threading.get_ident()}')
        test_data_flow = inquiry_cancel_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"inquiry_cancel sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def periodic_inquiry_mode(self, max_period_length, min_period_length, lap, inquiry_length, num_responses):
        """
        PERIODIC_INQUIRY_MODE HCI Command
        Parameters:
        - max_period_length: 2 bytes
        - min_period_length: 2 bytes
        - lap: 3 bytes
        - inquiry_length: 1 bytes
        - num_responses: 1 bytes
        """
        self.logger.info(f'periodic_inquiry_mode - {threading.get_ident()}')
        test_data_flow = periodic_inquiry_mode_t()
        # Handle parameter assignments
        test_data_flow.max_period_length = self.swap_bytes_16(max_period_length)
        test_data_flow.min_period_length = self.swap_bytes_16(min_period_length)
        test_data_flow.lap = lap
        test_data_flow.inquiry_length = inquiry_length
        test_data_flow.num_responses = num_responses
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"periodic_inquiry_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def exit_periodic_inquiry_mode(self):
        """
        EXIT_PERIODIC_INQUIRY_MODE HCI Command
        """
        self.logger.info(f'exit_periodic_inquiry_mode - {threading.get_ident()}')
        test_data_flow = exit_periodic_inquiry_mode_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"exit_periodic_inquiry_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def create_connection(self, bd_addr, pkt_type, page_scan_rep_mode, reserved, clock_offset, allow_role_switch):
        """
        CREATE_CONNECTION HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - pkt_type: 2 bytes
        - page_scan_rep_mode: 1 bytes
        - reserved: 1 bytes
        - clock_offset: 2 bytes
        - allow_role_switch: 1 bytes
        """
        self.logger.info(f'create_connection - {threading.get_ident()}')
        test_data_flow = create_connection_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.pkt_type = self.swap_bytes_16(pkt_type)
        test_data_flow.page_scan_rep_mode = page_scan_rep_mode
        test_data_flow.reserved = reserved
        test_data_flow.clock_offset = self.swap_bytes_16(clock_offset)
        test_data_flow.allow_role_switch = allow_role_switch
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"create_connection sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def disconnect(self, connection_handle, reason):
        """
        DISCONNECT HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - reason: 1 bytes
        """
        self.logger.info(f'disconnect - {threading.get_ident()}')
        test_data_flow = disconnect_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.reason = reason
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"disconnect sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def create_connection_cancel(self, bd_addr):
        """
        CREATE_CONNECTION_CANCEL HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'create_connection_cancel - {threading.get_ident()}')
        test_data_flow = create_connection_cancel_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"create_connection_cancel sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def accept_connection_request(self, bd_addr, role):
        """
        ACCEPT_CONNECTION_REQUEST HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - role: 1 bytes
        """
        self.logger.info(f'accept_connection_request - {threading.get_ident()}')
        test_data_flow = accept_connection_request_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.role = role
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"accept_connection_request sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def reject_connection_request(self, bd_addr, reason):
        """
        REJECT_CONNECTION_REQUEST HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - reason: 1 bytes
        """
        self.logger.info(f'reject_connection_request - {threading.get_ident()}')
        test_data_flow = reject_connection_request_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.reason = reason
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"reject_connection_request sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def link_key_request_reply(self, bd_addr, link_key):
        """
        LINK_KEY_REQUEST_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - link_key: 16 bytes
        """
        self.logger.info(f'link_key_request_reply - {threading.get_ident()}')
        test_data_flow = link_key_request_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.link_key = link_key
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"link_key_request_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def link_key_request_negative_reply(self, bd_addr):
        """
        LINK_KEY_REQUEST_NEGATIVE_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'link_key_request_negative_reply - {threading.get_ident()}')
        test_data_flow = link_key_request_negative_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"link_key_request_negative_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def pin_code_request_reply(self, bd_addr, pin_code_length, pin_code):
        """
        PIN_CODE_REQUEST_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - pin_code_length: 1 bytes
        - pin_code: 16 bytes
        """
        self.logger.info(f'pin_code_request_reply - {threading.get_ident()}')
        test_data_flow = pin_code_request_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.pin_code_length = pin_code_length
        test_data_flow.pin_code = pin_code
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"pin_code_request_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def pin_code_request_negative_reply(self, bd_addr):
        """
        PIN_CODE_REQUEST_NEGATIVE_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'pin_code_request_negative_reply - {threading.get_ident()}')
        test_data_flow = pin_code_request_negative_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"pin_code_request_negative_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def change_connection_packet_type(self, connection_handle, pkt_type):
        """
        CHANGE_CONNECTION_PACKET_TYPE HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - pkt_type: 2 bytes
        """
        self.logger.info(f'change_connection_packet_type - {threading.get_ident()}')
        test_data_flow = change_connection_packet_type_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.pkt_type = self.swap_bytes_16(pkt_type)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"change_connection_packet_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def authentication_requested(self, connection_handle):
        """
        AUTHENTICATION_REQUESTED HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'authentication_requested - {threading.get_ident()}')
        test_data_flow = authentication_requested_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"authentication_requested sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def set_connection_encryption(self, connection_handle, encryption_enable):
        """
        SET_CONNECTION_ENCRYPTION HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - encryption_enable: 1 bytes
        """
        self.logger.info(f'set_connection_encryption - {threading.get_ident()}')
        test_data_flow = set_connection_encryption_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.encryption_enable = encryption_enable
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"set_connection_encryption sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def change_connection_link_key(self, connection_handle):
        """
        CHANGE_CONNECTION_LINK_KEY HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'change_connection_link_key - {threading.get_ident()}')
        test_data_flow = change_connection_link_key_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"change_connection_link_key sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def remote_name_request(self, bd_addr, page_scan_rep_mode, reserved, clk_offset):
        """
        REMOTE_NAME_REQUEST HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - page_scan_rep_mode: 1 bytes
        - reserved: 1 bytes
        - clk_offset: 2 bytes
        """
        self.logger.info(f'remote_name_request - {threading.get_ident()}')
        test_data_flow = remote_name_request_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.page_scan_rep_mode = page_scan_rep_mode
        test_data_flow.reserved = reserved
        test_data_flow.clk_offset = self.swap_bytes_16(clk_offset)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"remote_name_request sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def remote_name_request_cancel(self, bd_addr):
        """
        REMOTE_NAME_REQUEST_CANCEL HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'remote_name_request_cancel - {threading.get_ident()}')
        test_data_flow = remote_name_request_cancel_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"remote_name_request_cancel sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_remote_supported_features(self, connection_handle):
        """
        READ_REMOTE_SUPPORTED_FEATURES HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_remote_supported_features - {threading.get_ident()}')
        test_data_flow = read_remote_supported_features_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_remote_supported_features sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_remote_extended_features(self, connection_handle, page_number):
        """
        READ_REMOTE_EXTENDED_FEATURES HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - page_number: 1 bytes
        """
        self.logger.info(f'read_remote_extended_features - {threading.get_ident()}')
        test_data_flow = read_remote_extended_features_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.page_number = page_number
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_remote_extended_features sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_remote_version_information(self, connection_handle):
        """
        READ_REMOTE_VERSION_INFORMATION HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_remote_version_information - {threading.get_ident()}')
        test_data_flow = read_remote_version_information_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_remote_version_information sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_clock_offset(self, connection_handle):
        """
        READ_CLOCK_OFFSET HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_clock_offset - {threading.get_ident()}')
        test_data_flow = read_clock_offset_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_clock_offset sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def setup_synchronous_connection(self, connection_handle, transmit_bandwidth, receive_bandwidth, max_latency, voice_setting, retransmission_effort, pkt_type):
        """
        SETUP_SYNCHRONOUS_CONNECTION HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - transmit_bandwidth: 4 bytes
        - receive_bandwidth: 4 bytes
        - max_latency: 2 bytes
        - voice_setting: 2 bytes
        - retransmission_effort: 1 bytes
        - pkt_type: 2 bytes
        """
        self.logger.info(f'setup_synchronous_connection - {threading.get_ident()}')
        test_data_flow = setup_synchronous_connection_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.transmit_bandwidth = transmit_bandwidth
        test_data_flow.receive_bandwidth = receive_bandwidth
        test_data_flow.max_latency = self.swap_bytes_16(max_latency)
        test_data_flow.voice_setting = self.swap_bytes_16(voice_setting)
        test_data_flow.retransmission_effort = retransmission_effort
        test_data_flow.pkt_type = self.swap_bytes_16(pkt_type)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"setup_synchronous_connection sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def accept_synchronous_connection_request(self, bd_addr, transmit_bandwidth, receive_bandwidth, max_latency, voice_setting, retransmission_effort, pkt_type):
        """
        ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - transmit_bandwidth: 4 bytes
        - receive_bandwidth: 4 bytes
        - max_latency: 2 bytes
        - voice_setting: 2 bytes
        - retransmission_effort: 1 bytes
        - pkt_type: 2 bytes
        """
        self.logger.info(f'accept_synchronous_connection_request - {threading.get_ident()}')
        test_data_flow = accept_synchronous_connection_request_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.transmit_bandwidth = transmit_bandwidth
        test_data_flow.receive_bandwidth = receive_bandwidth
        test_data_flow.max_latency = self.swap_bytes_16(max_latency)
        test_data_flow.voice_setting = self.swap_bytes_16(voice_setting)
        test_data_flow.retransmission_effort = retransmission_effort
        test_data_flow.pkt_type = self.swap_bytes_16(pkt_type)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"accept_synchronous_connection_request sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def reject_synchronous_connection_request(self, bd_addr, reason):
        """
        REJECT_SYNCHRONOUS_CONNECTION_REQUEST HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - reason: 1 bytes
        """
        self.logger.info(f'reject_synchronous_connection_request - {threading.get_ident()}')
        test_data_flow = reject_synchronous_connection_request_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.reason = reason
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"reject_synchronous_connection_request sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def io_capability_request_reply(self, bd_addr, io_capability, oob_data_present, authentication_requirements):
        """
        IO_CAPABILITY_REQUEST_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - io_capability: 1 bytes
        - oob_data_present: 1 bytes
        - authentication_requirements: 1 bytes
        """
        self.logger.info(f'io_capability_request_reply - {threading.get_ident()}')
        test_data_flow = io_capability_request_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.io_capability = io_capability
        test_data_flow.oob_data_present = oob_data_present
        test_data_flow.authentication_requirements = authentication_requirements
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"io_capability_request_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def user_confirmation_request_reply(self, bd_addr):
        """
        USER_CONFIRMATION_REQUEST_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'user_confirmation_request_reply - {threading.get_ident()}')
        test_data_flow = user_confirmation_request_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"user_confirmation_request_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def user_confirmation_request_negative_reply(self, bd_addr):
        """
        USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'user_confirmation_request_negative_reply - {threading.get_ident()}')
        test_data_flow = user_confirmation_request_negative_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"user_confirmation_request_negative_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def user_passkey_request_reply(self, bd_addr, numeric_value):
        """
        USER_PASSKEY_REQUEST_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - numeric_value: 4 bytes
        """
        self.logger.info(f'user_passkey_request_reply - {threading.get_ident()}')
        test_data_flow = user_passkey_request_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.numeric_value = self.swap_bytes_32(numeric_value)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"user_passkey_request_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def user_passkey_request_negative_reply(self, bd_addr):
        """
        USER_PASSKEY_REQUEST_NEGATIVE_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'user_passkey_request_negative_reply - {threading.get_ident()}')
        test_data_flow = user_passkey_request_negative_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"user_passkey_request_negative_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def remote_oob_data_request_reply(self, bd_addr, c, r):
        """
        REMOTE_OOB_DATA_REQUEST_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - c: 16 bytes
        - r: 16 bytes
        """
        self.logger.info(f'remote_oob_data_request_reply - {threading.get_ident()}')
        test_data_flow = remote_oob_data_request_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.c = c
        test_data_flow.r = r
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"remote_oob_data_request_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def remote_oob_data_request_negative_reply(self, bd_addr):
        """
        REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        """
        self.logger.info(f'remote_oob_data_request_negative_reply - {threading.get_ident()}')
        test_data_flow = remote_oob_data_request_negative_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"remote_oob_data_request_negative_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def io_capability_request_negative_reply(self, bd_addr, reason):
        """
        IO_CAPABILITY_REQUEST_NEGATIVE_REPLY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - reason: 1 bytes
        """
        self.logger.info(f'io_capability_request_negative_reply - {threading.get_ident()}')
        test_data_flow = io_capability_request_negative_reply_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.reason = reason
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"io_capability_request_negative_reply sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def sniff_mode(self, connection_handle, sniff_max_interval, sniff_min_interval, sniff_attempt, sniff_timeout):
        """
        SNIFF_MODE HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - sniff_max_interval: 2 bytes
        - sniff_min_interval: 2 bytes
        - sniff_attempt: 2 bytes
        - sniff_timeout: 2 bytes
        """
        self.logger.info(f'sniff_mode - {threading.get_ident()}')
        test_data_flow = sniff_mode_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.sniff_max_interval = self.swap_bytes_16(sniff_max_interval)
        test_data_flow.sniff_min_interval = self.swap_bytes_16(sniff_min_interval)
        test_data_flow.sniff_attempt = self.swap_bytes_16(sniff_attempt)
        test_data_flow.sniff_timeout = self.swap_bytes_16(sniff_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"sniff_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def exit_sniff_mode(self, connection_handle):
        """
        EXIT_SNIFF_MODE HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'exit_sniff_mode - {threading.get_ident()}')
        test_data_flow = exit_sniff_mode_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"exit_sniff_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def qos_setup(self, connection_handle, unused, service_type, token_rate, peak_bandwidth, latency, delay_variation):
        """
        QOS_SETUP HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - unused: 1 bytes
        - service_type: 1 bytes
        - token_rate: 4 bytes
        - peak_bandwidth: 4 bytes
        - latency: 4 bytes
        - delay_variation: 4 bytes
        """
        self.logger.info(f'qos_setup - {threading.get_ident()}')
        test_data_flow = qos_setup_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.unused = unused
        test_data_flow.service_type = service_type
        test_data_flow.token_rate = token_rate
        test_data_flow.peak_bandwidth = peak_bandwidth
        test_data_flow.latency = latency
        test_data_flow.delay_variation = delay_variation
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"qos_setup sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def role_discovery(self, connection_handle):
        """
        ROLE_DISCOVERY HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'role_discovery - {threading.get_ident()}')
        test_data_flow = role_discovery_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"role_discovery sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def switch_role(self, bd_addr, role):
        """
        SWITCH_ROLE HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - role: 1 bytes
        """
        self.logger.info(f'switch_role - {threading.get_ident()}')
        test_data_flow = switch_role_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.role = role
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"switch_role sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_link_policy_settings(self, connection_handle):
        """
        READ_LINK_POLICY_SETTINGS HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_link_policy_settings - {threading.get_ident()}')
        test_data_flow = read_link_policy_settings_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_link_policy_settings sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_link_policy_settings(self, connection_handle, link_policy_settings):
        """
        WRITE_LINK_POLICY_SETTINGS HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - link_policy_settings: 2 bytes
        """
        self.logger.info(f'write_link_policy_settings - {threading.get_ident()}')
        test_data_flow = write_link_policy_settings_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.link_policy_settings = self.swap_bytes_16(link_policy_settings)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_link_policy_settings sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_default_link_policy_settings(self):
        """
        READ_DEFAULT_LINK_POLICY_SETTINGS HCI Command
        """
        self.logger.info(f'read_default_link_policy_settings - {threading.get_ident()}')
        test_data_flow = read_default_link_policy_settings_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_default_link_policy_settings sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_default_link_policy_settings(self, default_link_policy_settings):
        """
        WRITE_DEFAULT_LINK_POLICY_SETTINGS HCI Command
        Parameters:
        - default_link_policy_settings: 2 bytes
        """
        self.logger.info(f'write_default_link_policy_settings - {threading.get_ident()}')
        test_data_flow = write_default_link_policy_settings_t()
        # Handle parameter assignments
        test_data_flow.default_link_policy_settings = self.swap_bytes_16(default_link_policy_settings)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_default_link_policy_settings sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def sniff_subrating(self, connection_handle, max_latency, min_remote_timeout, min_local_timeout):
        """
        SNIFF_SUBRATING HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - max_latency: 2 bytes
        - min_remote_timeout: 2 bytes
        - min_local_timeout: 2 bytes
        """
        self.logger.info(f'sniff_subrating - {threading.get_ident()}')
        test_data_flow = sniff_subrating_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.max_latency = self.swap_bytes_16(max_latency)
        test_data_flow.min_remote_timeout = self.swap_bytes_16(min_remote_timeout)
        test_data_flow.min_local_timeout = self.swap_bytes_16(min_local_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"sniff_subrating sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def set_event_mask(self, event_mask):
        """
        SET_EVENT_MASK HCI Command
        Parameters:
        - event_mask: 8 bytes
        """
        self.logger.info(f'set_event_mask - {threading.get_ident()}')
        test_data_flow = set_event_mask_t()
        # Handle parameter assignments
        test_data_flow.event_mask = event_mask
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"set_event_mask sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def reset(self):
        """
        RESET HCI Command
        """
        self.logger.info(f'reset - {threading.get_ident()}')
        test_data_flow = reset_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"reset sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def set_event_filter(self, filter_type, filter_condition_type, condition):
        """
        SET_EVENT_FILTER HCI Command
        Parameters:
        - filter_type: 1 bytes
        - filter_condition_type: 1 bytes
        - condition: 1 bytes
        """
        self.logger.info(f'set_event_filter - {threading.get_ident()}')
        test_data_flow = set_event_filter_t()
        # Handle parameter assignments
        test_data_flow.filter_type = filter_type
        test_data_flow.filter_condition_type = filter_condition_type
        test_data_flow.condition = condition
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"set_event_filter sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_pin_type(self):
        """
        READ_PIN_TYPE HCI Command
        """
        self.logger.info(f'read_pin_type - {threading.get_ident()}')
        test_data_flow = read_pin_type_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_pin_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_pin_type(self, pin_type):
        """
        WRITE_PIN_TYPE HCI Command
        Parameters:
        - pin_type: 1 bytes
        """
        self.logger.info(f'write_pin_type - {threading.get_ident()}')
        test_data_flow = write_pin_type_t()
        # Handle parameter assignments
        test_data_flow.pin_type = pin_type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_pin_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_stored_link_key(self, bd_addr, read_all_flag):
        """
        READ_STORED_LINK_KEY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - read_all_flag: 1 bytes
        """
        self.logger.info(f'read_stored_link_key - {threading.get_ident()}')
        test_data_flow = read_stored_link_key_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.read_all_flag = read_all_flag
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_stored_link_key sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_stored_link_key(self, num_keys_to_write, bd_addr, link_key):
        """
        WRITE_STORED_LINK_KEY HCI Command
        Parameters:
        - num_keys_to_write: 1 bytes
        - bd_addr: 6 bytes
        - link_key: 16 bytes
        """
        self.logger.info(f'write_stored_link_key - {threading.get_ident()}')
        test_data_flow = write_stored_link_key_t()
        # Handle parameter assignments
        test_data_flow.num_keys_to_write = num_keys_to_write
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.link_key = link_key
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_stored_link_key sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def delete_stored_link_key(self, bd_addr, delete_all_flag):
        """
        DELETE_STORED_LINK_KEY HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - delete_all_flag: 1 bytes
        """
        self.logger.info(f'delete_stored_link_key - {threading.get_ident()}')
        test_data_flow = delete_stored_link_key_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.delete_all_flag = delete_all_flag
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"delete_stored_link_key sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_local_name(self, local_name):
        """
        WRITE_LOCAL_NAME HCI Command
        Parameters:
        - local_name: -1 bytes
        """
        self.logger.info(f'write_local_name - {threading.get_ident()}')
        test_data_flow = write_local_name_t()
        # Handle parameter assignments
        test_data_flow.local_name = local_name
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_local_name sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_local_name(self, sampledata):
        """
        READ_LOCAL_NAME HCI Command
        Parameters:
        - sampledata: 1 bytes
        """
        self.logger.info(f'read_local_name - {threading.get_ident()}')
        test_data_flow = read_local_name_t()
        # Handle parameter assignments
        test_data_flow.sampledata = sampledata
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_local_name sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_connection_accept_timeout(self):
        """
        READ_CONNECTION_ACCEPT_TIMEOUT HCI Command
        """
        self.logger.info(f'read_connection_accept_timeout - {threading.get_ident()}')
        test_data_flow = read_connection_accept_timeout_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_connection_accept_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_connection_accept_timeout(self, conn_accept_timeout):
        """
        WRITE_CONNECTION_ACCEPT_TIMEOUT HCI Command
        Parameters:
        - conn_accept_timeout: 2 bytes
        """
        self.logger.info(f'write_connection_accept_timeout - {threading.get_ident()}')
        test_data_flow = write_connection_accept_timeout_t()
        # Handle parameter assignments
        test_data_flow.conn_accept_timeout = self.swap_bytes_16(conn_accept_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_connection_accept_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_page_timeout(self):
        """
        READ_PAGE_TIMEOUT HCI Command
        """
        self.logger.info(f'read_page_timeout - {threading.get_ident()}')
        test_data_flow = read_page_timeout_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_page_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_page_timeout(self, page_timeout):
        """
        WRITE_PAGE_TIMEOUT HCI Command
        Parameters:
        - page_timeout: 2 bytes
        """
        self.logger.info(f'write_page_timeout - {threading.get_ident()}')
        test_data_flow = write_page_timeout_t()
        # Handle parameter assignments
        test_data_flow.page_timeout = self.swap_bytes_16(page_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_page_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_scan_enable(self):
        """
        READ_SCAN_ENABLE HCI Command
        """
        self.logger.info(f'read_scan_enable - {threading.get_ident()}')
        test_data_flow = read_scan_enable_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_scan_enable sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_scan_enable(self, scan_enable):
        """
        WRITE_SCAN_ENABLE HCI Command
        Parameters:
        - scan_enable: 1 bytes
        """
        self.logger.info(f'write_scan_enable - {threading.get_ident()}')
        test_data_flow = write_scan_enable_t()
        # Handle parameter assignments
        test_data_flow.scan_enable = scan_enable
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_scan_enable sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_page_scan_activity(self):
        """
        READ_PAGE_SCAN_ACTIVITY HCI Command
        """
        self.logger.info(f'read_page_scan_activity - {threading.get_ident()}')
        test_data_flow = read_page_scan_activity_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_page_scan_activity sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_page_scan_activity(self, page_scan_interval, page_scan_window):
        """
        WRITE_PAGE_SCAN_ACTIVITY HCI Command
        Parameters:
        - page_scan_interval: 2 bytes
        - page_scan_window: 2 bytes
        """
        self.logger.info(f'write_page_scan_activity - {threading.get_ident()}')
        test_data_flow = write_page_scan_activity_t()
        # Handle parameter assignments
        test_data_flow.page_scan_interval = self.swap_bytes_16(page_scan_interval)
        test_data_flow.page_scan_window = self.swap_bytes_16(page_scan_window)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_page_scan_activity sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_inquiry_scan_activity(self):
        """
        READ_INQUIRY_SCAN_ACTIVITY HCI Command
        """
        self.logger.info(f'read_inquiry_scan_activity - {threading.get_ident()}')
        test_data_flow = read_inquiry_scan_activity_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_inquiry_scan_activity sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_inquiry_scan_activity(self, inquiry_scan_interval, inquiry_scan_window):
        """
        WRITE_INQUIRY_SCAN_ACTIVITY HCI Command
        Parameters:
        - inquiry_scan_interval: 2 bytes
        - inquiry_scan_window: 2 bytes
        """
        self.logger.info(f'write_inquiry_scan_activity - {threading.get_ident()}')
        test_data_flow = write_inquiry_scan_activity_t()
        # Handle parameter assignments
        test_data_flow.inquiry_scan_interval = self.swap_bytes_16(inquiry_scan_interval)
        test_data_flow.inquiry_scan_window = self.swap_bytes_16(inquiry_scan_window)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_inquiry_scan_activity sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_authentication_enable(self):
        """
        READ_AUTHENTICATION_ENABLE HCI Command
        """
        self.logger.info(f'read_authentication_enable - {threading.get_ident()}')
        test_data_flow = read_authentication_enable_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_authentication_enable sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_authentication_enable(self, authentication_enable):
        """
        WRITE_AUTHENTICATION_ENABLE HCI Command
        Parameters:
        - authentication_enable: 1 bytes
        """
        self.logger.info(f'write_authentication_enable - {threading.get_ident()}')
        test_data_flow = write_authentication_enable_t()
        # Handle parameter assignments
        test_data_flow.authentication_enable = authentication_enable
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_authentication_enable sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_class_of_device(self):
        """
        READ_CLASS_OF_DEVICE HCI Command
        """
        self.logger.info(f'read_class_of_device - {threading.get_ident()}')
        test_data_flow = read_class_of_device_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_class_of_device sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_class_of_device(self, class_of_device):
        """
        WRITE_CLASS_OF_DEVICE HCI Command
        Parameters:
        - class_of_device: 3 bytes
        """
        self.logger.info(f'write_class_of_device - {threading.get_ident()}')
        test_data_flow = write_class_of_device_t()
        # Handle parameter assignments
        test_data_flow.class_of_device = class_of_device
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_class_of_device sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_voice_setting(self):
        """
        READ_VOICE_SETTING HCI Command
        """
        self.logger.info(f'read_voice_setting - {threading.get_ident()}')
        test_data_flow = read_voice_setting_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_voice_setting sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_voice_setting(self, voice_setting):
        """
        WRITE_VOICE_SETTING HCI Command
        Parameters:
        - voice_setting: 2 bytes
        """
        self.logger.info(f'write_voice_setting - {threading.get_ident()}')
        test_data_flow = write_voice_setting_t()
        # Handle parameter assignments
        test_data_flow.voice_setting = self.swap_bytes_16(voice_setting)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_voice_setting sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_automatic_flush_timeout(self, connection_handle):
        """
        READ_AUTOMATIC_FLUSH_TIMEOUT HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_automatic_flush_timeout - {threading.get_ident()}')
        test_data_flow = read_automatic_flush_timeout_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_automatic_flush_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_automatic_flush_timeout(self, connection_handle, flush_timeout):
        """
        WRITE_AUTOMATIC_FLUSH_TIMEOUT HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - flush_timeout: 2 bytes
        """
        self.logger.info(f'write_automatic_flush_timeout - {threading.get_ident()}')
        test_data_flow = write_automatic_flush_timeout_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.flush_timeout = self.swap_bytes_16(flush_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_automatic_flush_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_transmit_power_level(self, connection_handle, type):
        """
        READ_TRANSMIT_POWER_LEVEL HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - type: 1 bytes
        """
        self.logger.info(f'read_transmit_power_level - {threading.get_ident()}')
        test_data_flow = read_transmit_power_level_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.type = type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_transmit_power_level sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_link_supervision_timeout(self, connection_handle):
        """
        READ_LINK_SUPERVISION_TIMEOUT HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_link_supervision_timeout - {threading.get_ident()}')
        test_data_flow = read_link_supervision_timeout_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_link_supervision_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_link_supervision_timeout(self, connection_handle, link_supervision_timeout):
        """
        WRITE_LINK_SUPERVISION_TIMEOUT HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - link_supervision_timeout: 2 bytes
        """
        self.logger.info(f'write_link_supervision_timeout - {threading.get_ident()}')
        test_data_flow = write_link_supervision_timeout_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.link_supervision_timeout = self.swap_bytes_16(link_supervision_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_link_supervision_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_number_of_supported_iac(self):
        """
        READ_NUMBER_OF_SUPPORTED_IAC HCI Command
        """
        self.logger.info(f'read_number_of_supported_iac - {threading.get_ident()}')
        test_data_flow = read_number_of_supported_iac_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_number_of_supported_iac sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_current_iac_lap(self):
        """
        READ_CURRENT_IAC_LAP HCI Command
        """
        self.logger.info(f'read_current_iac_lap - {threading.get_ident()}')
        test_data_flow = read_current_iac_lap_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_current_iac_lap sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_current_iac_lap(self, num_current_iac, lap):
        """
        WRITE_CURRENT_IAC_LAP HCI Command
        Parameters:
        - num_current_iac: 1 bytes
        - lap: 3 bytes
        """
        self.logger.info(f'write_current_iac_lap - {threading.get_ident()}')
        test_data_flow = write_current_iac_lap_t()
        # Handle parameter assignments
        test_data_flow.num_current_iac = num_current_iac
        test_data_flow.lap = lap
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_current_iac_lap sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def set_afh_host_channel_classification(self, channel_map):
        """
        SET_AFH_HOST_CHANNEL_CLASSIFICATION HCI Command
        Parameters:
        - channel_map: 10 bytes
        """
        self.logger.info(f'set_afh_host_channel_classification - {threading.get_ident()}')
        test_data_flow = set_afh_host_channel_classification_t()
        # Handle parameter assignments
        test_data_flow.channel_map = channel_map
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"set_afh_host_channel_classification sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_inquiry_scan_type(self):
        """
        READ_INQUIRY_SCAN_TYPE HCI Command
        """
        self.logger.info(f'read_inquiry_scan_type - {threading.get_ident()}')
        test_data_flow = read_inquiry_scan_type_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_inquiry_scan_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_inquiry_scan_type(self, inquiry_scan_type):
        """
        WRITE_INQUIRY_SCAN_TYPE HCI Command
        Parameters:
        - inquiry_scan_type: 1 bytes
        """
        self.logger.info(f'write_inquiry_scan_type - {threading.get_ident()}')
        test_data_flow = write_inquiry_scan_type_t()
        # Handle parameter assignments
        test_data_flow.inquiry_scan_type = inquiry_scan_type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_inquiry_scan_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_inquiry_mode(self):
        """
        READ_INQUIRY_MODE HCI Command
        """
        self.logger.info(f'read_inquiry_mode - {threading.get_ident()}')
        test_data_flow = read_inquiry_mode_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_inquiry_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_inquiry_mode(self, inquiry_mode):
        """
        WRITE_INQUIRY_MODE HCI Command
        Parameters:
        - inquiry_mode: 1 bytes
        """
        self.logger.info(f'write_inquiry_mode - {threading.get_ident()}')
        test_data_flow = write_inquiry_mode_t()
        # Handle parameter assignments
        test_data_flow.inquiry_mode = inquiry_mode
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_inquiry_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_page_scan_type(self):
        """
        READ_PAGE_SCAN_TYPE HCI Command
        """
        self.logger.info(f'read_page_scan_type - {threading.get_ident()}')
        test_data_flow = read_page_scan_type_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_page_scan_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_page_scan_type(self, page_scan_type):
        """
        WRITE_PAGE_SCAN_TYPE HCI Command
        Parameters:
        - page_scan_type: 1 bytes
        """
        self.logger.info(f'write_page_scan_type - {threading.get_ident()}')
        test_data_flow = write_page_scan_type_t()
        # Handle parameter assignments
        test_data_flow.page_scan_type = page_scan_type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_page_scan_type sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_afh_channel_assessment_mode(self):
        """
        READ_AFH_CHANNEL_ASSESSMENT_MODE HCI Command
        """
        self.logger.info(f'read_afh_channel_assessment_mode - {threading.get_ident()}')
        test_data_flow = read_afh_channel_assessment_mode_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_afh_channel_assessment_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_afh_channel_assessment_mode(self, afh_channel_assessment_mode):
        """
        WRITE_AFH_CHANNEL_ASSESSMENT_MODE HCI Command
        Parameters:
        - afh_channel_assessment_mode: 1 bytes
        """
        self.logger.info(f'write_afh_channel_assessment_mode - {threading.get_ident()}')
        test_data_flow = write_afh_channel_assessment_mode_t()
        # Handle parameter assignments
        test_data_flow.afh_channel_assessment_mode = afh_channel_assessment_mode
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_afh_channel_assessment_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_extended_inquiry_response(self):
        """
        READ_EXTENDED_INQUIRY_RESPONSE HCI Command
        """
        self.logger.info(f'read_extended_inquiry_response - {threading.get_ident()}')
        test_data_flow = read_extended_inquiry_response_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_extended_inquiry_response sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_extended_inquiry_response(self, fec_required, extended_inquiry_response):
        """
        WRITE_EXTENDED_INQUIRY_RESPONSE HCI Command
        Parameters:
        - fec_required: 1 bytes
        - extended_inquiry_response: 240 bytes
        """
        self.logger.info(f'write_extended_inquiry_response - {threading.get_ident()}')
        test_data_flow = write_extended_inquiry_response_t()
        # Handle parameter assignments
        test_data_flow.fec_required = fec_required
        test_data_flow.extended_inquiry_response = extended_inquiry_response
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_extended_inquiry_response sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def refresh_encryption_key(self, connection_handle):
        """
        REFRESH_ENCRYPTION_KEY HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'refresh_encryption_key - {threading.get_ident()}')
        test_data_flow = refresh_encryption_key_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"refresh_encryption_key sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_simple_pairing_mode(self):
        """
        READ_SIMPLE_PAIRING_MODE HCI Command
        """
        self.logger.info(f'read_simple_pairing_mode - {threading.get_ident()}')
        test_data_flow = read_simple_pairing_mode_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_simple_pairing_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_simple_pairing_mode(self, simple_pairing_mode):
        """
        WRITE_SIMPLE_PAIRING_MODE HCI Command
        Parameters:
        - simple_pairing_mode: 1 bytes
        """
        self.logger.info(f'write_simple_pairing_mode - {threading.get_ident()}')
        test_data_flow = write_simple_pairing_mode_t()
        # Handle parameter assignments
        test_data_flow.simple_pairing_mode = simple_pairing_mode
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_simple_pairing_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_local_oob_data(self):
        """
        READ_LOCAL_OOB_DATA HCI Command
        """
        self.logger.info(f'read_local_oob_data - {threading.get_ident()}')
        test_data_flow = read_local_oob_data_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_local_oob_data sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_inquiry_response_transmit_power_level(self):
        """
        READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL HCI Command
        """
        self.logger.info(f'read_inquiry_response_transmit_power_level - {threading.get_ident()}')
        test_data_flow = read_inquiry_response_transmit_power_level_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_inquiry_response_transmit_power_level sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_inquiry_transmit_power_level(self, tx_power_level):
        """
        WRITE_INQUIRY_TRANSMIT_POWER_LEVEL HCI Command
        Parameters:
        - tx_power_level: 1 bytes
        """
        self.logger.info(f'write_inquiry_transmit_power_level - {threading.get_ident()}')
        test_data_flow = write_inquiry_transmit_power_level_t()
        # Handle parameter assignments
        test_data_flow.tx_power_level = tx_power_level
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_inquiry_transmit_power_level sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def send_keypress_notification(self, bd_addr, notification_type):
        """
        SEND_KEYPRESS_NOTIFICATION HCI Command
        Parameters:
        - bd_addr: 6 bytes
        - notification_type: 1 bytes
        """
        self.logger.info(f'send_keypress_notification - {threading.get_ident()}')
        test_data_flow = send_keypress_notification_t()
        # Handle parameter assignments
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.notification_type = notification_type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"send_keypress_notification sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def enhanced_flush(self, handle, packet_type):
        """
        ENHANCED_FLUSH HCI Command
        Parameters:
        - handle: 2 bytes
        - packet_type: 1 bytes
        """
        self.logger.info(f'enhanced_flush - {threading.get_ident()}')
        test_data_flow = enhanced_flush_t()
        # Handle parameter assignments
        test_data_flow.handle = self.swap_bytes_16(handle)
        test_data_flow.packet_type = packet_type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"enhanced_flush sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_enhanced_transmit_power_level(self, connection_handle, type):
        """
        READ_ENHANCED_TRANSMIT_POWER_LEVEL HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - type: 1 bytes
        """
        self.logger.info(f'read_enhanced_transmit_power_level - {threading.get_ident()}')
        test_data_flow = read_enhanced_transmit_power_level_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.type = type
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_enhanced_transmit_power_level sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_secure_connections_host_support(self):
        """
        READ_SECURE_CONNECTIONS_HOST_SUPPORT HCI Command
        """
        self.logger.info(f'read_secure_connections_host_support - {threading.get_ident()}')
        test_data_flow = read_secure_connections_host_support_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_secure_connections_host_support sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_secure_connections_host_support(self, secure_connections_host_support):
        """
        WRITE_SECURE_CONNECTIONS_HOST_SUPPORT HCI Command
        Parameters:
        - secure_connections_host_support: 1 bytes
        """
        self.logger.info(f'write_secure_connections_host_support - {threading.get_ident()}')
        test_data_flow = write_secure_connections_host_support_t()
        # Handle parameter assignments
        test_data_flow.secure_connections_host_support = secure_connections_host_support
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_secure_connections_host_support sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_extended_page_timeout(self):
        """
        READ_EXTENDED_PAGE_TIMEOUT HCI Command
        """
        self.logger.info(f'read_extended_page_timeout - {threading.get_ident()}')
        test_data_flow = read_extended_page_timeout_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_extended_page_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_extended_page_timeout(self, extended_page_timeout):
        """
        WRITE_EXTENDED_PAGE_TIMEOUT HCI Command
        Parameters:
        - extended_page_timeout: 2 bytes
        """
        self.logger.info(f'write_extended_page_timeout - {threading.get_ident()}')
        test_data_flow = write_extended_page_timeout_t()
        # Handle parameter assignments
        test_data_flow.extended_page_timeout = self.swap_bytes_16(extended_page_timeout)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_extended_page_timeout sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_extended_inquiry_length(self):
        """
        READ_EXTENDED_INQUIRY_LENGTH HCI Command
        """
        self.logger.info(f'read_extended_inquiry_length - {threading.get_ident()}')
        test_data_flow = read_extended_inquiry_length_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_extended_inquiry_length sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_extended_inquiry_length(self, extended_inquiry_length):
        """
        WRITE_EXTENDED_INQUIRY_LENGTH HCI Command
        Parameters:
        - extended_inquiry_length: 2 bytes
        """
        self.logger.info(f'write_extended_inquiry_length - {threading.get_ident()}')
        test_data_flow = write_extended_inquiry_length_t()
        # Handle parameter assignments
        test_data_flow.extended_inquiry_length = self.swap_bytes_16(extended_inquiry_length)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_extended_inquiry_length sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_local_version_information(self, sampledata):
        """
        READ_LOCAL_VERSION_INFORMATION HCI Command
        Parameters:
        - sampledata: 1 bytes
        """
        self.logger.info(f'read_local_version_information - {threading.get_ident()}')
        test_data_flow = read_local_version_information_t()
        # Handle parameter assignments
        test_data_flow.sampledata = sampledata
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_local_version_information sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_local_supported_commands(self):
        """
        READ_LOCAL_SUPPORTED_COMMANDS HCI Command
        """
        self.logger.info(f'read_local_supported_commands - {threading.get_ident()}')
        test_data_flow = read_local_supported_commands_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_local_supported_commands sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_local_supported_features(self):
        """
        READ_LOCAL_SUPPORTED_FEATURES HCI Command
        """
        self.logger.info(f'read_local_supported_features - {threading.get_ident()}')
        test_data_flow = read_local_supported_features_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_local_supported_features sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_local_extended_features(self, page_number):
        """
        READ_LOCAL_EXTENDED_FEATURES HCI Command
        Parameters:
        - page_number: 1 bytes
        """
        self.logger.info(f'read_local_extended_features - {threading.get_ident()}')
        test_data_flow = read_local_extended_features_t()
        # Handle parameter assignments
        test_data_flow.page_number = page_number
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_local_extended_features sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_buffer_size(self):
        """
        READ_BUFFER_SIZE HCI Command
        """
        self.logger.info(f'read_buffer_size - {threading.get_ident()}')
        test_data_flow = read_buffer_size_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_buffer_size sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_bd_addr(self):
        """
        READ_BD_ADDR HCI Command
        """
        self.logger.info(f'read_bd_addr - {threading.get_ident()}')
        test_data_flow = read_bd_addr_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_bd_addr sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_link_quality(self, handle):
        """
        READ_LINK_QUALITY HCI Command
        Parameters:
        - handle: 2 bytes
        """
        self.logger.info(f'read_link_quality - {threading.get_ident()}')
        test_data_flow = read_link_quality_t()
        # Handle parameter assignments
        test_data_flow.handle = self.swap_bytes_16(handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_link_quality sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_rssi(self, handle):
        """
        READ_RSSI HCI Command
        Parameters:
        - handle: 2 bytes
        """
        self.logger.info(f'read_rssi - {threading.get_ident()}')
        test_data_flow = read_rssi_t()
        # Handle parameter assignments
        test_data_flow.handle = self.swap_bytes_16(handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_rssi sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_afh_channel_map(self, connection_handle):
        """
        READ_AFH_CHANNEL_MAP HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_afh_channel_map - {threading.get_ident()}')
        test_data_flow = read_afh_channel_map_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_afh_channel_map sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_clock(self, connection_handle, which_clock):
        """
        READ_CLOCK HCI Command
        Parameters:
        - connection_handle: 2 bytes
        - which_clock: 1 bytes
        """
        self.logger.info(f'read_clock - {threading.get_ident()}')
        test_data_flow = read_clock_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        test_data_flow.which_clock = which_clock
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_clock sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def read_encryption_key_size(self, connection_handle):
        """
        READ_ENCRYPTION_KEY_SIZE HCI Command
        Parameters:
        - connection_handle: 2 bytes
        """
        self.logger.info(f'read_encryption_key_size - {threading.get_ident()}')
        test_data_flow = read_encryption_key_size_t()
        # Handle parameter assignments
        test_data_flow.connection_handle = self.swap_bytes_16(connection_handle)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"read_encryption_key_size sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def enable_device_under_test_mode(self):
        """
        ENABLE_DEVICE_UNDER_TEST_MODE HCI Command
        """
        self.logger.info(f'enable_device_under_test_mode - {threading.get_ident()}')
        test_data_flow = enable_device_under_test_mode_t()
        # Handle parameter assignments
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"enable_device_under_test_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def write_simple_pairing_debug_mode(self, simple_pairing_debug_mode):
        """
        WRITE_SIMPLE_PAIRING_DEBUG_MODE HCI Command
        Parameters:
        - simple_pairing_debug_mode: 1 bytes
        """
        self.logger.info(f'write_simple_pairing_debug_mode - {threading.get_ident()}')
        test_data_flow = write_simple_pairing_debug_mode_t()
        # Handle parameter assignments
        test_data_flow.simple_pairing_debug_mode = simple_pairing_debug_mode
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"write_simple_pairing_debug_mode sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def echo(self, echo_str):
        """
        ECHO HCI Command
        Parameters:
        - echo_str: 1 bytes
        """
        self.logger.info(f'echo - {threading.get_ident()}')
        test_data_flow = echo_t()
        # Handle parameter assignments
        test_data_flow.echo_str = echo_str
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"echo sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def virtual_listen_connection(self, connect_port):
        """
        VIRTUAL_LISTEN_CONNECTION HCI Command
        Parameters:
        - connect_port: 2 bytes
        """
        self.logger.info(f'virtual_listen_connection - {threading.get_ident()}')
        test_data_flow = virtual_listen_connection_t()
        # Handle parameter assignments
        test_data_flow.connect_port = self.swap_bytes_16(connect_port)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"virtual_listen_connection sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def virtual_cancel_listen(self, empty_byte):
        """
        VIRTUAL_CANCEL_LISTEN HCI Command
        Parameters:
        - empty_byte: 1 bytes
        """
        self.logger.info(f'virtual_cancel_listen - {threading.get_ident()}')
        test_data_flow = virtual_cancel_listen_t()
        # Handle parameter assignments
        test_data_flow.empty_byte = empty_byte
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"virtual_cancel_listen sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def virtual_connect(self, connect_ip, connect_port):
        """
        VIRTUAL_CONNECT HCI Command
        Parameters:
        - connect_ip: 4 bytes
        - connect_port: 2 bytes
        """
        self.logger.info(f'virtual_connect - {threading.get_ident()}')
        test_data_flow = virtual_connect_t()
        # Handle parameter assignments
        test_data_flow.connect_ip = connect_ip
        test_data_flow.connect_port = self.swap_bytes_16(connect_port)
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"virtual_connect sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def virtual_cancel_connect(self, empty_byte):
        """
        VIRTUAL_CANCEL_CONNECT HCI Command
        Parameters:
        - empty_byte: 1 bytes
        """
        self.logger.info(f'virtual_cancel_connect - {threading.get_ident()}')
        test_data_flow = virtual_cancel_connect_t()
        # Handle parameter assignments
        test_data_flow.empty_byte = empty_byte
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"virtual_cancel_connect sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def internal_event_trigger_from_lpw(self, meta_data, lpw_evnt_type, bd_addr, data):
        """
        INTERNAL_EVENT_TRIGGER_FROM_LPW HCI Command
        Parameters:
        - meta_data: 4 bytes
        - lpw_evnt_type: 1 bytes
        - bd_addr: 6 bytes
        - data: -1 bytes
        """
        self.logger.info(f'internal_event_trigger_from_lpw - {threading.get_ident()}')
        test_data_flow = internal_event_trigger_from_lpw_t()
        # Handle parameter assignments
        test_data_flow.meta_data = meta_data
        test_data_flow.lpw_evnt_type = lpw_evnt_type
        test_data_flow.bd_addr = self.convert_mac_to_bytes(bd_addr)
        test_data_flow.data = data
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"internal_event_trigger_from_lpw sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def kill_firmware(self, empty_byte):
        """
        KILL_FIRMWARE HCI Command
        Parameters:
        - empty_byte: 1 bytes
        """
        self.logger.info(f'kill_firmware - {threading.get_ident()}')
        test_data_flow = kill_firmware_t()
        # Handle parameter assignments
        test_data_flow.empty_byte = empty_byte
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"kill_firmware sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

    def set_local_lmp_features(self, local_features):
        """
        SET_LOCAL_LMP_FEATURES HCI Command
        Parameters:
        - local_features: 8 bytes
        """
        self.logger.info(f'set_local_lmp_features - {threading.get_ident()}')
        test_data_flow = set_local_lmp_features_t()
        # Handle parameter assignments
        test_data_flow.local_features = local_features
        hci_cmd = test_data_flow.prep_hci()
        self.logger.info(f"set_local_lmp_features sending {hci_cmd} in thread - {threading.get_ident()}")
        self.issue_command(hci_cmd)

