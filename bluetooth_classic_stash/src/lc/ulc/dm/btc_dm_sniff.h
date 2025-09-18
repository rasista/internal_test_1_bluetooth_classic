#ifndef _BTC_DM_SNIFF_H_
#define _BTC_DM_SNIFF_H_
#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"

#define IS_IN_SNIFF_STATE(acl_link_mgmt_info) \
    (((acl_link_mgmt_info)->connected_sub_state == CONNECTED_SNIFF_MODE) || \
     ((acl_link_mgmt_info)->connected_sub_state == CONNECTED_SNIFF_TRANSITION_MODE))

#define SNIFF_SUBRATING_INSTANT_MULTIPLIER    14
#define MIN_SNIFF_OFFSET_SLOTS    32
#define SAFE_SNIFF_TRANSITION_SLOTS      400
#define IS_SNIFF_ATTEMPT_INVALID(attempt) (!(attempt))
#define IS_INTERVAL_ODD(interval) ((interval) & 1)
#define IS_INTERVAL_RANGE_INVALID(max_interval, min_interval) ((max_interval) < (min_interval))
#define HAS_OTHER_ACTIVE_PEERS(priority_roles, peer_id) ((priority_roles) & ~(BIT(peer_id)))
#define IS_SUPERVISION_TIMEOUT_VIOLATED(link_mgmt_info, max_interval) \
    (((link_mgmt_info)->link_supervision_enabled == LINK_SUPERVISION_ENABLED) && \
     ((link_mgmt_info)->link_supervision_timeout < (max_interval)))

#define IS_SNIFF_PARAMS_INVALID(sniff_params, acl_link_mgmt_info, max_interval, min_interval) \
  (IS_SNIFF_ATTEMPT_INVALID((sniff_params)->sniff_attempt) || \
   IS_INTERVAL_ODD(max_interval) || IS_INTERVAL_ODD(min_interval) || \
   IS_SUPERVISION_TIMEOUT_VIOLATED(acl_link_mgmt_info, max_interval) || \
   IS_INTERVAL_RANGE_INVALID(max_interval, min_interval))

#define CALCULATE_SNIFF_RANDOM_EVEN_INTERVAL(min_interval, random_value, interval_range) \
    ((min_interval) + (((random_value) % (interval_range)) & ~1))

#define CALCULATE_MAX_SNIFF_SUBRATE(remote_latency, t_sniff_interval) \
    ((remote_latency) / (t_sniff_interval))

#define CALCULATE_SNIFF_SUBRATING_INSTANT(next_anchor_point, t_sniff_interval)   ((next_anchor_point) + (SNIFF_SUBRATING_INSTANT_MULTIPLIER * (t_sniff_interval)))

#define CALCULATE_D_SNIFF(next_anchor, curr_time, sniff_attempt, sniff_tout) \
    (MAX(MIN_SNIFF_OFFSET_SLOTS, ((next_anchor) - (curr_time)) / BT_SLOT_TIME_US) + \
     ((sniff_attempt) * (sniff_tout)))

#define IS_NO_SNIFF_CONFLICT(other_start_time, our_start_time, our_attempt_duration) \
    (((other_start_time) > (our_start_time)) && \
     (((other_start_time) - (our_start_time)) > (our_attempt_duration)))
void sniff_role_change_from_lpw(void *ctx, pkb_t *pkb);
void sniff_subrating_params_update_from_lpw(void *ctx, pkb_t *pkb);
uint8_t dm_enable_sniff_mode_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb);
uint16_t dm_calc_remote_max_sniff_subrate(ulc_t *ulc, uint8_t peer_id);
void calc_sniff_subrating_instant(ulc_t *ulc, uint8_t peer_id);
void lmp_change_link_supervision_tout_from_hci(ulc_t *ulc, uint8_t peer_id);
#endif


