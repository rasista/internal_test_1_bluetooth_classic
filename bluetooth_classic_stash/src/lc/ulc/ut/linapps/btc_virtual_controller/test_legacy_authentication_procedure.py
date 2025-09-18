from tests_utils import *

# Test functions for Bluetooth Legacy Authentication Procedure:
# - validate_authentication_with_link_key_at_both_controller: Tests authentication when both devices have stored link keys in controller
# - validate_host_without_link_key_with_negative_pin_code: Tests authentication failure when host rejects PIN code request
# - validate_host_without_link_key_with_same_positive_pin_code_at_both: Tests successful authentication with matching PIN codes
# - validate_host_without_link_key_with_different_positive_pin_code_at_both: Tests authentication failure with mismatched PIN codes
# - validate_host_with_link_key_with_same_link_key_at_both: Tests successful authentication when host provides matching link keys
# - validate_host_with_link_key_with_different_link_key_at_both: Tests authentication failure when host provides mismatched link keys
# - validate_host_without_link_key_with_positive_fixed_pin_at_both: Tests authentication with fixed PIN type on both devices
# - validate_host_without_link_key_with_positive_fixed_pin_at_peripheral: Tests authentication with fixed PIN on peripheral only
# - test_legacy_authentication_procedure: Main test function that executes all authentication scenarios

FIXED_PIN_TYPE  = 1
VARIABLE_PIN_TYPE = 0
PINCODE_SIZE = 16



def disable_secure_authentication(dut_device, logger):
  logger.info("Disabling secure authentication for legacy authentication testing")
  dut_device.interfaces.btc.write_simple_pairing_mode(DISABLE_SIMPLE_SECURE_PAIRING)
  assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for disable simple pairing"
  logger.info("Secure authentication disabled successfully")
  
  
def configure_authentication_pin_type(dut_device, pin_type, logger):
  logger.info(" Configure_authentication_pin_type Test Start")
  dut_device.interfaces.btc.write_pin_type(pin_type)
  assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for write pin type"
  logger.info(" Configure_authentication_pin_type Test Completed")


def delete_stored_link_keys(dut_device, bd_addr, logger):
  logger.info(f"Deleting stored link keys for device {bd_addr}")
  dut_device.interfaces.btc.delete_stored_link_key(bd_addr, HCI_DEL_STORED_LINK_KEY_FOR_ALL_BD_ADDR)
  assert dut_device.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for delete stored link key"



def validate_authentication_with_link_key_at_both_controller(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_authentication_with_link_key_at_both_controller start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
  link_key = "0123456789012345"

  dut_central.interfaces.btc.write_stored_link_key(1, peripheral_bd_addr, link_key)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for write stored link key"

  dut_peripheral.interfaces.btc.write_stored_link_key(1, central_bd_addr, link_key)
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for write stored link key"

  # Request authentication - should use existing link keys for authentication
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_status for authentication request"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS authentication_complete event"
  logger.info("Testing validate_authentication_with_link_key_at_both_controller Completed")


def validate_host_without_link_key_with_negative_pin_code(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing host without link key with negative pin code scenario start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
  
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending negative link key reply since host doesn't have link key
  dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request negative reply"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=peripheral_bd_addr), "Failed to receive pin_code_request event on central"
  dut_central.interfaces.btc.pin_code_request_negative_reply(peripheral_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request negative reply"
  
  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=PIN_OR_KEY_MISSING), "Failed to receive PIN_OR_KEY_MISSING authentication_complete event"
  
  logger.info("Testing host without link key with negative pin code scenario Completed")

  


def validate_host_without_link_key_with_same_positive_pin_code_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_host_without_link_key_with_same_positive_pin_code_at_both start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
  
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending negative link key reply since host doesn't have link key
  dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request negative reply"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=peripheral_bd_addr), "Failed to receive pin_code_request event on central"
  pin_code = "0123456789012345"
  dut_central.interfaces.btc.pin_code_request_reply(peripheral_bd_addr, PINCODE_SIZE, pin_code)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request reply"

  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=central_bd_addr), "Failed to receive pin_code_request event on peripheral"
  dut_peripheral.interfaces.btc.pin_code_request_reply(central_bd_addr, PINCODE_SIZE, pin_code)
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request reply"

  dut_central.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=peripheral_bd_addr)
  dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=central_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS authentication_complete event"
 
  logger.info("Testing validate_host_without_link_key_with_same_positive_pin_code_at_both Completed")



