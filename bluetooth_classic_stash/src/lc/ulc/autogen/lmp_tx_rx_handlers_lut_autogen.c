#include "lmp_tx_rx_handlers_autogen.h"
#include <lmp.h>



const lmp_rx_handler_t lmp_rx_lut[] = {
NULL, /* Index 0 is reserved */
lmp_name_req_rx_handler, // RX handler 
lmp_name_res_rx_handler, // RX handler 
lmp_accepted_rx_handler, // RX handler 
lmp_not_accepted_rx_handler, // RX handler 
lmp_clkoffset_req_rx_handler, // RX handler 
lmp_clkoffset_res_rx_handler, // RX handler 
lmp_detach_rx_handler, // RX handler 
lmp_in_rand_rx_handler, // RX handler 
lmp_comb_key_rx_handler, // RX handler 
lmp_unit_key_rx_handler, // RX handler 
lmp_au_rand_rx_handler, // RX handler 
lmp_sres_rx_handler, // RX handler 
lmp_temp_rand_rx_handler, // RX handler 
lmp_temp_key_rx_handler, // RX handler 
lmp_encryption_mode_req_rx_handler, // RX handler 
lmp_encryption_key_size_req_rx_handler, // RX handler 
lmp_start_encryption_req_rx_handler, // RX handler 
lmp_stop_encryption_req_rx_handler, // RX handler 
lmp_switch_req_rx_handler, // RX handler 
lmp_hold_rx_handler, // RX handler 
lmp_hold_req_rx_handler, // RX handler 
NULL, /* Opcode 22 not used */ 
lmp_sniff_req_rx_handler, // RX handler for opcode 23
lmp_unsniff_req_rx_handler, // RX handler 
NULL, /* Opcode 25 not used */ 
NULL, /* Opcode 26 not used */ 
NULL, /* Opcode 27 not used */ 
NULL, /* Opcode 28 not used */ 
NULL, /* Opcode 29 not used */ 
NULL, /* Opcode 30 not used */ 
lmp_incr_power_req_rx_handler, // RX handler for opcode 31
lmp_decr_power_req_rx_handler, // RX handler 
lmp_max_power_rx_handler, // RX handler 
lmp_min_power_rx_handler, // RX handler 
lmp_auto_rate_rx_handler, // RX handler 
lmp_preferred_rate_rx_handler, // RX handler 
lmp_version_req_rx_handler, // RX handler 
lmp_version_res_rx_handler, // RX handler 
lmp_features_req_rx_handler, // RX handler 
lmp_features_res_rx_handler, // RX handler 
lmp_quality_of_service_rx_handler, // RX handler 
lmp_quality_of_service_req_rx_handler, // RX handler 
lmp_SCO_link_req_rx_handler, // RX handler 
lmp_remove_SCO_link_req_rx_handler, // RX handler 
lmp_max_slot_rx_handler, // RX handler 
lmp_max_slot_req_rx_handler, // RX handler 
lmp_timimg_accuracy_req_rx_handler, // RX handler 
lmp_timing_accuracy_res_rx_handler, // RX handler 
lmp_setup_complete_rx_handler, // RX handler 
lmp_use_semi_permanent_key_rx_handler, // RX handler 
lmp_host_connection_req_rx_handler, // RX handler 
lmp_slot_offset_rx_handler, // RX handler 
lmp_page_mode_req_rx_handler, // RX handler 
lmp_page_scan_mode_req_rx_handler, // RX handler 
lmp_supervision_timeout_rx_handler, // RX handler 
lmp_test_activate_rx_handler, // RX handler 
lmp_test_control_rx_handler, // RX handler 
lmp_encryption_key_size_mask_req_rx_handler, // RX handler 
lmp_encryption_key_size_mask_res_rx_handler, // RX handler 
lmp_set_afh_rx_handler, // RX handler 
lmp_encapsulated_header_rx_handler, // RX handler 
lmp_encapsulated_payload_rx_handler, // RX handler 
lmp_simple_pairing_confirm_rx_handler, // RX handler 
lmp_simple_pairing_number_rx_handler, // RX handler 
lmp_dhkey_check_rx_handler, // RX handler 
lmp_pause_encryption_aes_req_rx_handler, // RX handler 
};

