from tests_utils import *


#define Vendor Powercontrol event SUB_EVENT Macros
VENDOR_POWER_CONTROL_RES_RX                   =  1
VENDOR_POWER_CONTROL_RES_RX_MIN_POWER_REACHED =  2
VENDOR_POWER_CONTROL_RES_RX_MAX_POWER_REACHED =  3
VENDOR_POWER_CONTROL_MIN_POWER_RX             =  4
VENDOR_POWER_CONTROL_MAX_POWER_RX             =  5
VENDOR_POWER_CONTROL_POWER_INDEX_UPDATED      =  6




# -30 -70 -50 -30 -70   // Average value -50
sample_rssi_in_golden_range_signal = [25,
                     0xe2, 0xba, 0xce, 0xe2, 0xba, 0xe2, 0xba, 0xce, 0xe2, 0xba,  
                     0xe2, 0xba, 0xce, 0xe2, 0xba, 0xe2, 0xba, 0xce, 0xe2, 0xba,  
                     0xe2, 0xba, 0xce, 0xe2, 0xba]  

# -10 -20 -30 -10 -30   // Average value -20
sample_rssi_strong_signal = [25, 
                     0xf6, 0xec, 0xe2, 0xf6, 0xe2 , 0xf6, 0xec, 0xe2, 0xf6, 0xe2,  
                     0xf6, 0xec, 0xe2, 0xf6, 0xe2 , 0xf6, 0xec, 0xe2, 0xf6, 0xe2,  
                     0xf6, 0xec, 0xe2, 0xf6, 0xe2]  

# -90 -70 -90 -80 -70   // Average value -80
sample_rssi_weak_signal = [25, 
                     0xa6, 0xba, 0xa6, 0xb0, 0xba, 0xa6, 0xba, 0xa6, 0xb0, 0xba, 
                     0xa6, 0xba, 0xa6, 0xb0, 0xba, 0xa6, 0xba, 0xa6, 0xb0, 0xba, 
                     0xa6, 0xba, 0xa6, 0xb0, 0xba] 


feature_bitmap_enhanced_power_control_supported = [0xbf, 0xfe, 0x0d, 0xfe, 0x9b, 0xff, 0x59, 0x87]
feature_bitmap_enhanced_power_control_not_supported = [0xbf, 0xfe, 0x0d, 0xfe, 0x9b, 0xff, 0x59, 0x83]

def validate_supported_feature_request(dut_central, conn_handle, logger):
    logger.info("Testing validate_supported_feature_request start")
    dut_central.interfaces.btc.read_remote_supported_features(conn_handle)
    assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value = BT_SUCCESS_STATUS), "failed to recevive BT_SUCCESS_STATUS cmd status for read supported features"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="read_remote_supported_features_complete", value = BT_SUCCESS_STATUS), "failed to receive read_remote_supported_features cmd complete event"
    logger.info("Testing validate_supported_feature_request Completed")

def send_rssi_samples(dut, bd_addr, rssi_samples, num_iterations, expected_power_indices, logger):
    """Helper function to send RSSI samples and verify power index changes"""
    for i in range(num_iterations):
        # Send four batches of 25 samples each
        for batch in range(4):
            dut.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, POWER_CONTROL_NOTIFICATION_FROM_LPW, bd_addr, rssi_samples)
            assert dut.interfaces.btc.waitUntilEvent(variable="cmd_complete"), "failed to execute vendor command"
            logger.info(f"{(batch + 1)*25} Entries RSSI sent to peripheral")
            #printing i and batch values for debugging
            logger.info(f"i value: {i}, batch value: {batch} , len(expected_power_indices): {len(expected_power_indices)}")

        if expected_power_indices and i < len(expected_power_indices):
            logger.info(f"Transmit power index will be at {expected_power_indices[i]} by now")

