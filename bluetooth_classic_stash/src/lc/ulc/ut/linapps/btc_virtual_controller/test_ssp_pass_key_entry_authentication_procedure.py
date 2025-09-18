from tests_utils import *


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

PASSKEY_ENTRY_START = 0
PASSKEY_ENTRY_COMPLETED = 4

SECURE_CONNECTIONS_SUPPORTED = 1


def enable_secure_simple_pairing(dut_device, logger):
    logger.info("Enabling Simple Secure Pairing")
    dut_device.interfaces.btc.write_simple_pairing_mode(ENABLE_SIMPLE_SECURE_PAIRING)
    result = dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to enable SSP - cmd_complete event not received or returned failure"
    logger.info("Simple Secure Pairing enabled successfully")

def enable_secure_connection(dut_device, secure_connection_support, logger):
    logger.info(f"Configuring Secure Connections support to {secure_connection_support}")
    dut_device.interfaces.btc.write_secure_connections_host_support(secure_connection_support)
    result = dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, f"Failed to configure Secure Connections support to {secure_connection_support} - cmd_complete event not received or returned failure"
    logger.info(f"Secure Connections support configured successfully to {secure_connection_support}")

def delete_stored_link_keys_ssp(dut_device, bd_addr, logger):
    logger.info(f"Deleting stored link keys for SSP device {bd_addr}")
    dut_device.interfaces.btc.delete_stored_link_key(bd_addr, HCI_DEL_STORED_LINK_KEY_FOR_ALL_BD_ADDR)
    result = dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, f"Failed to delete stored link key for device {bd_addr} - cmd_complete event not received or returned failure"
    logger.info(f"Successfully deleted stored link keys for device {bd_addr}")





def validate_authentication_with_passkey_entry_matching(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with passkey entry (matching) start")

    delete_stored_link_keys_ssp(dut_central, peripheral_bd_addr, logger)
    delete_stored_link_keys_ssp(dut_peripheral, central_bd_addr, logger)
   
    # enable_secure_connection(dut_central, SECURE_CONNECTIONS_SUPPORTED, logger)
    # enable_secure_connection(dut_peripheral, SECURE_CONNECTIONS_SUPPORTED, logger)

    logger.info("Requesting authentication for passkey entry test")
    dut_central.interfaces.btc.authentication_requested(conn_handle)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful cmd_status for authentication request in passkey entry test"

    # Handle link key request first
    logger.info("Handling link key request for passkey entry test")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr)
    assert result, f"Failed to receive link_key_request for peripheral {peripheral_bd_addr} in passkey entry test"
    
    dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for link_key_request_negative_reply in passkey entry test"

    # Handle IO capability exchange on both devices
    logger.info("Handling IO capability exchange for passkey entry test")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=peripheral_bd_addr)
    assert result, f"Failed to receive io_capability_request for peripheral {peripheral_bd_addr} on central in passkey entry test"
    
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_DISPLAY_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central in passkey entry test"

    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr)
    assert result, f"Failed to receive io_capability_request for central {central_bd_addr} on peripheral in passkey entry test"
    
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_KEYBOARD_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral in passkey entry test"

    # Wait for IO capability response
    logger.info("Waiting for IO capability responses in passkey entry test")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr)
    assert result, f"Failed to receive io_capability_response for peripheral {peripheral_bd_addr} on central in passkey entry test"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr)
    assert result, f"Failed to receive io_capability_response for central {central_bd_addr} on peripheral in passkey entry test"

    # Wait for passkey notification and entry
    logger.info("Waiting for passkey notification and request")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="user_passkey_notification", value=peripheral_bd_addr)
    assert result, f"Failed to receive user_passkey_notification for peripheral {peripheral_bd_addr} on central"

    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_passkey_request", value=central_bd_addr)
    assert result, f"Failed to receive user_passkey_request for central {central_bd_addr} on peripheral"
    
    logger.info("Sending keypress notifications")
    dut_peripheral.interfaces.btc.send_keypress_notification(central_bd_addr, PASSKEY_ENTRY_START)  
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for send_keypress_notification (started)"
    
    dut_peripheral.interfaces.btc.send_keypress_notification(central_bd_addr, PASSKEY_ENTRY_COMPLETED)  
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for send_keypress_notification (completed)"
    
    # Get passkey and validate it
    logger.info("Getting passkey from central device")
    connected_device_info = dut_central.interfaces.btc.get_acl_connection_info()
    assert connected_device_info, "Failed to get ACL connection info from central device"
    
    pass_key = connected_device_info.passkey
    assert pass_key is not None, "Passkey is None - failed to retrieve passkey from connection info"
    assert isinstance(pass_key, (int, str)), f"Invalid passkey type: {type(pass_key)}"
    
    logger.info(f"Passkey retrieved successfully: {pass_key}")
    dut_peripheral.interfaces.btc.user_passkey_request_reply(central_bd_addr, pass_key)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_passkey_request_reply"

    # Wait for simple pairing complete on both devices
    logger.info("Waiting for simple pairing complete in passkey entry test")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful simple_pairing_complete on central in passkey entry test"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful simple_pairing_complete on peripheral in passkey entry test"

    # Wait for link key notifications and authentication complete
    logger.info("Waiting for link key notifications in passkey entry test")
    result = dut_central.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=peripheral_bd_addr)
    assert result, f"Failed to receive link_key_notification for peripheral {peripheral_bd_addr} on central in passkey entry test"
    
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=central_bd_addr)
    assert result, f"Failed to receive link_key_notification for central {central_bd_addr} on peripheral in passkey entry test"
    
    result = dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive successful authentication_complete in passkey entry test"

    logger.info("Testing SSP authentication with passkey entry (matching) completed successfully")