const lmp_rx_handler_t lmp_rx_ext_lut[] = {
NULL, /* Index 0 is reserved */
lmp_accepted_ext_rx_handler, // RX handler 
lmp_not_accepted_ext_rx_handler, // RX handler 
lmp_features_req_ext_rx_handler, // RX handler 
lmp_features_res_ext_rx_handler, // RX handler 
lmp_clk_adj_rx_handler, // RX handler 
lmp_clk_adj_ack_rx_handler, // RX handler 
lmp_clk_adj_req_rx_handler, // RX handler 
NULL, /* Extended opcode 8 not used */ 
NULL, /* Extended opcode 9 not used */ 
NULL, /* Extended opcode 10 not used */ 
lmp_packet_type_table_req_rx_handler, // RX handler for extended opcode 11
lmp_eSCO_link_req_rx_handler, // RX handler 
lmp_remove_eSCO_link_req_rx_handler, // RX handler 
NULL, /* Extended opcode 14 not used */ 
NULL, /* Extended opcode 15 not used */ 
lmp_channel_classification_req_rx_handler, // RX handler for extended opcode 16
lmp_channel_classification_rx_handler, // RX handler 
NULL, /* Extended opcode 18 not used */ 
NULL, /* Extended opcode 19 not used */ 
NULL, /* Extended opcode 20 not used */ 
lmp_sniff_subrating_req_rx_handler, // RX handler for extended opcode 21
lmp_sniff_subrating_res_rx_handler, // RX handler 
lmp_pause_encryption_req_rx_handler, // RX handler 
lmp_resume_encryption_req_rx_handler, // RX handler 
lmp_io_capability_req_rx_handler, // RX handler 
lmp_io_capability_res_rx_handler, // RX handler 
lmp_numeric_comparison_failed_rx_handler, // RX handler 
lmp_passkey_failed_rx_handler, // RX handler 
lmp_oob_failed_rx_handler, // RX handler 
lmp_keypress_notification_rx_handler, // RX handler 
lmp_power_control_req_rx_handler, // RX handler 
lmp_power_control_res_rx_handler, // RX handler 
lmp_ping_req_rx_handler, // RX handler 
lmp_ping_res_rx_handler, // RX handler 
lmp_sam_set_type0_rx_handler, // RX handler 
lmp_sam_define_map_rx_handler, // RX handler 
lmp_sam_switch_rx_handler, // RX handler 
};

