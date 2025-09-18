#ifndef __LMP__AFH_CLASSIFICATION_H__
#define __LMP_AFH_CLASSIFICATION_H__

#include "btc_dev.h"
#include "ulc.h"
#include "pkb_mgmt_struct.h"
#include "lmp_get_defines_autogen.h"

#define CHANNEL_ASSESSMENT_ENABLED 1
#define GOOD_CHANNEL_PAIR 0x03
#define BAD_GOOD_CHANNEL_PAIR 0x02
#define GOOD_BAD_CHANNEL_PAIR 0x01
#define GOOD_CHANNEL 0x03
#define UNKNOWN_CHANNEL 0x02
#define BAD_CHANNEL 0x01
#define AFH_INSTANT_CLOCK_MASK    0x0FFFFFFF
#define AFH_DEFAULT_MIN_INTERVAL        0xbb78
#define AFH_DEFAULT_MAX_INTERVAL        0xbb80
#define AFH_CHANNEL_MAP_DEFAULT_LOW     0xFFFFFFFF
#define AFH_CHANNEL_MAP_DEFAULT_HIGH    0xFFFFFFFF
#define AFH_CHANNEL_MAP_DEFAULT_UPPER   0x7FFF
#define AFH_RANDOM_MASK                 0xFFFF
#define AFH_CLOCK_PROGRESSION_SIM       0x100

#define GET_CHANNEL_PAIR_BITS(channel_pair, bit_pos) \
  ((channel_pair & (BIT(bit_pos) | BIT((bit_pos + 1)))) >> (bit_pos))

//extracting and shifting channel pair bits
#define EXTRACT_CHANNEL_PAIR(channel_pair, bit_pos) \
  do { channel_pair = ((channel_pair & (BIT(bit_pos) | BIT((bit_pos + 1)))) >> (bit_pos)); } while(0)

//checking if channel pair is good
#define IS_GOOD_CHANNEL_PAIR(channel_pair) \
  (channel_pair == GOOD_CHANNEL_PAIR)

//checking channel pair classification status
#define IS_UNKNOWN_CHANNEL(channel_pair)   ((channel_pair) == UNKNOWN_CHANNEL)
#define IS_BAD_CHANNEL(channel_pair)        ((channel_pair) == BAD_CHANNEL)

//checking if channel assessment is enabled
#define IS_CHANNEL_ASSESSMENT_ENABLED(btc_dev_mgmt_info) \
  (btc_dev_mgmt_info->channel_assessment_mode == CHANNEL_ASSESSMENT_ENABLED)

//checking if AFH reporting is enabled
#define IS_AFH_REPORTING_ENABLED(acl_dev_mgmt_info) \
  (acl_dev_mgmt_info->afh_reporting_mode == AFH_REPORTING_ENABLED)

//generating random number
#define GENERATE_AFH_RANDOM_NUM(random_num_ptr) \
  generate_rand_num(2, (uint8_t *)(random_num_ptr), PSEUDO_RAND_NUM)

//checking if AFH is enabled in LMP packet
#define IS_AFH_ENABLED_IN_RX_PKT(bt_pkt_start) \
  (GET_LMP_SET_AFH_PARAMETER_AFH_MODE(bt_pkt_start) == AFH_ENABLE)

#define IS_BIT_SET_IN_MAP(map, bit_index) \
  ((map)[(bit_index) / 8] & (1 << ((bit_index) % 8)))

//AFH channel map operations
#define SET_CHANNEL_PAIR_GOOD(map, byte_pos, bit_pos) \
  ((map)[byte_pos] |= (BIT(bit_pos) | BIT((bit_pos + 1))))

#define SET_CHANNEL_PAIR_BAD(map, byte_pos, bit_pos) \
  ((map)[byte_pos] &= ~(BIT(bit_pos) | BIT((bit_pos + 1))))

//to extract channel pair classification from AFH channel classification map
#define GET_CHANNEL_PAIR_CLASSIFICATION(map, byte_pos, bit_pos) \
  (((map)[byte_pos] >> (bit_pos)) & 0x03)

//calculate AFH instant with even alignment (rounding off the last bit)
#define CALCULATE_AFH_INSTANT(master_clock) \
  (((master_clock) & AFH_INSTANT_CLOCK_MASK) + 1 + AFH_INTERVAL_500_MS) & ~BIT(0)

//calculate AFH classification instant (current clock + AFH interval)
#define CALCULATE_AFH_CLASSIFICATION_INSTANT(afh_interval) \
  (get_clock_from_lpw() + (afh_interval))

// Macro wrappers for common operations
#define CHANNEL_MAP_AND(dst, src) \
  perform_channel_map_operation(dst, src, NULL, CHANNEL_MAP_OP_AND)

#define CHANNEL_MAP_OR(dst, src) \
  perform_channel_map_operation(dst, src, NULL, CHANNEL_MAP_OP_OR)

#define CHANNEL_MAP_COPY(dst, src) \
  perform_channel_map_operation(dst, src, NULL, CHANNEL_MAP_OP_COPY)

#define CHANNEL_MAP_AND_COMBINE(dst, src1, src2) \
  perform_channel_map_operation(dst, src1, src2, CHANNEL_MAP_OP_AND_COMBINE)

#define CHANNEL_MAP_SET_DEFAULT(dst) \
  perform_channel_map_operation(dst, NULL, NULL, CHANNEL_MAP_OP_SET_DEFAULT)

#define BIT_POSITION_MASK   0x7

// Enum for different channel map operations
typedef enum {
  CHANNEL_MAP_OP_AND,     // dst[i] &= src[i]
  CHANNEL_MAP_OP_OR,      // dst[i] |= src[i]
  CHANNEL_MAP_OP_COPY,    // dst[i] = src[i]
  CHANNEL_MAP_OP_AND_COMBINE, // dst[i] = (src1[i] & src2[i])
  CHANNEL_MAP_OP_SET_DEFAULT  // Set to default AFH values
} channel_map_operation_t;

// Function declarations for AFH classification
void lmp_start_channel_classification(ulc_t *ulc, pkb_t *pkb);
void channel_classification_timeout_handler(ulc_t *ulc, pkb_t *pkb);
void lmp_central_send_classification(ulc_t *ulc, pkb_t *pkb);
void lmp_update_afh_channel_map(acl_dev_mgmt_info_t *acl_dev_mgmt_info, uint8_t *temp_map, uint8_t update_classification);
void bt_calculate_classification_channel_map(uint8_t *temp_map);
uint8_t get_no_of_ones(uint8_t *map, uint8_t max_channels);
uint32_t get_clock_from_lpw();
void perform_channel_map_operation(uint8_t *dst_map, const uint8_t *src1_map, const uint8_t *src2_map, channel_map_operation_t operation);

#endif
