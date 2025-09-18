from tests_utils import *

def validate_read_clk_offset_from_central(dut_central, conn_handle, logger):
    logger.info("Testing validate_read_clk_offset_central start")
    dut_central.interfaces.btc.read_clock_offset(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd status for read_clock_offset"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_clock_offset_complete", value = BT_SUCCESS_STATUS), "failed to receive read_clock_offset_complete cmd complete event"
    logger.info("Testing read_clock_offset_central Completed")

def validate_read_clk_offset_from_peripheral(dut_peripheral, conn_handle, logger):
    logger.info("Testing validate_read_clk_offset_peripheral start")
    dut_peripheral.interfaces.btc.hciEventVars.expected_conn_handle = conn_handle
    dut_peripheral.interfaces.btc.read_clock_offset(conn_handle)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd status for read_clock_offset"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="read_clock_offset_complete", value = BT_SUCCESS_STATUS), "failed to receive read_clock_offset_complete cmd complete event for peripheral"
    logger.info("Testing read_clock_offset_peripheral Completed")

def validate_multiple_read_clk_offset_requests(dut_central, conn_handle, logger):
    """Test sending duplicate clock offset requests"""
    logger.info("Testing duplicate read clock offset requests")
    dut_central.interfaces.btc.read_clock_offset(conn_handle)
    # Send first request
    dut_central.interfaces.btc.read_clock_offset(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive success for first read_clock_offset request"
    
    # Wait for first request to complete
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_clock_offset_complete", value = BT_SUCCESS_STATUS), "Failed to receive read_clock_offset_complete event first"
    logger.info("Testing duplicate read clock offset requests completed")
    dut_central.interfaces.btc.read_clock_offset(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive COMMAND_STATUS for second read_clock_offset request"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_clock_offset_complete", value = BT_SUCCESS_STATUS), "Failed to receive read_clock_offset_complete event second"
    logger.info("Testing duplicate read clock offset requests completed")

def validate_read_clk_offset_without_connection(dut_central, conn_handle, logger):
    """Test read clock offset without connection"""
    logger.info("Testing read clock offset without connection")
    dut_central.interfaces.btc.read_clock_offset(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=UNKNOWN_CONNECTION_IDENTIFIER), "Failed to receive COMMAND_STATUS for read_clock_offset with unknown connection identifier"
    logger.info("Testing read clock offset after disconnection completed")
    
def validate_read_clk_offset_with_invalid_connection_handle(dut_central, conn_handle, logger):
    """Test read clock offset with invalid connection handle"""
    logger.info("Testing read clock offset with_invalid_connection_handle")
    dut_central.interfaces.btc.read_clock_offset(INVALID_CONN_HANDLE)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=UNKNOWN_CONNECTION_IDENTIFIER), "Failed to receive COMMAND_STATUS for read_clock_offset with unknown connection identifier"
    logger.info("Testing read clock offset with_invalid_connection_handle completed")
    
def test_read_clock_offset_procedure(duts, logger, debugs):
    logger.info(f"test_read_clock_offset_procedure Test case in debug {debugs}")
    if not debugs:
        start_firmware(2)
    
    try:
        [dut_central, dut_peripheral] = duts
        setFileNames(duts)
        
        # Bluetooth addresses of central and peripheral for testing
        central_bd_addr = get_device_bd_addr(dut_central)
        peripheral_bd_addr = get_device_bd_addr(dut_peripheral)
        connected_device_info_central = dut_central.interfaces.btc.get_acl_connection_info()
        connected_device_info_peripheral = dut_peripheral.interfaces.btc.get_acl_connection_info()
        
        # Step 1: Virtual TCP socket connection Validation
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        # Step 2: read clock offset validation without connection
        validate_read_clk_offset_without_connection(dut_central, 1, logger)

        # Step 3: BT Create connection Validation
        validate_bt_create_connection_procedure (dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
        
        # Step 4: read clock offset validation with invalid connection handle
        validate_read_clk_offset_with_invalid_connection_handle(dut_central, 1, logger)
        
        # Step 5: read clock offset validation triggered by central
        logger.info(f"test_suported_features_procedure conn_handle {connected_device_info_central.conn_handle}")
        validate_read_clk_offset_from_central(dut_central, connected_device_info_central.conn_handle, logger)

        # Step 6: read clock offset validation triggered by peripheral
        logger.info(f"test_suported_features_procedure conn_handle {connected_device_info_peripheral.conn_handle}")
        validate_read_clk_offset_from_peripheral(dut_peripheral, connected_device_info_peripheral.conn_handle, logger)
        
        # Step 7: duplicate read clock offset validation
        logger.info(f"test_suported_features_procedure conn_handle {connected_device_info_central.conn_handle}")
        validate_multiple_read_clk_offset_requests(dut_central, connected_device_info_central.conn_handle, logger)

       # Step 8: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e