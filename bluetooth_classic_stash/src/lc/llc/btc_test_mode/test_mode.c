/**
 * @file test_mode.c
 * @author  (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2024-2025
 *
 */
#include "test_mode.h"
#include "btc_device.h"
#include "btc_rail.h"
#include "test_mode_sm_autogen.h"
#ifdef SIM
#include "rail_simulation.h"
#else
#include "rail_seq.h"
#include "btc_rail_intf.h"
#endif
#include "lpw_scheduler.h"
#include "lpw_to_host_interface.h"
#include "debug_logs.h"
#include "btc_mem_mgmt.h"

test_mode_t test_mode_g;

// Static encryption configs for test mode
static RAIL_SEQ_E0EncryptionConfig_t test_mode_e0_config;
static RAIL_SEQ_AESEncryptionConfig_t test_mode_aes_config;

const uint8_t payload_type[] = {
  0x00,
  0xFF,
  0x55,
  0xF0,
};
/**
 * @brief function is used to initialise inquiry parameters.
 * Should be seen as called HCI request
 *
 * @param test_mode_params - whether transmitter or receiver
 * In the test mode params hss_cmd follows params should be set
 * dev_addr: It is a 48-bit Address in hexadecimal format, e.g.,0023A7010203
 * pkt_type: Type of the packet to be transmitted, as per the Bluetooth standard.
 * pkt_length: Length of the packet, in bytes, to be transmitted.
 * br_edr_mode : basic rate - 1      enhanced_rate - 2 or 3
 * channel_index    - Transmit/Receive channel index, as per the Bluetooth standard. i.e, 0 to 78
 * link_type : sco - 0      acl - 1      esco - 2
 * scrambler_seed: Initial seed to be used for whitening. It should be set to ‘0’ in order to disable whitening.
 * no_of_packets: Number of packets to be transmitted. It is valid only when the <tx_mode> is set to Burst mode
 * payload_type: Type of payload to be transmitted.  ‘0’ – Payload consists of all zeros
                                                                                ‘1’ – Payload consists of all 0xFF’s
                                                                                ‘2’ – Payload consists of all 0x55’s
                                                                                ‘3’ – Payload consists of all 0xF0’s
                                                                                ‘4’ – Payload consists of PN9 sequence.

 * tx_power: Transmit power value should be between 0 and 18
 * hopping type : no hopping -0      fixed hopping - 1      random hopping - 2
 * loop_back_mode : Disable - 0  Enable - 1
 *
 * Few of the above parameters are applicable only for transmitter or receiver mode.
 *
 * @param btc_dev_p - pointer to the global btc device structure
 */
/**
 * @brief function is used to start test mode on the basis of transmit_mode, 1- TX, 2-RX
 *
 * @param inquiry- Testmode state machine control block, structure of test mode
 */
