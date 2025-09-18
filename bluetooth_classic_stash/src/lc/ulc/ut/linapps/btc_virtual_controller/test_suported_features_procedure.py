from tests_utils import *

def validate_supported_feature_request(dut_central, conn_handle, logger):
    logger.info("Testing validate_supported_feature_request start")
    dut_central.interfaces.btc.read_remote_supported_features(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to recevive BT_SUCCESS_STATUS cmd status for read supported features"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_remote_supported_features_complete", value = BT_SUCCESS_STATUS), "failed to receive read_remote_supported_features complete event"
    logger.info("Testing validate_supported_feature_request Completed")


def test_suported_features_procedure(duts, logger, debugs):
    logger.info(f"test_suported_features_procedure Test case in debug {debugs}")
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
        
        # Step 3: Suported Features Request Validation
        connected_device_info = dut_central.interfaces.btc.get_acl_connection_info()
        logger.info(f"test_suported_features_procedure conn_handle {connected_device_info.conn_handle}")
        validate_supported_feature_request(dut_central, connected_device_info.conn_handle, logger)
        
       # Step 4: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e