#include "host_to_lpw_interface.h"
#include "usched_acl.h"
#include "pkb_mgmt.h"
#include "access_code_gen.h"
#ifndef SIM
#include "pti.h"
#include "rail_seq.h"
#endif

shared_mem_t host_to_lpw_command_queue;
pkb_pool_t pkb_pool_g[num_supported_pkb_chunk_type];
/**
 * @brief All the handlers below take the input_buffer from host and then fill
 * the cmd_id and cmd_type in the payload in the buffer on the basis of which
 * further start,stop,set,get handlers of respective state machines are called.
 *
 * @param payload
 * @return void*
 */
void set_common_hss_cmd_fields(uint8_t *hss_cmd, uint8_t cmd_type, uint8_t procedure_id)
{
  *HSS_CMD_CMD_TYPE(hss_cmd) = cmd_type;
  *HSS_CMD_PAYLOAD_START_PROCEDURE_ID(hss_cmd) = procedure_id;
}

void prep_hss_cmd_start_page(uint8_t *hss_cmd, hss_cmd_start_page_t *start_page_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_page);
  *HSS_CMD_START_PAGE_STEP_SIZE(hss_cmd) = start_page_args->step_size;
  *HSS_CMD_START_PAGE_PRIORITY(hss_cmd) = start_page_args->priority;
  *HSS_CMD_START_PAGE_END_TSF(hss_cmd) = start_page_args->end_tsf;
}

void prep_hss_cmd_start_page_scan(uint8_t *hss_cmd, hss_cmd_start_page_scan_t *start_page_scan_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_page_scan);
  *HSS_CMD_START_PAGE_SCAN_PRIORITY(hss_cmd) = start_page_scan_args->priority;
  *HSS_CMD_START_PAGE_SCAN_STEP_SIZE(hss_cmd) = start_page_scan_args->step_size;
  *HSS_CMD_START_PAGE_SCAN_END_TSF(hss_cmd) = start_page_scan_args->end_tsf;
}

void prep_hss_cmd_start_test_mode(uint8_t *hss_cmd, hss_cmd_start_test_mode_t *start_test_mode_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_test_mode);
  *HSS_CMD_START_TEST_MODE_TRANSMIT_MODE(hss_cmd) = start_test_mode_args->transmit_mode;
  *HSS_CMD_START_TEST_MODE_BR_EDR_MODE(hss_cmd) = start_test_mode_args->br_edr_mode;
  *HSS_CMD_START_TEST_MODE_PKT_TYPE(hss_cmd) = start_test_mode_args->pkt_type;
  *HSS_CMD_START_TEST_MODE_PKT_LEN(hss_cmd) = start_test_mode_args->pkt_len;
  *HSS_CMD_START_TEST_MODE_PAYLOAD_TYPE(hss_cmd) = start_test_mode_args->payload_type;
  *HSS_CMD_START_TEST_MODE_NO_OF_PKTS(hss_cmd) = start_test_mode_args->no_of_pkts;
  *HSS_CMD_START_TEST_MODE_INTER_PACKET_GAP(hss_cmd) = start_test_mode_args->inter_packet_gap;
  *HSS_CMD_START_TEST_MODE_LINK_TYPE(hss_cmd) = start_test_mode_args->link_type;
  *HSS_CMD_START_TEST_MODE_LOOPBACK_MODE(hss_cmd) = start_test_mode_args->loopback_mode;
  *HSS_CMD_START_TEST_MODE_SCRAMBLER_SEED(hss_cmd) = start_test_mode_args->scrambler_seed;
  *HSS_CMD_START_TEST_MODE_RX_WINDOW(hss_cmd) = start_test_mode_args->rx_window;
  memcpy(HSS_CMD_START_TEST_MODE_BD_ADDRESS(hss_cmd), start_test_mode_args->bd_address, sizeof(start_test_mode_args->bd_address));
  access_code_synth((uint32_t *)start_test_mode_args->bd_address, (uint32_t *)start_test_mode_args->sync_word);
  memcpy(HSS_CMD_START_TEST_MODE_SYNC_WORD(hss_cmd), start_test_mode_args->sync_word, sizeof(start_test_mode_args->sync_word));
}

void prep_hss_cmd_set_test_mode_params(uint8_t *hss_cmd, hss_cmd_set_test_mode_params_t *set_test_mode_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_test_mode_params);
  *HSS_CMD_SET_TEST_MODE_PARAMS_TEST_SCENARIO(hss_cmd)      = set_test_mode_args->test_scenario;
  *HSS_CMD_SET_TEST_MODE_PARAMS_HOPPING_MODE(hss_cmd)       = set_test_mode_args->hopping_mode;
  *HSS_CMD_SET_TEST_MODE_PARAMS_TX_FREQ(hss_cmd)            = set_test_mode_args->tx_freq;
  *HSS_CMD_SET_TEST_MODE_PARAMS_RX_FREQ(hss_cmd)            = set_test_mode_args->rx_freq;
  *HSS_CMD_SET_TEST_MODE_PARAMS_PWR_CNTRL_MODE(hss_cmd)     = set_test_mode_args->pwr_cntrl_mode;
  *HSS_CMD_SET_TEST_MODE_PARAMS_POLL_PERIOD(hss_cmd)        = set_test_mode_args->poll_period;
  *HSS_CMD_SET_TEST_MODE_PARAMS_PKT_TYPE(hss_cmd)           = set_test_mode_args->pkt_type;
  *HSS_CMD_SET_TEST_MODE_PARAMS_LINK_TYPE(hss_cmd)          = set_test_mode_args->link_type;
  *HSS_CMD_SET_TEST_MODE_PARAMS_TEST_LT_TYPE(hss_cmd)       = set_test_mode_args->test_lt_type;
  *HSS_CMD_SET_TEST_MODE_PARAMS_TEST_STARTED(hss_cmd)       = set_test_mode_args->test_started;
  *HSS_CMD_SET_TEST_MODE_PARAMS_TEST_MODE_TYPE(hss_cmd)     = set_test_mode_args->test_mode_type;
  *HSS_CMD_SET_TEST_MODE_PARAMS_LEN_OF_SEQ(hss_cmd)         = set_test_mode_args->len_of_seq;
}

