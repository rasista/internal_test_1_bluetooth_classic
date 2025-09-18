#include <stdint.h>
#include "jansson_binaries/include/jansson_config.h"
#include "jansson_binaries/include/jansson.h"
#include "string.h"
#include "sl_btc_seq_interface.h"
#include <stdlib.h>

// Function declarations for parsing elements
void json2c_hss_cmd_payload_start_procedure_payload_init_btc_device(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_page(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_page_scan(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_inquiry(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_inquiry_scan(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_acl_central(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_acl_peripheral(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_esco_central(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_esco_peripheral(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_roleswitch_central(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_roleswitch_peripheral(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure_payload_test_mode(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_page(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_page_scan(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_inquiry(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_E0_encryption(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_AES_encryption(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_encryption_parameters_aes(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_encryption_parameters_e0(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_inquiry_scan(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_afh_parameters_acl(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_sniff_parameters_acl(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_device_parameters_acl(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_test_mode_params(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_device_parameters_esco(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_roleswitch_parameters(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_page(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_inquiry(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_page_scan(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_inquiry_scan(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_acl_central(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_esco_central(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_roleswitch_central(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_acl_peripheral(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_esco_peripheral(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_start_procedure(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_get_procedure_payload(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_set_procedure(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_stop_procedure(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd_payload_get_procedure(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_cmd(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_event_payload_page_complete(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_event_payload_page_scan_complete(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_event_payload_btc_slot_offset(json_t *json_obj, uint8_t *hss_cmd);
void json2c_hss_event(json_t *json_obj, uint8_t *hss_cmd);

// Function prototypes for JSON-to-C field parsing
void json2c_uint8_t(uint8_t *field, json_t *json_obj, uint8_t length);
void json2c_array_uint8_t(uint8_t *field, json_t *json_obj, json_t *value, uint16_t length);
void json2c_uint16_t(uint16_t *field, json_t *json_obj, uint8_t length);
void json2c_uint32_t(uint32_t *field, json_t *json_obj, uint8_t length);

// Implementation for parsing the hss_cmd_payload_start_procedure_payload_init_btc_device element
void json2c_hss_cmd_payload_start_procedure_payload_init_btc_device(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_INIT_BTC_DEVICE_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_INIT_BTC_DEVICE_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_page element
void json2c_hss_cmd_payload_start_procedure_payload_page(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_PAGE_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_PAGE_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_PAGE_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_page_scan element
void json2c_hss_cmd_payload_start_procedure_payload_page_scan(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_PAGE_SCAN_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_PAGE_SCAN_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_PAGE_SCAN_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_inquiry element
void json2c_hss_cmd_payload_start_procedure_payload_inquiry(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_INQUIRY_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_INQUIRY_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_INQUIRY_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_inquiry_scan element
void json2c_hss_cmd_payload_start_procedure_payload_inquiry_scan(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_INQUIRY_SCAN_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_INQUIRY_SCAN_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_INQUIRY_SCAN_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_acl_central element
void json2c_hss_cmd_payload_start_procedure_payload_acl_central(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ACL_CENTRAL_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ACL_CENTRAL_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ACL_CENTRAL_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ACL_CENTRAL_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_ACL_CENTRAL_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_acl_peripheral element
void json2c_hss_cmd_payload_start_procedure_payload_acl_peripheral(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ACL_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ACL_PERIPHERAL_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ACL_PERIPHERAL_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ACL_PERIPHERAL_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ACL_PERIPHERAL_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_ACL_PERIPHERAL_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_esco_central element
void json2c_hss_cmd_payload_start_procedure_payload_esco_central(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ESCO_CENTRAL_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ESCO_CENTRAL_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ESCO_CENTRAL_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ESCO_CENTRAL_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_ESCO_CENTRAL_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_esco_peripheral element
void json2c_hss_cmd_payload_start_procedure_payload_esco_peripheral(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ESCO_PERIPHERAL_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ESCO_PERIPHERAL_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ESCO_PERIPHERAL_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ESCO_PERIPHERAL_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_ESCO_PERIPHERAL_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_roleswitch_central element
void json2c_hss_cmd_payload_start_procedure_payload_roleswitch_central(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ROLESWITCH_CENTRAL_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ROLESWITCH_CENTRAL_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ROLESWITCH_CENTRAL_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ROLESWITCH_CENTRAL_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_ROLESWITCH_CENTRAL_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_roleswitch_peripheral element
void json2c_hss_cmd_payload_start_procedure_payload_roleswitch_peripheral(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ROLESWITCH_PERIPHERAL_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_ROLESWITCH_PERIPHERAL_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "priority")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ROLESWITCH_PERIPHERAL_PRIORITY(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "step_size")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_ROLESWITCH_PERIPHERAL_STEP_SIZE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "end_tsf")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_ROLESWITCH_PERIPHERAL_END_TSF(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure_payload_test_mode element
void json2c_hss_cmd_payload_start_procedure_payload_test_mode(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_TEST_MODE_BD_ADDRESS(hss_cmd), array, json_value, 3);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_START_TEST_MODE_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "br_edr_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_BR_EDR_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "channel_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_CHANNEL_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "link_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_LINK_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "inter_packet_gap")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_INTER_PACKET_GAP(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "scrambler_seed")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_SCRAMBLER_SEED(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "pkt_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_PKT_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "pkt_len")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_START_TEST_MODE_PKT_LEN(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "payload_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_PAYLOAD_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_power")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_TX_POWER(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "hopping_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_HOPPING_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "loopback_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_LOOPBACK_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "transmit_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_TRANSMIT_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encryption_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_TEST_MODE_ENCRYPTION_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "no_of_pkts")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_TEST_MODE_NO_OF_PKTS(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "rx_window")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_START_TEST_MODE_RX_WINDOW(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_page element
void json2c_hss_cmd_payload_set_procedure_payload_page(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "clk_e_offset")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_PAGE_CLK_E_OFFSET(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "hci_trigger")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PAGE_HCI_TRIGGER(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_pwr_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PAGE_TX_PWR_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "remote_scan_rep_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PAGE_REMOTE_SCAN_REP_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_PAGE_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_PAGE_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "lt_addr")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PAGE_LT_ADDR(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_page_scan element
void json2c_hss_cmd_payload_set_procedure_payload_page_scan(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "window")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_PAGE_SCAN_WINDOW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "interval")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_PAGE_SCAN_INTERVAL(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "curr_scan_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PAGE_SCAN_CURR_SCAN_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_pwr_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PAGE_SCAN_TX_PWR_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_PAGE_SCAN_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_PAGE_SCAN_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_inquiry element
void json2c_hss_cmd_payload_set_procedure_payload_inquiry(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "lap_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_INQUIRY_LAP_ADDRESS(hss_cmd), array, json_value, 3);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_INQUIRY_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "hci_trigger")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_INQUIRY_HCI_TRIGGER(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "length")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_INQUIRY_LENGTH(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "EIR_value")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_INQUIRY_EIR_VALUE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_pwr_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_INQUIRY_TX_PWR_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_E0_encryption element
void json2c_hss_cmd_payload_set_procedure_payload_E0_encryption(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "masterClock")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_MASTERCLOCK(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey0")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_ENCKEY0(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey1")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_ENCKEY1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey2")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_ENCKEY2(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey3")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_ENCKEY3(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bdAddr0")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_BDADDR0(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bdAddr1")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_E0_ENCRYPTION_BDADDR1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "keyLength")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_E0_ENCRYPTION_KEYLENGTH(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_AES_encryption element
void json2c_hss_cmd_payload_set_procedure_payload_AES_encryption(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "aesKeyBytes_0_3")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_0_3(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_4_7")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_4_7(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_8_11")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_8_11(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_12_15")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_12_15(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesIv1")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESIV1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesIv2")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESIV2(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "dayCountAndDir")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_AES_ENCRYPTION_DAYCOUNTANDDIR(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "zeroLenAclW")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_AES_ENCRYPTION_ZEROLENACLW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesPldCntr1")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_AESPLDCNTR1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesPldCntr2")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AES_ENCRYPTION_AESPLDCNTR2(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_encryption_parameters_aes element
void json2c_hss_cmd_payload_set_procedure_payload_encryption_parameters_aes(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encryption_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_ENCRYPTION_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_0_3")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_0_3(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_4_7")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_4_7(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_8_11")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_8_11(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesKeyBytes_12_15")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_12_15(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesIv1")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESIV1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesIv2")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESIV2(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "dayCountAndDir")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_DAYCOUNTANDDIR(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "zeroLenAclW")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_ZEROLENACLW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesPldCntr1")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESPLDCNTR1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "aesPldCntr2")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESPLDCNTR2(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_encryption_parameters_e0 element
void json2c_hss_cmd_payload_set_procedure_payload_encryption_parameters_e0(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encryption_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCRYPTION_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "masterClock")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_MASTERCLOCK(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey0")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY0(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey1")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey2")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY2(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "encKey3")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY3(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bdAddr0")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_BDADDR0(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "bdAddr1")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_BDADDR1(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "keyLength")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_KEYLENGTH(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_inquiry_scan element
void json2c_hss_cmd_payload_set_procedure_payload_inquiry_scan(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "window")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_INQUIRY_SCAN_WINDOW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "interval")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_INQUIRY_SCAN_INTERVAL(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "eir_data_length")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_INQUIRY_SCAN_EIR_DATA_LENGTH(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "eir_data")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_INQUIRY_SCAN_EIR_DATA(hss_cmd), array, json_value, 260);
        } else
        if (!strcmp(json_key, "EIR_value")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_INQUIRY_SCAN_EIR_VALUE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "lap_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_INQUIRY_SCAN_LAP_ADDRESS(hss_cmd), array, json_value, 3);
        } else
        if (!strcmp(json_key, "sync_word")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_INQUIRY_SCAN_SYNC_WORD(hss_cmd), array, json_value, 8);
        } else
        if (!strcmp(json_key, "curr_scan_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_INQUIRY_SCAN_CURR_SCAN_TYPE(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_afh_parameters_acl element
void json2c_hss_cmd_payload_set_procedure_payload_afh_parameters_acl(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AFH_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "no_of_valid_afh_channels")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AFH_PARAMETERS_ACL_NO_OF_VALID_AFH_CHANNELS(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "piconet_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_AFH_PARAMETERS_ACL_PICONET_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "afh_new_channel_map")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_AFH_PARAMETERS_ACL_AFH_NEW_CHANNEL_MAP(hss_cmd), array, json_value, 10);
        } else
        if (!strcmp(json_key, "channel_map_in_use")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_CMD_SET_AFH_PARAMETERS_ACL_CHANNEL_MAP_IN_USE(hss_cmd), array, json_value, 10);
        } else
        if (!strcmp(json_key, "afh_new_channel_map_instant")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_AFH_PARAMETERS_ACL_AFH_NEW_CHANNEL_MAP_INSTANT(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_sniff_parameters_acl element
void json2c_hss_cmd_payload_set_procedure_payload_sniff_parameters_acl(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_SNIFF_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "sniff_instant")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_INSTANT(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "sniff_interval")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_INTERVAL(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "sniff_attempt")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_ATTEMPT(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "sniff_timeout")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_TIMEOUT(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_device_parameters_acl element
void json2c_hss_cmd_payload_set_procedure_payload_device_parameters_acl(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "peer_role")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ACL_PEER_ROLE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_pwr_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ACL_TX_PWR_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "flow")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ACL_FLOW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "br_edr_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ACL_BR_EDR_MODE(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_test_mode_params element
void json2c_hss_cmd_payload_set_procedure_payload_test_mode_params(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "test_scenario")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_TEST_SCENARIO(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "hopping_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_HOPPING_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_freq")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_TX_FREQ(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "rx_freq")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_RX_FREQ(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "pwr_cntrl_mode")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_PWR_CNTRL_MODE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "poll_period")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_POLL_PERIOD(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "pkt_type")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_TEST_MODE_PARAMS_PKT_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "link_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_LINK_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "test_lt_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_TEST_LT_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "test_started")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_TEST_STARTED(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "test_mode_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_TEST_MODE_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "sco_pkt_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_TEST_MODE_PARAMS_SCO_PKT_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "len_of_seq")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_TEST_MODE_PARAMS_LEN_OF_SEQ(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_device_parameters_esco element
void json2c_hss_cmd_payload_set_procedure_payload_device_parameters_esco(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "peer_role")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_PEER_ROLE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "lt_addr")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_LT_ADDR(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "flow")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_FLOW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "t_esco")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_T_ESCO(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "d_esco")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_D_ESCO(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "w_esco")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_W_ESCO(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "is_esco")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_IS_ESCO(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "is_edr")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_IS_EDR(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch element
void json2c_hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "peer_role")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_PEER_ROLE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "lt_addr")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_LT_ADDR(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "tx_pwr_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_TX_PWR_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "flow")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_FLOW(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "clk_e_offset")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_CLK_E_OFFSET(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_roleswitch_parameters element
void json2c_hss_cmd_payload_set_procedure_payload_roleswitch_parameters(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_ROLESWITCH_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "new_lt_addr")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_ROLESWITCH_PARAMETERS_NEW_LT_ADDR(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "roleswitch_slot_offset")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_SLOT_OFFSET(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "roleswitch_instant")) {
            // Process the macro directly here
            json2c_uint32_t(HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_INSTANT(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel element
void json2c_hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "channel")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_FIXED_BTC_CHANNEL_CHANNEL(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_page element
void json2c_hss_cmd_payload_stop_procedure_payload_page(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "current_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_PAGE_CURRENT_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_inquiry element
void json2c_hss_cmd_payload_stop_procedure_payload_inquiry(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "current_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_INQUIRY_CURRENT_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_page_scan element
void json2c_hss_cmd_payload_stop_procedure_payload_page_scan(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "current_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_PAGE_SCAN_CURRENT_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_inquiry_scan element
void json2c_hss_cmd_payload_stop_procedure_payload_inquiry_scan(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "current_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_INQUIRY_SCAN_CURRENT_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_acl_central element
void json2c_hss_cmd_payload_stop_procedure_payload_acl_central(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_esco_central element
void json2c_hss_cmd_payload_stop_procedure_payload_esco_central(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_roleswitch_central element
void json2c_hss_cmd_payload_stop_procedure_payload_roleswitch_central(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_acl_peripheral element
void json2c_hss_cmd_payload_stop_procedure_payload_acl_peripheral(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_ACL_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_esco_peripheral element
void json2c_hss_cmd_payload_stop_procedure_payload_esco_peripheral(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral element
void json2c_hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_device_index")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_start_procedure element
void json2c_hss_cmd_payload_start_procedure(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "id")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_START_PROCEDURE_ID(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_get_procedure_payload element
void json2c_hss_cmd_payload_get_procedure_payload(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "id")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_GET_PROCEDURE_PAYLOAD_ID(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_set_procedure element
void json2c_hss_cmd_payload_set_procedure(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "id")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_SET_PROCEDURE_ID(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_stop_procedure element
void json2c_hss_cmd_payload_stop_procedure(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "id")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_STOP_PROCEDURE_ID(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd_payload_get_procedure element
void json2c_hss_cmd_payload_get_procedure(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "id")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_GET_PROCEDURE_ID(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_cmd element
void json2c_hss_cmd(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "cmd_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_CMD_CMD_TYPE(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_event_payload_page_complete element
void json2c_hss_event_payload_page_complete(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_info_idx")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_EVENT_PAGE_COMPLETE_CONNECTION_INFO_IDX(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_event_payload_page_scan_complete element
void json2c_hss_event_payload_page_scan_complete(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "connection_info_idx")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_EVENT_PAGE_SCAN_COMPLETE_CONNECTION_INFO_IDX(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "remote_bd_address")) {
            json_t *array = json_object_get(json_obj, json_key);
            json2c_array_uint8_t(HSS_EVENT_PAGE_SCAN_COMPLETE_REMOTE_BD_ADDRESS(hss_cmd), array, json_value, 6);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_event_payload_btc_slot_offset element
void json2c_hss_event_payload_btc_slot_offset(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "roleswitch_slot_offset")) {
            // Process the macro directly here
            json2c_uint16_t(HSS_EVENT_BTC_SLOT_OFFSET_ROLESWITCH_SLOT_OFFSET(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}
// Implementation for parsing the hss_event element
void json2c_hss_event(json_t *json_obj, uint8_t *hss_cmd)
{
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "event_type")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_EVENT_EVENT_TYPE(hss_cmd), json_value, 1);
        } else
        if (!strcmp(json_key, "event_status")) {
            // Process the macro directly here
            json2c_uint8_t(HSS_EVENT_EVENT_STATUS(hss_cmd), json_value, 1);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
}

// Parser function for the command hss_cmd (main entry point)
void sl_btc_seq_interface_command_parser(uint8_t *out_hss_cmd, uint8_t *in_hss_cmd)
{
    json_t *json_obj = json_loads((char *)in_hss_cmd, 0, NULL);
    if (json_obj == NULL) {
        fprintf(stderr, "Error loading JSON\n");
    }
    const char *json_key;
    json_t *json_value;
    json_object_foreach(json_obj, json_key, json_value) {
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_init_btc_device")) {
            json2c_hss_cmd_payload_start_procedure_payload_init_btc_device(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_page")) {
            json2c_hss_cmd_payload_start_procedure_payload_page(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_page_scan")) {
            json2c_hss_cmd_payload_start_procedure_payload_page_scan(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_inquiry")) {
            json2c_hss_cmd_payload_start_procedure_payload_inquiry(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_inquiry_scan")) {
            json2c_hss_cmd_payload_start_procedure_payload_inquiry_scan(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_acl_central")) {
            json2c_hss_cmd_payload_start_procedure_payload_acl_central(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_acl_peripheral")) {
            json2c_hss_cmd_payload_start_procedure_payload_acl_peripheral(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_esco_central")) {
            json2c_hss_cmd_payload_start_procedure_payload_esco_central(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_esco_peripheral")) {
            json2c_hss_cmd_payload_start_procedure_payload_esco_peripheral(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_roleswitch_central")) {
            json2c_hss_cmd_payload_start_procedure_payload_roleswitch_central(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_roleswitch_peripheral")) {
            json2c_hss_cmd_payload_start_procedure_payload_roleswitch_peripheral(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure_payload_test_mode")) {
            json2c_hss_cmd_payload_start_procedure_payload_test_mode(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_page")) {
            json2c_hss_cmd_payload_set_procedure_payload_page(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_page_scan")) {
            json2c_hss_cmd_payload_set_procedure_payload_page_scan(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_inquiry")) {
            json2c_hss_cmd_payload_set_procedure_payload_inquiry(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_E0_encryption")) {
            json2c_hss_cmd_payload_set_procedure_payload_E0_encryption(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_AES_encryption")) {
            json2c_hss_cmd_payload_set_procedure_payload_AES_encryption(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_encryption_parameters_aes")) {
            json2c_hss_cmd_payload_set_procedure_payload_encryption_parameters_aes(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_encryption_parameters_e0")) {
            json2c_hss_cmd_payload_set_procedure_payload_encryption_parameters_e0(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_inquiry_scan")) {
            json2c_hss_cmd_payload_set_procedure_payload_inquiry_scan(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_afh_parameters_acl")) {
            json2c_hss_cmd_payload_set_procedure_payload_afh_parameters_acl(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_sniff_parameters_acl")) {
            json2c_hss_cmd_payload_set_procedure_payload_sniff_parameters_acl(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_device_parameters_acl")) {
            json2c_hss_cmd_payload_set_procedure_payload_device_parameters_acl(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_test_mode_params")) {
            json2c_hss_cmd_payload_set_procedure_payload_test_mode_params(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_device_parameters_esco")) {
            json2c_hss_cmd_payload_set_procedure_payload_device_parameters_esco(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch")) {
            json2c_hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_roleswitch_parameters")) {
            json2c_hss_cmd_payload_set_procedure_payload_roleswitch_parameters(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel")) {
            json2c_hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_page")) {
            json2c_hss_cmd_payload_stop_procedure_payload_page(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_inquiry")) {
            json2c_hss_cmd_payload_stop_procedure_payload_inquiry(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_page_scan")) {
            json2c_hss_cmd_payload_stop_procedure_payload_page_scan(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_inquiry_scan")) {
            json2c_hss_cmd_payload_stop_procedure_payload_inquiry_scan(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_acl_central")) {
            json2c_hss_cmd_payload_stop_procedure_payload_acl_central(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_esco_central")) {
            json2c_hss_cmd_payload_stop_procedure_payload_esco_central(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_roleswitch_central")) {
            json2c_hss_cmd_payload_stop_procedure_payload_roleswitch_central(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_acl_peripheral")) {
            json2c_hss_cmd_payload_stop_procedure_payload_acl_peripheral(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_esco_peripheral")) {
            json2c_hss_cmd_payload_stop_procedure_payload_esco_peripheral(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral")) {
            json2c_hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_start_procedure")) {
            json2c_hss_cmd_payload_start_procedure(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_get_procedure_payload")) {
            json2c_hss_cmd_payload_get_procedure_payload(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_set_procedure")) {
            json2c_hss_cmd_payload_set_procedure(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_stop_procedure")) {
            json2c_hss_cmd_payload_stop_procedure(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd_payload_get_procedure")) {
            json2c_hss_cmd_payload_get_procedure(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_cmd")) {
            json2c_hss_cmd(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_event_payload_page_complete")) {
            json2c_hss_event_payload_page_complete(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_event_payload_page_scan_complete")) {
            json2c_hss_event_payload_page_scan_complete(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_event_payload_btc_slot_offset")) {
            json2c_hss_event_payload_btc_slot_offset(json_value, out_hss_cmd);
        } else
        if (!strcmp(json_key, "hss_event")) {
            json2c_hss_event(json_value, out_hss_cmd);
        } else
        {
            printf("Rail Fail at file: %s, line: %d\n", __FILE__, __LINE__); // Print error message
            exit(EXIT_FAILURE);
        }
    }
    json_decref(json_obj);
}

// Basic functions to convert JSON data to C types (used by macros)
void json2c_uint8_t(uint8_t *field, json_t *json_obj, uint8_t length)
{
    *field = (uint8_t)json_integer_value(json_obj);
}

void json2c_array_uint8_t(uint8_t *field, json_t *json_obj, json_t *value, uint16_t length)
{
    size_t index;
    json_array_foreach(json_obj, index, value)
    {
        *(field + index) = (uint8_t)json_integer_value(value);
    }
}

void json2c_uint16_t(uint16_t *field, json_t *json_obj, uint8_t length)
{
    *field = (uint16_t)json_integer_value(json_obj);
}

void json2c_uint32_t(uint32_t *field, json_t *json_obj, uint8_t length)
{
    *field = (uint32_t)json_integer_value(json_obj);
}