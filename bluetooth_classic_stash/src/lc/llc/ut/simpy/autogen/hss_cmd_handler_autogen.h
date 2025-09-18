#ifndef HSS_CMD_H
#define HSS_CMD_H

#include <stdint.h> 
#include "message_decode.h"

void hss_cmd_payload_start_procedure_payload_init_btc_device_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_page_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_page_scan_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_inquiry_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_inquiry_scan_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_acl_central_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_acl_peripheral_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_esco_central_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_esco_peripheral_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_roleswitch_central_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_roleswitch_peripheral_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_test_mode_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_page_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_page_scan_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_inquiry_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_e0_encryption_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_aes_encryption_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_encryption_parameters_aes_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_encryption_parameters_e0_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_inquiry_scan_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_afh_parameters_acl_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_sniff_parameters_acl_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_device_parameters_acl_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_test_mode_params_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_device_parameters_esco_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_device_parameters_roleswitch_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_roleswitch_parameters_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_set_fixed_btc_channel_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_page_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_inquiry_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_page_scan_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_inquiry_scan_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_acl_central_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_esco_central_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_roleswitch_central_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_acl_peripheral_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_esco_peripheral_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_roleswitch_peripheral_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_payload_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_start_procedure_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_get_procedure_payload_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_payload_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_set_procedure_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_payload_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_stop_procedure_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_get_procedure_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_payload_handler(uint32_t argc, uint8_t *argv[]);
void hss_cmd_handler(uint32_t argc, uint8_t *argv[]);
void hss_event_payload_page_complete_handler(uint32_t argc, uint8_t *argv[]);
void hss_event_payload_page_scan_complete_handler(uint32_t argc, uint8_t *argv[]);
void hss_event_payload_btc_slot_offset_handler(uint32_t argc, uint8_t *argv[]);
void hss_event_payload_handler(uint32_t argc, uint8_t *argv[]);
void hss_event_handler(uint32_t argc, uint8_t *argv[]);

#endif