void start_test_mode(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  uint32_t btcReference = 0;
  uint64_t protimerReference = 0;
  (void) btc_dev_p;
  test_mode_t *test_mode = &test_mode_g;
  test_mode->pkt_type = *HSS_CMD_START_TEST_MODE_PKT_TYPE(hss_cmd);
  test_mode->br_edr_mode = *HSS_CMD_START_TEST_MODE_BR_EDR_MODE(hss_cmd);
  test_mode->pkt_len = *HSS_CMD_START_TEST_MODE_PKT_LEN(hss_cmd);
  test_mode->payload_type = *HSS_CMD_START_TEST_MODE_PAYLOAD_TYPE(hss_cmd);
  test_mode->channel_index = *HSS_CMD_START_TEST_MODE_CHANNEL_INDEX(hss_cmd);
  test_mode->link_type = *HSS_CMD_START_TEST_MODE_LINK_TYPE(hss_cmd);
  test_mode->scrambler_seed = *HSS_CMD_START_TEST_MODE_SCRAMBLER_SEED(hss_cmd);
  test_mode->no_of_pkts = *HSS_CMD_START_TEST_MODE_NO_OF_PKTS(hss_cmd);
  test_mode->tx_power = *HSS_CMD_START_TEST_MODE_TX_POWER(hss_cmd);
  test_mode->hopping_type = *HSS_CMD_START_TEST_MODE_HOPPING_TYPE(hss_cmd);
  test_mode->loop_back_mode = *HSS_CMD_START_TEST_MODE_LOOPBACK_MODE(hss_cmd);
  test_mode->state = *HSS_CMD_START_TEST_MODE_TRANSMIT_MODE(hss_cmd);
  test_mode->inter_packet_gap = *HSS_CMD_START_TEST_MODE_INTER_PACKET_GAP(hss_cmd);
  test_mode->rx_window = *HSS_CMD_START_TEST_MODE_RX_WINDOW(hss_cmd); // in seconds
  memcpy(test_mode->bd_addr, HSS_CMD_START_TEST_MODE_BD_ADDRESS(hss_cmd), LAP_ADDR_LEN);
  memcpy(test_mode->sync_word, HSS_CMD_START_TEST_MODE_SYNC_WORD(hss_cmd), SYNC_WORD_LEN);
  test_mode->test_mode_sm = &test_mode_sm;

#ifdef SIM
  log2vcd_prints_wrapper(test_mode->test_mode_sm);
  #else
  test_mode->trx_config.bd_clk_counter_handle.bd_clk_counter_id = BTC_NATIVE_CLK;
#endif
  RAIL_SEQ_BTC_InitBtcDate(BTC_NATIVE_CLK, btcReference, protimerReference);
  if ((test_mode->loop_back_mode) && (test_mode->state == TEST_MODE_STATE_RX)) {
    RAIL_SEQ_BTC_ConfigAutoMode(BTC_NATIVE_CLK, BTCCLKMODE_INIT);
    rail_set_btc_clk_instance(&test_mode->trx_config.bd_clk_counter_handle);
  }
  RAIL_SEQ_BTC_setAccessAddress(test_mode->sync_word);
  RAIL_SEQ_BTC_setHecInit(test_mode->bd_addr[3]);
  RAIL_SEQ_BTC_setCrcInit(test_mode->bd_addr[3]);
  RAIL_SEQ_BTC_setWhitening(test_mode->scrambler_seed);
  SM_STEP(TEST_MODE_SM, test_mode->test_mode_sm, START_PROCEDURE);
}

/**
 * @brief function is used to create the test mode packet.
 *
 * @param test_mode_p - structure of test mode containing information of transmitter or receiver mode.
 */
