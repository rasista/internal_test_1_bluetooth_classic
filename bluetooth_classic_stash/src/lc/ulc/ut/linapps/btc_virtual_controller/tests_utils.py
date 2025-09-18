import threading
import time
import pytest
import os
import logging
import sys
import subprocess
from utf.harness import Harness
from scapy.all import *
work_dir = os.getenv("WORKSPACE_PATH", subprocess.run(["git","rev-parse","--show-toplevel"], stdout=subprocess.PIPE).stdout.decode().strip())
UTF_Framework = f"{work_dir}/utf/utf_lib_btdm/utf_lib_btdm/interfaces/hci/"
sys.path.append(f"{UTF_Framework}")
BUILD_PATH = f"{work_dir}/bin"
from utf.utils.fixtures import get_fixture_params
from typing import Generator , List
from utf.devices import Device
devices = []
UART_PORT_PATH= BUILD_PATH
log_file = f"Firmware_Logs.txt"
logger = logging.getLogger(__name__)
from lpw_to_ulc_notification_events import *
from btc_error_codes import *
CONNECT_PORT = 5001


def kill_firmware(duts):
    # dut_central, dut_peripheral=None
    dut_central = duts[0]
    try:
        dut_peripheral = duts[1]
    except:
        dut_peripheral = None
        pass
    if(dut_peripheral != None):
        dut_central.interfaces.btc.cancel_connect()
        dut_peripheral.interfaces.btc.cancel_listen()
        assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", timeout=15), "Failed to cancel listen"
        assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", timeout=15), "Failed to cancel connect"
        dut_peripheral.interfaces.btc.killFW()
    dut_central.interfaces.btc.killFW()

def clear_queues(duts):
    for dut in duts:
        dut.interfaces.btc.init_queue()

def start_firmware(n_devices, fw_name=f"{BUILD_PATH}/btc_virtual_controller"):
    global devices
    global logger
    try:
        if not os.path.isdir(f"{BUILD_PATH}"):
            subprocess.run(["sh","configure.sh"])
        for i in range(n_devices):
            log_file = f"Firmware_Logs_D{i+1}.txt"
            retProc = ""
            with open(log_file, "a") as log_file:
                log_file.write(f"\n\n\n\n\n\n\nTest case Starts : {time.ctime()}\n")
                retProc = subprocess.Popen([f"{fw_name}", f"{BUILD_PATH}/D{i+1}_UARTfw"], stdout=log_file, stderr=subprocess.PIPE)
            devices.append(retProc)
            time.sleep(2)
    except Exception as e:
        logger.error(f"Failed to start firmware {e}")
        exit(1)
    finally:
        time.sleep(1)

def check_firmware(fw_name=f"{BUILD_PATH}/btc_virtual_controller"):
    global devices
    pid_fw = [str(i) for i in os.popen(f'pidof {fw_name}').read().rstrip().lstrip().split()]
    devices_pids = [str(i.pid) for i in devices]
    devices_names = [str(i.args).split()[1] for i in devices]
    for i in range(len(devices_pids)):
        devices.pop()
        if devices_pids[i] not in pid_fw:
            assert len(devices_pids)==len(pid_fw), f"Firmware killed {devices_names[i]}"

@pytest.fixture()
def duts(
    request: pytest.FixtureRequest, harness: Harness, logger
) -> Generator[List[Device], None, None]:
    devices = harness.create_devices_for_role('dut', request)
    fixture_params = get_fixture_params(request)
    for i in range(len(devices)): #wrapper function 
        for param_name in fixture_params:
            has_index_in_params = i in fixture_params[param_name]  
            if has_index_in_params:
                devices[i].update_config(
                    param_name, **fixture_params[param_name][i]
                )
    [dut_central, dut_peripheral] = devices
    dut_peripheral.interfaces.btc.init_hci_task()
    time.sleep(3)
    dut_central.interfaces.btc.init_hci_task()
    time.sleep(3)
    yield devices
    for dut in devices:
        dut.interfaces.btc.close()
        dut.restore_config()
        
def setFileNames(duts):
    i = 1
    for dut in duts:
        dut.interfaces.btc.setFileName(f'hci_dump_{i}.txt')
        i += 1

def create_virtual_tcp_socket (dut_central, dut_peripheral, logger):
    logger.info("Establishing  virtual tcp socket connection between central and peripheral devices")

    # Triggering virtual tcp socket connection request at the client device
    dut_central.interfaces.btc.virtual_connect(connect_port=CONNECT_PORT, connect_ip=[127,0,0,1])
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete"), "failed to trigger virtual tcp socket connection request at client"
    # Triggering virtual tcp socket connection listen at the server device
    dut_peripheral.interfaces.btc.virtual_listen_connection(connect_port=CONNECT_PORT)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete"), "failed to trigger virtual tcp socket connection listen mode at server"
    
    # Validation of the virtual tcp socket connection establishment
    assert dut_central.interfaces.btc.waitUntilEvent(variable="virtual_connect_complete", value = BT_SUCCESS_STATUS, timeout=15), "failed to establish tcp socket connection at client"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="virtual_connect_complete", value = BT_SUCCESS_STATUS, timeout=15), "failed to establish tcp socket connection at server"

def terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger):
    logger.info("Terminating  virtual tcp socket connection between central and peripheral devices")
  
    # Cancel listen on peripheral
    dut_peripheral.interfaces.btc.virtual_cancel_listen(0)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="virtual_disconnect_complete", value = BT_SUCCESS_STATUS, timeout=15), "failed to cancel listen connection"

    # Cancel connection on central
    dut_central.interfaces.btc.virtual_cancel_connect(0)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="virtual_disconnect_complete", value = BT_SUCCESS_STATUS, timeout=15), "failed to cancel established connection"


def validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger):
    logger.info("Testing validate_bt_create_connection_procedure Start")
    dut_central.interfaces.btc.create_connection(peripheral_bd_addr,PKT_TYPE, PAGE_SCAN_REP_MODE, RESERVED, CLK_OFFSET, ALLOW_ROLE_SWITCH)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to trigger create connection request at central"
    # Triggering the page scan completed event at the peripheral device
    dut_peripheral.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, PAGE_SCAN_SUCCESS_NOTIFICATION_FROM_LPW, central_bd_addr, DATA)
    # Triggering the page completed event at the central device
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, PAGE_SUCCESS_NOTIFICATION_FROM_LPW, peripheral_bd_addr, DATA)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="conn_request"), "failed to receive host connection request at peripheral device"
    dut_peripheral.interfaces.btc.accept_connection_request(central_bd_addr, 1)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to accept host connection request at peripheral device"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="conn_complete", value = peripheral_bd_addr), "failed to establish acl connection at central device"
    dut_peripheral.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, CONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, central_bd_addr, DATA)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="conn_complete", value = central_bd_addr), "failed to establish acl connection at peripheral device"
    logger.info("Testing validate_bt_create_connection_procedure Complete")

def get_device_bd_addr(dut_device):
    dut_device.interfaces.btc.read_bd_addr()
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd_complete for read bd_addr"
    bd_addr = dut_device.interfaces.btc.get_device_bd_addr()
    return bd_addr

def swap32(value):
    return ((value & 0x000000FF) << 24) | \
           ((value & 0x0000FF00) << 8)  | \
           ((value & 0x00FF0000) >> 8)  | \
           ((value & 0xFF000000) >> 24)



        