void prep_hss_cmd_start_inquiry(uint8_t *hss_cmd, hss_cmd_start_inquiry_t *start_inquiry_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_inquiry);
  *HSS_CMD_START_INQUIRY_PRIORITY(hss_cmd) = start_inquiry_args->priority;
  *HSS_CMD_START_INQUIRY_STEP_SIZE(hss_cmd) = start_inquiry_args->step_size;
  *HSS_CMD_START_INQUIRY_END_TSF(hss_cmd) = start_inquiry_args->end_tsf;
}

void prep_hss_cmd_start_inquiry_scan(uint8_t *hss_cmd, hss_cmd_start_inquiry_scan_t *start_inquiry_scan_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_inquiry_scan);
  *HSS_CMD_START_INQUIRY_SCAN_PRIORITY(hss_cmd) = start_inquiry_scan_args->priority;
  *HSS_CMD_START_INQUIRY_SCAN_STEP_SIZE(hss_cmd) = start_inquiry_scan_args->step_size;
  *HSS_CMD_START_INQUIRY_SCAN_END_TSF(hss_cmd) = start_inquiry_scan_args->end_tsf;
}

void prep_hss_cmd_start_acl_central(uint8_t *hss_cmd, hss_cmd_start_acl_central_t *start_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_acl_central);
  *HSS_CMD_START_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd) = start_acl_args->connection_device_index;
  memcpy(HSS_CMD_START_ACL_CENTRAL_BD_ADDRESS(hss_cmd), start_acl_args->bd_address, sizeof(start_acl_args->bd_address));
  access_code_synth((uint32_t *)start_acl_args->bd_address, (uint32_t *)start_acl_args->sync_word);
  memcpy(HSS_CMD_START_ACL_CENTRAL_SYNC_WORD(hss_cmd), start_acl_args->sync_word, sizeof(start_acl_args->sync_word));
  *HSS_CMD_START_ACL_CENTRAL_PRIORITY(hss_cmd) = start_acl_args->priority;
  *HSS_CMD_START_ACL_CENTRAL_STEP_SIZE(hss_cmd) = start_acl_args->step_size;
  *HSS_CMD_START_ACL_CENTRAL_END_TSF(hss_cmd) = start_acl_args->end_tsf;
}

void prep_hss_cmd_start_acl_peripheral(uint8_t *hss_cmd, hss_cmd_start_acl_peripheral_t *start_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_acl_peripheral);
  *HSS_CMD_START_ACL_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd) = start_acl_args->connection_device_index;
  memcpy(HSS_CMD_START_ACL_PERIPHERAL_BD_ADDRESS(hss_cmd), start_acl_args->bd_address, sizeof(start_acl_args->bd_address));
  access_code_synth((uint32_t *)start_acl_args->bd_address, (uint32_t *)start_acl_args->sync_word);
  memcpy(HSS_CMD_START_ACL_PERIPHERAL_SYNC_WORD(hss_cmd), start_acl_args->sync_word, sizeof(start_acl_args->sync_word));
  *HSS_CMD_START_ACL_PERIPHERAL_PRIORITY(hss_cmd) = start_acl_args->priority;
  *HSS_CMD_START_ACL_PERIPHERAL_STEP_SIZE(hss_cmd) = start_acl_args->step_size;
  *HSS_CMD_START_ACL_PERIPHERAL_END_TSF(hss_cmd) = start_acl_args->end_tsf;
}

void prep_hss_cmd_start_esco_central(uint8_t *hss_cmd, hss_cmd_start_esco_central_t *start_esco_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_esco_central);
  *HSS_CMD_START_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd) = start_esco_args->connection_device_index;
  *HSS_CMD_START_ESCO_CENTRAL_PRIORITY(hss_cmd) = start_esco_args->priority;
  *HSS_CMD_START_ESCO_CENTRAL_STEP_SIZE(hss_cmd) = start_esco_args->step_size;
  *HSS_CMD_START_ESCO_CENTRAL_END_TSF(hss_cmd) = start_esco_args->end_tsf;
  memcpy(HSS_CMD_START_ESCO_CENTRAL_BD_ADDRESS(hss_cmd), start_esco_args->bd_address, sizeof(start_esco_args->bd_address));
  access_code_synth((uint32_t *)start_esco_args->bd_address, (uint32_t *)start_esco_args->sync_word);
  memcpy(HSS_CMD_START_ESCO_CENTRAL_SYNC_WORD(hss_cmd), start_esco_args->sync_word, sizeof(start_esco_args->sync_word));
}

void prep_hss_cmd_start_esco_peripheral(uint8_t *hss_cmd, hss_cmd_start_esco_peripheral_t *start_esco_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_esco_peripheral);
  *HSS_CMD_START_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd) = start_esco_args->connection_device_index;
  *HSS_CMD_START_ESCO_PERIPHERAL_PRIORITY(hss_cmd) = start_esco_args->priority;
  *HSS_CMD_START_ESCO_PERIPHERAL_STEP_SIZE(hss_cmd) = start_esco_args->step_size;
  *HSS_CMD_START_ESCO_PERIPHERAL_END_TSF(hss_cmd) = start_esco_args->end_tsf;
  memcpy(HSS_CMD_START_ESCO_PERIPHERAL_BD_ADDRESS(hss_cmd), start_esco_args->bd_address, sizeof(start_esco_args->bd_address));
  access_code_synth((uint32_t *)start_esco_args->bd_address, (uint32_t *)start_esco_args->sync_word);
  memcpy(HSS_CMD_START_ESCO_PERIPHERAL_SYNC_WORD(hss_cmd), start_esco_args->sync_word, sizeof(start_esco_args->sync_word));
}