const lmp_tx_handler_t lmp_tx_lut[] = {
NULL, /* Index 0 is reserved */
lmp_name_req_tx_handler, // TX handler 
lmp_name_res_tx_handler, // TX handler 
lmp_accepted_tx_handler, // TX handler 
lmp_not_accepted_tx_handler, // TX handler 
lmp_clkoffset_req_tx_handler, // TX handler 
lmp_clkoffset_res_tx_handler, // TX handler 
lmp_detach_tx_handler, // TX handler 
lmp_in_rand_tx_handler, // TX handler 
lmp_comb_key_tx_handler, // TX handler 
lmp_unit_key_tx_handler, // TX handler 
lmp_au_rand_tx_handler, // TX handler 
lmp_sres_tx_handler, // TX handler 
lmp_temp_rand_tx_handler, // TX handler 
lmp_temp_key_tx_handler, // TX handler 
lmp_encryption_mode_req_tx_handler, // TX handler 
lmp_encryption_key_size_req_tx_handler, // TX handler 
lmp_start_encryption_req_tx_handler, // TX handler 
lmp_stop_encryption_req_tx_handler, // TX handler 
lmp_switch_req_tx_handler, // TX handler 
lmp_hold_tx_handler, // TX handler 
lmp_hold_req_tx_handler, // TX handler 
NULL, /* Opcode 22 not used */ 
lmp_sniff_req_tx_handler, // TX handler for opcode 23
lmp_unsniff_req_tx_handler, // TX handler 
NULL, /* Opcode 25 not used */ 
NULL, /* Opcode 26 not used */ 
NULL, /* Opcode 27 not used */ 
NULL, /* Opcode 28 not used */ 
NULL, /* Opcode 29 not used */ 
NULL, /* Opcode 30 not used */ 
lmp_incr_power_req_tx_handler, // TX handler for opcode 31
lmp_decr_power_req_tx_handler, // TX handler 
lmp_max_power_tx_handler, // TX handler 
lmp_min_power_tx_handler, // TX handler 
lmp_auto_rate_tx_handler, // TX handler 
lmp_preferred_rate_tx_handler, // TX handler 
lmp_version_req_tx_handler, // TX handler 
lmp_version_res_tx_handler, // TX handler 
lmp_features_req_tx_handler, // TX handler 
lmp_features_res_tx_handler, // TX handler 
lmp_quality_of_service_tx_handler, // TX handler 
lmp_quality_of_service_req_tx_handler, // TX handler 
lmp_SCO_link_req_tx_handler, // TX handler 
lmp_remove_SCO_link_req_tx_handler, // TX handler 
lmp_max_slot_tx_handler, // TX handler 
lmp_max_slot_req_tx_handler, // TX handler 
lmp_timimg_accuracy_req_tx_handler, // TX handler 
lmp_timing_accuracy_res_tx_handler, // TX handler 
lmp_setup_complete_tx_handler, // TX handler 
lmp_use_semi_permanent_key_tx_handler, // TX handler 
lmp_host_connection_req_tx_handler, // TX handler 
lmp_slot_offset_tx_handler, // TX handler 
lmp_page_mode_req_tx_handler, // TX handler 
lmp_page_scan_mode_req_tx_handler, // TX handler 
lmp_supervision_timeout_tx_handler, // TX handler 
lmp_test_activate_tx_handler, // TX handler 
lmp_test_control_tx_handler, // TX handler 
lmp_encryption_key_size_mask_req_tx_handler, // TX handler 
lmp_encryption_key_size_mask_res_tx_handler, // TX handler 
lmp_set_afh_tx_handler, // TX handler 
lmp_encapsulated_header_tx_handler, // TX handler 
lmp_encapsulated_payload_tx_handler, // TX handler 
lmp_simple_pairing_confirm_tx_handler, // TX handler 
lmp_simple_pairing_number_tx_handler, // TX handler 
lmp_dhkey_check_tx_handler, // TX handler 
lmp_pause_encryption_aes_req_tx_handler, // TX handler 
};

lmp_tx_handler_t lmp_tx_ext_lut[] = {
NULL, /* Index 0 is reserved */
lmp_accepted_ext_tx_handler, // TX handler 
lmp_not_accepted_ext_tx_handler, // TX handler 
lmp_features_req_ext_tx_handler, // TX handler 
lmp_features_res_ext_tx_handler, // TX handler 
lmp_clk_adj_tx_handler, // TX handler 
lmp_clk_adj_ack_tx_handler, // TX handler 
lmp_clk_adj_req_tx_handler, // TX handler 
NULL, /* Extended opcode 8 not used */ 
NULL, /* Extended opcode 9 not used */ 
NULL, /* Extended opcode 10 not used */ 
lmp_packet_type_table_req_tx_handler, // TX handler for extended opcode 11
lmp_eSCO_link_req_tx_handler, // TX handler 
lmp_remove_eSCO_link_req_tx_handler, // TX handler 
NULL, /* Extended opcode 14 not used */ 
NULL, /* Extended opcode 15 not used */ 
lmp_channel_classification_req_tx_handler, // TX handler for extended opcode 16
lmp_channel_classification_tx_handler, // TX handler 
NULL, /* Extended opcode 18 not used */ 
NULL, /* Extended opcode 19 not used */ 
NULL, /* Extended opcode 20 not used */ 
lmp_sniff_subrating_req_tx_handler, // TX handler for extended opcode 21
lmp_sniff_subrating_res_tx_handler, // TX handler 
lmp_pause_encryption_req_tx_handler, // TX handler 
lmp_resume_encryption_req_tx_handler, // TX handler 
lmp_io_capability_req_tx_handler, // TX handler 
lmp_io_capability_res_tx_handler, // TX handler 
lmp_numeric_comparison_failed_tx_handler, // TX handler 
lmp_passkey_failed_tx_handler, // TX handler 
lmp_oob_failed_tx_handler, // TX handler 
lmp_keypress_notification_tx_handler, // TX handler 
lmp_power_control_req_tx_handler, // TX handler 
lmp_power_control_res_tx_handler, // TX handler 
lmp_ping_req_tx_handler, // TX handler 
lmp_ping_res_tx_handler, // TX handler 
lmp_sam_set_type0_tx_handler, // TX handler 
lmp_sam_define_map_tx_handler, // TX handler 
lmp_sam_switch_tx_handler, // TX handler 
};






