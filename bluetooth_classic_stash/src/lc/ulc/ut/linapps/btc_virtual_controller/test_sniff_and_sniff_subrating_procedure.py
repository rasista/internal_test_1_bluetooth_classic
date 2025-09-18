from tests_utils import *

# Test functions for Bluetooth Sniff and Sniff Subrating Procedure:
# - configure_default_link_policy_settings: Configures default link policy settings for the device
# - configure_supervision_timeout: Configures supervision timeout for a specific connection with validation
# - configure_connection_link_policy_settings: Configures link policy settings for a specific connection
# - validate_sniff_mode_with_invalid_parameters: Tests sniff mode failure with invalid parameters (negative test)
# - validate_sniff_mode_entry: Tests successful entry into sniff mode with valid parameters
# - validate_sniff_mode_exit: Tests successful exit from sniff mode back to active mode
# - validate_sniff_subrating_enable: Tests enabling sniff subrating on an existing sniff connection
# - validate_sniff_subrating_disable: Tests disabling sniff subrating and returning to normal sniff mode
# - validate_sniff_subrating_with_invalid_parameters: Tests sniff subrating on active connection (invalid scenario)
# - test_sniff_and_sniff_subrating_procedure: Main test function that executes sniff and subrating test scenarios

# Sniff Mode Constants
SNIFF_MAX_INTERVAL = 0x0800  # 1.28 seconds
SNIFF_MIN_INTERVAL = 0x0006  # 3.75ms
SNIFF_ATTEMPT = 0x0001
SNIFF_TIMEOUT = 0x0001

# Sniff Subrating Constants
MAX_LATENCY = 0x0010
MIN_REMOTE_TIMEOUT = 0x0002
MIN_LOCAL_TIMEOUT = 0x0002

# Supervision Timeout Constants
DEFAULT_SUPERVISION_TIMEOUT = 0x7D00  # 20 seconds (0x7D00 * 0.625ms)
MIN_SUPERVISION_TIMEOUT = 0x000A      # 6.25ms minimum
MAX_SUPERVISION_TIMEOUT = 0xC80       # 20 seconds maximum

# Test Data Constants
TEST_DATA_SIZE = 256
TEST_PACKET_COUNT = 10

# Link Policy Constants
ENABLE_SNIFF_MODE_POLICY = 0x0004
DEFAULT_LINK_POLICY = 0x0000
ENABLE_ALL_LMP_FEATURES = 0x000F


def read_default_link_policy_settings(dut_device, logger):
    """Read default link policy settings from the device"""
    logger.info("Reading default link policy settings")
    dut_device.interfaces.btc.read_default_link_policy_settings()
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for read default link policy settings"


def read_connection_link_policy_settings(dut_device, conn_handle, logger):
    """Read link policy settings for a specific connection"""
    logger.info(f"Reading connection link policy settings for handle {conn_handle}")
    dut_device.interfaces.btc.read_link_policy_settings(conn_handle)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for read link policy settings"


def read_supervision_timeout(dut_device, conn_handle, logger):
    """Read supervision timeout for a specific connection"""
    logger.info(f"Reading supervision timeout for handle {conn_handle}")
    dut_device.interfaces.btc.read_link_supervision_timeout(conn_handle)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for read link supervision timeout"


def configure_default_link_policy_settings(dut_device, policy_settings, logger):
    """Configure default link policy settings for the device"""
    logger.info(f"Configuring default link policy settings: 0x{policy_settings:04X}")
    dut_device.interfaces.btc.write_default_link_policy_settings(policy_settings)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for write default link policy settings"
    read_default_link_policy_settings(dut_device, logger)
    logger.info("Default link policy settings configured successfully")

def configure_connection_link_policy_settings(dut_device, conn_handle, policy_settings, logger):
    """Configure link policy settings for a specific connection"""
    logger.info(f"Configuring connection link policy settings for handle {conn_handle}: 0x{policy_settings:04X}")
    dut_device.interfaces.btc.write_link_policy_settings(conn_handle, policy_settings)
    assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for write link policy settings"    
    read_connection_link_policy_settings(dut_device, conn_handle, logger)
    logger.info("Connection link policy settings configured successfully")