void prep_hss_cmd_init_btc_device(uint8_t *hss_cmd, hss_cmd_init_btc_device_t *init_btc_device_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_init_btc_device);
  memcpy(HSS_CMD_INIT_BTC_DEVICE_BD_ADDRESS(hss_cmd), init_btc_device_args->bd_address, sizeof(init_btc_device_args->bd_address));
  access_code_synth((uint32_t *)init_btc_device_args->bd_address, (uint32_t *)init_btc_device_args->sync_word);
  memcpy(HSS_CMD_INIT_BTC_DEVICE_SYNC_WORD(hss_cmd), init_btc_device_args->sync_word, sizeof(init_btc_device_args->sync_word));
}

void prep_hss_cmd_start_roleswitch_central(uint8_t *hss_cmd, hss_cmd_start_roleswitch_central_t *start_roleswitch_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_roleswitch_central);
  *HSS_CMD_START_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd) = start_roleswitch_args->connection_device_index;
  *HSS_CMD_START_ROLESWITCH_CENTRAL_PRIORITY(hss_cmd) = start_roleswitch_args->priority;
  *HSS_CMD_START_ROLESWITCH_CENTRAL_STEP_SIZE(hss_cmd) = start_roleswitch_args->step_size;
  *HSS_CMD_START_ROLESWITCH_CENTRAL_END_TSF(hss_cmd) = start_roleswitch_args->end_tsf;
  memcpy(HSS_CMD_START_ROLESWITCH_CENTRAL_BD_ADDRESS(hss_cmd), start_roleswitch_args->bd_address, sizeof(start_roleswitch_args->bd_address));
  access_code_synth((uint32_t *)start_roleswitch_args->bd_address, (uint32_t *)start_roleswitch_args->sync_word);
  memcpy(HSS_CMD_START_ROLESWITCH_CENTRAL_SYNC_WORD(hss_cmd), start_roleswitch_args->sync_word, sizeof(start_roleswitch_args->sync_word));
}

void prep_hss_cmd_start_roleswitch_peripheral(uint8_t *hss_cmd, hss_cmd_start_roleswitch_peripheral_t *start_roleswitch_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_start_procedure, hss_cmd_start_roleswitch_peripheral);
  *HSS_CMD_START_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd) = start_roleswitch_args->connection_device_index;
  *HSS_CMD_START_ROLESWITCH_PERIPHERAL_PRIORITY(hss_cmd) = start_roleswitch_args->priority;
  *HSS_CMD_START_ROLESWITCH_PERIPHERAL_STEP_SIZE(hss_cmd) = start_roleswitch_args->step_size;
  *HSS_CMD_START_ROLESWITCH_PERIPHERAL_END_TSF(hss_cmd) = start_roleswitch_args->end_tsf;
  memcpy(HSS_CMD_START_ROLESWITCH_PERIPHERAL_BD_ADDRESS(hss_cmd), start_roleswitch_args->bd_address, sizeof(start_roleswitch_args->bd_address));
  access_code_synth((uint32_t *)start_roleswitch_args->bd_address, (uint32_t *)start_roleswitch_args->sync_word);
  memcpy(HSS_CMD_START_ROLESWITCH_PERIPHERAL_SYNC_WORD(hss_cmd), start_roleswitch_args->sync_word, sizeof(start_roleswitch_args->sync_word));
}

void prep_hss_cmd_set_page(uint8_t *hss_cmd, hss_cmd_set_page_t *set_page_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_page);
  *HSS_CMD_SET_PAGE_CLK_E_OFFSET(hss_cmd) = set_page_args->clk_e_offset;
  *HSS_CMD_SET_PAGE_HCI_TRIGGER(hss_cmd) = set_page_args->hci_trigger;
  *HSS_CMD_SET_PAGE_TX_PWR_INDEX(hss_cmd) = set_page_args->tx_pwr_index;
  *HSS_CMD_SET_PAGE_REMOTE_SCAN_REP_MODE(hss_cmd) = set_page_args->remote_scan_rep_mode;
  *HSS_CMD_SET_PAGE_LT_ADDR(hss_cmd) = set_page_args->lt_addr;
  memcpy(HSS_CMD_SET_PAGE_BD_ADDRESS(hss_cmd), set_page_args->bd_address, sizeof(set_page_args->bd_address));
  access_code_synth((uint32_t *)set_page_args->bd_address, (uint32_t *)set_page_args->sync_word);
  memcpy(HSS_CMD_SET_PAGE_SYNC_WORD(hss_cmd), set_page_args->sync_word, sizeof(set_page_args->sync_word));
}

void prep_hss_cmd_set_page_scan(uint8_t *hss_cmd, hss_cmd_set_page_scan_t *set_page_scan_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_page_scan);
  *HSS_CMD_SET_PAGE_SCAN_WINDOW(hss_cmd) = set_page_scan_args->window;
  *HSS_CMD_SET_PAGE_SCAN_INTERVAL(hss_cmd) = set_page_scan_args->interval;
  *HSS_CMD_SET_PAGE_SCAN_CURR_SCAN_TYPE(hss_cmd) = set_page_scan_args->curr_scan_type;
  *HSS_CMD_SET_PAGE_SCAN_TX_PWR_INDEX(hss_cmd) = set_page_scan_args->tx_pwr_index;
  memcpy(HSS_CMD_SET_PAGE_SCAN_BD_ADDRESS(hss_cmd), set_page_scan_args->bd_address, sizeof(set_page_scan_args->bd_address));
  access_code_synth((uint32_t *)set_page_scan_args->bd_address, (uint32_t *)set_page_scan_args->sync_word);
  memcpy(HSS_CMD_SET_PAGE_SCAN_SYNC_WORD(hss_cmd), set_page_scan_args->sync_word, sizeof(set_page_scan_args->sync_word));
}

