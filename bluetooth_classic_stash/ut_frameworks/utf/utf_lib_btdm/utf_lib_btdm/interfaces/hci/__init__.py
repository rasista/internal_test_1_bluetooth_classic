import threading
import sys
import os
from queue import Queue, Empty
from typing import Optional
from utf.devices import Device
from utf.interfaces import ControlInterface
from utf.interfaces.cli import CLITransport
BUILD_PATH = os.getenv("BUILD_PATH")
if BUILD_PATH:
    sys.path.append(f"{BUILD_PATH}/../src/lc/ulc/autogen")
from hci_event_vars_autogen import HCI_Event_Vars

def create_interface(device, config, cli_transport):
    return HCI(device, config, cli_transport)


class HCI(ControlInterface):
    def __init__(self, device: Device,
                 config: Optional[dict],
                 cli_transport: str):
        super().__init__(config)
        self.bus: CLITransport = device.create_interface(cli_transport)
        self.bus.register_rx_data_observer(self.rx_data_observer)
        self._stop_processing = False
        self.reader_thread: Optional[threading.Thread] = None
        self.hci_queue = Queue()
        self.hci_lock = threading.Semaphore(1)
        self.hciEventVars = HCI_Event_Vars()
        
    def rx_data_observer(self, timestamp, record):
        # process based on response and keep it in respective queue
        self.hci_queue.put_nowait(record)
        self.logger.info(f"rx_data_observer called for packet - {record} in thread - {self.reader_thread.ident}")

    def issue_command(self, command):
        self.hci_lock.acquire()
        with open(self.fileName, "a") as f:
            f.write("000000 00 00 00 00 "+ command.hex(" ") + "\n")
        self.bus.write_bytes(command)

    def get_from_queue(self):   
        try:
            data = self.hci_queue.get_nowait()
            self.logger.info(f"get_from_queue - {data}")
            return data
        except Empty as _:
            return None

    def close(self):
        self.logger.info('closing interface')
        self.bus.release_resources()
        self._stop_processing = True
        if self.reader_thread is not None:
            self.logger.info('stopping thread')
            self.reader_thread.join()
            self.reader_thread = None
