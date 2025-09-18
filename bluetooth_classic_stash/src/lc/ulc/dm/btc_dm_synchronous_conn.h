#ifndef _BTC_LMP_SYNCHRONOUS_CONN_H_
#define _BTC_LMP_SYNCHRONOUS_CONN_H_
#include "btc_dev.h"
#include "btc_hci.h"
#include "ulc.h"


void dm_sco_link_formation_indication_from_lm(ulc_t *ulc, uint8_t peer_id, uint8_t accept_cmd_type, uint8_t status);
void brm_new_esco_connection_ind(ulc_t *ulc, uint8_t peer_id, acl_peer_info_t *acl_peer_info);
void dm_esco_link_formation_indication_from_lm(ulc_t *ulc, uint8_t peer_id, uint8_t status);
#endif


