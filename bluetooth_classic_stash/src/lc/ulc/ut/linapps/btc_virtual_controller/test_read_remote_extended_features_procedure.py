from tests_utils import *

def validate_read_remote_extended_features_request(dut_central, conn_handle, page_num, logger):
    """Validate Read Remote Extended Features command with specified page number"""
    logger.info(f"Testing validate_read_remote_extended_features_request with page_num={page_num}")
    dut_central.interfaces.btc.read_remote_extended_features(conn_handle, page_num)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), f"failed to receive BT_SUCCESS_STATUS cmd status for read remote extended features page {page_num}"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_remote_extended_features_complete",value = BT_SUCCESS_STATUS), f"failed to receive read_remote_extended_features_complete complete event for page {page_num}"
    logger.info(f"Testing validate_read_remote_extended_features_request with page_num={page_num} completed")

def validate_read_remote_extended_features_invalid_handle(dut_central, conn_handle, logger):
    """Validate Read Remote Extended Features command with invalid connection handle"""
    logger.info("Testing validate_read_remote_extended_features_invalid_handle")
    dut_central.interfaces.btc.read_remote_extended_features(conn_handle, 0)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=UNKNOWN_CONNECTION_IDENTIFIER), "failed to receive UNKNOWN_CONNECTION_IDENTIFIER for invalid handle"
    logger.info("Testing validate_read_remote_extended_features_invalid_handle completed")

def validate_read_remote_extended_features_multiple_pages(dut_central, conn_handle, logger):
    """Validate Read Remote Extended Features command with multiple consecutive pages"""
    logger.info("Testing validate_read_remote_extended_features_multiple_pages")
    
    # Test reading pages 0, 1, 2 in sequence
    for page_num in range (0, 50):
        dut_central.interfaces.btc.read_remote_extended_features(conn_handle, page_num)
        assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), f"failed to receive BT_SUCCESS_STATUS for page {page_num}"
        assert dut_central.interfaces.btc.waitUntilEvent(variable="read_remote_extended_features_complete",value = BT_SUCCESS_STATUS), f"failed to receive read_remote_extended_features_complete complete event for validate_read_remote_extended_features_multiple_pages- page {page_num}"
    logger.info("Testing validate_read_remote_extended_features_multiple_pages completed")

def test_read_remote_extended_features(duts, logger, debugs):
    """Main test function for Read Remote Extended Features command covering all scenarios"""
    logger.info(f"test_read_remote_extended_features Test case started in debug {debugs}")
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
        
        connected_device_info = dut_central.interfaces.btc.get_acl_connection_info()
        logger.info(f"test_suported_features_procedure conn_handle {connected_device_info.conn_handle}")
        
        # Step 3: Basic Read Remote Extended Features Validation
        validate_read_remote_extended_features_request(dut_central, connected_device_info.conn_handle, 1, logger)
        
        # Step 4: Multiple Pages Read Remote Extended Features Validation
        validate_read_remote_extended_features_multiple_pages(dut_central, connected_device_info.conn_handle, logger)
        
        # Step 5: Invalid Handle Read Remote Extended Features Validation
        validate_read_remote_extended_features_invalid_handle(dut_central, INVALID_CONN_HANDLE, logger)

        # Step 6: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e 