const lmp_accepted_rx_handler_t lmp_accepted_lut[] = {
NULL, /* Index 0 is reserved */
NULL, /* Opcode 1 not used */ 
NULL, /* Opcode 2 not used */ 
NULL, /* Opcode 3 not used */ 
NULL, /* Opcode 4 not used */ 
NULL, /* Opcode 5 not used */ 
NULL, /* Opcode 6 not used */ 
NULL, /* Opcode 7 not used */ 
lmp_in_rand_accepted_rx_handler, // Accepted handler for opcode 8
NULL, /* Opcode 9 not used */ 
NULL, /* Opcode 10 not used */ 
NULL, /* Opcode 11 not used */ 
NULL, /* Opcode 12 not used */ 
NULL, /* Opcode 13 not used */ 
NULL, /* Opcode 14 not used */ 
lmp_encryption_mode_req_accepted_rx_handler, // Accepted handler for opcode 15
lmp_encryption_key_size_req_accepted_rx_handler, // Accepted handler 
lmp_start_encryption_req_accepted_rx_handler, // Accepted handler 
lmp_stop_encryption_req_accepted_rx_handler, // Accepted handler 
lmp_switch_req_accepted_rx_handler, // Accepted handler 
NULL, /* Opcode 20 not used */ 
NULL, /* Opcode 21 not used */ 
NULL, /* Opcode 22 not used */ 
lmp_sniff_req_accepted_rx_handler, // Accepted handler for opcode 23
lmp_unsniff_req_accepted_rx_handler, // Accepted handler 
NULL, /* Opcode 25 not used */ 
NULL, /* Opcode 26 not used */ 
NULL, /* Opcode 27 not used */ 
NULL, /* Opcode 28 not used */ 
NULL, /* Opcode 29 not used */ 
NULL, /* Opcode 30 not used */ 
NULL, /* Opcode 31 not used */ 
NULL, /* Opcode 32 not used */ 
NULL, /* Opcode 33 not used */ 
NULL, /* Opcode 34 not used */ 
NULL, /* Opcode 35 not used */ 
NULL, /* Opcode 36 not used */ 
NULL, /* Opcode 37 not used */ 
NULL, /* Opcode 38 not used */ 
NULL, /* Opcode 39 not used */ 
NULL, /* Opcode 40 not used */ 
NULL, /* Opcode 41 not used */ 
lmp_quality_of_service_req_accepted_rx_handler, // Accepted handler for opcode 42
NULL, /* Opcode 43 not used */ 
NULL, /* Opcode 44 not used */ 
NULL, /* Opcode 45 not used */ 
lmp_max_slot_req_accepted_rx_handler, // Accepted handler for opcode 46
NULL, /* Opcode 47 not used */ 
NULL, /* Opcode 48 not used */ 
NULL, /* Opcode 49 not used */ 
NULL, /* Opcode 50 not used */ 
lmp_host_connection_req_accepted_rx_handler, // Accepted handler for opcode 51
NULL, /* Opcode 52 not used */ 
NULL, /* Opcode 53 not used */ 
NULL, /* Opcode 54 not used */ 
NULL, /* Opcode 55 not used */ 
NULL, /* Opcode 56 not used */ 
NULL, /* Opcode 57 not used */ 
NULL, /* Opcode 58 not used */ 
NULL, /* Opcode 59 not used */ 
NULL, /* Opcode 60 not used */ 
lmp_encapsulated_header_accepted_rx_handler, // Accepted handler for opcode 61
lmp_encapsulated_payload_accepted_rx_handler, // Accepted handler 
NULL, /* Opcode 63 not used */ 
lmp_simple_pairing_number_accepted_rx_handler, // Accepted handler for opcode 64
lmp_dhkey_check_accepted_rx_handler, // Accepted handler 
};



