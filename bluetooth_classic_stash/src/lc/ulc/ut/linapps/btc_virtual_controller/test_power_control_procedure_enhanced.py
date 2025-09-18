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

def verify_power_control_notification(dut, expected_notification, logger):
    """Helper function to verify power control notifications"""
    assert dut.interfaces.btc.waitUntilEvent(variable="vendor_event"), \
           "failed to receive vendor event"
    if dut.interfaces.btc.hciEventVars.vendor_power_control_notification != expected_notification:
        assert False, "Failed to receive proper vendor enhanced power control response notification"

def trigger_power_control_test_enhanced(dut_power_control_responder, responder_bd_address, dut_lpw_rssi_samples_feeder, logger):
    """Validate Enhanced Power control event with different RSSI levels"""
    logger.info("Testing power_control_test_enhanced with initial power index 10 at central scenario")

    # Test decreasing power with strong signal (10 -> 7 -> 4 -> 1)
    expected_power_indices = [7, 4, 1]  # Expected power index progression
    for i, expected_index in enumerate(expected_power_indices):
        # Send one batch of samples (100 entries)
        send_rssi_samples(dut_lpw_rssi_samples_feeder, responder_bd_address, 
                         sample_rssi_strong_signal, 1, [expected_index], logger)
        
        # Verify appropriate notification
        expected_notification = (VENDOR_POWER_CONTROL_RES_RX_MIN_POWER_REACHED 
                               if expected_index == 1 
                               else VENDOR_POWER_CONTROL_RES_RX)
        verify_power_control_notification(dut_lpw_rssi_samples_feeder, expected_notification, logger)
        logger.info(f"Power index decreased to {expected_index}")
    logger.info("MAX to min COMPLETED, sending weak signal to increase remote output power")

    # Test increasing power with weak signal (1 -> 10)
    send_rssi_samples(dut_lpw_rssi_samples_feeder, responder_bd_address,
                     sample_rssi_weak_signal, 1, [10], logger)
    verify_power_control_notification(dut_lpw_rssi_samples_feeder, 
                                   VENDOR_POWER_CONTROL_RES_RX_MAX_POWER_REACHED, 
                                   logger)
    logger.info("MAX to min and to MAX cycle COMPLETED, Enhanced power test passed")



def test_power_control_procedure(duts, logger, debugs):
    logger.info(f"test_power_control_procedure Test case in debug {debugs}")
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
        
        # Step 3: Suported Features Request Validation, needed for power control validation 
        validate_supported_feature_request(dut_central, 1, logger)

        # Peripheral is power control request initiator and Central is responder
        # Step 4: Power control Request Validation  (responder, responder_bd_addr, initiator, logger)
        trigger_power_control_test_enhanced(dut_central, central_bd_addr, dut_peripheral, logger)

        # Central is power control request initiator and Peripheral is responder
        # Step 5: Power control Request Validation  (responder, responder_bd_addr, initiator, logger)
        trigger_power_control_test_enhanced(dut_peripheral, peripheral_bd_addr, dut_central, logger)

        # Step 6: Termination of Virtual TCP socket connection Validation
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e
