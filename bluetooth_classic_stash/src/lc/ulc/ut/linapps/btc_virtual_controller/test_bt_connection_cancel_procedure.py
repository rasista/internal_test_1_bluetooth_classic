from tests_utils import *

def validate_connection_cancel_with_no_pending_connection_req(dut_central, dut_peripheral, peripheral_bd_addr, logger):
    logger.info("validate_connection_cancel_with_no_pending_connection_req test started")
    dut_central.interfaces.btc.create_connection_cancel(peripheral_bd_addr)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=UNKNOWN_CONNECTION_IDENTIFIER), "Failed to receive UNKNOWN_CONNECTION_IDENTIFIER for connection cancel Req"
    logger.info("validate_connection_cancel_with_no_pending_connection_req test completed")

def validate_connection_cancel_on_existing_bt_connection(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger):
    logger.info("Testing validate_connection_cancel_on_existing_bt_connection test start")
    validate_bt_create_connection_procedure (dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
    dut_central.interfaces.btc.create_connection_cancel(peripheral_bd_addr)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=ACL_CONNECTION_ALREADY_EXISTS), "failed to ACL_CONNECTION_ALREADY_EXISTS cmd complete event"
    logger.info("Testing validate_connection_cancel_on_existing_bt_connection test completed")


def test_bt_connection_cancel_procedure(duts, logger, debugs):
    logger.info(f" test_bt_connection_cancel_procedure Test case started in debug {debugs}")
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
        
        # Step 2: Connection cancel with no pending connection request Validation
        validate_connection_cancel_with_no_pending_connection_req(dut_central, dut_peripheral, peripheral_bd_addr, logger)
                
        # Step 2: Connection cancel with on Existing connection Validation
        validate_connection_cancel_on_existing_bt_connection(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
        
        # Step 4: Termination of the Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e
