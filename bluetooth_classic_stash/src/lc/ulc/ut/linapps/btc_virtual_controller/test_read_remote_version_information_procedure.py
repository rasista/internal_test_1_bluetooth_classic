from tests_utils import *

def validate_read_remote_version_information_request(dut_central, conn_handle, logger):
    """Validate Read Remote Version Information command"""
    logger.info("Testing validate_read_remote_version_information_request")
    dut_central.interfaces.btc.read_remote_version_information(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd status for read remote version information"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_remote_version_info_complete", value = BT_SUCCESS_STATUS), "failed to receive read_remote_version_info_complete event FOR validate_read_remote_version_information_request"
    logger.info("Testing validate_read_remote_version_information_request completed")

def validate_read_remote_version_information_invalid_handle(dut_central, conn_handle,logger):
    """Validate Read Remote Version Information command with invalid connection handle"""
    logger.info("Testing validate_read_remote_version_information_invalid_handle")
    dut_central.interfaces.btc.read_remote_version_information(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=UNKNOWN_CONNECTION_IDENTIFIER), "failed to receive UNKNOWN_CONNECTION_IDENTIFIER for invalid handle"
    logger.info("Testing validate_read_remote_version_information_invalid_handle completed")

def validate_read_remote_version_information_multiple_requests(dut_central, conn_handle, logger):
    """Validate Read Remote Version Information command with multiple requests"""
    logger.info("Testing validate_read_remote_version_information_multiple_requests")
    
    # Send multiple requests to test command handling
    for i in range(3):
        dut_central.interfaces.btc.read_remote_version_information(conn_handle)
        assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), f"failed to receive BT_SUCCESS_STATUS for request {i+1}"
        assert dut_central.interfaces.btc.waitUntilEvent(variable="read_remote_version_info_complete", value = BT_SUCCESS_STATUS), f"failed to receive read_remote_version_info_complete event for request {i+1}"
    logger.info("Testing validate_read_remote_version_information_multiple_requests completed")

def test_read_remote_version_information(duts, logger, debugs):
    """Main test function for Read Remote Version Information command covering all scenarios"""
    logger.info(f"test_read_remote_version_information Test case started in debug {debugs}")
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
        
        # Step 3: Basic Read Remote Version Information Validation
        connected_device_info = dut_central.interfaces.btc.get_acl_connection_info()
        logger.info(f"test_suported_features_procedure conn_handle {connected_device_info.conn_handle}")
        validate_read_remote_version_information_request(dut_central, connected_device_info.conn_handle, logger)

        # Step 4: Multiple Requests Read Remote Version Information Validation
        validate_read_remote_version_information_multiple_requests(dut_central, connected_device_info.conn_handle, logger)
        
        # Step 6: Invalid Handle Read Remote Version Information Validation
        validate_read_remote_version_information_invalid_handle(dut_central, INVALID_CONN_HANDLE,logger)
        
        # Step 7: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e 