def validate_host_without_link_key_with_different_positive_pin_code_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_host_without_link_key_with_different_positive_pin_code_at_both start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
   
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending negative link key reply since host doesn't have link key
  dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request negative reply"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=peripheral_bd_addr), "Failed to receive pin_code_request event on central"
  pin_code = "0123456789012345"
  dut_central.interfaces.btc.pin_code_request_reply(peripheral_bd_addr, PINCODE_SIZE, pin_code)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request reply"

  pin_code_2 = "0000000000000000"
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=central_bd_addr), "Failed to receive pin_code_request event on peripheral"
  dut_peripheral.interfaces.btc.pin_code_request_reply(central_bd_addr, PINCODE_SIZE, pin_code_2)
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request reply"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive AUTHENTICATION_FAILURE authentication_complete event"
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="disconnection_complete", value=BT_SUCCESS_STATUS), "Failed to receive disconnection_complete event"

  dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, peripheral_bd_addr, DATA)
  dut_peripheral.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, central_bd_addr, DATA)

  logger.info("Testing validate_host_without_link_key_with_different_positive_pin_code_at_both Completed")

  


def validate_host_with_link_key_with_same_link_key_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_host_with_link_key_with_same_link_key_at_both start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
  
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending positive link key reply since host has link key
  link_key = "0123456789012345"
  dut_central.interfaces.btc.link_key_request_reply(peripheral_bd_addr, link_key)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request reply"
 
 # Expect link key request event since controller doesn't have link key
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_key_request", value=central_bd_addr), "Failed to receive link_key_request event when controller host has no link key"
  dut_peripheral.interfaces.btc.link_key_request_reply(central_bd_addr, link_key)
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request reply"
 
  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS authentication_complete event"
  logger.info("Testing validate_host_with_link_key_with_same_link_key_at_both Completed")
  
  
  
def validate_host_with_link_key_with_different_link_key_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_host_with_link_key_with_different_link_key_at_both start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
  
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending positive link key reply since host has link key
  link_key = "0123456789012345"
  link_key_2 = "0123456789012346"

  dut_central.interfaces.btc.link_key_request_reply(peripheral_bd_addr, link_key)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request reply"
 
 # Expect link key request event since controller doesn't have link key
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_key_request", value=central_bd_addr), "Failed to receive link_key_request event when controller host has no link key"
  dut_peripheral.interfaces.btc.link_key_request_reply(central_bd_addr, link_key_2)
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request reply"
 
  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive AUTHENTICATION_FAILURE authentication_complete event"
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="disconnection_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS disconnection_complete event"

  dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, peripheral_bd_addr, DATA)
  dut_peripheral.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, central_bd_addr, DATA)


  logger.info("Testing validate_host_with_link_key_with_different_link_key_at_both Completed")



def validate_host_without_link_key_with_positive_fixed_pin_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_host_without_link_key_with_positive_fixed_pin_at_both start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)


  configure_authentication_pin_type(dut_central, FIXED_PIN_TYPE, logger)
  configure_authentication_pin_type(dut_peripheral, FIXED_PIN_TYPE, logger)
    
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending negative link key reply since host doesn't have link key
  dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request negative reply"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=peripheral_bd_addr), "Failed to receive pin_code_request event on central"
  pin_code = "0123456789012345"
  dut_central.interfaces.btc.pin_code_request_reply(peripheral_bd_addr, PINCODE_SIZE, pin_code)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request reply"
  
  assert dut_peripheral.interfaces.btc.waitUntilEvent(variable="disconnection_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS disconnection_complete event"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=CONN_REJECTED_DUE_TO_SECURITY_REASONS), "Failed to receive CONN_REJECTED_DUE_TO_SECURITY_REASONS authentication_complete event"
  
  dut_central.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, peripheral_bd_addr, DATA)
  dut_peripheral.interfaces.btc.internal_event_trigger_from_lpw(META_DATA, DISCONNECTION_COMPLETE_NOTIFICATION_FROM_LPW, central_bd_addr, DATA)

  configure_authentication_pin_type(dut_central, VARIABLE_PIN_TYPE, logger)
  configure_authentication_pin_type(dut_peripheral, VARIABLE_PIN_TYPE, logger)
  logger.info("Testing validate_host_without_link_key_with_positive_fixed_pin_at_both Completed")