void prep_hss_cmd_set_inquiry(uint8_t *hss_cmd, hss_cmd_set_inquiry_t *set_inquiry_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_inquiry);
  *HSS_CMD_SET_INQUIRY_LENGTH(hss_cmd) = set_inquiry_args->length;
  *HSS_CMD_SET_INQUIRY_HCI_TRIGGER(hss_cmd) = set_inquiry_args->hci_trigger;
  *HSS_CMD_SET_INQUIRY_EIR_VALUE(hss_cmd) = set_inquiry_args->EIR_value;
  *HSS_CMD_SET_INQUIRY_TX_PWR_INDEX(hss_cmd) = set_inquiry_args->tx_pwr_index;
  memcpy(HSS_CMD_SET_INQUIRY_LAP_ADDRESS(hss_cmd), set_inquiry_args->lap_address, sizeof(set_inquiry_args->lap_address));
  access_code_synth((uint32_t *)set_inquiry_args->lap_address, (uint32_t *)set_inquiry_args->sync_word);
  memcpy(HSS_CMD_SET_INQUIRY_SYNC_WORD(hss_cmd), set_inquiry_args->sync_word, sizeof(set_inquiry_args->sync_word));
}

void prep_hss_cmd_set_e0_encryption(uint8_t *hss_cmd, hss_cmd_set_E0_encryption_t *set_e0_encryption_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_encryption);
  *HSS_CMD_START_TEST_MODE_ENCRYPTION_MODE(hss_cmd) = 1;
  *HSS_CMD_SET_E0_ENCRYPTION_MASTERCLOCK(hss_cmd) = set_e0_encryption_args->masterClock;
  *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY0(hss_cmd)    = set_e0_encryption_args->encKey0;
  *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY1(hss_cmd)    = set_e0_encryption_args->encKey1;
  *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY2(hss_cmd)    = set_e0_encryption_args->encKey2;
  *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY3(hss_cmd)    = set_e0_encryption_args->encKey3;
  *HSS_CMD_SET_E0_ENCRYPTION_BDADDR0(hss_cmd)    = set_e0_encryption_args->bdAddr0;
  *HSS_CMD_SET_E0_ENCRYPTION_BDADDR1(hss_cmd)    = set_e0_encryption_args->bdAddr1;
  *HSS_CMD_SET_E0_ENCRYPTION_KEYLENGTH(hss_cmd)  = set_e0_encryption_args->keyLength;
}

void prep_hss_cmd_set_aes_encryption(uint8_t *hss_cmd, hss_cmd_set_AES_encryption_t *set_aes_encryption_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_encryption);
  *HSS_CMD_START_TEST_MODE_ENCRYPTION_MODE(hss_cmd) = 2;
  *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_0_3(hss_cmd) = set_aes_encryption_args->aesKeyBytes_0_3;
  *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_4_7(hss_cmd) = set_aes_encryption_args->aesKeyBytes_4_7;
  *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_8_11(hss_cmd) = set_aes_encryption_args->aesKeyBytes_8_11;
  *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_12_15(hss_cmd) = set_aes_encryption_args->aesKeyBytes_12_15;
  *HSS_CMD_SET_AES_ENCRYPTION_AESIV1(hss_cmd)         = set_aes_encryption_args->aesIv1;
  *HSS_CMD_SET_AES_ENCRYPTION_AESIV2(hss_cmd)         = set_aes_encryption_args->aesIv2;
  *HSS_CMD_SET_AES_ENCRYPTION_DAYCOUNTANDDIR(hss_cmd) = set_aes_encryption_args->dayCountAndDir;
  *HSS_CMD_SET_AES_ENCRYPTION_ZEROLENACLW(hss_cmd)    = set_aes_encryption_args->zeroLenAclW;
  *HSS_CMD_SET_AES_ENCRYPTION_AESPLDCNTR1(hss_cmd)    = set_aes_encryption_args->aesPldCntr1;
  *HSS_CMD_SET_AES_ENCRYPTION_AESPLDCNTR2(hss_cmd)    = set_aes_encryption_args->aesPldCntr2;
}

void prep_hss_cmd_set_e0_encryption_parameters(uint8_t *hss_cmd, hss_cmd_set_e0_encryption_parameters_t *set_e0_encryption_parameters_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_encryption);
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd) = set_e0_encryption_parameters_args->connection_device_index;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCRYPTION_MODE(hss_cmd) = set_e0_encryption_parameters_args->encryption_mode;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_MASTERCLOCK(hss_cmd) = set_e0_encryption_parameters_args->masterClock;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY0(hss_cmd) = set_e0_encryption_parameters_args->encKey0;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY1(hss_cmd) = set_e0_encryption_parameters_args->encKey1;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY2(hss_cmd) = set_e0_encryption_parameters_args->encKey2;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_ENCKEY3(hss_cmd) = set_e0_encryption_parameters_args->encKey3;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_BDADDR0(hss_cmd) = set_e0_encryption_parameters_args->bdAddr0;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_BDADDR1(hss_cmd) = set_e0_encryption_parameters_args->bdAddr1;
  *HSS_CMD_SET_E0_ENCRYPTION_PARAMETERS_KEYLENGTH(hss_cmd) = set_e0_encryption_parameters_args->keyLength;
}

