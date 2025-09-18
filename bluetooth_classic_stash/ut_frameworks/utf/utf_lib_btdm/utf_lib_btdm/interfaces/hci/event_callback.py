import threading
import time
import queue
import sys 
import os
from queue import Empty
from typing import Optional
from utf.devices import Device
from enum import Enum
BUILD_PATH = os.getenv("BUILD_PATH")
sys.path.append(f"{BUILD_PATH}/../src/lc/ulc/autogen")
from hci_event_callback_autogen import *
from callback_handlers_def_autogen import call_back_handler_def
from hci_common_api_autogen import HciCommonApi

def create_interface(device, config, cli_transport):
    return EventCallback(device, config, cli_transport)

    
class EventCallback(HciCommonApi):
    def __init__(self, device: Device,
                 config: Optional[dict],
                 cli_transport: str):
        self.device = device
        super().__init__(device, config, cli_transport)
        self.completed = False
        self.packet_storage_queue = queue.Queue()
        self.call_back_handler = call_back_handler_def()
        self.call_back_handler.set_semaphore(self.hci_lock, self.hciEventVars.eventHandlerSemaphore)
        self.call_back_handler.setEventVars_handler(self.hciEventVars)

    def print_packets(self):
        """Method to continuously print packets from the storage queue."""
        while not self._stop_processing:
            try:
                packet = self.packet_storage_queue.get(timeout=1)
                self.logger.info(f'Printing packet from storage queue: {packet}')
            except queue.Empty:
                continue 
   
    def init_hci_task(self):
        def HCI_receive_task():
            event_code = 0
            while not self._stop_processing:
                print("waiting for packet")
                packet = self.get_from_queue()
                if packet:
                    with open(self.fileName, "a") as f:
                        f.write("000000 00 00 00 00 "+ packet.hex(" ") + "\n")
                    print("packet found")
                    self.packet_storage_queue.put(packet)
                    event_code = packet[1]
                    self.logger.info(f'HCI_receive_task Recived packet event code - {event_code}')
                    try:
                        handler = self.call_back_handler.command_handlers.get(event_code,
                                                                            self.call_back_handler.handle_unknown_command)
                        self.logger.info(f'handler - {handler}')
                        handler(packet, event_code)
                    except Exception as e:
                            self.logger.error(e)                    
                time.sleep(0.1)
        self.reader_thread = threading.Thread(target=HCI_receive_task)
        self._stop_processing = False
        self.reader_thread.start()
        self.logger.info(f'init_hci_task of device {self.device.name} - {self.reader_thread.ident}')
       
        ### thread for printing the packets
        self.printer_thread = threading.Thread(target=self.print_packets)
        self.printer_thread.start()

    def get_device_bd_addr(self):
        bd_addr = getattr(self.hciEventVars, "device_bd_addr")
        return bd_addr
    
    def set_remote_device_bd_addr(self, remote_device_bd_addr):
        setattr(self.hciEventVars, "remote_device_bd_addr", remote_device_bd_addr)
        
    def get_acl_connection_info(self):
        return self.hciEventVars.acl_connection_info

    def waitUntilEvent(self, variable, value=1, timeout=2):
        """Wait until a specific variable reaches the expected value or timeout occurs.
        This method attempts to acquire a semaphore that is released whenever any variable
        is updated by packet handlers. Upon acquisition, it checks if the specified variable
        has reached the desired value. If not, it tries again until timeout expires.
        """
        try:
            curr_timeout = timeout
            start_time = time.time()
            end_time = start_time + curr_timeout
            while (curr_timeout - (end_time - start_time) < curr_timeout):
                self.hciEventVars.eventHandlerSemaphore.acquire(timeout=curr_timeout)
                actual_value = getattr(self.hciEventVars, variable)
                self.logger.info(f"actual_value = {actual_value} expected_value = {value}")
                if actual_value == value:
                    setattr(self.hciEventVars, variable, 0xFF)  # Reset the variable to default value
                    return True
                start_time = time.time()
                curr_timeout = end_time - start_time
            return False
        except Exception as e:
            self.hciEventVars.eventHandlerSemaphore.release()
            assert False, f"Failed to acquire lock {e} {variable}"