def validate_host_without_link_key_with_positive_fixed_pin_at_peripheral(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger):
  logger.info("Testing validate_host_without_link_key_with_positive_fixed_pin_at_peripheral start")

  delete_stored_link_keys(dut_central, central_bd_addr, logger)
  delete_stored_link_keys(dut_peripheral, peripheral_bd_addr, logger)
 
  configure_authentication_pin_type(dut_central, VARIABLE_PIN_TYPE, logger)
  configure_authentication_pin_type(dut_peripheral, FIXED_PIN_TYPE, logger)
    
  # Request authentication - should trigger PIN code request since no link key exists
  dut_central.interfaces.btc.authentication_requested(conn_handle)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_status", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd status for authentication request"

  # Expect link key request event since controller doesn't have link key
  assert dut_central.interfaces.btc.waitUntilEvent(variable="link_key_request", value=peripheral_bd_addr), "Failed to receive link_key_request event when controller host has no link key"

  # Sending negative link key reply since host doesn't have link key
  dut_central.interfaces.btc.link_key_request_negative_reply(peripheral_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for link key request negative reply"

  assert dut_central.interfaces.btc.waitUntilEvent(variable="pin_code_request", value=peripheral_bd_addr), "Failed to receive pin_code_request event on central"
  pin_code = "0000000000000000"
  dut_central.interfaces.btc.pin_code_request_reply(peripheral_bd_addr, 0, pin_code)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="cmd_complete", value=BT_SUCCESS_STATUS), "Failed to receive BT_SUCCESS_STATUS cmd_complete for pin code request reply"
  
  dut_central.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=peripheral_bd_addr)
  dut_peripheral.interfaces.btc.waitUntilEvent(variable="link_key_notification", value=central_bd_addr)
  assert dut_central.interfaces.btc.waitUntilEvent(variable="authentication_complete", value=AUTHENTICATION_FAILURE), "Failed to receive authentication_complete event"

  configure_authentication_pin_type(dut_peripheral, VARIABLE_PIN_TYPE, logger)
  logger.info("Testing validate_host_without_link_key_with_positive_fixed_pin_at_peripheral Completed")



def test_legacy_authentication_procedure(duts, logger, debugs):
  logger.info(f"test_legacy_authentication_procedure Test case started in debug {debugs}")
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

    # Step 3: Disable secure authentication for legacy testing
    disable_secure_authentication(dut_central, logger)
    disable_secure_authentication(dut_peripheral, logger)
        
    # Step 4: Configure the pin type to variable for both central and peripheral
    configure_authentication_pin_type(dut_central, VARIABLE_PIN_TYPE, logger)
    configure_authentication_pin_type(dut_peripheral, VARIABLE_PIN_TYPE, logger)
    
    # Step 4: Host without link key scenario Validation
    connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
    conn_handle = connected_device_info.conn_handle
    
    i = 0
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_authentication_with_link_key_at_both_controller(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_with_link_key_with_same_link_key_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_without_link_key_with_same_positive_pin_code_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_without_link_key_with_positive_fixed_pin_at_peripheral(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)

    #  NEGATIVE TEST CASES     
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_without_link_key_with_different_positive_pin_code_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
    connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
    conn_handle = connected_device_info.conn_handle
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_without_link_key_with_positive_fixed_pin_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_bt_create_connection_procedure(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, logger)
    connected_device_info = dut_peripheral.interfaces.btc.get_acl_connection_info()
    conn_handle = connected_device_info.conn_handle
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_without_link_key_with_negative_pin_code(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
    
    logger.info(f" ################# Test case number: {(i := i + 1)}")
    validate_host_with_link_key_with_different_link_key_at_both(dut_central, dut_peripheral, central_bd_addr, peripheral_bd_addr, conn_handle, logger)
      
        # Step 5: Termination of the Virtual TCP Socket connection
    terminate_virtual_tcp_socket(dut_central, dut_peripheral, logger)
  except Exception as e:
    logger.error(f"Test failed with error: {str(e)}")
    assert False, e