const lmp_not_accepted_rx_handler_t lmp_not_accepted_lut[] = {
NULL, /* Index 0 is reserved */
NULL, /* Opcode 1 not used */ 
NULL, /* Opcode 2 not used */ 
NULL, /* Opcode 3 not used */ 
NULL, /* Opcode 4 not used */ 
NULL, /* Opcode 5 not used */ 
NULL, /* Opcode 6 not used */ 
NULL, /* Opcode 7 not used */ 
lmp_in_rand_not_accepted_rx_handler, // Not Accepted handler for opcode 8
NULL, /* Opcode 9 not used */ 
NULL, /* Opcode 10 not used */ 
lmp_au_rand_not_accepted_rx_handler, // Not Accepted handler for opcode 11
NULL, /* Opcode 12 not used */ 
NULL, /* Opcode 13 not used */ 
NULL, /* Opcode 14 not used */ 
lmp_encryption_mode_req_not_accepted_rx_handler, // Not Accepted handler for opcode 15
lmp_encryption_key_size_req_not_accepted_rx_handler, // Not Accepted handler 
lmp_start_encryption_req_not_accepted_rx_handler, // Not Accepted handler 
lmp_stop_encryption_req_not_accepted_rx_handler, // Not Accepted handler 
lmp_switch_req_not_accepted_rx_handler, // Not Accepted handler 
NULL, /* Opcode 20 not used */ 
NULL, /* Opcode 21 not used */ 
NULL, /* Opcode 22 not used */ 
lmp_sniff_req_not_accepted_rx_handler, // Not Accepted handler for opcode 23
NULL, /* Opcode 24 not used */ 
NULL, /* Opcode 25 not used */ 
NULL, /* Opcode 26 not used */ 
NULL, /* Opcode 27 not used */ 
NULL, /* Opcode 28 not used */ 
NULL, /* Opcode 29 not used */ 
NULL, /* Opcode 30 not used */ 
lmp_incr_power_req_not_accepted_rx_handler, // Not Accepted handler for opcode 31
lmp_decr_power_req_not_accepted_rx_handler, // Not Accepted handler 
lmp_max_power_not_accepted_rx_handler, // Not Accepted handler 
lmp_min_power_not_accepted_rx_handler, // Not Accepted handler 
NULL, /* Opcode 35 not used */ 
NULL, /* Opcode 36 not used */ 
NULL, /* Opcode 37 not used */ 
NULL, /* Opcode 38 not used */ 
NULL, /* Opcode 39 not used */ 
NULL, /* Opcode 40 not used */ 
NULL, /* Opcode 41 not used */ 
lmp_quality_of_service_req_not_accepted_rx_handler, // Not Accepted handler for opcode 42
NULL, /* Opcode 43 not used */ 
NULL, /* Opcode 44 not used */ 
NULL, /* Opcode 45 not used */ 
lmp_max_slot_req_not_accepted_rx_handler, // Not Accepted handler for opcode 46
NULL, /* Opcode 47 not used */ 
NULL, /* Opcode 48 not used */ 
NULL, /* Opcode 49 not used */ 
NULL, /* Opcode 50 not used */ 
lmp_host_connection_req_not_accepted_rx_handler, // Not Accepted handler for opcode 51
NULL, /* Opcode 52 not used */ 
NULL, /* Opcode 53 not used */ 
NULL, /* Opcode 54 not used */ 
NULL, /* Opcode 55 not used */ 
NULL, /* Opcode 56 not used */ 
NULL, /* Opcode 57 not used */ 
NULL, /* Opcode 58 not used */ 
NULL, /* Opcode 59 not used */ 
NULL, /* Opcode 60 not used */ 
lmp_encapsulated_header_not_accepted_rx_handler, // Not Accepted handler for opcode 61
lmp_encapsulated_payload_not_accepted_rx_handler, // Not Accepted handler 
lmp_simple_pairing_confirm_not_accepted_rx_handler, // Not Accepted handler 
lmp_simple_pairing_number_not_accepted_rx_handler, // Not Accepted handler 
lmp_dhkey_check_not_accepted_rx_handler, // Not Accepted handler 
};


