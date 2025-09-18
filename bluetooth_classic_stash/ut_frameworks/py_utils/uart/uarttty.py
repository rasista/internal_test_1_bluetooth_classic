import serial
import numpy as np
import time
import os
import sys
import binascii
import threading
workspace_path = os.getenv("WORKSPACE_PATH","workspace_path")
sys.path.append(f"{workspace_path}/bin")
sys.path.append(f"{workspace_path}/src/lc/ulc/ut/linapps/btc_virtual_controller")
sys.path.append(f"{workspace_path}/ut_frameworks/py_utils/thread")
from hci_intf_autogen import *
from conftest import ports
from hci_threads import threads


# Get the port info from command line arguments
# port_info = sys.argv[1]

# Create the UART interface with the specified port
uart_write = serial.Serial(f"/dev/pts/{ports[1]}", 115200)
thread = threads(uart_write, uart_write)


class uart:
    def __init__(self):
        self.count=0
    def write_to_uart(self,hex_array_final):
        while True:
            try:
                if(self.count<=10):
                    thread.write_with_lock(hex_array_final.tobytes())
                    print("Data sent")
                    #add delay 200ms
                    time.sleep(0.5)
                else:
                    hci_cmd = np.array([0x01, 0x1, 0xfc, 0x4, 0xff,0X01,0X01,0X01], dtype=np.uint8)
                    thread.write_with_lock(hci_cmd.tobytes())
                    sys.exit(1)
                self.count+=1
            except KeyboardInterrupt as e:
                print(f"Process killed {e}")
                uart_write.flush()
                uart_write.reset_input_buffer()
                uart_write.reset_output_buffer()
                sys.exit(1)

    def read_from_uart(self):
        while True:
            try:
                if(self.count <= 10):
                    #read on uart port
                    data = thread.read_with_lock(8)
                    #thread.lock.release()
                    print("Received data: ", data)
                    #add delay 200ms
                    time.sleep(0.5)
                else:
                    sys.exit(0)
            except KeyboardInterrupt as e:
                print(f"Process killed {e}")
                uart_write.flush()
                uart_write.reset_input_buffer()
                uart_write.reset_output_buffer()
                sys.exit(1)