void form_test_mode_pkt(test_mode_t *test_mode_p)
{
  btc_trx_configs_t *btc_trx_configs = &test_mode_p->trx_config;
  uint32_t num_of_slots = 0;
  uint16_t pkt_len = test_mode_p->pkt_len;
  uint8_t *test_mode_pkt = NULL;
  uint8_t *payload_start = NULL;
  uint8_t payload_hdr_len = 0;
  uint8_t mode = test_mode_p->state;

  if ((mode) || test_mode_p->loop_back_mode) {
    // Allocate a buffer for the EIR packet
    test_mode_pkt = (uint8_t *)allocate_buffer();
    if (test_mode_pkt == NULL) {
      return;     // Buffer allocation failed
    }
    num_of_slots = packet_type_slot_lookup_table[test_mode_p->pkt_type][test_mode_p->br_edr_mode];

    if (num_of_slots == 0xFF) {
      // Invalid packet type;
    }

    test_mode_p->no_of_slots = (num_of_slots << 1);
    payload_start = test_mode_pkt + BT_TX_PKT_HDR_LEN;
    if (test_mode_p->link_type == TEST_MODE_ACL_LINK_TYPE) {
      if (((test_mode_p->pkt_type == BT_DM1_PKT_TYPE) || (test_mode_p->pkt_type == BT_DH1_PKT_TYPE)) && (test_mode_p->br_edr_mode == TEST_MODE_BASIC_RATE)) {
        payload_hdr_len = 1;
        /* Filling the length in payload header */
        payload_start[0] = 2 /*ACL START */ | (1 << 2 /* FLOW GO */) | (pkt_len << 3);
      } else if ((test_mode_p->pkt_type > BT_FHS_PKT_TYPE)) {
        payload_hdr_len = 2;
        /* Filling the length in payload header */
        *(uint16_t *)&payload_start[0] = 2 /*ACL START */ | (1 << 2 /* FLOW GO */) | (pkt_len << 3);
      }
      payload_start += payload_hdr_len;
    }
    while (pkt_len) {
      *payload_start = payload_type[test_mode_p->payload_type];
      payload_start++;
      pkt_len--;
    }
  }

  btc_trx_configs->btc_rail_trx_config.specialPktType = SL_RAIL_BTC_NORMAL_PKT;
  //btc_trx_configs->btc_rail_trx_config.linkConfig = 0;

  if ((test_mode_p->link_type == TEST_MODE_SCO_LINK_TYPE) && (test_mode_p->pkt_type == BT_DV_PKT_TYPE)) {
    if ((mode) || (test_mode_p->loop_back_mode)) {
      /* Filling ACL Header in the DV Packet.*/
      payload_start[TEST_MODE_HV1_VOICE_PAYLOAD_LEN] = 2 /*ACL_START*/ | (1 << 2 /* FLOW FIXME*/)
                                                       | ((test_mode_p->pkt_len - 1 - TEST_MODE_HV1_VOICE_PAYLOAD_LEN) << 3);
    }
    btc_trx_configs->btc_rail_trx_config.specialPktType = SL_RAIL_BTC_DV_PKT;
  }

  if (test_mode_p->link_type == TEST_MODE_ESCO_LINK_TYPE) {
    btc_trx_configs->btc_rail_trx_config.linkConfig.esco = 1;
    btc_trx_configs->btc_rail_trx_config.linkConfig.payloadLen = (test_mode_p->pkt_len);
  } else if (test_mode_p->link_type == TEST_MODE_ACL_LINK_TYPE) {
    btc_trx_configs->btc_rail_trx_config.linkConfig.acl = 1;
  } else {
    btc_trx_configs->btc_rail_trx_config.linkConfig.sco = 1;
  }

  btc_trx_configs->btc_rail_trx_config.linkConfig.enhancedRate = test_mode_p->br_edr_mode;

  if ((mode) || (test_mode_p->loop_back_mode)) {
    /* Update packet header fields */
    test_mode_pkt[0] = 0 | (test_mode_p->pkt_type << 3) | (1 /*FLOW*/ << 7);
    test_mode_pkt[1] = 0;

    btc_trx_configs->btc_rail_trx_config.packetConfig.packetType = test_mode_p->pkt_type;
    btc_trx_configs->btc_rail_trx_config.txPacketLen = (test_mode_p->pkt_len + BT_TX_PKT_HDR_LEN + payload_hdr_len);
    btc_trx_configs->tx_pkt = test_mode_pkt;
  }
}

/**
 * @brief function is used to set the encryption parameters.
 *
 * @param hss_cmd - pointer to the HSS command structure
 * @param btc_dev_p - pointer to the global btc device structure
 */