def validate_authentication_with_passkey_entry_Negative_from_initiator(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with passkey entry Negative from initiator start")

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
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_KEYBOARD_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr), "Failed to receive io_capability_request on peripheral"
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_DISPLAY_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral"

    # Wait for IO capability response
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr), "Failed to receive io_capability_response on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr), "Failed to receive io_capability_response on peripheral"

    # Wait for passkey notification and entry
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_passkey_notification", value=central_bd_addr), "Failed to receive user_passkey_notification"

    assert dut_central.interfaces.btc.waitUntilEvent(variable="user_passkey_request", value=peripheral_bd_addr), "Failed to receive user_passkey_request"
    
    dut_central.interfaces.btc.user_passkey_request_negative_reply(peripheral_bd_addr)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_passkey_request_reply"

    # Wait for simple pairing complete on both devices
    assert dut_central.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete on peripheral"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive authentication_complete"

    logger.info("Testing SSP authentication with passkey entry Negative from intiator completed")
    
    
def validate_authentication_with_passkey_entry_Negative_from_responder(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with passkey entry Negative from responder start")

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
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_DISPLAY_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr), "Failed to receive io_capability_request on peripheral"
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_KEYBOARD_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral"

    # Wait for IO capability response
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr), "Failed to receive io_capability_response on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr), "Failed to receive io_capability_response on peripheral"

    # Wait for passkey notification and entry
    assert dut_central.interfaces.btc.waitUntilEvent(variable="user_passkey_notification", value=peripheral_bd_addr), "Failed to receive user_passkey_notification"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_passkey_request", value=central_bd_addr), "Failed to receive user_passkey_request"
    
    logger.info("Sending keypress notifications")
    dut_peripheral.interfaces.btc.send_keypress_notification(central_bd_addr, PASSKEY_ENTRY_START)  
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for send_keypress_notification (started)"
    
    dut_peripheral.interfaces.btc.send_keypress_notification(central_bd_addr, PASSKEY_ENTRY_COMPLETED)  
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for send_keypress_notification (completed)"
    

    dut_peripheral.interfaces.btc.user_passkey_request_negative_reply(central_bd_addr)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_passkey_request_reply"

    # Wait for simple pairing complete on both devices
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete on peripheral"

    assert dut_central.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete on central"
    assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive authentication_complete"

    logger.info("Testing SSP authentication with passkey entry Negative from responder completed")
    
    

