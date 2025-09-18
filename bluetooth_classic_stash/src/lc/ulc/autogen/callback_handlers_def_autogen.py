import threading
import logging
import os
import sys 
from typing import Optional
from utf.devices import Device
from utf_lib_btdm.interfaces.hci import HCI
BUILD_PATH = os.getenv("BUILD_PATH")
sys.path.append(f"{BUILD_PATH}/../src/lc/ulc/autogen")
sys.path.append(f"{BUILD_PATH}/../src/lc/ulc/ut/linapps/btc_virtual_controller")
from hci_event_callback_autogen import *
from hci_intf_autogen import *
from btc_error_codes import *
READ_BD_ADDR_OPCODE = 4105

class call_back_handler_def(callback_handlers):
    def set_semaphore(self, hci_lock=None, event_lock=None):
        self.hci_lock = hci_lock
        self.event_lock = event_lock
   
    def setEventVars_handler(self, hciEventVars):
        self.hciEventVars = hciEventVars

    def inquiry_complete_callback_handler(self, packet, opcode):
        """
        INQUIRY_COMPLETE Event Callback Handler
        Event Code: 1
        Parameters:
        - status: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'inquiry_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.inquiry_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def inquiry_result_callback_handler(self, packet, opcode):
        """
        INQUIRY_RESULT Event Callback Handler
        Event Code: 2
        Parameters:
        - num_resp: 1 bytes
        - bd_addr: 6 bytes
        - page_scan_rep_mode: 1 bytes
        - reserved: 2 bytes
        - class_of_device: 3 bytes
        - clock_offset: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'inquiry_result_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.inquiry_result = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def conn_complete_callback_handler(self, packet, opcode):
        """
        CONN_COMPLETE Event Callback Handler
        Event Code: 3
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - bd_addr: 6 bytes
        - link_type: 1 bytes
        - encryption_enable: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'conn_complete_callback_handler - {threading.get_ident()}')
        
        # Special handling for conn_complete event
        status = packet[3]
        if status == 0:
            conn_handle = packet[4] | (packet[5] << 8)
            conn_type = packet[12]
            bd_addr_bytes = packet[6:12]  # BD Address is 6 bytes (indices 6-11)
            received_bd_addr  = list(bd_addr_bytes )
            self.hciEventVars.conn_complete = received_bd_addr
            logger.info(f'conn_complete_callback_handler conn_handle = {conn_handle}')
            logger.info(f'conn_complete_callback_handler conn_type = {conn_type}')
            logger.info(f'conn_complete_callback_handler received_bd_addr = {received_bd_addr}')
            self.hciEventVars.acl_connection_info.conn_handle = conn_handle
            self.hciEventVars.acl_connection_info.conn_type = conn_type
            self.hciEventVars.acl_connection_info.bd_addr = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def conn_request_callback_handler(self, packet, opcode):
        """
        CONN_REQUEST Event Callback Handler
        Event Code: 4
        Parameters:
        - bd_addr: 6 bytes
        - class_of_device: 3 bytes
        - link_type: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'conn_request_callback_handler - {threading.get_ident()}')
        
        self.hciEventVars.conn_request = 1
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def disconnection_complete_callback_handler(self, packet, opcode):
        """
        DISCONNECTION_COMPLETE Event Callback Handler
        Event Code: 5
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - reason: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'disconnection_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.disconnection_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def authentication_complete_callback_handler(self, packet, opcode):
        """
        AUTHENTICATION_COMPLETE Event Callback Handler
        Event Code: 6
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'authentication_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.authentication_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def remote_name_request_complete_callback_handler(self, packet, opcode):
        """
        REMOTE_NAME_REQUEST_COMPLETE Event Callback Handler
        Event Code: 7
        Parameters:
        - status: 1 bytes
        - bd_addr: 6 bytes
        - remote_name: 248 bytes
        """
        logger = logging.getLogger()
        logger.info(f'remote_name_request_complete_callback_handler - {threading.get_ident()}')
        
        # Special handling for remote_name_request_complete event
        bd_addr_bytes = packet[4:10]  # BD Address is 6 bytes (indices 4-9)
        # Format BD address as a colon-separated hex string (common format)
        bd_addr_hex = ':'.join([f'{byte:02X}' for byte in bd_addr_bytes])
        received_bd_addr = bytes.fromhex(bd_addr_hex.replace(':', ''))
        received_bd_addr = list(received_bd_addr)

        connected_bd_addr = self.hciEventVars.acl_connection_info.bd_addr
        name_bytes = packet[10:]
        logger.info(f'remote_name_request_complete_callback_handler connected_bd_addr {connected_bd_addr}')
        logger.info(f'remote_name_request_complete_callback_handler Received bd_addr {received_bd_addr}')
        if packet[3] == 0 and received_bd_addr == connected_bd_addr:
            self.hciEventVars.remote_name_request_complete = name_bytes.decode('utf-8', errors='ignore').replace('\x00', '').strip()
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def encrytpion_change_v1_callback_handler(self, packet, opcode):
        """
        ENCRYTPION_CHANGE_V1 Event Callback Handler
        Event Code: 8
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - encryption_enable: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'encrytpion_change_v1_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.encrytpion_change_v1 = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def encrytpion_change_v2_callback_handler(self, packet, opcode):
        """
        ENCRYTPION_CHANGE_V2 Event Callback Handler
        Event Code: 89
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - encryption_enable: 1 bytes
        - encryption_key_size: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'encrytpion_change_v2_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.encrytpion_change_v2 = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def change_connection_link_key_complete_callback_handler(self, packet, opcode):
        """
        CHANGE_CONNECTION_LINK_KEY_COMPLETE Event Callback Handler
        Event Code: 9
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'change_connection_link_key_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.change_connection_link_key_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def link_key_type_changed_callback_handler(self, packet, opcode):
        """
        LINK_KEY_TYPE_CHANGED Event Callback Handler
        Event Code: 10
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - key_flag: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'link_key_type_changed_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.link_key_type_changed = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def read_remote_supported_features_complete_callback_handler(self, packet, opcode):
        """
        READ_REMOTE_SUPPORTED_FEATURES_COMPLETE Event Callback Handler
        Event Code: 11
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - lmp_features: 8 bytes
        """
        logger = logging.getLogger()
        logger.info(f'read_remote_supported_features_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.read_remote_supported_features_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def read_remote_version_info_complete_callback_handler(self, packet, opcode):
        """
        READ_REMOTE_VERSION_INFO_COMPLETE Event Callback Handler
        Event Code: 12
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - version: 1 bytes
        - company_identifier: 2 bytes
        - sub_version: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'read_remote_version_info_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.read_remote_version_info_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def qos_setup_complete_callback_handler(self, packet, opcode):
        """
        QOS_SETUP_COMPLETE Event Callback Handler
        Event Code: 13
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - unused: 1 bytes
        - service_type: 1 bytes
        - token_rate: 4 bytes
        - peak_bandwidth: 4 bytes
        - latency: 4 bytes
        - delay_variation: 4 bytes
        """
        logger = logging.getLogger()
        logger.info(f'qos_setup_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.qos_setup_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def cmd_complete_callback_handler(self, packet, opcode):
        """
        CMD_COMPLETE Event Callback Handler
        Event Code: 14
        Parameters:
        - num_hci_cmd_pkts: 1 bytes
        - cmd_opcode: 2 bytes
        - return_params: None bytes
        """
        logger = logging.getLogger()
        logger.info(f'cmd_complete_callback_handler - {threading.get_ident()}')
        
        # Special handling for cmd_complete event
        len = packet[2]
        if len > 3:
            self.hciEventVars.cmd_complete = packet[6]
            rx_opcode = ((packet[5] << 8) | packet[4])
            if (rx_opcode == READ_BD_ADDR_OPCODE):
                bd_addr_bytes = packet[7:13]  # BD Address is 6 bytes (indices 7 -12)
                bd_addr = list(bd_addr_bytes) 
                self.hciEventVars.device_bd_addr = bd_addr
                logger.info(f"Recived Device BD Address: {bd_addr}")
        else:
            self.hciEventVars.cmd_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def cmd_status_callback_handler(self, packet, opcode):
        """
        CMD_STATUS Event Callback Handler
        Event Code: 15
        Parameters:
        - status: 1 bytes
        - num_hci_cmd_pkts: 1 bytes
        - cmd_opcode: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'cmd_status_callback_handler - {threading.get_ident()}')
        
        # Special handling for cmd_status event
        self.hciEventVars.cmd_status = packet[3]
        logger.info(f"received cmd status  = {packet[3]}")
        logger.info(f"cmd status  = {self.hciEventVars.cmd_status}")
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def hardware_error_callback_handler(self, packet, opcode):
        """
        HARDWARE_ERROR Event Callback Handler
        Event Code: 16
        Parameters:
        - hw_code: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'hardware_error_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.hardware_error = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def flush_occurred_callback_handler(self, packet, opcode):
        """
        FLUSH_OCCURRED Event Callback Handler
        Event Code: 17
        Parameters:
        - handle: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'flush_occurred_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.flush_occurred = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def role_change_callback_handler(self, packet, opcode):
        """
        ROLE_CHANGE Event Callback Handler
        Event Code: 18
        Parameters:
        - status: 1 bytes
        - bd_addr: 6 bytes
        - new_role: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'role_change_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.role_change = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def num_of_completed_pkts_callback_handler(self, packet, opcode):
        """
        NUM_OF_COMPLETED_PKTS Event Callback Handler
        Event Code: 19
        Parameters:
        - num_handles: 1 bytes
        - conn_handle: 2 bytes
        - num_completed_pkts: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'num_of_completed_pkts_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.num_of_completed_pkts = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def mode_change_callback_handler(self, packet, opcode):
        """
        MODE_CHANGE Event Callback Handler
        Event Code: 20
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - current_mode: 1 bytes
        - interval: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'mode_change_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.mode_change = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def return_link_keys_callback_handler(self, packet, opcode):
        """
        RETURN_LINK_KEYS Event Callback Handler
        Event Code: 21
        Parameters:
        - num_keys: 1 bytes
        - bd_addr: 6 bytes
        - link_key: 16 bytes
        """
        logger = logging.getLogger()
        logger.info(f'return_link_keys_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.return_link_keys = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def pin_code_request_callback_handler(self, packet, opcode):
        """
        PIN_CODE_REQUEST Event Callback Handler
        Event Code: 22
        Parameters:
        - bd_addr: 6 bytes
        """
        logger = logging.getLogger()
        logger.info(f'pin_code_request_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.pin_code_request = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def link_key_request_callback_handler(self, packet, opcode):
        """
        LINK_KEY_REQUEST Event Callback Handler
        Event Code: 23
        Parameters:
        - bd_addr: 6 bytes
        """
        logger = logging.getLogger()
        logger.info(f'link_key_request_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.link_key_request = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def link_key_notification_callback_handler(self, packet, opcode):
        """
        LINK_KEY_NOTIFICATION Event Callback Handler
        Event Code: 24
        Parameters:
        - bd_addr: 6 bytes
        - link_key: 16 bytes
        - key_type: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'link_key_notification_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.link_key_notification = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def loopback_cmd_callback_handler(self, packet, opcode):
        """
        LOOPBACK_CMD Event Callback Handler
        Event Code: 25
        Parameters:
        - hci_cmd_pkts: None bytes
        """
        logger = logging.getLogger()
        logger.info(f'loopback_cmd_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.loopback_cmd = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def data_buffer_overflow_callback_handler(self, packet, opcode):
        """
        DATA_BUFFER_OVERFLOW Event Callback Handler
        Event Code: 26
        Parameters:
        - link_type: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'data_buffer_overflow_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.data_buffer_overflow = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def max_slots_change_callback_handler(self, packet, opcode):
        """
        MAX_SLOTS_CHANGE Event Callback Handler
        Event Code: 27
        Parameters:
        - conn_handle: 2 bytes
        - lmpmax_slots: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'max_slots_change_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.max_slots_change = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def read_clock_offset_complete_callback_handler(self, packet, opcode):
        """
        READ_CLOCK_OFFSET_COMPLETE Event Callback Handler
        Event Code: 28
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - clock_offset: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'read_clock_offset_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.read_clock_offset_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def conn_pkt_type_changed_callback_handler(self, packet, opcode):
        """
        CONN_PKT_TYPE_CHANGED Event Callback Handler
        Event Code: 29
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - pkt_type: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'conn_pkt_type_changed_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.conn_pkt_type_changed = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def qos_violation_callback_handler(self, packet, opcode):
        """
        QOS_VIOLATION Event Callback Handler
        Event Code: 30
        Parameters:
        - handle: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'qos_violation_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.qos_violation = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def page_scan_rep_mode_change_callback_handler(self, packet, opcode):
        """
        PAGE_SCAN_REP_MODE_CHANGE Event Callback Handler
        Event Code: 32
        Parameters:
        - bd_addr: 6 bytes
        - page_scan_rep_mode: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'page_scan_rep_mode_change_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.page_scan_rep_mode_change = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def flow_specification_complete_callback_handler(self, packet, opcode):
        """
        FLOW_SPECIFICATION_COMPLETE Event Callback Handler
        Event Code: 33
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - unused: 1 bytes
        - flow_direction: 1 bytes
        - service_type: 1 bytes
        - token_rate: 4 bytes
        - token_bucket_size: 4 bytes
        - peak_bandwidth: 4 bytes
        - access_latency: 4 bytes
        """
        logger = logging.getLogger()
        logger.info(f'flow_specification_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.flow_specification_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def inquiry_result_rssi_callback_handler(self, packet, opcode):
        """
        INQUIRY_RESULT_RSSI Event Callback Handler
        Event Code: 34
        Parameters:
        - num_resp: 1 bytes
        - bd_addr: 6 bytes
        - page_scan_repition_mode: 1 bytes
        - reserved: 1 bytes
        - class_of_device: 3 bytes
        - clock_offset: 2 bytes
        - rssi: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'inquiry_result_rssi_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.inquiry_result_rssi = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def read_remote_extended_features_complete_callback_handler(self, packet, opcode):
        """
        READ_REMOTE_EXTENDED_FEATURES_COMPLETE Event Callback Handler
        Event Code: 35
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - pg_num: 1 bytes
        - max_page_num: 1 bytes
        - extended_lmp_features: 8 bytes
        """
        logger = logging.getLogger()
        logger.info(f'read_remote_extended_features_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.read_remote_extended_features_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def sync_conn_complete_callback_handler(self, packet, opcode):
        """
        SYNC_CONN_COMPLETE Event Callback Handler
        Event Code: 44
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - bd_addr: 6 bytes
        - link_type: 1 bytes
        - transmission_interval: 1 bytes
        - retransmissionWindow: 1 bytes
        - rx_pkt_length: 2 bytes
        - tx_pkt_length: 2 bytes
        - air_mode: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'sync_conn_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.sync_conn_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def sync_conn_changed_callback_handler(self, packet, opcode):
        """
        SYNC_CONN_CHANGED Event Callback Handler
        Event Code: 45
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - transmission_interval: 1 bytes
        - retransmissionWindow: 1 bytes
        - rx_pkt_length: 2 bytes
        - tx_pkt_length: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'sync_conn_changed_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.sync_conn_changed = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def sniff_subrating_callback_handler(self, packet, opcode):
        """
        SNIFF_SUBRATING Event Callback Handler
        Event Code: 46
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        - max_txlatency: 2 bytes
        - max_rx_latency: 2 bytes
        - min_remote_timeout: 2 bytes
        - min_local_timeout: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'sniff_subrating_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.sniff_subrating = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def extended_inquiry_result_callback_handler(self, packet, opcode):
        """
        EXTENDED_INQUIRY_RESULT Event Callback Handler
        Event Code: 47
        Parameters:
        - num_resp: 1 bytes
        - bd_addr: 6 bytes
        - page_scan_repition_mode: 1 bytes
        - reserved: 1 bytes
        - class_of_device: 3 bytes
        - clock_offset: 2 bytes
        - rssi: 1 bytes
        - extended_inquiry_resp: 240 bytes
        """
        logger = logging.getLogger()
        logger.info(f'extended_inquiry_result_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.extended_inquiry_result = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def encryption_key_refresh_complete_callback_handler(self, packet, opcode):
        """
        ENCRYPTION_KEY_REFRESH_COMPLETE Event Callback Handler
        Event Code: 48
        Parameters:
        - status: 1 bytes
        - conn_handle: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'encryption_key_refresh_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.encryption_key_refresh_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def io_capability_req_callback_handler(self, packet, opcode):
        """
        IO_CAPABILITY_REQ Event Callback Handler
        Event Code: 49
        Parameters:
        - bd_addr: 6 bytes
        """
        logger = logging.getLogger()
        logger.info(f'io_capability_req_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.io_capability_req = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def io_capability_resp_callback_handler(self, packet, opcode):
        """
        IO_CAPABILITY_RESP Event Callback Handler
        Event Code: 50
        Parameters:
        - bd_addr: 6 bytes
        - io_capability: 1 bytes
        - oob_data_present: 1 bytes
        - authentication_requirements: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'io_capability_resp_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.io_capability_resp = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def user_confirmation_request_callback_handler(self, packet, opcode):
        """
        USER_CONFIRMATION_REQUEST Event Callback Handler
        Event Code: 51
        Parameters:
        - bd_addr: 6 bytes
        - numeric_value: 4 bytes
        """
        logger = logging.getLogger()
        logger.info(f'user_confirmation_request_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.user_confirmation_request = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def user_passkey_request_callback_handler(self, packet, opcode):
        """
        USER_PASSKEY_REQUEST Event Callback Handler
        Event Code: 52
        Parameters:
        - bd_addr: 6 bytes
        """
        logger = logging.getLogger()
        logger.info(f'user_passkey_request_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.user_passkey_request = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def remote_oob_data_request_callback_handler(self, packet, opcode):
        """
        REMOTE_OOB_DATA_REQUEST Event Callback Handler
        Event Code: 53
        Parameters:
        - bd_addr: 6 bytes
        """
        logger = logging.getLogger()
        logger.info(f'remote_oob_data_request_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.remote_oob_data_request = received_bd_addr
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def simple_pairing_complete_callback_handler(self, packet, opcode):
        """
        SIMPLE_PAIRING_COMPLETE Event Callback Handler
        Event Code: 54
        Parameters:
        - status: 1 bytes
        - bd_addr: 6 bytes
        """
        logger = logging.getLogger()
        logger.info(f'simple_pairing_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.simple_pairing_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def link_supervision_timeout_changed_callback_handler(self, packet, opcode):
        """
        LINK_SUPERVISION_TIMEOUT_CHANGED Event Callback Handler
        Event Code: 56
        Parameters:
        - conn_handle: 2 bytes
        - link_supervision_timeout: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'link_supervision_timeout_changed_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.link_supervision_timeout_changed = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def enhanced_flush_complete_callback_handler(self, packet, opcode):
        """
        ENHANCED_FLUSH_COMPLETE Event Callback Handler
        Event Code: 57
        Parameters:
        - handle: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'enhanced_flush_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.enhanced_flush_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def user_passkey_notification_callback_handler(self, packet, opcode):
        """
        USER_PASSKEY_NOTIFICATION Event Callback Handler
        Event Code: 59
        Parameters:
        - bd_addr: 6 bytes
        - passkey: 4 bytes
        """
        logger = logging.getLogger()
        logger.info(f'user_passkey_notification_callback_handler - {threading.get_ident()}')
        
        bd_addr_bytes = packet[3:9]  # BD Address is 6 bytes (indices 3-8)
        received_bd_addr  = list(bd_addr_bytes )
        self.hciEventVars.user_passkey_notification = received_bd_addr
        self.hciEventVars.acl_connection_info.passkey = (packet[9] | (packet[10] << 8) | (packet[11] << 16) | (packet[12] << 24))
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def keypress_notification_callback_handler(self, packet, opcode):
        """
        KEYPRESS_NOTIFICATION Event Callback Handler
        Event Code: 60
        Parameters:
        - bd_addr: 6 bytes
        - notification_type: 1 bytes
        """
        logger = logging.getLogger()
        logger.info(f'keypress_notification_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.keypress_notification = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def remote_host_supported_features_notification_callback_handler(self, packet, opcode):
        """
        REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION Event Callback Handler
        Event Code: 61
        Parameters:
        - bd_addr: 6 bytes
        - host_supported_features: 8 bytes
        """
        logger = logging.getLogger()
        logger.info(f'remote_host_supported_features_notification_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.remote_host_supported_features_notification = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def num_of_completed_data_blocks_callback_handler(self, packet, opcode):
        """
        NUM_OF_COMPLETED_DATA_BLOCKS Event Callback Handler
        Event Code: 72
        Parameters:
        - total_num_data_blocks: 2 bytes
        - num_handles: 1 bytes
        - conn_handle: 2 bytes
        - num_completed_pkts: 2 bytes
        - num_completed_blocks: 2 bytes
        """
        logger = logging.getLogger()
        logger.info(f'num_of_completed_data_blocks_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.num_of_completed_data_blocks = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def vendor_event_callback_handler(self, packet, opcode):
        """
        VENDOR_EVENT Event Callback Handler
        Event Code: 255
        Parameters:
        - subevent_code: 1 bytes
        - vendor_specific_params: 9 bytes
        """
        logger = logging.getLogger()
        logger.info(f'vendor_event_callback_handler - {threading.get_ident()}')
        
        # Special handling for vendor_event
        self.hciEventVars.vendor_event = 1
        self.hciEventVars.vendor_power_control_notification = packet[4]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def virtual_connect_complete_callback_handler(self, packet, opcode):
        """
        VIRTUAL_CONNECT_COMPLETE Event Callback Handler
        Event Code: 252
        Parameters:
        - vendor_id: 2 bytes
        - sub_event_code: 1 bytes
        - virtual_conn_handle: 2 bytes
        - bd_addr: 6 bytes
        - virtual_link_type: 1 bytes
        - status: 1 bytes
        - reserved: 3 bytes
        """
        logger = logging.getLogger()
        logger.info(f'virtual_connect_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.virtual_connect_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

    def virtual_disconnect_complete_callback_handler(self, packet, opcode):
        """
        VIRTUAL_DISCONNECT_COMPLETE Event Callback Handler
        Event Code: 253
        Parameters:
        - vendor_id: 2 bytes
        - sub_event_code: 2 bytes
        - virtual_conn_handle: 2 bytes
        - reason: 1 bytes
        - reserved: 3 bytes
        """
        logger = logging.getLogger()
        logger.info(f'virtual_disconnect_complete_callback_handler - {threading.get_ident()}')
        
        # Standard event handling - set the event flag
        self.hciEventVars.virtual_disconnect_complete = packet[3]
        
        self.event_lock.release()
        self.hci_lock.release()
        logger.info(f"UNLOCKING THREAD $$$$  {threading.get_ident()}")