def configure_supervision_timeout(dut_central, dut_peripheral, conn_handle, timeout_value, logger):
    """Configure supervision timeout for a specific connection"""
    logger.info(f"Configuring supervision timeout for handle {conn_handle}: 0x{timeout_value:04X} ({timeout_value * 0.625}ms)")
    dut_central.interfaces.btc.write_link_supervision_timeout(conn_handle, timeout_value)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for write link supervision timeout"

    # Validate mode change event after supervision timeout configuration
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_supervision_timeout_changed", value=conn_handle), "Failed to receive link_supervision_timeout_changed event after supervision timeout configuration"
    read_supervision_timeout(dut_peripheral, conn_handle, logger)
    logger.info("Supervision timeout configured successfully with mode change validation")


def validate_sniff_mode_with_invalid_parameters(dut_central, conn_handle, logger):
    """Test sniff mode failure with invalid parameters"""
    logger.info("Testing validate_sniff_mode_with_invalid_parameters start")
    
    # Request sniff mode with invalid parameters (min > max)
    invalid_max = 0x0004
    invalid_min = 0x0008
    # Invalid max and min intervals
    dut_central.interfaces.btc.sniff_mode(conn_handle, invalid_max, invalid_min, SNIFF_ATTEMPT, SNIFF_TIMEOUT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=UNSUPPORTED_LMP_PARAMETER_VALUE), "Failed to receive UNSUPPORTED_LMP_PARAMETER_VALUE for invalid sniff parameters"

    # invalid tsniff and supervisiontimeout intervals
    dut_central.interfaces.btc.sniff_mode(conn_handle, DEFAULT_SUPERVISION_TIMEOUT, DEFAULT_SUPERVISION_TIMEOUT, SNIFF_ATTEMPT, SNIFF_TIMEOUT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=UNSUPPORTED_LMP_PARAMETER_VALUE), "Failed to receive UNSUPPORTED_LMP_PARAMETER_VALUE for invalid sniff parameters"
    logger.info("Testing validate_sniff_mode_with_invalid_parameters Completed")


