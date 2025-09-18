from tests_utils import *

# Test functions for Bluetooth Simple Secure Pairing (SSP) Numeric Comparison Authentication:
# - validate_authentication_with_numeric_comparison_positive: Tests successful SSP authentication using numeric comparison with user confirmation
# - validate_authentication_with_numeric_comparison_negative_from_initiator: Tests SSP authentication failure when initiator rejects numeric comparison
# - validate_authentication_with_numeric_comparison_negative_from_responder: Tests SSP authentication failure when responder rejects numeric comparison
# - Test_ssp_numaric_comarision_authentication_procedure: Main test function that executes all SSP numeric comparison authentication scenarios

# SSP Constants
SSP_NUMERIC_COMPARISON = 0
SSP_PASSKEY_ENTRY = 1  
SSP_JUST_WORKS = 2
SSP_OUT_OF_BAND = 3

# Simple Secure Pairing Mode
ENABLE_SIMPLE_SECURE_PAIRING = 0x01
DISABLE_SIMPLE_SECURE_PAIRING = 0x00

# IO Capabilities
IO_CAPABILITY_DISPLAY_ONLY = 0
IO_CAPABILITY_DISPLAY_YES_NO = 1
IO_CAPABILITY_KEYBOARD_ONLY = 2
IO_CAPABILITY_NO_INPUT_NO_OUTPUT = 3

# Authentication Requirements
MITM_PROTECTION_REQUIRED = 1
MITM_PROTECTION_NOT_REQUIRED = 0

# OOB Authentication Data
OOB_AUTHENTICATION_DATA_NOT_PRESENT = 0
OOB_AUTHENTICATION_DATA_PRESENT = 1

def enable_secure_simple_pairing(dut_device, logger):
    logger.info("Enabling Simple Secure Pairing")
    dut_device.interfaces.btc.write_simple_pairing_mode(ENABLE_SIMPLE_SECURE_PAIRING)
    result = dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to enable SSP - cmd_complete event not received or returned failure"
    logger.info("Simple Secure Pairing enabled successfully")


def delete_stored_link_keys_ssp(dut_device, bd_addr, logger):
    logger.info(f"Deleting stored link keys for SSP device {bd_addr}")
    dut_device.interfaces.btc.delete_stored_link_key(bd_addr, HCI_DEL_STORED_LINK_KEY_FOR_ALL_BD_ADDR)
    result = dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, f"Failed to delete stored link key for device {bd_addr} - cmd_complete event not received or returned failure"
    logger.info(f"Successfully deleted stored link keys for device {bd_addr}")

