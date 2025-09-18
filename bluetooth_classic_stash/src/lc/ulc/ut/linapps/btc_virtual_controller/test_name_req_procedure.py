from tests_utils import *

def validate_remote_name_request(dut_central, dut_peripheral, bd_addr, logger):
    logger.info("Testing remote name request start")
    dut_peripheral.interfaces.btc.write_local_name("BT_PERIPHERAL")
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to receive BT_SUCCESS_STATUS cmd_complete for write local device name"

    dut_central.interfaces.btc.remote_name_request(bd_addr,PAGE_SCAN_REP_MODE, RESERVED, CLK_OFFSET)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for remote name req"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="remote_name_request_complete",value = "BT_PERIPHERAL"), "Failed to remote_name_req_complete cmd complete event"
    logger.info("Testing remote name request Completed")


def validate_remote_name_request_cancel(dut_central, bd_addr, logger):
    logger.info("Testing remote name request cancel start")
    dut_central.interfaces.btc.remote_name_request_cancel(bd_addr)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=INVALID_HCI_COMMAND_PARAMETERS), "failed to receive INVALID_HCI_COMMAND_PARAMETERS cmd_complete for remote name request"
    
    # Disabling the below code as it is not working in the current implementation due to LIMITATION in the UTF FRAME WORK.
    # dut_central.interfaces.btc.remote_name_request(bd_addr)
    # dut_central.interfaces.btc.remote_name_req_cancel(bd_addr)
    # assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for remote name req "
    # assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to receive  BT_SUCCESS_STATUS cmd omplete for remote name request cancel"
    logger.info("Testing remote name request Completed")
    


def test_name_request_procedure(duts, logger, debugs):
    logger.info(f"test_name_request_procedure Test case started in debug  {debugs}")
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
        validate_bt_create_connection_procedure (dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)

        # Step 3: Remote Name request Validation
        validate_remote_name_request(dut_central, dut_peripheral, peripheral_bd_addr, logger)
        
        # Step 4: Remote Name Request Cancel Validation
        validate_remote_name_request_cancel(dut_central, peripheral_bd_addr, logger)
         
        # Step 5: Termination of the Virtual TCP Socket connection
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e