void prep_hss_cmd_set_aes_encryption_parameters(uint8_t *hss_cmd, hss_cmd_set_aes_encryption_parameters_t *set_aes_encryption_parameters_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_encryption);
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd) = set_aes_encryption_parameters_args->connection_device_index;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_ENCRYPTION_MODE(hss_cmd) = set_aes_encryption_parameters_args->encryption_mode;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_0_3(hss_cmd) = set_aes_encryption_parameters_args->aesKeyBytes_0_3;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_4_7(hss_cmd) = set_aes_encryption_parameters_args->aesKeyBytes_4_7;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_8_11(hss_cmd) = set_aes_encryption_parameters_args->aesKeyBytes_8_11;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESKEYBYTES_12_15(hss_cmd) = set_aes_encryption_parameters_args->aesKeyBytes_12_15;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESIV1(hss_cmd) = set_aes_encryption_parameters_args->aesIv1;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESIV2(hss_cmd) = set_aes_encryption_parameters_args->aesIv2;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_DAYCOUNTANDDIR(hss_cmd) = set_aes_encryption_parameters_args->dayCountAndDir;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_ZEROLENACLW(hss_cmd) = set_aes_encryption_parameters_args->zeroLenAclW;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESPLDCNTR1(hss_cmd) = set_aes_encryption_parameters_args->aesPldCntr1;
  *HSS_CMD_SET_AES_ENCRYPTION_PARAMETERS_AESPLDCNTR2(hss_cmd) = set_aes_encryption_parameters_args->aesPldCntr2;
}

void prep_hss_cmd_set_inquiry_scan(uint8_t *hss_cmd, hss_cmd_set_inquiry_scan_t *set_inquiry_scan_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_inquiry_scan);
  *HSS_CMD_SET_INQUIRY_SCAN_WINDOW(hss_cmd) = set_inquiry_scan_args->window;
  *HSS_CMD_SET_INQUIRY_SCAN_INTERVAL(hss_cmd) = set_inquiry_scan_args->interval;
  *HSS_CMD_SET_INQUIRY_SCAN_CURR_SCAN_TYPE(hss_cmd) = set_inquiry_scan_args->curr_scan_type;
  *HSS_CMD_SET_INQUIRY_SCAN_EIR_VALUE(hss_cmd) = set_inquiry_scan_args->EIR_value;
  *HSS_CMD_SET_INQUIRY_SCAN_EIR_DATA_LENGTH(hss_cmd) = set_inquiry_scan_args->eir_data_length;
  memcpy(HSS_CMD_SET_INQUIRY_SCAN_EIR_DATA(hss_cmd), set_inquiry_scan_args->eir_data, set_inquiry_scan_args->eir_data_length);
  memcpy(HSS_CMD_SET_INQUIRY_SCAN_LAP_ADDRESS(hss_cmd), set_inquiry_scan_args->lap_address, sizeof(set_inquiry_scan_args->lap_address));
  access_code_synth((uint32_t *)set_inquiry_scan_args->lap_address, (uint32_t *)set_inquiry_scan_args->sync_word);
  memcpy(HSS_CMD_SET_INQUIRY_SCAN_SYNC_WORD(hss_cmd), set_inquiry_scan_args->sync_word, sizeof(set_inquiry_scan_args->sync_word));
}

void prep_hss_cmd_set_afh_parameters_acl(uint8_t *hss_cmd, hss_cmd_set_afh_parameters_acl_t *set_afh_parameters_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_afh_parameters_acl);
  *HSS_CMD_SET_AFH_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd) = set_afh_parameters_acl_args->connection_device_index;
  *HSS_CMD_SET_AFH_PARAMETERS_ACL_NO_OF_VALID_AFH_CHANNELS(hss_cmd) = set_afh_parameters_acl_args->no_of_valid_afh_channels;
  *HSS_CMD_SET_AFH_PARAMETERS_ACL_PICONET_TYPE(hss_cmd) = set_afh_parameters_acl_args->piconet_type;
  *HSS_CMD_SET_AFH_PARAMETERS_ACL_AFH_NEW_CHANNEL_MAP_INSTANT(hss_cmd) = set_afh_parameters_acl_args->afh_new_channel_map_instant;
  memcpy(HSS_CMD_SET_AFH_PARAMETERS_ACL_AFH_NEW_CHANNEL_MAP(hss_cmd), set_afh_parameters_acl_args->afh_new_channel_map, sizeof(set_afh_parameters_acl_args->afh_new_channel_map));
  memcpy(HSS_CMD_SET_AFH_PARAMETERS_ACL_CHANNEL_MAP_IN_USE(hss_cmd), set_afh_parameters_acl_args->channel_map_in_use, sizeof(set_afh_parameters_acl_args->channel_map_in_use));
}

void prep_hss_cmd_set_sniff_parameters_acl(uint8_t *hss_cmd, hss_cmd_set_sniff_parameters_acl_t *set_sniff_parameters_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_sniff_parameters_acl);
  *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd) = set_sniff_parameters_acl_args->connection_device_index;
  *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_INSTANT(hss_cmd) = set_sniff_parameters_acl_args->sniff_instant;
  *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_INTERVAL(hss_cmd) = set_sniff_parameters_acl_args->sniff_interval;
  *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_ATTEMPT(hss_cmd) = set_sniff_parameters_acl_args->sniff_attempt;
  *HSS_CMD_SET_SNIFF_PARAMETERS_ACL_SNIFF_TIMEOUT(hss_cmd) = set_sniff_parameters_acl_args->sniff_timeout;
}