def validate_authentication_with_numeric_comparison_positive(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with numeric comparison (positive) start")

    delete_stored_link_keys_ssp(dut_central, peripheral_bd_addr, logger)
    delete_stored_link_keys_ssp(dut_peripheral, central_bd_addr, logger)
    

    # Request authentication
    logger.info("Requesting authentication from central device")
    dut_central.interfaces.btc.authentication_requested(conn_handle)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful cmd_status for authentication request"

    # Handle link key request first (as per spec)
    logger.info("Waiting for link_key_request on central")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr)
    assert result, f"Failed to receive link_key_request for peripheral {peripheral_bd_addr}"
    
    dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for link_key_request_negative_reply"

    # Handle IO capability exchange on central
    logger.info("Handling IO capability exchange on central")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=peripheral_bd_addr)
    assert result, f"Failed to receive io_capability_request for peripheral {peripheral_bd_addr} on central"
    
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_DISPLAY_YES_NO, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central"

    # Handle IO capability exchange on peripheral
    logger.info("Handling IO capability exchange on peripheral")
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr)
    assert result, f"Failed to receive io_capability_response for central {central_bd_addr} on peripheral"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr)
    assert result, f"Failed to receive io_capability_request for central {central_bd_addr} on peripheral"
    
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_DISPLAY_YES_NO, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral"

    # Wait for IO capability response from both devices
    logger.info("Waiting for IO capability response on central")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr)
    assert result, f"Failed to receive io_capability_response for peripheral {peripheral_bd_addr} on central"

    # Wait for numeric comparison request and confirm on both devices
    logger.info("Waiting for user confirmation requests on both devices")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="user_confirmation_request", value=peripheral_bd_addr)
    assert result, f"Failed to receive user_confirmation_request for peripheral {peripheral_bd_addr} on central"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_confirmation_request", value=central_bd_addr)
    assert result, f"Failed to receive user_confirmation_request for central {central_bd_addr} on peripheral"
    
    logger.info("Confirming user confirmation requests on both devices")
    dut_central.interfaces.btc.user_confirmation_request_reply(peripheral_bd_addr)
    dut_peripheral.interfaces.btc.user_confirmation_request_reply(central_bd_addr)
    
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_confirmation_request_reply on central"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_confirmation_request_reply on peripheral"

    # Wait for simple pairing complete on both devices
    logger.info("Waiting for simple pairing complete on both devices")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful simple_pairing_complete on central"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful simple_pairing_complete on peripheral"

    # Wait for link key notification and authentication complete
    logger.info("Waiting for link key notifications and authentication complete")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=peripheral_bd_addr)
    assert result, f"Failed to receive link_key_notification for peripheral {peripheral_bd_addr} on central"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=central_bd_addr)
    assert result, f"Failed to receive link_key_notification for central {central_bd_addr} on peripheral"
    
    result = dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful authentication_complete on central"

    logger.info("Testing SSP authentication with numeric comparison (positive) completed successfully")

