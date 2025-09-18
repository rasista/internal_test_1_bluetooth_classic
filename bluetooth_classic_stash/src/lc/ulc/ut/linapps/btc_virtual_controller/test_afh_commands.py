from tests_utils import *
from btc_error_codes import *

# Test functions for Bluetooth AFH (Adaptive Frequency Hopping) Commands:
# - validate_set_afh_host_channel_classification_success: Tests successful set_afh_host_channel_classification with valid channel map (79 active channels)
# - validate_set_afh_host_channel_classification_insufficient_channels: Tests set_afh_host_channel_classification with insufficient active channels (<20 channels)
# - validate_write_afh_channel_assessment_mode_disabled: Tests write_afh_channel_assessment_mode with disabled mode
# - validate_afh_commands_in_sequence: Tests AFH commands in sequence to verify they work together (enable mode, read mode, set classification)
# - validate_afh_commands_with_connection_state: Tests AFH commands in the context of an established Bluetooth connection
# - test_afh_commands: Main test function that validates all AFH command scenarios including positive and negative test cases

# AFH Channel Assessment Mode values
CHANNEL_ASSESSMENT_DISABLED = 0x00
CHANNEL_ASSESSMENT_ENABLED = 0x01

def validate_set_afh_host_channel_classification_success(dut_central, logger):
    """Test successful set_afh_host_channel_classification with valid parameters"""
    logger.info("Testing set_afh_host_channel_classification with valid parameters")
    
    # Create a valid channel map with channels 0-78 active (79 channels total)
    valid_channel_map = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F]
    
    dut_central.interfaces.btc.set_afh_host_channel_classification(valid_channel_map)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to set AFH host channel classification with valid parameters"

def validate_set_afh_host_channel_classification_insufficient_channels(dut_central, logger):
    """Test set_afh_host_channel_classification with insufficient active channels"""
    logger.info("Testing set_afh_host_channel_classification with insufficient channels")
    
    # Create a channel map with less than 20 active channels
    insufficient_channel_map = [0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
    
    dut_central.interfaces.btc.set_afh_host_channel_classification(insufficient_channel_map)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=INVALID_HCI_COMMAND_PARAMETERS), "Failed to receive INVALID_HCI_COMMAND_PARAMETERS for insufficient channels"

def validate_write_afh_channel_assessment_mode_disabled(dut_central, logger):
    """Test write_afh_channel_assessment_mode with disabled mode"""
    logger.info("Testing write_afh_channel_assessment_mode with disabled mode")
    
    dut_central.interfaces.btc.write_afh_channel_assessment_mode(CHANNEL_ASSESSMENT_DISABLED)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to write AFH channel assessment mode as disabled"

def validate_afh_commands_in_sequence(dut_central, logger):
    """Test AFH commands in sequence to verify they work together"""
    logger.info("Testing AFH commands in sequence")
    
    # Step 1: Set channel assessment mode to enabled
    dut_central.interfaces.btc.write_afh_channel_assessment_mode(CHANNEL_ASSESSMENT_ENABLED)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to write AFH channel assessment mode as enabled"
    
    # Step 2: Read the mode to verify it was set
    dut_central.interfaces.btc.read_afh_channel_assessment_mode()
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to read AFH channel assessment mode"
    
    # Step 3: Set host channel classification
    valid_channel_map = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F]  # 79 channels active
    dut_central.interfaces.btc.set_afh_host_channel_classification(valid_channel_map)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to set AFH host channel classification"

def validate_afh_commands_with_connection_state(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger):
    """Test AFH commands in the context of an established connection"""
    logger.info("Testing AFH commands with established connection")
    
    # Establish a connection first
    validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
    
    # Test AFH commands with active connection
    dut_central.interfaces.btc.write_afh_channel_assessment_mode(CHANNEL_ASSESSMENT_ENABLED)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to write AFH channel assessment mode with active connection"
    
    dut_central.interfaces.btc.read_afh_channel_assessment_mode()
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to read AFH channel assessment mode with active connection"    
    
    valid_channel_map = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F]  # 79 channels active
    dut_central.interfaces.btc.set_afh_host_channel_classification(valid_channel_map)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to set AFH host channel classification with active connection"

def test_afh_commands(duts, logger, debugs):
    """Main test function that validates all AFH command scenarios"""
    logger.info(f"test_afh_commands Test case started in debug {debugs}")
    if not debugs:
        start_firmware(2)
    
    try:
        [dut_central, dut_peripheral] = duts
        setFileNames(duts)
        
        # Get device addresses
        central_bd_addr = get_device_bd_addr(dut_central)
        peripheral_bd_addr = get_device_bd_addr(dut_peripheral)
        
        # Step 1: Setup initial connection
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        # Step 2: Test set_afh_host_channel_classification scenarios
        validate_set_afh_host_channel_classification_success(dut_central, logger)

        validate_set_afh_host_channel_classification_insufficient_channels(dut_central, logger)

        # Step 3: Test write_afh_channel_assessment_mode disabled
        validate_write_afh_channel_assessment_mode_disabled(dut_central, logger)
        
        # Step 4: Test integration scenarios
        validate_afh_commands_in_sequence(dut_central, logger)
        
        # Step 5: Test connection-based scenarios
        validate_afh_commands_with_connection_state(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
        
        # Step 6: Clean up
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        logger.info("Completed comprehensive AFH commands tests")
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e