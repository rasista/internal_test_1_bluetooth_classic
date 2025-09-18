#ifndef _BTC_DM_AUTHENTICATION_H
#define _BTC_DM_AUTHENTICATION_H

#define COMB_KEY                 0x00
#define LOCAL_UNIT_KEY           0x01
#define REMOTE_UNIT_KEY          0x02
#define DEBUG_COMB_KEY           0x03
#define UNAUTHENTICATED_COMB_KEY 0x04
#define AUTHENTICATED_COMB_KEY   0x05
#define CHANGED_COMB_KEY         0x06

void lmp_process_central_authentication_state(ulc_t *ulc, uint8_t peer_id, uint8_t lmp_recvd_pkt_flag, uint8_t reason_code);
void lmp_link_key_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_link_key_neg_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_pin_code_req_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_pin_code_req_neg_reply_recvd_from_hci(ulc_t *ulc, uint8_t peer_id);
void lmp_auth_req_from_hci(ulc_t *ulc, uint8_t peer_id);
#endif
