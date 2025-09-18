#ifndef BTC_LMP_TEST_MODE_H
#define BTC_LMP_TEST_MODE_H

#include "lmp.h"


#define TEST_MODE_HANDLER_INDEX 2
// Test mode state defines
#define TEST_MODE_ENABLED   1
#define TEST_MODE_DISABLED  0


#define IS_TEST_MODE_BLOCKING_HOST_PKTS(ulc)   ((ulc)->btc_dev_info.btc_dev_mgmt_info.testmode_block_host_pkts == 1)

#define CRC_PASS 1
#define CRC_FAIL 0

// Test mode feature support macro
#define IS_HOST_DEV_TEST_MODE_ENABLED(dev_mgmt_info)  ((dev_mgmt_info)->host_dev_test_mode_enable & DEVICE_TEST_MODE_ENABLED)

// Test mode state management macros
#define SET_TEST_MODE_ENABLED(acl_link_mgmt_info)   ((acl_link_mgmt_info)->dev_in_test_mode = TEST_MODE_ENABLED)
#define SET_TEST_MODE_DISABLED(acl_link_mgmt_info)  ((acl_link_mgmt_info)->dev_in_test_mode = TEST_MODE_DISABLED)
#define IS_TEST_MODE_ENABLED(ulc, pkb)   (((btc_dev_info_t *)&((ulc)->btc_dev_info))->acl_peer_info[GET_PEER_ID(pkb)]->acl_link_mgmt_info.dev_in_test_mode == TEST_MODE_ENABLED)
#define IS_HV1_PKT_TYPE(test_mode_params)  ((test_mode_params)->pkt_type == BT_HV1_PKT_TYPE)
#define IS_HV2_PKT_TYPE(test_mode_params)  ((test_mode_params)->pkt_type == BT_HV2_PKT_TYPE)
#define IS_HV3_PKT_TYPE(test_mode_params)  ((test_mode_params)->pkt_type == BT_HV3_PKT_TYPE)
#define IS_DV_PKT_TYPE(test_mode_params)   ((test_mode_params)->pkt_type == BT_DV_PKT_TYPE)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define SET_TEST_LT_TYPE(params) \
    do { \
        if ((params)->test_lt_type == 0x02) { \
            (params)->test_lt_type = TEST_ACL; \
        } else if ((params)->test_lt_type == 0x00) { \
            if (IS_HV1_PKT_TYPE(params) || IS_HV2_PKT_TYPE(params) || \
                IS_HV3_PKT_TYPE(params) || IS_DV_PKT_TYPE(params)) { \
                (params)->test_lt_type = TEST_SCO; \
            } else { \
                (params)->test_lt_type = TEST_ACL; \
            } \
        } else if (((params)->test_lt_type == 0x01) || \
                   ((params)->test_lt_type == 0x03)) { \
            (params)->test_lt_type = TEST_ESCO; \
        } else { \
            BTC_ASSERT(!"Invalid test_lt_type "); \
        } \
    } while (0)

    
#define SET_TEST_MODE_TYPE(test_mode_type, test_lt_type) \
    do { \
        if ((test_lt_type) == TEST_SCO) { \
            *(test_mode_type) = SCO_TEST_MODE; \
        } else if ((test_lt_type) == TEST_ESCO) { \
            *(test_mode_type) = ESCO_TEST_MODE; \
        } else { \
            *(test_mode_type) = ACL_TEST_MODE; \
        } \
    } while (0)

void test_mode_set_pkt_type(uint16_t *acl_pkt_type, uint8_t ptt, uint8_t pkt_type);
void lmp_process_test_config(ulc_t *ulc, uint8_t peer_id);
void btc_process_test_mode_packets (void *ctx, pkb_t *pkb);
#endif // BTC_LMP_TEST_MODE_H