void set_test_mode_encryption(uint8_t *hss_cmd, btc_dev_t *btc_dev_p)
{
  (void)btc_dev_p;
  test_mode_t *test_mode_p = &test_mode_g;
  btc_trx_configs_t *btc_trx_configs = &test_mode_p->trx_config;
  test_mode_p->encryption_mode =  *HSS_CMD_START_TEST_MODE_ENCRYPTION_MODE(hss_cmd);

  if (test_mode_p->encryption_mode == TEST_ENCRYPTION_MODE_E0) {
    // Use static E0 encryption config for test mode
    btc_trx_configs->e0EncConfig = &test_mode_e0_config;

    RAIL_SEQ_E0EncryptionConfig_t *e0EncConfig = btc_trx_configs->e0EncConfig;
    e0EncConfig->masterClock = *HSS_CMD_SET_E0_ENCRYPTION_MASTERCLOCK(hss_cmd);
    e0EncConfig->encKey0    = *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY0(hss_cmd);
    e0EncConfig->encKey1    = *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY1(hss_cmd);
    e0EncConfig->encKey2    = *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY2(hss_cmd);
    e0EncConfig->encKey3    = *HSS_CMD_SET_E0_ENCRYPTION_ENCKEY3(hss_cmd);
    e0EncConfig->bdAddr0    = *HSS_CMD_SET_E0_ENCRYPTION_BDADDR0(hss_cmd);
    e0EncConfig->bdAddr1    = *HSS_CMD_SET_E0_ENCRYPTION_BDADDR1(hss_cmd);
    e0EncConfig->keyLength  = *HSS_CMD_SET_E0_ENCRYPTION_KEYLENGTH(hss_cmd);
    RAIL_SEQ_BTC_setE0Encryption(e0EncConfig);
  } else if (test_mode_p->encryption_mode == TEST_ENCRYPTION_MODE_AES) {
    // Use static AES encryption config for test mode
    btc_trx_configs->aesEncConfig = &test_mode_aes_config;
    RAIL_SEQ_AESEncryptionConfig_t *aesEncConfig = btc_trx_configs->aesEncConfig;
    aesEncConfig->aesKeyBytes_0_3 = *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_0_3(hss_cmd);
    aesEncConfig->aesKeyBytes_4_7 = *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_4_7(hss_cmd);
    aesEncConfig->aesKeyBytes_8_11 = *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_8_11(hss_cmd);
    aesEncConfig->aesKeyBytes_12_15 = *HSS_CMD_SET_AES_ENCRYPTION_AESKEYBYTES_12_15(hss_cmd);
    aesEncConfig->aesIv1         = *HSS_CMD_SET_AES_ENCRYPTION_AESIV1(hss_cmd);
    aesEncConfig->aesIv2         = *HSS_CMD_SET_AES_ENCRYPTION_AESIV2(hss_cmd);
    aesEncConfig->dayCountAndDir = *HSS_CMD_SET_AES_ENCRYPTION_DAYCOUNTANDDIR(hss_cmd);
    aesEncConfig->zeroLenAclW    = *HSS_CMD_SET_AES_ENCRYPTION_ZEROLENACLW(hss_cmd);
    aesEncConfig->aesPldCntr1    = *HSS_CMD_SET_AES_ENCRYPTION_AESPLDCNTR1(hss_cmd);
    aesEncConfig->aesPldCntr2    = *HSS_CMD_SET_AES_ENCRYPTION_AESPLDCNTR2(hss_cmd);
    RAIL_SEQ_BTC_setAESEncryption(aesEncConfig);
  }
}

/**
 * @brief function handles the start procedure trigger for test mode state machine.
 *
 * @param test_mode_sm-structure of test mode state machine containing information of transmitter or receiver mode.
 */
