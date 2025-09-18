#ifndef SL_STRUCTS_BTC_SEQ_INTERFACE_H
#define SL_STRUCTS_BTC_SEQ_INTERFACE_H
 
#include <stdint.h>
void sl_btc_seq_interface_command_parser(uint8_t *out_buff, uint8_t *in_buff);
 
    /****
        command is used to initialize a BTC LPW device with parameters-
        bd_address of native device
    ****/
 
typedef struct hss_cmd_init_btc_device_struct {
    uint8_t bd_address[6];
    uint8_t sync_word[8];
} hss_cmd_init_btc_device_t;
    /****
        command is used to start page procedure with parameters
        clock_e_offset- estimated offset of clock to be used WRT native clock
        bd_address- bd_address of paging device
        remote_scan_rep_mode- ToDo Add description
    ****/
 
typedef struct hss_cmd_start_page_struct {
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_page_t;
 
typedef struct hss_cmd_start_page_scan_struct {
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_page_scan_t;
 
typedef struct hss_cmd_start_inquiry_struct {
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_inquiry_t;
 
typedef struct hss_cmd_start_inquiry_scan_struct {
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_inquiry_scan_t;
 
typedef struct hss_cmd_start_acl_central_struct {
    uint8_t connection_device_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_acl_central_t;
 
typedef struct hss_cmd_start_acl_peripheral_struct {
    uint8_t connection_device_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_acl_peripheral_t;
 
typedef struct hss_cmd_start_esco_central_struct {
    uint8_t connection_device_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_esco_central_t;
 
typedef struct hss_cmd_start_esco_peripheral_struct {
    uint8_t connection_device_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_esco_peripheral_t;
 
typedef struct hss_cmd_start_roleswitch_central_struct {
    uint8_t connection_device_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_roleswitch_central_t;
 
typedef struct hss_cmd_start_roleswitch_peripheral_struct {
    uint8_t connection_device_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t priority;
    uint8_t step_size;
    uint32_t end_tsf;
} hss_cmd_start_roleswitch_peripheral_t;
 
typedef struct hss_cmd_start_test_mode_struct {
    uint8_t bd_address[3];
    uint8_t sync_word[8];
    uint8_t br_edr_mode;
    uint8_t channel_index;
    uint8_t link_type;
    uint8_t inter_packet_gap;
    uint8_t scrambler_seed;
    uint8_t pkt_type;
    uint16_t pkt_len;
    uint8_t payload_type;
    uint8_t tx_power;
    uint8_t hopping_type;
    uint8_t loopback_mode;
    uint8_t transmit_mode;
    uint8_t encryption_mode;
    uint32_t no_of_pkts;
    uint32_t rx_window;
} hss_cmd_start_test_mode_t;
 
typedef struct hss_cmd_set_page_struct {
    uint32_t clk_e_offset;
    uint8_t hci_trigger;
    uint8_t tx_pwr_index;
    uint8_t remote_scan_rep_mode;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
    uint8_t lt_addr;
} hss_cmd_set_page_t;
 
typedef struct hss_cmd_set_page_scan_struct {
    uint32_t window;
    uint32_t interval;
    uint8_t curr_scan_type;
    uint8_t tx_pwr_index;
    uint8_t bd_address[6];
    uint8_t sync_word[8];
} hss_cmd_set_page_scan_t;
 
typedef struct hss_cmd_set_inquiry_struct {
    uint8_t lap_address[3];
    uint8_t sync_word[8];
    uint8_t hci_trigger;
    uint32_t length;
    uint8_t EIR_value;
    uint8_t tx_pwr_index;
} hss_cmd_set_inquiry_t;
 
typedef struct hss_cmd_set_E0_encryption_struct {
    uint32_t masterClock;
    uint32_t encKey0;
    uint32_t encKey1;
    uint32_t encKey2;
    uint32_t encKey3;
    uint32_t bdAddr0;
    uint16_t bdAddr1;
    uint8_t keyLength;
} hss_cmd_set_E0_encryption_t;
 
typedef struct hss_cmd_set_AES_encryption_struct {
    uint32_t aesKeyBytes_0_3;
    uint32_t aesKeyBytes_4_7;
    uint32_t aesKeyBytes_8_11;
    uint32_t aesKeyBytes_12_15;
    uint32_t aesIv1;
    uint32_t aesIv2;
    uint16_t dayCountAndDir;
    uint16_t zeroLenAclW;
    uint32_t aesPldCntr1;
    uint8_t aesPldCntr2;
} hss_cmd_set_AES_encryption_t;
 
typedef struct hss_cmd_set_aes_encryption_parameters_struct {
    uint8_t connection_device_index;
    uint8_t encryption_mode;
    uint32_t aesKeyBytes_0_3;
    uint32_t aesKeyBytes_4_7;
    uint32_t aesKeyBytes_8_11;
    uint32_t aesKeyBytes_12_15;
    uint32_t aesIv1;
    uint32_t aesIv2;
    uint16_t dayCountAndDir;
    uint16_t zeroLenAclW;
    uint32_t aesPldCntr1;
    uint8_t aesPldCntr2;
} hss_cmd_set_aes_encryption_parameters_t;
 
typedef struct hss_cmd_set_e0_encryption_parameters_struct {
    uint8_t connection_device_index;
    uint8_t encryption_mode;
    uint32_t masterClock;
    uint32_t encKey0;
    uint32_t encKey1;
    uint32_t encKey2;
    uint32_t encKey3;
    uint32_t bdAddr0;
    uint16_t bdAddr1;
    uint8_t keyLength;
} hss_cmd_set_e0_encryption_parameters_t;
 
typedef struct hss_cmd_set_inquiry_scan_struct {
    uint32_t window;
    uint32_t interval;
    uint32_t eir_data_length;
    uint8_t eir_data[260];
    uint8_t EIR_value;
    uint8_t lap_address[3];
    uint8_t sync_word[8];
    uint8_t curr_scan_type;
} hss_cmd_set_inquiry_scan_t;
 
typedef struct hss_cmd_set_afh_parameters_acl_struct {
    uint8_t connection_device_index;
    uint8_t no_of_valid_afh_channels;
    uint8_t piconet_type;
    uint8_t afh_new_channel_map[10];
    uint8_t channel_map_in_use[10];
    uint32_t afh_new_channel_map_instant;
} hss_cmd_set_afh_parameters_acl_t;
 
typedef struct hss_cmd_set_sniff_parameters_acl_struct {
    uint8_t connection_device_index;
    uint32_t sniff_instant;
    uint32_t sniff_interval;
    uint32_t sniff_attempt;
    uint32_t sniff_timeout;
} hss_cmd_set_sniff_parameters_acl_t;
 
typedef struct hss_cmd_set_device_parameters_acl_struct {
    uint8_t connection_device_index;
    uint8_t peer_role;
    uint8_t tx_pwr_index;
    uint8_t flow;
    uint8_t br_edr_mode;
} hss_cmd_set_device_parameters_acl_t;
 
typedef struct hss_cmd_set_test_mode_params_struct {
    uint8_t test_scenario;
    uint8_t hopping_mode;
    uint8_t tx_freq;
    uint8_t rx_freq;
    uint8_t pwr_cntrl_mode;
    uint8_t poll_period;
    uint16_t pkt_type;
    uint8_t link_type;
    uint8_t test_lt_type;
    uint8_t test_started;
    uint8_t test_mode_type;
    uint8_t sco_pkt_type;
    uint16_t len_of_seq;
} hss_cmd_set_test_mode_params_t;
 
typedef struct hss_cmd_set_device_parameters_esco_struct {
    uint8_t connection_device_index;
    uint8_t peer_role;
    uint8_t lt_addr;
    uint8_t flow;
    uint32_t t_esco;
    uint32_t d_esco;
    uint32_t w_esco;
    uint8_t is_esco;
    uint8_t is_edr;
} hss_cmd_set_device_parameters_esco_t;
 
typedef struct hss_cmd_set_device_parameters_roleswitch_struct {
    uint8_t connection_device_index;
    uint8_t peer_role;
    uint8_t lt_addr;
    uint8_t tx_pwr_index;
    uint8_t flow;
    uint32_t clk_e_offset;
} hss_cmd_set_device_parameters_roleswitch_t;
 
typedef struct hss_cmd_set_roleswitch_parameters_struct {
    uint8_t connection_device_index;
    uint8_t new_lt_addr;
    uint16_t roleswitch_slot_offset;
    uint32_t roleswitch_instant;
} hss_cmd_set_roleswitch_parameters_t;
 
typedef struct hss_cmd_set_fixed_btc_channel_struct {
    uint8_t channel;
} hss_cmd_set_fixed_btc_channel_t;
 
typedef struct hss_cmd_stop_page_struct {
    uint8_t current_device_index;
} hss_cmd_stop_page_t;
 
typedef struct hss_cmd_stop_inquiry_struct {
    uint8_t current_device_index;
} hss_cmd_stop_inquiry_t;
 
typedef struct hss_cmd_stop_page_scan_struct {
    uint8_t current_device_index;
} hss_cmd_stop_page_scan_t;
 
typedef struct hss_cmd_stop_inquiry_scan_struct {
    uint8_t current_device_index;
} hss_cmd_stop_inquiry_scan_t;
 
typedef struct hss_cmd_stop_acl_central_struct {
    uint8_t connection_device_index;
} hss_cmd_stop_acl_central_t;
 
typedef struct hss_cmd_stop_esco_central_struct {
    uint8_t connection_device_index;
} hss_cmd_stop_esco_central_t;
 
typedef struct hss_cmd_stop_roleswitch_central_struct {
    uint8_t connection_device_index;
} hss_cmd_stop_roleswitch_central_t;
 
typedef struct hss_cmd_stop_acl_peripheral_struct {
    uint8_t connection_device_index;
} hss_cmd_stop_acl_peripheral_t;
 
typedef struct hss_cmd_stop_esco_peripheral_struct {
    uint8_t connection_device_index;
} hss_cmd_stop_esco_peripheral_t;
 
typedef struct hss_cmd_stop_roleswitch_peripheral_struct {
    uint8_t connection_device_index;
} hss_cmd_stop_roleswitch_peripheral_t;
 
typedef struct hss_cmd_start_procedure_struct {
    uint8_t id;
} hss_cmd_start_procedure_t;
 
typedef struct hss_cmd_get_procedure_payload_struct {
    uint8_t id;
} hss_cmd_get_procedure_payload_t;
 
typedef struct hss_cmd_set_procedure_struct {
    uint8_t id;
} hss_cmd_set_procedure_t;
 
typedef struct hss_cmd_stop_procedure_struct {
    uint8_t id;
} hss_cmd_stop_procedure_t;
 
typedef struct hss_cmd_get_procedure_struct {
    uint8_t id;
} hss_cmd_get_procedure_t;
 
typedef struct hss_cmd_struct {
    uint8_t cmd_type;
} hss_cmd_t;
 
typedef struct hss_event_page_complete_struct {
    uint8_t connection_info_idx;
} hss_event_page_complete_t;
 
typedef struct hss_event_page_scan_complete_struct {
    uint8_t connection_info_idx;
    uint8_t remote_bd_address[6];
} hss_event_page_scan_complete_t;
 
typedef struct hss_event_btc_slot_offset_struct {
    uint16_t roleswitch_slot_offset;
} hss_event_btc_slot_offset_t;
 
typedef struct hss_event_struct {
    uint8_t event_type;
    uint8_t event_status;
} hss_event_t;
 
#endif