void prep_hss_cmd_set_device_parameters_acl(uint8_t *hss_cmd, hss_cmd_set_device_parameters_acl_t *set_device_parameters_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_device_parameters_acl);
  *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_CONNECTION_DEVICE_INDEX(hss_cmd) = set_device_parameters_acl_args->connection_device_index;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_TX_PWR_INDEX(hss_cmd) = set_device_parameters_acl_args->tx_pwr_index;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_PEER_ROLE(hss_cmd) = set_device_parameters_acl_args->peer_role;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_FLOW(hss_cmd) = set_device_parameters_acl_args->flow;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ACL_BR_EDR_MODE(hss_cmd) = set_device_parameters_acl_args->br_edr_mode;
}

void prep_hss_cmd_set_device_parameters_esco(uint8_t *hss_cmd, hss_cmd_set_device_parameters_esco_t *set_device_parameters_esco)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_device_parameters_esco);
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_CONNECTION_DEVICE_INDEX(hss_cmd) = set_device_parameters_esco->connection_device_index;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_FLOW(hss_cmd) = set_device_parameters_esco->flow;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_LT_ADDR(hss_cmd) = set_device_parameters_esco->lt_addr;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_T_ESCO(hss_cmd) = set_device_parameters_esco->t_esco;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_D_ESCO(hss_cmd) = set_device_parameters_esco->d_esco;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_W_ESCO(hss_cmd) = set_device_parameters_esco->w_esco;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ESCO_PEER_ROLE(hss_cmd) = set_device_parameters_esco->peer_role;
}

void prep_hss_cmd_set_device_parameters_roleswitch(uint8_t *hss_cmd, hss_cmd_set_device_parameters_roleswitch_t *set_device_parameters_roleswitch)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_device_parameters_roleswitch);
  *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_CONNECTION_DEVICE_INDEX(hss_cmd) = set_device_parameters_roleswitch->connection_device_index;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_CLK_E_OFFSET(hss_cmd) = set_device_parameters_roleswitch->clk_e_offset;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_FLOW(hss_cmd) = set_device_parameters_roleswitch->flow;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_LT_ADDR(hss_cmd) = set_device_parameters_roleswitch->lt_addr;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_PEER_ROLE(hss_cmd) = set_device_parameters_roleswitch->peer_role;
  *HSS_CMD_SET_DEVICE_PARAMETERS_ROLESWITCH_TX_PWR_INDEX(hss_cmd) = set_device_parameters_roleswitch->tx_pwr_index;
}

void prep_hss_cmd_set_roleswitch_parameters(uint8_t *hss_cmd, hss_cmd_set_roleswitch_parameters_t *set_roleswitch_parameters)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_roleswitch_parameters);
  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_CONNECTION_DEVICE_INDEX(hss_cmd) = set_roleswitch_parameters->connection_device_index;
  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_NEW_LT_ADDR(hss_cmd) = set_roleswitch_parameters->new_lt_addr;
  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_INSTANT(hss_cmd) = set_roleswitch_parameters->roleswitch_instant;
  *HSS_CMD_SET_ROLESWITCH_PARAMETERS_ROLESWITCH_SLOT_OFFSET(hss_cmd) = set_roleswitch_parameters->roleswitch_slot_offset;
}

void prep_hss_cmd_set_fixed_btc_channel(uint8_t *hss_cmd, hss_cmd_set_fixed_btc_channel_t *set_fixed_btc_channel)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_set_procedure, hss_cmd_set_fixed_btc_channel);
  *HSS_CMD_SET_FIXED_BTC_CHANNEL_CHANNEL(hss_cmd) = set_fixed_btc_channel->channel;
}

void prep_hss_cmd_stop_page(uint8_t *hss_cmd, hss_cmd_stop_page_t *stop_page_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_page);
  *HSS_CMD_STOP_PAGE_CURRENT_DEVICE_INDEX(hss_cmd) = stop_page_args->current_device_index;
}

void prep_hss_cmd_stop_inquiry(uint8_t *hss_cmd, hss_cmd_stop_inquiry_t *stop_inquiry_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_inquiry);
  *HSS_CMD_STOP_INQUIRY_CURRENT_DEVICE_INDEX(hss_cmd) = stop_inquiry_args->current_device_index;
}

void prep_hss_cmd_stop_page_scan(uint8_t *hss_cmd, hss_cmd_stop_page_scan_t *stop_page_scan_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_page_scan);
  *HSS_CMD_STOP_PAGE_SCAN_CURRENT_DEVICE_INDEX(hss_cmd) = stop_page_scan_args->current_device_index;
}

void prep_hss_cmd_stop_inquiry_scan(uint8_t *hss_cmd, hss_cmd_stop_inquiry_scan_t *stop_inquiry_scan_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_inquiry_scan);
  *HSS_CMD_STOP_INQUIRY_SCAN_CURRENT_DEVICE_INDEX(hss_cmd) = stop_inquiry_scan_args->current_device_index;
}

void prep_hss_cmd_stop_acl_central(uint8_t *hss_cmd, hss_cmd_stop_acl_central_t *stop_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_acl_central);
  *HSS_CMD_STOP_ACL_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd) = stop_acl_args->connection_device_index;
}

void prep_hss_cmd_stop_acl_peripheral(uint8_t *hss_cmd, hss_cmd_stop_acl_peripheral_t *stop_acl_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_acl_peripheral);
  *HSS_CMD_STOP_ACL_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd) = stop_acl_args->connection_device_index;
}

void prep_hss_cmd_stop_esco_central(uint8_t *hss_cmd, hss_cmd_stop_esco_central_t *stop_esco_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_esco_central);
  *HSS_CMD_STOP_ESCO_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd) = stop_esco_args->connection_device_index;
}

void prep_hss_cmd_stop_esco_peripheral(uint8_t *hss_cmd, hss_cmd_stop_esco_peripheral_t *stop_esco_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_esco_peripheral);
  *HSS_CMD_STOP_ESCO_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd) = stop_esco_args->connection_device_index;
}