void test_mode_start_procedure_handler(sm_t *test_mode_sm_p)
{
  /* Based on transmitter mode or receiver mode move state machine to Tx or Rx.*/
  test_mode_t *test_mode_p = &test_mode_g;
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  btc_dev_t *dev = BTC_DEV_PTR;
  dev->l2_scheduled_sm = test_mode_sm_p;
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &test_mode_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.txPower = test_mode_p->tx_power;
  btc_trx_configs->btc_rail_trx_config.encryptLink = test_mode_p->encryption_mode;

  if (test_mode_p->pkt_type == TEST_MODE_ID_PKT_TYPE) {
    SET_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType);
    test_mode_p->no_of_slots = 1; //Since ID Packet is half slot packet.
  } else {
    form_test_mode_pkt(test_mode_p);
  }

  // configure_nonconn_slot_params(btc_trx_configs, &test_mode_p->bd_clk_counter_handle, test_mode_p->sync_word, BTC_BD_CLK_2NP0);

  uint32_t current_bd_clk = 0;
  current_bd_clk = RAIL_SEQ_BTC_GetBtcDate(BTC_NATIVE_CLK) >> CLK_SCALE_FACTOR;
  btc_trx_configs->btc_rail_trx_config.clk = current_bd_clk + test_mode_p->inter_packet_gap + SLOT_WIDTH;
  btc_trx_configs->btc_rail_trx_config.tsf =
    RAIL_SEQ_BTC_ConvertBtcDate(BTC_NATIVE_CLK, btc_trx_configs->btc_rail_trx_config.clk) >> TSF_SCALE_FACTOR;

  if (test_mode_p->state == TEST_MODE_STATE_TX) {
    TEST_MODE_DBG_STATS_INCR(stats->schedules);

    if (IS_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType)) {
      status = rail_schedule_id_tx(btc_trx_configs);
    } else {
      status = rail_schedule_tx(btc_trx_configs);
    }
    SM_STATE(test_mode_sm_p, TEST_MODE_TX_SCHEDULED);
    if (status != BTC_RAIL_PASS_STATUS) {
      TEST_MODE_DBG_STATS_INCR(stats->schedule_abort);
      //  if ID TX packet schedule failed, move forward assuming TX abort
      SM_STEP(TEST_MODE_SM, test_mode_sm_p, TX_ABORT);
    }
  } else if (test_mode_p->state == TEST_MODE_STATE_RX) {
    SET_CONF_FLAG_RX(btc_trx_configs->btc_rail_trx_config.isRx);
    TEST_MODE_DBG_STATS_INCR(stats->schedules);
    btc_trx_configs->rx_window = test_mode_p->rx_window * TEST_MODE_MICROSECONDS_TO_SECONDS; // Set a default RX window of 100 seconds
    if (IS_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType)) {
      status = rail_schedule_id_rx(btc_trx_configs);
    } else {
      status = rail_schedule_rx(btc_trx_configs);
    }
    SM_STATE(test_mode_sm_p, TEST_MODE_WAITING_FOR_RX_COMPLETE);

    if (status != BTC_RAIL_PASS_STATUS) {
      TEST_MODE_DBG_STATS_INCR(stats->schedule_abort);
      //  if ID TX packet schedule failed, move forward assuming TX abort
      SM_STEP(TEST_MODE_SM, test_mode_sm_p, RX_ABORT);
    }
  }
}

/**
 * @brief function handles the transmission and Aborts of Transmit packets for test mode state machine.
 *
 * @param test_mode_sm - structure of test mode state machine containing information of transmitter or receiver mode.
 * @param test_mode_t - structure of test mode containing information of transmitter or receiver mode.
 */
