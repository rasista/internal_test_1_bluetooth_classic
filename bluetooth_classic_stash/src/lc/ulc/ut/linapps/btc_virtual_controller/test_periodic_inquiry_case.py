from tests_utils import *
        
def validate_standard_periodic_inquiry(dut_central, dut_peripheral, bd_addr, logger):
    logger.info("Testing standard periodic_inquiry mode")
    
    # Test exit_periodic_inquiry_mode on a fresh system (should result in COMMAND_DISALLOWED)
    dut_central.interfaces.btc.exit_periodic_inquiry_mode()
    # Command will be disallowed since periodic_inquiry is not started
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=COMMAND_DISALLOWED), "failed to cancel periodic_inquiry COMMAND_DISALLOWED as periodic_inquiry not started"
    
    logger.info("Testing standard periodic_inquiry mode")
    # This mode is used for periodic_inquiry without RSSI
    dut_central.interfaces.btc.write_inquiry_mode(INQUIRY_RESULT)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write periodic_inquiry mode with inquiry result"
    
    # Read the set periodic inquiry mode to verify
    dut_central.interfaces.btc.read_inquiry_mode()
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to read periodic_inquiry mode with inquiry result"
    
    # Start periodic_inquiry on central device (repeating periodic_inquiry at intervals)
    dut_central.interfaces.btc.periodic_inquiry_mode(MAX_PERIOD_LENGTH, MIN_PERIOD_LENGTH, LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to start periodic_inquiry with inquiry result"
    
    # Test starting periodic_inquiry when already in progress
    dut_central.interfaces.btc.periodic_inquiry_mode(MAX_PERIOD_LENGTH, MIN_PERIOD_LENGTH, LAP, INQUIRY_LENGTH, NUM_RESPONSES)    # Command will be Disallowed with error code (periodic_inquiry already in progress)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=COMMAND_DISALLOWED), "failed to periodic_inquiry_mode COMMAND_DISALLOWED as periodic_inquiry already in progress"

    # Adding inquiry scan parameters on peripheral device
    dut_peripheral.interfaces.btc.write_inquiry_scan_activity(INQUIRY_SCAN_ACTIVITY, INQUIRY_SCAN_WINDOW)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write_inquiry_scan_activity for periodic_inquiry case"
    
    # Enable periodic_inquiry scan on peripheral device so it can be discovered
    dut_peripheral.interfaces.btc.write_scan_enable(SCAN_ENABLE)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to periodic_inquiry_mode scan for periodic_inquiry case"

    # last event triggered to get the results of periodic_inquiry
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_result"), "failed to receive Inquiry Result"

    # Trigger periodic_inquiry complete indication event
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, INQUIRY_COMPLETE_INDICATION_FROM_LPW, bd_addr, DATA)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_complete", value = BT_SUCCESS_STATUS), "failed to complete periodic_inquiry for periodic_inquiry case"

def validate_periodic_inquiry_with_rssi(dut_central, dut_peripheral, bd_addr, logger):
    logger.info("Testing periodic_inquiry mode with rssi")

    # This mode is used for periodic_inquiry with RSSI
    dut_central.interfaces.btc.write_inquiry_mode(INQUIRY_RESULT_WITH_RSSI)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write periodic_inquiry mode with inquiry result with RSSI"

    # Read the set periodic inquiry mode to verify
    dut_central.interfaces.btc.read_inquiry_mode()
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to read periodic_inquiry mode with inquiry result with RSSI"

    # Adding inquiry scan parameters on peripheral device
    dut_peripheral.interfaces.btc.write_inquiry_scan_activity(INQUIRY_SCAN_ACTIVITY, INQUIRY_SCAN_WINDOW)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write_inquiry_scan_activity for periodic_inquiry with RSSI case"
    
    # Enable periodic_inquiry scan on peripheral device so it can be discovered
    dut_peripheral.interfaces.btc.write_scan_enable(SCAN_ENABLE)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to start periodic_inquiry scan for periodic_inquiry with RSSI case"
    
    # Verifying the results of periodic_inquiry with RSSI event
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_result_rssi"), "failed to receive Inquiry Result with RSSI"
    
    # Trigger periodic_inquiry complete indication event
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, INQUIRY_COMPLETE_INDICATION_FROM_LPW, bd_addr, DATA)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_complete", value = BT_SUCCESS_STATUS), "failed to complete periodic_inquiry for periodic_inquiry with RSSI case"
    
def validate_extended_periodic_inquiry(dut_central, dut_peripheral, bd_addr, logger):
    logger.info("Testing extended periodic_inquiry mode")
    
    # This mode is used for periodic_inquiry with extended inquiry results
    dut_central.interfaces.btc.write_inquiry_mode(INQUIRY_RESULT_EXTENDED)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write periodic_inquiry mode with extended inquiry result"
    
    # Read the set periodic inquiry mode to verify
    dut_central.interfaces.btc.read_inquiry_mode()
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to read periodic_inquiry mode with extended inquiry result"

    # Adding inquiry scan parameters on peripheral device
    dut_peripheral.interfaces.btc.write_inquiry_scan_activity(INQUIRY_SCAN_ACTIVITY, INQUIRY_SCAN_WINDOW)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write_inquiry_scan_activity for extended periodic_inquiry case"
    
    # Enable periodic_inquiry scan on peripheral device so it can be discovered
    dut_peripheral.interfaces.btc.write_scan_enable(SCAN_ENABLE)
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to start periodic_inquiry scan for extended periodic_inquiry case"
    
    # Verifying the results of periodic_inquiry with extended format
    assert dut_central.interfaces.btc.waitUntilEvent(variable="extended_inquiry_result"), "failed to receive Inquiry Result with Extended Format"

    # Trigger periodic_inquiry complete indication event
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, INQUIRY_COMPLETE_INDICATION_FROM_LPW, bd_addr, DATA)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_complete", value = BT_SUCCESS_STATUS), "failed to complete periodic_inquiry for extended periodic_inquiry case"

    # Stop the periodic_inquiry process on central device
    dut_central.interfaces.btc.exit_periodic_inquiry_mode()
    
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to stop periodic_inquiry with extended inquiry result"

def test_periodic_inquiry_mode(duts, logger, debugs):
    """Main test function that validates all periodic_inquiry modes including periodic_inquiry"""
    logger.info(f"Test case started {debugs}" )
    if not debugs:
        start_firmware(2)
    try:
        [dut_central, dut_peripheral] = duts
        setFileNames(duts)        
        
        # Define the Bluetooth address for testing
        bd_addr = get_device_bd_addr(dut_peripheral)
        
        # Step 1: Setup initial connection
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        # Step 2: Validate standard periodic_inquiry
        validate_standard_periodic_inquiry(dut_central, dut_peripheral, bd_addr, logger)
        
        # Step 3: Validate periodic_inquiry with RSSI
        validate_periodic_inquiry_with_rssi(dut_central, dut_peripheral, bd_addr, logger)
        
        # Step 4: Validate extended periodic_inquiry
        validate_extended_periodic_inquiry(dut_central, dut_peripheral, bd_addr, logger)
        
        # Step 5: Clean up
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e