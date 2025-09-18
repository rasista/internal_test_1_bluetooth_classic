#ifndef test_mode_H_
#define test_mode_H_
#include "btc_common_defines.h"
#include "sl_btc_seq_interface.h"
#include "state_machine.h"
#include "btc_rail.h"
#ifdef SIM
#include "protocol_btc_rail.h"
#endif
#include "acl_common.h"

#define SEQUENCE_0    0
#define SEQUENCE_1    1
#define SEQUENCE_2    2
#define SEQUENCE_F0   3

#define TEST_MODE_BASIC_RATE 0
#define TEST_MODE_ENHANCED_RATE 1

#define FREQ_HOP_DISABLE 0 // Frequency hopping disabled
#define FREQ_HOP_ENABLE  1 // Fixed frequency hopping enabled
#define FREQ_HOP_RANDOM  2 // Random frequency hopping enabled

#define LOOP_BACK_MODE_DISABLE 0
#define LOOP_BACK_MODE_ENABLE  1

#define TEST_MODE_STATE_RX   0
#define TEST_MODE_STATE_TX   1

#define TEST_MODE_ID_PKT_TYPE (1 << 4)

#define TEST_MODE_DBG_STATS_INCR(var)              var++;

#define TEST_MODE_ACL_LINK_TYPE 0
#define TEST_MODE_SCO_LINK_TYPE 1
#define TEST_MODE_ESCO_LINK_TYPE 2

#define TEST_ENCRYPTION_MODE_OFF 0
#define TEST_ENCRYPTION_MODE_E0  1
#define TEST_ENCRYPTION_MODE_AES 2

#define TEST_MODE_MICROSECONDS_TO_SECONDS 1000000 // 1000000 microseconds = 1 second

#define TEST_MODE_HV1_VOICE_PAYLOAD_LEN 10

typedef struct test_mode_stats_s {
  uint32_t schedules;
  uint32_t schedule_abort;
  uint32_t transmit_aborts;
  uint32_t transmit_dones;
  uint32_t receives_done;
  uint32_t receives_timeout; //Aborts
  uint32_t receives_abort;
  uint32_t crc_pass;
  uint32_t crc_fail;
} test_mode_stats_t;

typedef struct test_mode_s {
  uint8_t bd_addr[6];
  uint8_t pkt_type;
  uint16_t pkt_len;
  uint8_t br_edr_mode;
  uint8_t link_type;
  uint8_t payload_type;
  uint8_t hopping_type;
  uint8_t channel_index;
  uint8_t scrambler_seed;
  uint8_t tx_power;
  uint8_t loop_back_mode;
  uint8_t inter_packet_gap;
  uint8_t state;
  uint8_t no_of_slots;
  uint8_t encryption_mode;
  uint32_t rx_window; // in seconds
  uint32_t no_of_pkts;
  uint32_t sync_word[2];
  sm_t *test_mode_sm;
  RAIL_SEQ_BtcFhConfig_t btc_fh_data;
  btc_trx_configs_t trx_config;
  test_mode_stats_t test_mode_stats;
} test_mode_t;

void test_mode_tx_done_handler(sm_t *test_mode_sm);
void test_mode_tx_abort_handler(sm_t *test_mode_sm);
void test_mode_rx_done_handler(sm_t *test_mode_sm);
void test_mode_rx_timeout_handler(sm_t *test_mode_sm);
void test_mode_header_received_done_handler(sm_t *test_mode_sm);
void test_mode_header_received_timeout_handler(sm_t *test_mode_sm);
void test_mode_start_procedure_handler(sm_t *test_mode_sm);

#endif