void test_mode_tx_handler(sm_t *test_mode_sm_p, test_mode_t *test_mode_p)
{
  /* Based on the hopping mode check whether we need to stay in same transmit frequency or not
   * and proceed state machine.
   */
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &test_mode_p->trx_config;
  uint32_t BtcDate = 0;
  uint64_t ProtimerDate = 0;

  SET_CONF_FLAG_TX(btc_trx_configs->btc_rail_trx_config.isRx);
  if (test_mode_p->loop_back_mode) {
    RAIL_SEQ_BTC_GetLastRxDate(btc_trx_configs->bd_clk_counter_handle.bd_clk_counter_id, &BtcDate, &ProtimerDate);
    btc_trx_configs->btc_rail_trx_config.clk = BtcDate + test_mode_sm_p->last_rx_slots + 1; // Schedule Tx as a response in the next slot
    (void)ProtimerDate;
  } else {
    btc_trx_configs->btc_rail_trx_config.clk  = btc_trx_configs->btc_rail_trx_config.clk + test_mode_p->no_of_slots + test_mode_p->inter_packet_gap;
    btc_trx_configs->btc_rail_trx_config.tsf =
      RAIL_SEQ_BTC_ConvertBtcDate(BTC_NATIVE_CLK, btc_trx_configs->btc_rail_trx_config.clk) >> TSF_SCALE_FACTOR;
  }
  if ((stats->transmit_dones < test_mode_p->no_of_pkts) || (test_mode_p->no_of_pkts == 0)) {
    TEST_MODE_DBG_STATS_INCR(stats->schedules);

    if (IS_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType)) {
      status = rail_schedule_id_tx(btc_trx_configs);
    } else {
      status = rail_schedule_tx(btc_trx_configs);
    }
    SM_STATE(test_mode_sm_p, TEST_MODE_TX_SCHEDULED);

    if (status != BTC_RAIL_PASS_STATUS) {
      //  if ID TX packet schedule failed, move forward assuming TX abort
      TEST_MODE_DBG_STATS_INCR(stats->schedule_abort);
      SM_STEP(TEST_MODE_SM, test_mode_sm_p, TX_ABORT);
    }
  }
}

void test_mode_rx_handler(sm_t *test_mode_sm_p, test_mode_t *test_mode_p)
{
  /* Based on the hopping mode check whether we need to stay in same transmit frequency or not
   * and proceed state machine.
   */
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  uint32_t status;
  btc_trx_configs_t *btc_trx_configs = &test_mode_p->trx_config;
  btc_trx_configs->btc_rail_trx_config.clk  = btc_trx_configs->btc_rail_trx_config.clk + test_mode_p->no_of_slots + test_mode_p->inter_packet_gap;

  btc_trx_configs->btc_rail_trx_config.tsf =
    RAIL_SEQ_BTC_ConvertBtcDate(BTC_NATIVE_CLK, btc_trx_configs->btc_rail_trx_config.clk) >> TSF_SCALE_FACTOR;
  TEST_MODE_DBG_STATS_INCR(stats->schedules);
  SET_CONF_FLAG_RX(btc_trx_configs->btc_rail_trx_config.isRx);
  btc_trx_configs->rx_window = BTC_MIN_RAIL_RX_TIMEOUT;
  if (IS_CONF_FLAG_ID(btc_trx_configs->btc_rail_trx_config.specialPktType)) {
    status = rail_schedule_id_rx(btc_trx_configs);
  } else {
    status = rail_schedule_rx(btc_trx_configs);
  }
  SM_STATE(test_mode_sm_p, TEST_MODE_WAITING_FOR_RX_COMPLETE);

  if (status != BTC_RAIL_PASS_STATUS) {
    //  if ID TX packet schedule failed, move forward assuming TX abort
    TEST_MODE_DBG_STATS_INCR(stats->schedule_abort);
    SM_STEP(TEST_MODE_SM, test_mode_sm_p, RX_ABORT);
  }
}

/**
 * @brief function handles the transmission of Transmit packets for test mode state machine.
 *
 * @param test_mode_sm - structure of test mode state machine containing information of transmitter or receiver mode.
 */
void test_mode_tx_done_handler(sm_t *test_mode_sm_p)
{
  /* Based on the hopping mode check whether we need to stay in same transmit frequency or not
   * and proceed state machine.
   */
  test_mode_t *test_mode_p = &test_mode_g;
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  TEST_MODE_DBG_STATS_INCR(stats->transmit_dones);
  if ((test_mode_p->state == TEST_MODE_STATE_TX) && (!test_mode_p->loop_back_mode)) {
    test_mode_tx_handler(test_mode_sm_p, test_mode_p);
  } else if (test_mode_p->loop_back_mode) {
    RAIL_SEQ_BTC_ConfigAutoMode(BTC_NATIVE_CLK, BTCCLKMODE_AUTO);
    rail_set_btc_clk_instance(&test_mode_p->trx_config.bd_clk_counter_handle);
    test_mode_rx_handler(test_mode_sm_p, test_mode_p);
  }
}

