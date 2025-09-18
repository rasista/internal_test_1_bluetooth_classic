

#ifndef _BTC_LMP_SYNCHRONOUS_CONN_H_
#define _BTC_LMP_SYNCHRONOUS_CONN_H_
#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"

void copy_esco_params(esco_link_params_t *dest_params, esco_link_params_t *src_params, uint8_t reset);
uint8_t lmp_esco_params_accept_check(ulc_t *ulc,
                                     uint8_t peer_id,
                                     esco_link_params_t *esco_link_update_params);
uint8_t lmp_negotiate_esco_params(ulc_t *ulc, uint8_t peer_id, uint8_t tid, uint8_t esco_nego_status);
uint8_t lmp_accept_sync_conn_req_from_dm(ulc_t *ulc, uint8_t peer_id, uint8_t accept_cmd_type);

#endif
