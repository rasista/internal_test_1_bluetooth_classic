#ifndef _BTC_SNIFF_INTF_H
#define _BTC_SNIFF_INTF_H

#define SNIFF_SUPERVISION_TIMEOUT_MULTIPLIER    3
#define MAX_SNIFF_SUB_RATE 0xFF

#define IS_SNIFF_SUPERVISION_TIMEOUT_INVALID(supervision_timeout, sniff_interval) \
    ((supervision_timeout) < (SNIFF_SUPERVISION_TIMEOUT_MULTIPLIER * (sniff_interval)))

#define SNIFF_PARAMS_INVALID(sniff_params, t_sniff_orig, d_sniff_orig) \
    (((sniff_params)->t_sniff != (t_sniff_orig)) || ((sniff_params)->d_sniff != (d_sniff_orig)))

#define REJECT_SNIFF_IF_SUPERVISION_TIMEOUT_INVALID(ulc, peer_id, tid, acl_link_mgmt_info, sniff_params, lmp_input_params) \
    do { \
        if (IS_SNIFF_SUPERVISION_TIMEOUT_INVALID(acl_link_mgmt_info->link_supervision_timeout, sniff_params->t_sniff)) { \
            memset((uint8_t *)sniff_params, 0, sizeof(sniff_params_t)); \
            lmp_input_params[1] = UNSUPPORTED_LMP_PARAMETER_VALUE; \
            call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params); \
            return; \
        } \
    } while(0)


#define IS_ANCHOR_ALIGNMENT_VALID(d_sniff, t_sniff)   ((d_sniff) <= (t_sniff))

#define REJECT_SNIFF_ALIGNMENT_FAILED(ulc, peer_id, tid, sniff_params, lmp_input_params) \
    do { \
        memset((uint8_t *)sniff_params, 0, sizeof(sniff_params_t)); \
        lmp_input_params[1] = UNSUPPORTED_LMP_PARAMETER_VALUE; \
        call_lmp_tx_handler(ulc, ENCODE_LMP_CONTROL_WORD(LMP_NOT_ACCEPTED, 0, peer_id, tid), &lmp_input_params); \
        return; \
    } while(0)

#define HANDLE_SNIFF_ANCHOR_ALIGNMENT(ulc, priority_roles, peer_id, tid, sniff_params, lmp_input_params, d_sniff_temp) \
    do { \
        if (HAS_OTHER_ACTIVE_PEERS(priority_roles, peer_id)) { \
            d_sniff_temp = align_sniff_anchor_point(ulc, peer_id); \
            if (!IS_ANCHOR_ALIGNMENT_VALID(d_sniff_temp, sniff_params->t_sniff)) { \
                REJECT_SNIFF_ALIGNMENT_FAILED(ulc, peer_id, tid, sniff_params, lmp_input_params); \
            } \
        } \
    } while(0)

#define IS_SNIFF_SUBRATE_PARAMS_SET(sniff_params)  ((sniff_params)->sniff_subrate_params_set == SNIFF_SUBRATE_PARAMS_SET)
#define IS_IN_SNIFF_TRANSITION_MODE(acl_link_mgmt_info)   ((acl_link_mgmt_info)->connected_sub_state == CONNECTED_SNIFF_TRANSITION_MODE)
#define IS_IN_SNIFF_MODE(acl_link_mgmt_info)   ((acl_link_mgmt_info)->connected_sub_state == CONNECTED_SNIFF_MODE)

void exit_sniff_mode(ulc_t *ulc, uint8_t peer_id);
void bt_mode_change_event_to_hci(ulc_t *ulc, uint8_t peer_id, uint8_t status, uint16_t interval);
uint16_t align_sniff_anchor_point(ulc_t *ulc, uint8_t peer_id);
uint16_t get_priority_role_running(ulc_t *ulc, uint8_t peer_id);
void lmp_enable_or_disable_sniff_mode(ulc_t *ulc, uint8_t peer_id, uint8_t enable);
uint8_t lmp_change_sniff_subrating_params_from_hci(ulc_t *ulc, uint8_t peer_id, pkb_t *pkb);
#endif