def validate_authentication_with_passkey_entry_mismatched(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
    logger.info("Testing SSP authentication with passkey entry (mismatched) start")

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
    dut_central.interfaces.btc.io_capability_request_reply(peripheral_bd_addr, IO_CAPABILITY_DISPLAY_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on central"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_req", value=central_bd_addr), "Failed to receive io_capability_request on peripheral"
    dut_peripheral.interfaces.btc.io_capability_request_reply(central_bd_addr, IO_CAPABILITY_KEYBOARD_ONLY, OOB_AUTHENTICATION_DATA_NOT_PRESENT, MITM_PROTECTION_REQUIRED)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for io_capability_request_reply on peripheral"

    # Wait for IO capability response
    assert dut_central.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=peripheral_bd_addr), "Failed to receive io_capability_response on central"
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="io_capability_resp", value=central_bd_addr), "Failed to receive io_capability_response on peripheral"

    # Wait for passkey notification and entry
    assert dut_central.interfaces.btc.waitUntilEvent(variable="user_passkey_notification", value=peripheral_bd_addr), "Failed to receive user_passkey_notification"

    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="user_passkey_request", value=central_bd_addr), "Failed to receive user_passkey_request"
    
    dut_peripheral.interfaces.btc.send_keypress_notification(central_bd_addr, PASSKEY_ENTRY_START)  # Passkey entry started
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for send_keypress_notification (started)"
    
    dut_peripheral.interfaces.btc.send_keypress_notification(central_bd_addr, PASSKEY_ENTRY_COMPLETED)  # Passkey entry completed
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for send_keypress_notification (completed)"
    
    # Get correct passkey first, then use wrong one
    connected_device_info = dut_central.interfaces.btc.get_acl_connection_info()
    correct_passkey = connected_device_info.passkey
    wrong_passkey = 999999 if correct_passkey != 999999 else 123456  # Different from actual passkey
    logger.info(f"Correct passkey: {correct_passkey}, Using wrong passkey: {wrong_passkey}")
    
    dut_peripheral.interfaces.btc.user_passkey_request_reply(central_bd_addr, wrong_passkey)
    result = dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS)
    assert result, "Failed to receive cmd_complete for user_passkey_request_reply"

    # Wait for simple pairing complete with failure 
    assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="simple_pairing_complete", value=AUTHENTICATION_FAILURE), "Failed to receive simple_pairing_complete with failure on peripheral"

    assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive authentication_complete with failure"

    logger.info("Testing SSP authentication with passkey entry (mismatched) completed")
    

def test_ssp_pass_key_entry_authentication_procedure(duts, logger, debugs):
    logger.info(f"test_ssp_pass_key_entry_authentication_procedure Test case started in debug {debugs}")
    
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
        logger.info("Getting Bluetooth addresses for test devices")
        central_bd_addr = get_device_bd_addr(dut_central)
        peripheral_bd_addr = get_device_bd_addr(dut_peripheral)
       
        logger.info(f"Central BD Address: {central_bd_addr}")
        logger.info(f"Peripheral BD Address: {peripheral_bd_addr}")

        # Step 1: Virtual TCP socket connection Validation
        logger.info("Step 1: Creating virtual TCP socket connection")
        create_virtual_tcp_socket(dut_central, dut_peripheral, logger)

        # Step 2: BT Create connection Validation
        logger.info("Step 2: Validating BT connection procedure")
        validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)

        # Step 3: Enable Simple Secure Pairing
        logger.info("Step 3: Enabling Simple Secure Pairing on both devices")
        enable_secure_simple_pairing(dut_central, logger)
        enable_secure_simple_pairing(dut_peripheral, logger)

        # Get connection handle and validate it
        logger.info("Getting connection handle from peripheral device")
        connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
        assert connected_device_info, "Failed to get ACL connection info from peripheral device"
        
        conn_handle = connected_device_info.conn_handle
        assert conn_handle is not None, "Connection handle is None"
        assert isinstance(conn_handle, int), f"Invalid connection handle type: {type(conn_handle)}"
        
        logger.info(f"Connection handle: {conn_handle}")

        test_count = 0

               
        # PASSKEY ENTRY TEST CASES
        logger.info(f" ################# SSP Test case number: {(test_count := test_count + 1)} - Passkey Entry Matching")
        validate_authentication_with_passkey_entry_matching(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
       
        logger.info(f" ################# SSP Test case number: {(test_count := test_count + 1)} - Passkey Entry Negative from Initiator")
        validate_authentication_with_passkey_entry_Negative_from_initiator(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)

        logger.info(f" ################# SSP Test case number: {(test_count := test_count + 1)} - Passkey Entry Negative from Responder")
        validate_authentication_with_passkey_entry_Negative_from_responder(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)

        # Step 5: Termination of the Virtual TCP Socket connection
        logger.info("Step 5: Terminating virtual TCP socket connection")
        terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)

        logger.info(f"All SSP test cases ({test_count}) completed successfully!")

    except Exception as e:
        logger.error(f"Test failed with error: {str(e)}")
        assert False, e