def validate_sniff_mode_entry(dut_central, dut_peripheral, conn_handle, logger):
    """Test successful entry into sniff mode"""
    logger.info("Testing validate_sniff_mode_entry start")
    
    # Request sniff mode
    dut_central.interfaces.btc.sniff_mode(conn_handle, SNIFF_MAX_INTERVAL, SNIFF_MIN_INTERVAL, SNIFF_ATTEMPT, SNIFF_TIMEOUT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_status for sniff mode request"
    
    # Wait for mode change event on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="mode_change", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS mode_change event on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="mode_change", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS mode_change event on peripheral"
    
    logger.info("Testing validate_sniff_mode_entry Completed")


def validate_sniff_mode_exit(dut_central, dut_peripheral, conn_handle, logger):
    """Test successful exit from sniff mode"""
    logger.info("Testing validate_sniff_mode_exit start")
    
    # Exit sniff mode
    dut_central.interfaces.btc.exit_sniff_mode(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_status for exit sniff mode"
    
    # Wait for mode change event on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="mode_change", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS mode_change event on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="mode_change", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS mode_change event on peripheral"
    
    logger.info("Testing validate_sniff_mode_exit Completed")

def validate_sniff_subrating_enable(dut_central, dut_peripheral, conn_handle, logger):
    """Test enabling sniff subrating on sniff connection"""
    logger.info("Testing validate_sniff_subrating_enable start")
    
    # First enter sniff mode
    validate_sniff_mode_entry(dut_central, dut_peripheral, conn_handle, logger)
    
    # Enable sniff subrating
    dut_central.interfaces.btc.sniff_subrating(conn_handle, MAX_LATENCY, MIN_REMOTE_TIMEOUT, MIN_LOCAL_TIMEOUT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for sniff subrating"
    
    # Wait for sniff subrating event
    assert dut_central.interfaces.btc.waitUntilEvent(variable="sniff_subrating", value=BT_SUCCESS_STATUS), "Failed to receive sniff_subrating event on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="sniff_subrating", value=BT_SUCCESS_STATUS), "Failed to receive sniff_subrating event on peripheral"
    
    logger.info("Testing validate_sniff_subrating_enable Completed")


def validate_sniff_subrating_disable(dut_central, dut_peripheral, conn_handle, logger):
    """Test disabling sniff subrating"""
    logger.info("Testing validate_sniff_subrating_disable start")
    
    # Disable sniff subrating by setting max latency to 0
    dut_central.interfaces.btc.sniff_subrating(conn_handle, 0x0000, MIN_REMOTE_TIMEOUT, MIN_LOCAL_TIMEOUT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for disable sniff subrating"
    
    logger.info("Testing validate_sniff_subrating_disable Completed")


def validate_sniff_subrating_with_invalid_parameters(dut_central, dut_peripheral, conn_handle, logger):
    """Test sniff subrating failure with invalid parameters"""
    logger.info("Testing validate_sniff_subrating_with_invalid_parameters start")
    
    # Try sniff subrating on non-sniff connection
    validate_sniff_mode_exit(dut_central, dut_peripheral, conn_handle, logger)
    
    dut_central.interfaces.btc.sniff_subrating(conn_handle, MAX_LATENCY, MIN_REMOTE_TIMEOUT, MIN_LOCAL_TIMEOUT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS for sniff subrating on active connection"
    
    logger.info("Testing validate_sniff_subrating_with_invalid_parameters Completed")


def test_sniff_and_sniff_subrating_procedure(duts, logger, debugs):
    """Main test function for sniff and sniff subrating procedures"""
    logger.info(f"test_sniff_and_sniff_subrating_procedure Test case started in debug {debugs}")
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

        # Get connection handle
        connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
        conn_handle = connected_device_info.conn_handle

        configure_default_link_policy_settings(dut_central, DEFAULT_LINK_POLICY, logger)
        configure_default_link_policy_settings(dut_peripheral, DEFAULT_LINK_POLICY, logger)

        configure_connection_link_policy_settings(dut_central, conn_handle, ENABLE_SNIFF_MODE_POLICY, logger)
        configure_connection_link_policy_settings(dut_peripheral, conn_handle, ENABLE_SNIFF_MODE_POLICY, logger)

        configure_supervision_timeout(dut_central, dut_peripheral, conn_handle, DEFAULT_SUPERVISION_TIMEOUT, logger)
        
        i = 0
        # NEGATIVE TEST CASES
        logger.info(f" ################# Test case number: {(i := i + 1)}")
        validate_sniff_mode_with_invalid_parameters(dut_central, conn_handle, logger)
        
        # POSITIVE TEST CASES
        logger.info(f" ################# Test case number: {(i := i + 1)}")
        validate_sniff_mode_entry(dut_central, dut_peripheral, conn_handle, logger)
 
        # logger.info(f" ################# Test case number: {(i := i + 1)}")
        validate_sniff_mode_exit(dut_central, dut_peripheral, conn_handle, logger)  
      
        # Sniff Sub rating
        logger.info(f" ################# Test case number: {(i := i + 1)}")
        validate_sniff_subrating_enable(dut_central, dut_peripheral, conn_handle, logger)

        logger.info(f" ################# Test case number: {(i := i + 1)}")
        validate_sniff_subrating_disable(dut_central, dut_peripheral, conn_handle, logger)
        
       
        logger.info(f" ################# Test case number: {(i := i + 1)}")
        validate_sniff_subrating_with_invalid_parameters(dut_central, dut_peripheral, conn_handle, logger)
        
        
        # # Step 5: Termination of the Virtual TCP Socket connection
        # terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        time.sleep(5)
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e