const lmp_not_accepted_ext_rx_handler_t lmp_not_accepted_ext_lut[] = {
NULL, /* Index 0 is reserved */
NULL, /* Extended opcode 1 not used */ 
NULL, /* Extended opcode 2 not used */ 
NULL, /* Extended opcode 3 not used */ 
NULL, /* Extended opcode 4 not used */ 
NULL, /* Extended opcode 5 not used */ 
NULL, /* Extended opcode 6 not used */ 
NULL, /* Extended opcode 7 not used */ 
NULL, /* Extended opcode 8 not used */ 
NULL, /* Extended opcode 9 not used */ 
NULL, /* Extended opcode 10 not used */ 
lmp_packet_type_table_req_not_accepted_ext_rx_handler, // Not Accepted handler for extended opcode 11
lmp_eSCO_link_req_not_accepted_ext_rx_handler, // Not Accepted handler 
lmp_remove_eSCO_link_req_not_accepted_ext_rx_handler, // Not Accepted handler 
NULL, /* Extended opcode 14 not used */ 
NULL, /* Extended opcode 15 not used */ 
NULL, /* Extended opcode 16 not used */ 
NULL, /* Extended opcode 17 not used */ 
NULL, /* Extended opcode 18 not used */ 
NULL, /* Extended opcode 19 not used */ 
NULL, /* Extended opcode 20 not used */ 
lmp_sniff_subrating_req_not_accepted_ext_rx_handler, // Not Accepted handler for extended opcode 21
NULL, /* Extended opcode 22 not used */ 
lmp_pause_encryption_req_not_accepted_ext_rx_handler, // Not Accepted handler for extended opcode 23
NULL, /* Extended opcode 24 not used */ 
lmp_io_capability_req_not_accepted_ext_rx_handler, // Not Accepted handler for extended opcode 25
NULL, /* Extended opcode 26 not used */ 
NULL, /* Extended opcode 27 not used */ 
NULL, /* Extended opcode 28 not used */ 
NULL, /* Extended opcode 29 not used */ 
NULL, /* Extended opcode 30 not used */ 
lmp_power_control_req_not_accepted_ext_rx_handler, // Not Accepted handler for extended opcode 31
lmp_power_control_res_not_accepted_ext_rx_handler, // Not Accepted handler 
};



const lmp_accepted_ext_rx_handler_t lmp_accepted_ext_lut[] = {
NULL, /* Index 0 is reserved */
NULL, /* Extended opcode 1 not used */ 
NULL, /* Extended opcode 2 not used */ 
NULL, /* Extended opcode 3 not used */ 
NULL, /* Extended opcode 4 not used */ 
NULL, /* Extended opcode 5 not used */ 
NULL, /* Extended opcode 6 not used */ 
NULL, /* Extended opcode 7 not used */ 
NULL, /* Extended opcode 8 not used */ 
NULL, /* Extended opcode 9 not used */ 
NULL, /* Extended opcode 10 not used */ 
lmp_packet_type_table_req_accepted_ext_rx_handler, // RX handler for extended opcode 11
lmp_eSCO_link_req_accepted_ext_rx_handler, // RX handler 
lmp_remove_eSCO_link_req_accepted_ext_rx_handler, // RX handler 
};