void prep_hss_cmd_stop_roleswitch_central(uint8_t *hss_cmd, hss_cmd_stop_roleswitch_central_t *stop_roleswitch_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_roleswitch_central);
  *HSS_CMD_STOP_ROLESWITCH_CENTRAL_CONNECTION_DEVICE_INDEX(hss_cmd) = stop_roleswitch_args->connection_device_index;
}

void prep_hss_cmd_stop_roleswitch_peripheral(uint8_t *hss_cmd, hss_cmd_stop_roleswitch_peripheral_t *stop_roleswitch_args)
{
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_stop_procedure, hss_cmd_stop_roleswitch_peripheral);
  *HSS_CMD_STOP_ROLESWITCH_PERIPHERAL_CONNECTION_DEVICE_INDEX(hss_cmd) = stop_roleswitch_args->connection_device_index;
}

void prep_hss_cmd_compute_slot_offset_event(uint8_t *hss_cmd, hss_event_btc_slot_offset_t *btc_slot_offset_event_args)
{
  (void)btc_slot_offset_event_args;
  set_common_hss_cmd_fields(hss_cmd, hss_cmd_get_procedure, hss_cmd_get_roleswitch_parameter_slot_offset);
}

void prep_hss_cmd_payload_start_procedure(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}

void prep_hss_cmd_payload_stop_procedure(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}

void prep_hss_cmd_payload_start_procedure_payload(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}

void prep_hss_cmd_payload_stop_procedure_payload(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}
void prep_hss_cmd_payload_set_procedure_payload(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}
void prep_hss_cmd_payload_set_procedure(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}
void prep_hss_cmd_payload_procedure(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}
void prep_hss_cmd_payload(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}
void prep_hss_cmd(uint8_t *hss_cmd)
{
  // RFU
  (void) hss_cmd;
}

// function to enqueue and push the pending command queue to LPW to get processed.
void hss_command_to_lpw(pkb_t *pkb, shared_mem_t *hss_to_lpw_command_queue)
{
  enqueue(&hss_to_lpw_command_queue->pending_queue, &pkb->queue_node);
  //TODO : Add semaphore release here to signal the command pending event to the LPW task to process the command in the queue
  //TODO: Call the below RAIL API by properly linking RAIL includes in the cmake
  // RAIL_USER_SendMbox(railHandle, &host_to_lpw_command_queue.pending_queue); RAIL API to be called to send the address of the enqueued command pending queue to LPW to get processed
}

// function to dequeue the processed command from LPW and to free up the buffer
void lpw_command_processed_handler(shared_mem_t *lpw_cdq)
{
  shared_mem_t *host_to_lpw_queue_ptr = (shared_mem_t *)lpw_cdq;
  queue_t *processed_queue = &host_to_lpw_queue_ptr->processed_queue;
  while (processed_queue->size > 0) {
    pkb_t *pkb = (pkb_t *)dequeue(processed_queue);
    // TODO : Add semaphore release here to signal the command processed event to the host interface task to process the next command in the queue
    pkb_free(pkb, &pkb_pool_g[pkb->pkb_chunk_type]);
  }
}

void execute_command(void (*prep_cmd_func)(uint8_t *, void *), void *cmd_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  pkb_t *pkb = pkb_alloc(&pkb_pool_g[pkb_chunk_type_medium], pkb_alloc_type_no_fail);
  prep_cmd_func((uint8_t *)((uint8_t *)pkb + PKB_DATA_OFFSET), cmd_handler);
  hss_command_to_lpw(pkb, hss_to_lpw_command_queue);
}

void init_device_cmd(hss_cmd_init_btc_device_t *init_btc_device, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_init_btc_device, init_btc_device, hss_to_lpw_command_queue);
}

void set_inquiry_cmd(hss_cmd_set_inquiry_t *set_inquiry_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_inquiry, set_inquiry_handler, hss_to_lpw_command_queue);
}

void set_e0_encryption_cmd(hss_cmd_set_E0_encryption_t *set_e0_encryption_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_e0_encryption, set_e0_encryption_handler, hss_to_lpw_command_queue);
}

void set_aes_encryption_cmd(hss_cmd_set_AES_encryption_t *set_aes_encryption_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_aes_encryption, set_aes_encryption_handler, hss_to_lpw_command_queue);
}

void set_e0_encryption_parameters_cmd(hss_cmd_set_e0_encryption_parameters_t *set_e0_encryption_parameters_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_e0_encryption_parameters, set_e0_encryption_parameters_handler, hss_to_lpw_command_queue);
}

void set_aes_encryption_parameters_cmd(hss_cmd_set_aes_encryption_parameters_t *set_aes_encryption_parameters_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_aes_encryption_parameters, set_aes_encryption_parameters_handler, hss_to_lpw_command_queue);
}

void start_inquiry_cmd(hss_cmd_start_inquiry_t *start_inquiry_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_inquiry, start_inquiry_handler, hss_to_lpw_command_queue);
}

void start_test_mode_cmd(hss_cmd_start_test_mode_t *start_test_mode_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_test_mode, start_test_mode_handler, hss_to_lpw_command_queue);
}

void set_test_mode_params_cmd(hss_cmd_set_test_mode_params_t *set_test_mode_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_test_mode_params, set_test_mode_handler, hss_to_lpw_command_queue);
}

void set_device_parameters_acl_cmd(hss_cmd_set_device_parameters_acl_t *set_device_parameters_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_device_parameters_acl, set_device_parameters_acl_handler, hss_to_lpw_command_queue);
}

void set_afh_parameters_acl_cmd(hss_cmd_set_afh_parameters_acl_t *set_afh_parameters_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_afh_parameters_acl, set_afh_parameters_acl_handler, hss_to_lpw_command_queue);
}

