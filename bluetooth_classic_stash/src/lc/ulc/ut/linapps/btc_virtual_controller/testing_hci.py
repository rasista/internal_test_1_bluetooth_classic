import sys
import os
import subprocess
import pytest
import glob
import threading
import time
import serial
from pathlib import Path
from build_fw import start_btc_virtual_controller
import argparse
import pdb
import threading
import copy 
import ctypes
import json 
import numpy as np
workspace_path = os.getenv("WORKSPACE_PATH","workspace_path")
sys.path.append(f"{workspace_path}/bin")
sys.path.append(f"{workspace_path}/ut_frameworks/py_utils/thread")
sys.path.append(f"{workspace_path}/ut_frameworks/py_utils/uart")
sys.path.append(f"{workspace_path}/bin/btc_ulc_bin")
from hci_intf_autogen import *
from hci_threads import threads
 
SIM_PATH = os.getenv("SIM_PATH")
BUILD_PATH = os.getenv("BUILD_PATH")
 
sys.path.append(SIM_PATH)
sys.path.append(BUILD_PATH)

CONNECT_PORT = 5001
 
def write_to_uart(hex_array_final,thread):
            time.sleep(5)
            thread.write_with_lock(hex_array_final.tobytes())
            print("Data sent")
            #add delay 200ms
            time.sleep(0.5)
 
def read_from_uart(thread):
        time.sleep(5)
        data = thread.read_with_lock(11)
        print("Received data: ", data)
       

def connect(port_info):
    uart_write = serial.Serial(f"{port_info}", 115200)
    thread = threads(uart_write, uart_write)
    test_data_flow = connect_t(command_bitmap=[0,0,0,4], command_pend=0, command_lock=0)
    test_data_flow.connect_port = CONNECT_PORT
    test_data_flow.connect_ip = [127,0,0,1]
    hex_array_final = test_data_flow.linearize()
    print(f"hex_final_array --> {hex_array_final}")
    print("Sending data: ", hex_array_final.tobytes())
    thread_r = threading.Thread(target = read_from_uart , args = (thread,))
    thread_w = threading.Thread(target = write_to_uart , args = (hex_array_final,thread,))
    uart_write.flush()
    uart_write.reset_input_buffer()
    uart_write.reset_output_buffer()
    thread_w.start()
    thread_r.start()

def listen_connection(port_info):
    uart_write = serial.Serial(f"{port_info}", 115200)
    thread = threads(uart_write, uart_write)
    test_data_flow = listen_connection_t()
    test_data_flow.connect_port = CONNECT_PORT
    hex_array_final = test_data_flow.linearize()
    print(f"hex_final_array --> {hex_array_final}")
    print("Sending data: ", hex_array_final.tobytes())
    thread_r = threading.Thread(target = read_from_uart ,args = (thread,))
    thread_w = threading.Thread(target = write_to_uart , args = (hex_array_final,thread,))
    uart_write.flush()
    uart_write.reset_input_buffer()
    uart_write.reset_output_buffer()
    thread_w.start()
    thread_r.start() 

def cancel_connect(port_info):
    uart_write = serial.Serial(f"{port_info}", 115200)
    thread = threads(uart_write, uart_write)
    test_data_flow = cancel_connect_t()
    hex_array_final = test_data_flow.linearize()
    print(f"hex_final_array --> {hex_array_final}")
    print("Sending data: ", hex_array_final.tobytes())
    thread_r = threading.Thread(target = read_from_uart ,args = (thread,))
    thread_w = threading.Thread(target = write_to_uart , args = (hex_array_final,thread,))
    uart_write.flush()
    uart_write.reset_input_buffer()
    uart_write.reset_output_buffer()
    thread_w.start()
    thread_r.start() 

def cancel_listen(port_info):
    uart_write = serial.Serial(f"{port_info}", 115200)
    thread = threads(uart_write, uart_write)
    test_data_flow = cancel_listen_t()
    hex_array_final = test_data_flow.linearize()
    print(f"hex_final_array --> {hex_array_final}")
    print("Sending data: ", hex_array_final.tobytes())
    thread_r = threading.Thread(target = read_from_uart ,args = (thread,))
    thread_w = threading.Thread(target = write_to_uart , args = (hex_array_final,thread,))
    uart_write.flush()
    uart_write.reset_input_buffer()
    uart_write.reset_output_buffer()
    thread_w.start()
    thread_r.start() 

def send_tx(port_info):
    uart_write = serial.Serial(f"{port_info}", 115200)
    thread = threads(uart_write, uart_write)
    test_data_flow = send_tx_t()
    test_data_flow.data_byte_1 = int(1)
    test_data_flow.data_byte_2 = int(2)
    test_data_flow.data_byte_3 = int(3)
    test_data_flow.data_byte_4 = int(4)
    test_data_flow.data_byte_5 = int(5)
    test_data_flow.data_byte_6 = int(6)
    test_data_flow.data_byte_7 = int(7)
    test_data_flow.data_byte_8 = int(8)
    test_data_flow.data_byte_9 = int(9)
    test_data_flow.data_byte_10 = int(10)
    test_data_flow.data_byte_11 = int(11)
    test_data_flow.data_byte_12 = int(12)
    test_data_flow.data_byte_13 = int(13)
    test_data_flow.data_byte_14 = int(14)
    test_data_flow.data_byte_15 = int(15)
    test_data_flow.data_byte_16 = int(16)
    test_data_flow.data_byte_17 = int(17)
    test_data_flow.data_byte_18 = int(18)
    test_data_flow.data_byte_19 = int(19)
    test_data_flow.data_byte_20 = int(20)
    hex_array_final = test_data_flow.linearize()
    print(f"hex_final_array --> {hex_array_final}")
    print("Sending data: ", hex_array_final.tobytes())
    thread_r = threading.Thread(target = read_from_uart ,args = (thread,))
    thread_w = threading.Thread(target = write_to_uart , args = (hex_array_final,thread,))
    uart_write.flush()
    uart_write.reset_input_buffer()
    uart_write.reset_output_buffer()
    thread_w.start()
    thread_r.start() 

def main():
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--port', type=str, required=True, help='Port for the serial connection')
    parser.add_argument('--listen_connection', action='store_true', help='If set, listen_connection_t will be called')
    parser.add_argument('--connect', action='store_true', help='If set, connect_t will be called')
    parser.add_argument('--cancel_connect', action='store_true', help='If set, cancel_connect_t will be called')
    parser.add_argument('--cancel_listen', action='store_true', help='If set, cancel_listen_t will be called')
    parser.add_argument('--send_tx', action='store_true', help='If set, send_tx_t will be called')

    args = parser.parse_args()

    if args.connect:
        connect(args.port)
    elif args.listen_connection:
        listen_connection(args.port)
    elif args.cancel_connect:
        cancel_connect(args.port)
    elif args.cancel_listen:
        cancel_listen(args.port)
    elif args.send_tx:
        send_tx(args.port)

if __name__ == "__main__":
    main()