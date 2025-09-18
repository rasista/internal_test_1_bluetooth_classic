import threading
import time 
import numpy as np
import serial

class threads:   
    def __init__(self , uart_read , uart_write):
        self.uart_write = uart_write
        self.uart_read = uart_read 
        self.lock = threading.Lock()

    def write_with_lock(self, data):
        with self.lock:
            print(f"inside write_with_lock")
            print(f"data: {data}")
            try:
                self.uart_write.write(data)
            except:
                print("process killed")
                self.uart_write.flush()
                self.uart_write.reset_output_buffer()

    def read_with_lock(self, num_bytes):
        with self.lock:
            print(f"inside read_with_lock")
            try:
                return self.uart_read.read(num_bytes)
            except:
                print("process killed")
                self.uart_read.flush()
                self.uart_read.reset_input_buffer()
        