void set_sniff_parameters_acl_cmd(hss_cmd_set_sniff_parameters_acl_t *set_sniff_parameters_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_sniff_parameters_acl, set_sniff_parameters_acl_handler, hss_to_lpw_command_queue);
}

void start_esco_central_cmd(hss_cmd_start_esco_central_t *start_esco_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_esco_central, start_esco_handler, hss_to_lpw_command_queue);
}

void start_esco_peripheral_cmd(hss_cmd_start_esco_peripheral_t *start_esco_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_esco_peripheral, start_esco_handler, hss_to_lpw_command_queue);
}

void start_roleswitch_central_cmd(hss_cmd_start_roleswitch_central_t *start_roleswitch_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_roleswitch_central, start_roleswitch_handler, hss_to_lpw_command_queue);
}

void set_device_parameters_roleswitch_cmd(hss_cmd_set_device_parameters_roleswitch_t *set_device_parameters_roleswitch_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_device_parameters_roleswitch, set_device_parameters_roleswitch_handler, hss_to_lpw_command_queue);
}

void set_roleswitch_parameters_cmd(hss_cmd_set_roleswitch_parameters_t *set_roleswitch_parameters_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_roleswitch_parameters, set_roleswitch_parameters_handler, hss_to_lpw_command_queue);
}

void start_roleswitch_peripheral_cmd(hss_cmd_start_roleswitch_peripheral_t *start_roleswitch_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_roleswitch_peripheral, start_roleswitch_handler, hss_to_lpw_command_queue);
}

void set_device_parameters_esco_cmd(hss_cmd_set_device_parameters_esco_t *set_device_parameters_esco_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_device_parameters_esco, set_device_parameters_esco_handler, hss_to_lpw_command_queue);
}

void set_inquiry_scan_cmd(hss_cmd_set_inquiry_scan_t *set_inquiry_scan_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_inquiry_scan, set_inquiry_scan_handler, hss_to_lpw_command_queue);
}

void set_page_cmd(hss_cmd_set_page_t *set_page_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_page, set_page_handler, hss_to_lpw_command_queue);
}

void set_page_scan_cmd(hss_cmd_set_page_scan_t *set_page_scan_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_page_scan, set_page_scan_handler, hss_to_lpw_command_queue);
}

void set_fixed_btc_channel_cmd(hss_cmd_set_fixed_btc_channel_t *set_fixed_btc_channel_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_set_fixed_btc_channel, set_fixed_btc_channel_handler, hss_to_lpw_command_queue);
}

void start_acl_central_cmd(hss_cmd_start_acl_central_t *start_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_acl_central, start_acl_handler, hss_to_lpw_command_queue);
}

void start_acl_peripheral_cmd(hss_cmd_start_acl_peripheral_t *start_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_acl_peripheral, start_acl_handler, hss_to_lpw_command_queue);
}

void stop_acl_central_cmd(hss_cmd_stop_acl_central_t *stop_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_stop_acl_central, stop_acl_handler, hss_to_lpw_command_queue);
}

void stop_acl_peripheral_cmd(hss_cmd_stop_acl_peripheral_t *stop_acl_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_stop_acl_peripheral, stop_acl_handler, hss_to_lpw_command_queue);
}

void start_page_cmd(hss_cmd_start_page_t *start_page_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_page, start_page_handler, hss_to_lpw_command_queue);
}

void stop_inquiry_cmd(hss_cmd_stop_inquiry_t *stop_inquiry_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_stop_inquiry, stop_inquiry_handler, hss_to_lpw_command_queue);
}

void stop_page_cmd(hss_cmd_stop_page_t *stop_page_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_stop_page, stop_page_handler, hss_to_lpw_command_queue);
}

void start_inquiry_scan_cmd(hss_cmd_start_inquiry_scan_t *start_inquiry_scan_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_inquiry_scan, start_inquiry_scan_handler, hss_to_lpw_command_queue);
}

void start_page_scan_cmd(hss_cmd_start_page_scan_t *start_page_scan_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_start_page_scan, start_page_scan_handler, hss_to_lpw_command_queue);
}

void stop_inquiry_scan_cmd(hss_cmd_stop_inquiry_scan_t *stop_inquiry_scan_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_stop_inquiry_scan, stop_inquiry_scan_handler, hss_to_lpw_command_queue);
}

void stop_page_scan_cmd(hss_cmd_stop_page_scan_t *stop_page_scan_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_stop_page_scan, stop_page_scan_handler, hss_to_lpw_command_queue);
}

void compute_slot_offset_event_cmd(hss_event_btc_slot_offset_t *btc_slot_offset_event_handler, shared_mem_t *hss_to_lpw_command_queue)
{
  execute_command((void (*)(uint8_t *, void *))prep_hss_cmd_compute_slot_offset_event, btc_slot_offset_event_handler, hss_to_lpw_command_queue);
}

#if 0 //Todo: Move this API to appropriate file
void update_afh_data_from_hss(btc_dev_t *btc_dev, uint32_t afh_new_channel_map_instant, uint8_t *afh_new_channel_map, uint8_t connection_device_index)
{
  // Declare an external pointer to a connection_info_t structure
  connection_info_t *connection_info = &btc_dev->connection_info[connection_device_index];
  // Store the new AFH channel map instant in the connection_info structure
  connection_info->afh_new_channel_map_instant = afh_new_channel_map_instant;
  // Copy the new AFH channel map to the connection_info structure.
  memcpy(connection_info->afh_new_channel_map, afh_new_channel_map, 10);
  SET_BIT(connection_info->procedure_instant_bitmap, AFH_INSTANT_PENDING_BIT);
}
#endif