def trigger_power_control_test_legacy(dut_central, responder_bd_address, dut_lpw_rssi_samples_feeder, logger):
    """Validate Power control event with legacy power control procedure"""
    logger.info("Testing power_control_test_legacy with initial power index 10 at central scenario")

    # Test decreasing power (strong signal)
    expected_power_indices = [7, 4, 1]  # Expected power index progression
    send_rssi_samples(dut_lpw_rssi_samples_feeder, responder_bd_address, 
                     sample_rssi_strong_signal, 4, expected_power_indices, logger)

    # Verify minimum power reached
    logger.info("Transmit power index minimum reached, LMP_MIN_POWER_INDICATION notification to be received")
    assert dut_lpw_rssi_samples_feeder.interfaces.btc.waitUntilEvent(variable="vendor_event"), \
           "failed to receive vendor event"
    if dut_lpw_rssi_samples_feeder.interfaces.btc.hciEventVars.vendor_power_control_notification != VENDOR_POWER_CONTROL_MIN_POWER_RX:
        assert False, "Failed to receive proper vendor legacy power control response notification"

    # Test increasing power (weak signal)
    logger.info("Weak RSSI sample sent to peripheral LMP_INCR CASE START NOW")
    expected_power_indices = [4, 7, 10]  # Expected power index progression
    send_rssi_samples(dut_lpw_rssi_samples_feeder, responder_bd_address, 
                     sample_rssi_weak_signal, 4, expected_power_indices, logger)

    # Verify maximum power reached
    logger.info("Transmit power index maximum reached, LMP_MAX_POWER_INDICATION notification to be received")
    assert dut_lpw_rssi_samples_feeder.interfaces.btc.waitUntilEvent(variable="vendor_event"), \
           "failed to receive vendor event"
    if dut_lpw_rssi_samples_feeder.interfaces.btc.hciEventVars.vendor_power_control_notification != VENDOR_POWER_CONTROL_MAX_POWER_RX:
        assert False, "Failed to receive proper vendor legacy power control response notification"

def verify_power_control_notification(dut, expected_notification, logger):
    """Helper function to verify power control notifications"""
    assert dut.interfaces.btc.waitUntilEvent(variable="vendor_event"), \
           "failed to receive vendor event"
    if dut.interfaces.btc.hciEventVars.vendor_power_control_notification != expected_notification:
        assert False, "Failed to receive proper vendor legacy power control response notification"

def set_local_lmp_features(dut, feature_bitmap, logger):
    """Helper function to set local LMP features and wait for completion"""
    dut.interfaces.btc.set_local_lmp_features(feature_bitmap)
    assert dut.interfaces.btc.waitUntilEvent(variable="cmd_complete"), \
           "failed to execute vendor command setLocalLmpFeatures"


def test_power_control_procedure(duts, logger, debugs):
    logger.info(f"test_power_control_procedure Test case in debug {debugs}")
    if not debugs:
        start_firmware(2)
    
    try:
        [dut_central, dut_peripheral] = duts
        setFileNames(duts)
        
        # Set LMP features for both devices
        set_local_lmp_features(dut_central, feature_bitmap_enhanced_power_control_not_supported, logger)
        set_local_lmp_features(dut_peripheral, feature_bitmap_enhanced_power_control_not_supported, logger)

        # Get the Bluetooth addresses of central and peripheral for testing
        central_bd_addr = get_device_bd_addr(dut_central)
        peripheral_bd_addr = get_device_bd_addr(dut_peripheral)
        
        # Step 1: Virtual TCP socket connection Validation
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        # Step 2: BT Create connection Validation
        validate_bt_create_connection_procedure (dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
        
        # Step 3: Suported Features Request Validation, needed for power control validation 
        validate_supported_feature_request(dut_central, 1, logger)

        # Peripheral is power control request initiator and Central is responder
        # Step 4: Power control Request Validation  (responder, responder_bd_addr, initiator, logger)
        trigger_power_control_test_legacy(dut_central, central_bd_addr, dut_peripheral, logger)

        # Central is power control request initiator and Peripheral is responder
        # Step 5: Power control Request Validation  (responder, responder_bd_addr, initiator, logger)
        trigger_power_control_test_legacy(dut_peripheral, peripheral_bd_addr, dut_central, logger)

        # Step 6: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e