def validate_authentication_with_numeric_comparison_negative_from_initiator(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with numeric comparison (negative from initiator) start")

    delete_stored_link_keys_ssp(dut_central, peripheral_bd_addr, logger)
    delete_stored_link_keys_ssp(dut_peripheral, central_bd_addr, logger)


    dut_central.interfaces.btc.authentication_requested(conn_handle)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_status for authentication request"

    # Handle link key request first
    assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request"
    dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for link_key_request_negative_reply"

    # Handle IO capability exchange on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=peripheral_bd_addr), "Failed to receive io_capability_request on central"
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_DISPLAY_YES_NO, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr), "Failed to receive io_capability_request on peripheral"
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_DISPLAY_YES_NO, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral"

    # Wait for IO capability response
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr), "Failed to receive io_capability_response on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr), "Failed to receive io_capability_response on peripheral"

    # Wait for numeric comparison request and reject on central
   
    assert dut_central.interfaces.btc.waitUntilEvent(variable="user_confirmation_request", value=peripheral_bd_addr), "Failed to receive user_confirmation_request on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_confirmation_request", value=central_bd_addr), "Failed to receive user_confirmation_request on peripheral"
    dut_central.interfaces.btc.user_confirmation_request_negative_reply(peripheral_bd_addr)
    dut_peripheral.interfaces.btc.user_confirmation_request_reply(central_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_confirmation_request_reply on central"
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_confirmation_request_reply on peripheral"


    # Wait for simple pairing complete with failure on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete with failure on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete with failure on peripheral"

    assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive authentication_complete with failure"

    logger.info("Testing SSP authentication with numeric comparison (negative from initiator) completed")
    
    
def validate_authentication_with_numeric_comparison_negative_from_responder(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with numeric comparison (negative from responder) start")

    delete_stored_link_keys_ssp(dut_central, peripheral_bd_addr, logger)
    delete_stored_link_keys_ssp(dut_peripheral, central_bd_addr, logger)


    dut_central.interfaces.btc.authentication_requested(conn_handle)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_status for authentication request"

    # Handle link key request first
    assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request"
    dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for link_key_request_negative_reply"

    # Handle IO capability exchange on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=peripheral_bd_addr), "Failed to receive io_capability_request on central"
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_DISPLAY_YES_NO, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr), "Failed to receive io_capability_request on peripheral"
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_DISPLAY_YES_NO, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral"

    # Wait for IO capability response
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr), "Failed to receive io_capability_response on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr), "Failed to receive io_capability_response on peripheral"

    # Wait for numeric comparison request and reject on central
   
    assert dut_central.interfaces.btc.waitUntilEvent(variable="user_confirmation_request", value=peripheral_bd_addr), "Failed to receive user_confirmation_request on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_confirmation_request", value=central_bd_addr), "Failed to receive user_confirmation_request on peripheral"
    dut_central.interfaces.btc.user_confirmation_request_reply(peripheral_bd_addr)
    dut_peripheral.interfaces.btc.user_confirmation_request_negative_reply(central_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_confirmation_request_reply on central"
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_confirmation_request_reply on peripheral"


    # Wait for simple pairing complete with failure on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete with failure on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete with failure on peripheral"

    assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive authentication_complete with failure"

    logger.info("Testing SSP authentication with numeric comparison (negative from responder) completed")



def test_ssp_numaric_comparision_authentication_procedure(duts, logger, debugs):
    logger.info(f"Test_ssp_numaric_comparision_authentication_procedure Test case started in debug {debugs}")
    
    # Validate input parameters
    assert duts and len(duts) == 2, f"Invalid duts parameter: expected 2 devices, got {len(duts) if duts else 0}"
    assert logger, "Logger parameter is required"
    
    if not debugs:
        start_firmware(2)

    try:
        [dut_central, dut_peripheral] = duts
        assert dut_central, "Central device is None"
        assert dut_peripheral, "Peripheral device is None"
        
        setFileNames(duts)

        # Get the Bluetooth addresses of central and peripheral for testing
        logger.info("Getting Bluetooth addresses for numeric comparison test devices")
        central_bd_addr = get_device_bd_addr(dut_central)
        peripheral_bd_addr = get_device_bd_addr(dut_peripheral)
        
        logger.info(f"Central BD Address: {central_bd_addr}")
        logger.info(f"Peripheral BD Address: {peripheral_bd_addr}")

        # Step 1: Virtual TCP socket connection Validation
        logger.info("Step 1: Creating virtual TCP socket connection for numeric comparison tests")
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)

        # Step 2: BT Create connection Validation
        logger.info("Step 2: Validating BT connection procedure for numeric comparison tests")
        validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)

        # Step 3: Enable Simple Secure Pairing
        logger.info("Step 3: Enabling Simple Secure Pairing for numeric comparison tests")
        enable_secure_simple_pairing(dut_central, logger)
        enable_secure_simple_pairing(dut_peripheral, logger)

        # Get connection handle and validate it
        logger.info("Getting connection handle for numeric comparison tests")
        connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
        assert connected_device_info, "Failed to get ACL connection info from peripheral device"
        
        conn_handle = connected_device_info.conn_handle
        assert conn_handle is not None, "Connection handle is None"
        assert isinstance(conn_handle, int), f"Invalid connection handle type: {type(conn_handle)}"
        
        logger.info(f"Connection handle: {conn_handle}")

        test_count = 0

        # NUMERIC COMPARISON TEST CASES
        logger.info(f" ################# SSP Numeric Comparison Test case number: {(test_count := test_count + 1)} - Numeric Comparison Positive")
        validate_authentication_with_numeric_comparison_positive(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)

        logger.info(f" ################# SSP Numeric Comparison Test case number: {(test_count := test_count + 1)} - Numeric Comparison Negative from Initiator")
        validate_authentication_with_numeric_comparison_negative_from_initiator(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
        
        logger.info(f" ################# SSP Numeric Comparison Test case number: {(test_count := test_count + 1)} - Numeric Comparison Negative from Responder")
        validate_authentication_with_numeric_comparison_negative_from_responder(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
        
        # Step 5: Termination of the Virtual TCP Socket connection
        logger.info("Step 5: Terminating virtual TCP socket connection for numeric comparison tests")
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
        
        logger.info(f"All SSP Numeric Comparison test cases ({test_count}) completed successfully!")

    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e