/**
 * @brief function handles the transmission of Transmit Abort packets for test mode state machine.
 *
 * @param test_mode_sm - structure of test mode state machine containing information of transmitter or receiver mode.
 */
void test_mode_tx_abort_handler(sm_t *test_mode_sm_p)
{
  /* Based on the hopping mode check whether we need to stay in same transmit frequency or not
   * and proceed state machine.
   */
  test_mode_t *test_mode_p = &test_mode_g;
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  TEST_MODE_DBG_STATS_INCR(stats->transmit_aborts);
  if ((test_mode_p->state == TEST_MODE_STATE_TX) && (!test_mode_p->loop_back_mode)) {
    test_mode_tx_handler(test_mode_sm_p, test_mode_p);
  } else if (test_mode_p->loop_back_mode) {
    test_mode_rx_handler(test_mode_sm_p, test_mode_p);
  }
}

/*
   void test_mode_header_received_done_handler(sm_t *test_mode_sm_p)
   {
   SM_STATE(test_mode_sm_p, TEST_MODE_WAITING_FOR_RX_COMPLETE);
   }


   void test_mode_header_received_timeout_handler(sm_t *test_mode_sm_p)
   {
   test_mode_t *test_mode_p = &test_mode_g;
   uint32_t status;
   btc_trx_configs_t *btc_trx_configs = &test_mode_p->trx_config;
   btc_trx_configs->btc_rail_trx_config.clk  = btc_trx_configs->btc_rail_trx_config.clk + test_mode_p->inter_packet_gap + SLOT_WIDTH;
   btc_trx_configs->btc_rail_trx_config.tsf =
    RAIL_SEQ_BTC_ConvertBtcDate(BTC_NATIVE_CLK, btc_trx_configs->btc_rail_trx_config.clk) >> TSF_SCALE_FACTOR;
   btc_trx_configs->end_tsf = 0xFFFFFFFF;
   status = rail_schedule_id_rx(btc_trx_configs);
   SM_STATE(test_mode_sm_p, TEST_MODE_WAITING_FOR_RX_PKT_HEADER);

   if (status != BTC_RAIL_PASS_STATUS) {
    //  if ID TX packet schedule failed, move forward assuming TX abort
    SM_STEP(TEST_MODE_SM, test_mode_sm_p, RX_ABORT);
   }
   SM_STATE(test_mode_sm_p, TEST_MODE_WAITING_FOR_RX_PKT_HEADER);
   }
 */
void test_mode_rx_done_handler(sm_t *test_mode_sm_p)
{
  test_mode_t *test_mode_p = &test_mode_g;
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  TEST_MODE_DBG_STATS_INCR(stats->receives_done);
  if (test_mode_p->loop_back_mode) {
    test_mode_tx_handler(test_mode_sm_p, test_mode_p);
  }
}

void test_mode_rx_timeout_handler(sm_t *test_mode_sm_p)
{
  test_mode_t *test_mode_p = &test_mode_g;
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  TEST_MODE_DBG_STATS_INCR(stats->receives_timeout);
  if (test_mode_p->loop_back_mode) {
    test_mode_tx_handler(test_mode_sm_p, test_mode_p);
  }
}

void test_mode_rx_abort_handler(sm_t *test_mode_sm_p)
{
  test_mode_t *test_mode_p = &test_mode_g;
  test_mode_stats_t *stats = &test_mode_p->test_mode_stats;
  TEST_MODE_DBG_STATS_INCR(stats->receives_abort);
  if (test_mode_p->loop_back_mode) {
    test_mode_tx_handler(test_mode_sm_p, test_mode_p);
  }
}
