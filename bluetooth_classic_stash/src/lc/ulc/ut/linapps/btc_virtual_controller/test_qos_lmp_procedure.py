from tests_utils import *

token_rate = 1000000  # 1 Mbps
peak_bandwidth = 2000000  # 2 Mbps
delay_variation = 5000  # 5ms
unused = 0


def validate_qos_setup_best_effort_request(dut_device, conn_handle, latency, status, logger):
    logger.info("Testing validate_qos_setup_best_effort_request start")

    # Configure QoS parameters
    service_type = 1  # Best effort
    
    dut_device.interfaces.btc.qos_setup(conn_handle, unused, service_type, token_rate, peak_bandwidth, latency, delay_variation)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd status for QoS setup"
    assert dut_device.interfaces.btc.waitUntilEvent(variable="qos_setup_complete",value = status), "failed to receive qos_setup_complete event"
    logger.info("Testing validate_qos_setup_request Completed")


def validate_qos_setup_granted_service_request(dut_device, remote_dut, conn_handle, latency, status, logger):
    logger.info("Testing validate_qos_setup_granted_service_request start")
    
    # Configure QoS parameters for granted service type
    service_type = 2  # Granted service
    
    dut_device.interfaces.btc.qos_setup(conn_handle, unused, service_type, token_rate, peak_bandwidth, latency, delay_variation)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd status for QoS granted service setup"
    assert remote_dut.interfaces.btc.waitUntilEvent(variable="qos_setup_complete",value = status), "failed to receive qos_setup_complete event for granted service"
    logger.info("Testing validate_qos_setup_granted_service_request Completed")


def validate_qos_setup_other_service_request(dut_device, conn_handle, latency, status, logger):
    logger.info("Testing validate_qos_setup_other_service_request start")

    # Configure QoS parameters
    service_type = 0  # Other service

    dut_device.interfaces.btc.qos_setup(conn_handle, unused, service_type, token_rate, peak_bandwidth, latency, delay_variation)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd status for QoS setup"
    assert dut_device.interfaces.btc.waitUntilEvent(variable="qos_setup_complete",value = status), "failed to receive qos_setup_complete event"
    logger.info("Testing validate_qos_setup_request Completed")

def test_qos_lmp_procedure(duts, logger, debugs):
    logger.info(f"test_qos_lmp_peripheral_procedure Test case in debug {debugs}")
    if not debugs:
        start_firmware(2)
    
    try:
        [dut_central, dut_peripheral] = duts
        setFileNames(duts)
        
        # Get the Bluetooth addresses of central and peripheral for testing
        central_bd_addr = get_device_bd_addr(dut_central)
        peripheral_bd_addr = get_device_bd_addr(dut_peripheral)
        
        # Step 1: Virtual TCP socket connection Validation
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        # Step 2: BT Create connection Validation
        validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)

        # Step 3: QoS LMP Setup Request Validation from Peripheral side
        connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
        conn_handle = connected_device_info.conn_handle

        latency = 5000  #5ms
        latency = swap32(latency)
        validate_qos_setup_best_effort_request(dut_central, conn_handle, latency, BT_SUCCESS_STATUS, logger)
        validate_qos_setup_granted_service_request(dut_central, dut_peripheral, conn_handle, latency, BT_SUCCESS_STATUS, logger)
        validate_qos_setup_other_service_request(dut_central, conn_handle, latency, BT_SUCCESS_STATUS, logger)
        latency = 1000  #1ms
        latency = swap32(latency)
        validate_qos_setup_best_effort_request(dut_central, conn_handle, latency, UNSPECIFIED_ERROR, logger)

        # Step 4: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e
