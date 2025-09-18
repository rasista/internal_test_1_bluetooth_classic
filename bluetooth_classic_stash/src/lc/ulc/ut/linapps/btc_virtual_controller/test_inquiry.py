from tests_utils import *

def validate_standard_inquiry(dut_central, dut_peripheral, bd_addr, logger):
    """Validate standard inquiry mode (Inquiry Result without RSSI)"""
    logger.info("Testing standard inquiry mode")

     # Test inquiry_cancel on a fresh system (should result in COMMAND_DISALLOWED)
    dut_central.interfaces.btc.inquiry_cancel()
    # Command Disallowed error code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=COMMAND_DISALLOWED), "Failed to receive COMMAND_DISALLOWED response for cancel inquiry cmd complete event"


    #Set inquiry mode to 0 (Inquiry Result)
    dut_central.interfaces.btc.write_inquiry_mode(INQUIRY_RESULT)
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write inquiry mode with inquiry result"
    
    # Verify the inquiry mode was properly set
    dut_central.interfaces.btc.read_inquiry_mode()
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to read inquiry mode with inquiry result"
    
    # Start inquiry process on central device to discover nearby devices
    dut_central.interfaces.btc.inquiry(LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "failed to start inquiry with inquiry result"
    
    # Test starting inquiry when already in progress
    dut_central.interfaces.btc.inquiry(LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    # Command Disallowed error code (inquiry already in progress)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=COMMAND_DISALLOWED), "failed to start inquiry COMMAND_DISALLOWED as inquiry already in progress"
    
    # Configure inquiry scan parameters on peripheral device
    dut_peripheral.interfaces.btc.write_inquiry_scan_activity(INQUIRY_SCAN_ACTIVITY, INQUIRY_SCAN_WINDOW)
    # Success status code from firmware
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write_inquiry_scan_activity for inquiry result case"
    
    # Enable inquiry scan on peripheral device so it can be discovered
    dut_peripheral.interfaces.btc.write_scan_enable(SCAN_ENABLE)
    # Success status code from firmware
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to start inquiry scan for inquiry result case"
    
    # Wait until central device discovers the peripheral device
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_result"), "failed to receive Inquiry Result without RSSI"
    
    # Trigger inquiry complete indication event
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, INQUIRY_COMPLETE_INDICATION_FROM_LPW, bd_addr, DATA)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_complete", value = BT_SUCCESS_STATUS), "failed to complete inquiry for inquiry result case"


def validate_inquiry_with_rssi(dut_central, dut_peripheral, bd_addr, logger):
    """Validate inquiry mode with RSSI (Inquiry Result with RSSI)"""
    logger.info("Testing inquiry mode with RSSI")
    # Set inquiry mode to 1 (Inquiry Result with RSSI)
    dut_central.interfaces.btc.write_inquiry_mode(INQUIRY_RESULT_WITH_RSSI)
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write inquiry mode with inquiry result with RSSI"
    
    # Verify the inquiry mode was properly set
    dut_central.interfaces.btc.read_inquiry_mode()
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to read inquiry mode with inquiry result with RSSI"
    
    # Start inquiry process on central device to discover nearby devices
    dut_central.interfaces.btc.inquiry(LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "failed to start inquiry with inquiry result with RSSI"

    dut_central.interfaces.btc.inquiry(LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    # Command Disallowed error code (inquiry already in progress)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=COMMAND_DISALLOWED), "failed to start inquiry with inquiry result with RSSI as inquiry already in progress"
    
    # Configure inquiry scan parameters on peripheral device
    dut_peripheral.interfaces.btc.write_inquiry_scan_activity(INQUIRY_SCAN_ACTIVITY, INQUIRY_SCAN_WINDOW)
    # Success status code from firmware
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write_inquiry_scan_activity for inquiry result with RSSI case"
    
    # Enable inquiry scan on peripheral device so it can be discovered
    dut_peripheral.interfaces.btc.write_scan_enable(SCAN_ENABLE)
    # Success status code from firmware
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to start inquiry scan for inquiry result with RSSI case"
    
    # Wait until central device discovers the peripheral device
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_result_rssi"), "failed to receive Inquiry Result with RSSI"
    
    # Trigger inquiry complete indication event
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, INQUIRY_COMPLETE_INDICATION_FROM_LPW, bd_addr, DATA)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_complete", value=BT_SUCCESS_STATUS), "failed to complete inquiry for inquiry result with RSSI case"


def validate_extended_inquiry(dut_central, dut_peripheral, bd_addr, logger):
    """Validate extended inquiry mode (Inquiry Result with Extended Response)"""
    logger.info("Testing extended inquiry mode")
    #Set inquiry mode to 2 (Extended Inquiry Result)
    dut_central.interfaces.btc.write_inquiry_mode(INQUIRY_RESULT_EXTENDED)
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write inquiry mode with extended inquiry result"
    
    # Verify the inquiry mode was properly set
    dut_central.interfaces.btc.read_inquiry_mode()
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to read inquiry mode with extended inquiry result"
    
    # Start inquiry process on central device to discover nearby devices
    dut_central.interfaces.btc.inquiry(LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    # Success status code from firmware
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "failed to start inquiry  with extended inquiry result"
    
    dut_central.interfaces.btc.inquiry(LAP, INQUIRY_LENGTH, NUM_RESPONSES)
    # Command Disallowed error code (inquiry already in progress)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=COMMAND_DISALLOWED), "failed to start inquiry with extended inquiry result as inquiry already in progress"
    
    # Configure inquiry scan parameters on peripheral device
    dut_peripheral.interfaces.btc.write_inquiry_scan_activity(INQUIRY_SCAN_ACTIVITY, INQUIRY_SCAN_WINDOW)
    # Success status code from firmware
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to write_inquiry_scan_activity for extended inquiry result case"
    
    # Enable inquiry scan on peripheral device so it can be discovered
    dut_peripheral.interfaces.btc.write_scan_enable(SCAN_ENABLE)
    # Success status code from firmware
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "failed to start inquiry scan for extended inquiry result case"
    
    # Wait until central device discovers the peripheral device with extended info
    assert dut_central.interfaces.btc.waitUntilEvent(variable="extended_inquiry_result"), "failed to receive Extended Inquiry Result"
    
    # Trigger inquiry complete indication event
    dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, INQUIRY_COMPLETE_INDICATION_FROM_LPW, bd_addr, DATA)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="inquiry_complete", value=BT_SUCCESS_STATUS), "failed to complete inquiry for extended inquiry result case"

def test_start_inquiry(duts, logger, debugs):
    """Main test function that validates all inquiry modes including periodic inquiry"""
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
        
        # Step 2: Validate standard inquiry (without RSSI)
        validate_standard_inquiry(dut_central, dut_peripheral, bd_addr, logger)
        
        # Step 3: Validate inquiry with RSSI
        validate_inquiry_with_rssi(dut_central, dut_peripheral, bd_addr, logger)
        
        # Step 4: Validate extended inquiry
        validate_extended_inquiry(dut_central, dut_peripheral, bd_addr, logger)
        
        # Step 5: Clean up
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e
